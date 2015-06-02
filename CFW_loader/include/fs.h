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
