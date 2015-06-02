#include "common.h"
#include "platform.h"

#define PLATFORM_REG ((volatile u32*)0x10140FFC)

// Strings of All supported firmware versions, currently.
char *platform_FWStrings[] = {
    "Old 3DS V. 4.1 - 4.5",
    "Old 3DS V. 5.0",
    "Old 3DS V. 5.1",
    "Old 3DS V. 6.0",
    "Old 3DS V. 6.1 - 6.3",
    "Old 3DS V. 7.0 - 7.1",
    "Old 3DS V. 7.2",
    "Old 3DS V. 8.0 - 8.1",
    "Old 3DS V. 9.0 - 9.2",
    "New 3DS V. 8.1",
    "New 3DS V. 9.0 - 9.2"
};

// @breif  [Unused] Determine platform of the console.
// @retval PLATFORM_N3DS for New3DS, and PLATFORM_3DS for Old3DS.
// @note   Maybe modified to support more platforms
Platform_UnitType Platform_CheckUnit(void) {
    switch (*PLATFORM_REG) {
        case 7:
            return PLATFORM_N3DS;
        case 1:
        default:
            return PLATFORM_3DS;
    }
}
