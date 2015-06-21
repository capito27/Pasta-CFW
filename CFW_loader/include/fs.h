#pragma once

#include "common.h"

bool FSInit();
void FSDeInit();

/** Opens existing files */
bool FSFileOpen(const char* path);

/** Opens new files (and creates them if they don't already exist) */
bool FSFileCreate(const char* path, bool truncate);

/** Reads contents of the opened file */
size_t FSFileRead(void* buf, size_t size, size_t foffset);

/** Writes to the opened file */
size_t FSFileWrite(void* buf, size_t size, size_t foffset);

/** Gets the size of the opened file */
size_t FSFileGetSize();

void FSFileClose();

enum{
	TWLN = 0x00012E00,
	TWLP = 0x09011A00,
	AGB_SAVE = 0x0B100000,
	FIRM0 = 0x0B130000,
	FIRM1 = 0x0B530000,
	CTRNAND = 0x0B95CA00,
}NAND_PARTITIONS;

typedef struct{
	u8* buffer;
	u8* keyY;
	u8* ctr;
	size_t size;
	u32 keyslot;
}PartitionInfo;  //This basic struct can be useful anytime, even if i'll add nand decryption/exploring

u32 DecryptPartition(PartitionInfo* info);
void GetNANDCTR(u8* ctr);
int nand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
int nand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
int emunand_readsectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);
int emunand_writesectors(uint32_t sector_no, uint32_t numsectors, uint8_t *out, unsigned int partition);