#include "common.h"
#include "fs.h"
#include "fatfs/ff.h"

static FATFS fs;
static FIL file;

u8 NANDCTR[16];

void InitializeNandCrypto(){
	u32 ctrStart=0;
    u32 listCtrStart[] = {0x080D7CAC, 0x080D748C, 0x080D740C, 0x080D794C};
    char* listSystem[] = {"4.x", "6.x", "7.x", "9.x"};
    u32 lenListCtrStart = sizeof(listCtrStart)/sizeof(u32);
    for(u32 c=0; c < lenListCtrStart; c++){
        if(*(u32*)listCtrStart[c] == 0x5C980){
            ctrStart = listCtrStart[c] + 0x30;
            break;
        }
    }

    //If value not in previous list start memory scanning (test range)
    if (ctrStart == 0){
        for(u32 c=0x080D7FFF; c > 0x080D7000; c--){
            if(*(u32*)c == 0x5C980){
                ctrStart = c + 0x30;
                break;
            }
        }
    }
    for(int i = 0; i < 16; i++){
        NANDCTR[i] = *((u8*)(ctrStart+(15-i))); //The CTR is stored backwards in memory.
    }


}

bool InitFS()
{
	*(u32*)0x10000020 = 0;
	*(u32*)0x10000020 = 0x340;
	return f_mount(&fs, "0:", 0) == FR_OK;
}

void DeinitFS()
{
    f_mount(NULL, "0:", 0);
}

bool FileOpen(File *Handle, const char* path, bool truncate)
{
    unsigned flags = FA_READ | FA_WRITE;
    flags |= truncate ? FA_CREATE_ALWAYS : FA_OPEN_EXISTING; //: FA_OPEN_ALWAYS;
    bool ret = (f_open(Handle, path, flags) == FR_OK);
    f_lseek(Handle, 0);
    return ret;
}

size_t FileRead(File *Handle, void* buf, size_t size, size_t foffset)
{
    UINT bytes_read = 0;
    f_lseek(Handle, foffset);
    f_read(Handle, buf, size, &bytes_read);
    return bytes_read;
}

size_t FileWrite(File *Handle, void* buf, size_t size, size_t foffset)
{
    UINT bytes_written = 0;
    f_lseek(Handle, foffset);
    f_write(Handle, buf, size, &bytes_written);
    return bytes_written;
}

size_t FileGetSize(File *Handle)
{
    return f_size(Handle);
}

void FileClose(File *Handle)
{
    f_close(Handle);
}


void GetNANDCTR(u8* ctr){
	for(int i = 0; i < 16; i++) *(ctr + i) = NANDCTR[i];
}
