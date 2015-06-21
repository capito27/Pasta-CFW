#include "fs.h"
#include "crypto.h"
#include "fatfs/ff.h"

//FileSystem Object for SD. And we only have one file present at the same time.
static FATFS fsFS;
static FIL fsFile;

// @breif  Init, and mount SDCard FileSystem.
// @retval true, If mounted OK.
// @note   The return value isn't used thus may cause some severe issue.
bool FSInit(void) {
	*(u32*)0x10000020 = 0;
	*(u32*)0x10000020 = 0x340;
	return f_mount(&fsFS, "0:", 0) == FR_OK;
}

// @breif  [Unused]Unmount SDCard FileSystem.
// @note   Well this only unmount the fs. I assume that is ok (to be unused)?
void FSDeinit(void) {
	f_mount(NULL, "0:", 0);
}

// @breif  Open existing File from SD, with given path.
// @param  path  where the file is.
// @retval operation succueeded?
// @note   This would also put the cursor at its beginning.
//         Should we should take use of the original return value?
bool FSFileOpen(const char* path) {
	u8 flags = FA_READ | FA_WRITE | FA_OPEN_EXISTING;
	bool ret = (f_open(&fsFile, path, flags) == FR_OK);
	//f_lseek(&fsFile, 0);
	f_sync(&fsFile);
	return ret;
}

// @breif  Create File from SD, with given path.
// @param  path  where the file would be.
// @retval operation succueeded?
// @note   This would also put the cursor at its beginning.
//         Should we should take use of the original return value?
bool FSFileCreate(const char* path, bool truncate) {
	u8 flags = FA_READ | FA_WRITE;
	flags |= truncate ? FA_CREATE_ALWAYS : FA_OPEN_ALWAYS;
	bool ret = (f_open(&fsFile, path, flags) == FR_OK);
	//f_lseek(&fsFile, 0);
	f_sync(&fsFile);
	return ret;
}

// @breif  Read from the opened File
// @param  buf     store the contents
// @param  size    content length
// @param  foffset where to start.
// @retval how many bytes actual read.
size_t FSFileRead(void* buf, size_t size, size_t foffset) {
	UINT bytes_read = 0;
	f_lseek(&fsFile, foffset);
	f_read(&fsFile, buf, size, &bytes_read);
	return bytes_read;
}

// @breif  Write to the opened File
// @param  buf     store the contents
// @param  size    content length
// @param  foffset where to start.
// @retval how many bytes actual written.
size_t FSFileWrite(void* buf, size_t size, size_t foffset) {
	UINT bytes_written = 0;
	f_lseek(&fsFile, foffset);
	f_write(&fsFile, buf, size, &bytes_written);
	f_sync(&fsFile);
	return bytes_written;
}

// @breif  [Unused]Tell the file size.
// @retval the file size.
// @note   this is expanded from a macro.
//         same as "return fs_file.fsize;"
size_t FSFileGetSize(void) {
	return f_size(&fsFile);
}

// @breif  Close the opened file.
void FSFileClose(void) {
	f_close(&fsFile);
}

//-------------------------------------------------
u8 NANDCTR[16];
#define BLOCK_SIZE  (8*1024*1024)


u32 DecryptPartition(PartitionInfo* info){
	size_t bytesWritten;
	if (info->keyY != NULL)
		setup_aeskey(info->keyslot, AES_BIG_INPUT | AES_NORMAL_INPUT, info->keyY);
	use_aeskey(info->keyslot);

	u8 ctr[16] __attribute__((aligned(32)));
	memcpy(ctr, info->ctr, 16);

	u32 size_bytes = info->size;
	for (u32 i = 0; i < size_bytes; i += BLOCK_SIZE) {
		u32 j;
		for (j = 0; (j < BLOCK_SIZE) && (i + j < size_bytes); j += 16) {
			set_ctr(AES_BIG_INPUT | AES_NORMAL_INPUT, ctr);
			aes_decrypt((void*)info->buffer + j, (void*)info->buffer + j, ctr, 1, AES_CTR_MODE);
			add_ctr(ctr, 1);
		}
	}
	return 0;
}

