#pragma once

typedef enum {
    PLATFORM_3DS,   //Old 3ds
    PLATFORM_N3DS,  //New 3ds
} Platform_UnitType;

typedef struct {
    Platform_UnitType console;
    u32 firmware;
} Platform_t;

//[Unused] Generate the version value for the actual version.
#define SYSTEM_VERSION(major, minor, revision) \
	(((major)<<24)|((minor)<<16)|((revision)<<8))

extern char *platform_FWStrings[];

Platform_UnitType Platform_CheckUnit();
//Un-implemented
//u32 osGetFirmVersion();
//u32 osGetKernelVersion();
