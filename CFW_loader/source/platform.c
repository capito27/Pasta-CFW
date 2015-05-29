#include "common.h"
#include "platform.h"

#define PLATFORM_REG ((volatile u32*)0x10140FFC)

// @breif  Determine platform of the console.
// @retval PLATFORM_N3DS for New3DS, and PLATFORM_3DS for Old3DS.
// @note   Maybe modified to support more platforms
Platform GetUnitPlatform(void) {
    switch (*PLATFORM_REG) {
        case 7:
            return PLATFORM_N3DS;
        default:
            return PLATFORM_3DS;
    }
}
