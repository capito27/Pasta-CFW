#include "fs.h"

#include "fatfs/ff.h"

static FATFS fsFS;
static FIL fsFile;

bool FSInit(void) {
	*(u32*)0x10000020 = 0;
	*(u32*)0x10000020 = 0x340;
	return f_mount(&fsFS, "0:", 0) == FR_OK;
}

void FSDeinit(void) {
	f_mount(NULL, "0:", 0);
}

bool FSFileOpen(const char* path) {
	unsigned flags = FA_READ | FA_WRITE | FA_OPEN_EXISTING;
	bool ret = (f_open(&fsFile, path, flags) == FR_OK);
	//f_lseek(&fsFile, 0);
	f_sync(&fsFile);
	return ret;
}

bool FSFileCreate(const char* path, bool truncate) {
	unsigned flags = FA_READ | FA_WRITE;
	flags |= truncate ? FA_CREATE_ALWAYS : FA_OPEN_ALWAYS;
	bool ret = (f_open(&fsFile, path, flags) == FR_OK);
	//f_lseek(&fsFile, 0);
	f_sync(&fsFile);
	return ret;
}

size_t FSFileRead(void* buf, size_t size, size_t foffset) {
	UINT bytes_read = 0;
	f_lseek(&fsFile, foffset);
	f_read(&fsFile, buf, size, &bytes_read);
	return bytes_read;
}

size_t FSFileWrite(void* buf, size_t size, size_t foffset) {
	UINT bytes_written = 0;
	f_lseek(&fsFile, foffset);
	f_write(&fsFile, buf, size, &bytes_written);
	f_sync(&fsFile);
	return bytes_written;
}

size_t FSFileGetSize(void) {
	return f_size(&fsFile);
}

void FSFileClose(void) {
	f_close(&fsFile);
}
