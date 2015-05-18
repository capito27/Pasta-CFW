#include "common.h"
#include "platform.h"

#define CONFIG_PLATFORM_REG ((volatile u32*)0x10140FFC)

Platform GetUnitPlatform()
{
    switch (*CONFIG_PLATFORM_REG) {
        case 7:
            return PLATFORM_N3DS;
        case 1:
        default:
            return PLATFORM_3DS;
    }

}
	
	//---------------------------------------------------------------------------------
u32 osGetFirmVersion() {
//---------------------------------------------------------------------------------
return (*(u32*)0x1FF80000) & ~0xFF;
}

//---------------------------------------------------------------------------------
u32 osGetKernelVersion() {
//---------------------------------------------------------------------------------
return (*(u32*)0x1FF80060) & ~0xFF;
}
