.arm
.align 4
.code 32
.text

.global arm9_start
@ insert your funky stuff here
@ this code will not be executed if "/arm9payload.bin" is present
arm9_start:
	B               skipvars

	@ offs 4, to be filled during runtime
	pa_arm9_entrypoint_backup: .long 0
	
skipvars:
	STMFD           SP!, {R0-R12,LR}

	@ delay execution. just for "fun"
	MOV             R1, #0x10
outer:
	
	MOV             R0, #0
	ADD             R0, R0, #0xFFFFFFFF
inner:
	SUBS            R0, R0, #1
	NOP
	BGT             inner
	SUBS            R1, R1, #1
	BGT             outer

	LDMFD           SP!, {R0-R12,LR}
	
	LDR             PC, pa_arm9_entrypoint_backup
	
.global arm9_end
arm9_end:
