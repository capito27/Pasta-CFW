#pragma once

typedef enum {
    PLATFORM_3DS,
    PLATFORM_N3DS,
} Platform_UnitType;

//[Unused] Generate the version value for the actual version.
#define SYSTEM_VERSION(major, minor, revision) \
	(((major)<<24)|((minor)<<16)|((revision)<<8))

extern char *platform_FWStrings[];

Platform_UnitType Platform_CheckUnit(void);
//Un-implemented
//u32 osGetFirmVersion();
//u32 osGetKernelVersion();
