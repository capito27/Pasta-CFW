#pragma once

#include "common.h"
#include "ff.h"
#define File FIL

enum{
	TWL0 = 0x00000000,
	TWLN = 0x00012E00,
	TWLP = 0x09011A00,
	AGB_SAVE = 0x0B100000,
	FIRM0 = 0x0B130000,
	FIRM1 = 0x0B530000,
	CTRNAND = 0x0B95CA00,
}NAND_PARTITIONS;

bool InitFS();
void DeinitFS();

bool FileOpen(File* Handle, const char* path, bool truncate);
size_t FileRead(File* Handle, void* buf, size_t size, size_t foffset);
size_t FileWrite(File* Handle, void* buf, size_t size, size_t foffset);
size_t FileGetSize(File* Handle);
void FileClose(File* Handle);

void GetNANDCTR(u8* ctr);
