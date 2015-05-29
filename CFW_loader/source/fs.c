#include "fs.h"

#include "fatfs/ff.h"

static FATFS fs_fs;
static FIL fs_file;

// @breif  Init, and mount SDCard FileSystem.
// @retval true, If mounted OK.
// @note   The return value isn't used thus may cause some severe issue.
bool FSInit(void) {
	*(u32*)0x10000020 = 0;
	*(u32*)0x10000020 = 0x340;
	return f_mount(&fs_fs, "0:", 0) == FR_OK;
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
	bool ret = (f_open(&fs_file, path, flags) == FR_OK);
	//f_lseek(&fs_file, 0); //Cause R/W gives offset, save the call this time.
	f_sync(&fs_file);
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
	bool ret = (f_open(&fs_file, path, flags) == FR_OK);
	//f_lseek(&fs_file, 0); //Cause R/W gives offset, save the call this time.
	f_sync(&fs_file);
	return ret;
}
// @breif  Read from the opened File
// @param  buf     store the contents
// @param  size    content length
// @param  foffset where to start.
// @retval how many bytes actual read.
size_t FSFileRead(void* buf, size_t size, size_t foffset) {
	UINT bytes_read = 0;
	f_lseek(&fs_file, foffset);
	f_read(&fs_file, buf, size, &bytes_read);
	return bytes_read;
}
// @breif  Write to the opened File
// @param  buf     store the contents
// @param  size    content length
// @param  foffset where to start.
// @retval how many bytes actual written.
size_t FSFileWrite(void* buf, size_t size, size_t foffset) {
	UINT bytes_written = 0;
	f_lseek(&fs_file, foffset);
	f_write(&fs_file, buf, size, &bytes_written);
	f_sync(&fs_file);
	return bytes_written;
}
// @breif  [Unused]Tell the file size.
// @retval the file size.
// @note   this is expanded from a macro.
//         same as "return fs_file.fsize;"
size_t FSFileGetSize() {
	return f_size(&fs_file);
}
// @breif  Close the opened file.
void FSFileClose(void) {
	f_close(&fs_file);
}