void InitializeNandCrypto(){
	u32 ctrStart = 0;
	u32 listCtrStart[] = { 0x080D7CAC, 0x080D748C, 0x080D740C, 0x080D794C };
	char* listSystem[] = { "4.x", "6.x", "7.x", "9.x" };
	u32 lenListCtrStart = sizeof(listCtrStart) / sizeof(u32);
	for (u32 c = 0; c < lenListCtrStart; c++){
		if (*(u32*)listCtrStart[c] == 0x5C980){
			ctrStart = listCtrStart[c] + 0x30;
			break;
		}
	}

	//If value not in previous list start memory scanning (test range)
	if (ctrStart == 0){
		for (u32 c = 0x080D7FFF; c > 0x080D7000; c--){
			if (*(u32*)c == 0x5C980){
				ctrStart = c + 0x30;
				break;
			}
		}
	}
	for (int i = 0; i < 16; i++){
		NANDCTR[i] = *((u8*)(ctrStart + (15 - i))); //The CTR is stored backwards in memory.
	}


}

void GetNANDCTR(u8* ctr){
	for (int i = 0; i < 16; i++) *(ctr + i) = NANDCTR[i];
}

int nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition){
	PartitionInfo info;
	u8 myCtr[16];
	for (int i = 0; i < 16; i++) myCtr[i] = NANDCTR[i];
	info.ctr = &myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition){
	case TWLN: info.keyslot = 0x3; break;
	case TWLP: info.keyslot = 0x3; break;
	case AGB_SAVE: info.keyslot = 0x7; break;
	case FIRM0: info.keyslot = 0x6; break;
	case FIRM1: info.keyslot = 0x6; break;
	case CTRNAND: info.keyslot = 0x4; break;
	default: info.keyslot = 0x3; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);

	sdmmc_nand_readsectors(sector_no + partition / 0x200, numsectors, out);
	DecryptPartition(&info);
}

int nand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition){
	PartitionInfo info;
	u8 myCtr[16];
	for (int i = 0; i < 16; i++) myCtr[i] = NANDCTR[i];
	info.ctr = &myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition){
	case TWLN: info.keyslot = 0x3; break;
	case TWLP: info.keyslot = 0x3; break;
	case AGB_SAVE: info.keyslot = 0x7; break;
	case FIRM0: info.keyslot = 0x6; break;
	case FIRM1: info.keyslot = 0x6; break;
	case CTRNAND: info.keyslot = 0x4; break;
	default: info.keyslot = 0x3; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);

	DecryptPartition(&info);
	//sdmmc_nand_writesectors(sector_no + partition/0x200, numsectors, out);	//Stubbed, i don't wanna risk
}

int emunand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition){
	PartitionInfo info;
	u8 myCtr[16];
	for (int i = 0; i < 16; i++) myCtr[i] = NANDCTR[i];
	info.ctr = &myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition){
	case TWLN: info.keyslot = 0x3; break;
	case TWLP: info.keyslot = 0x3; break;
	case AGB_SAVE: info.keyslot = 0x7; break;
	case FIRM0: info.keyslot = 0x6; break;
	case FIRM1: info.keyslot = 0x6; break;
	case CTRNAND: info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);

	sdmmc_sdcard_readsectors(sector_no + partition / 0x200, numsectors, out);
	DecryptPartition(&info);
}

int emunand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition){
	PartitionInfo info;
	u8 myCtr[16];
	for (int i = 0; i < 16; i++) myCtr[i] = NANDCTR[i];
	info.ctr = &myCtr; info.buffer = out; info.size = numsectors * 0x200; info.keyY = NULL;
	add_ctr(info.ctr, partition / 16);
	switch (partition){
	case TWLN: info.keyslot = 0x3; break;
	case TWLP: info.keyslot = 0x3; break;
	case AGB_SAVE: info.keyslot = 0x7; break;
	case FIRM0: info.keyslot = 0x6; break;
	case FIRM1: info.keyslot = 0x6; break;
	case CTRNAND: info.keyslot = 0x4; break;
	}
	add_ctr(info.ctr, sector_no * 0x20);

	DecryptPartition(&info);
	sdmmc_sdcard_writesectors(sector_no + partition / 0x200, numsectors, out);	//Stubbed, i don't wanna risk
}
