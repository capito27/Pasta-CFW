#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include "brahma.h"
#include "draw.h"
#include "input.h"
#include "rendering.h"
#include "hid.h"
#include "TOP_bin.h"
#include "BOT_bin.h"
#include "BOT_REBOOT_bin.h"
#include "BOT_EXIT_bin.h"
#include "BOT_BOOT_bin.h"
#include "BOT_CREDITS_bin.h"

//variables
char* systemVersion;
bool showcredits;

//Config values
char type = '0';
char auto_boot = '0';
char arm9_dumper = '0';

//For clock
#define SECONDS_IN_DAY 86400
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MINUTE 60

//Boot command
s32 quick_boot_firm(s32 load_from_disk) {
	if (load_from_disk)
		load_arm9_payload("/3ds/PastaCFW/loader.bin");
	firm_reboot();
}

void loadConfiguration() //Here we load the system.txt file, so that we can get needed values. 
{
	char buffer []= "010";
	if (fopen("/3ds/PastaCFW/system.txt", "r")) //If the file exists, read it. If it doesn't exists, then it gets created with saveConfiguration() function
	{
		FILE* cfg;
		cfg = fopen("/3ds/PastaCFW/system.txt", "r");
		fseek(cfg, SEEK_SET, 0);
		fread(buffer, 4, 1, cfg); //Reads the file
		fclose(cfg);
	}
	auto_boot = buffer[1]; //we read the values
}

void saveConfiguration()
{
	//We save the configuration file, now it includes the detected firmware type
	char buffer[]="01";
	buffer[0] = type;
	buffer[1] = auto_boot;
	FILE *f = fopen("/3ds/PastaCFW/system.txt", "w+");
	fprintf(f, "%s", buffer);
	fclose(f);
}

//Gets system version from kernel and writes it to system.txt
void getSystemVersion()
{
	//FIRSTLY, CHECK NEW 3DS
	u8 isN3DS = 0;
	APT_CheckNew3DS(NULL, &isN3DS);
    unsigned int kversion = *(unsigned int *)0x1FF80000;
	if (!isN3DS || kversion < 0x022C0600)
	{
		//-----------> OLD 3DS
		switch (kversion)
		{
		default:         // Unsupported
			type = '0';
			systemVersion = "unsupported";
			break;
		case 0x02220000: // 4.x
			type = '1';
			systemVersion = "Old 3DS V. 4.1 - 4.5";
			break;
		case 0x02230600: // 5.0
			type = '2';
			systemVersion = "Old 3DS V. 5.0";
			break;
		case 0x02240000: // 5.1
			type = '3';
			systemVersion = "Old 3DS V. 5.1";
			break;
		case 0x02250000: // 6.0
			type = '4';
			systemVersion = "Old 3DS V. 6.0";
			break;
		case 0x02260000: // 6.1
			type = '5';
			systemVersion = "Old 3DS V. 6.1 - 6.3";
			break;
		case 0x02270400: // 7.0-7.1
			type = '6';
			systemVersion = "Old 3DS V. 7.0 - 7.1";
			break;
		case 0x02280000: // 7.2
			type = '7';
			systemVersion = "Old 3DS V. 7.2";
			break;
		case 0x022C0600: // 8.x
			type = '8';
			systemVersion = "Old 3DS V. 8.0 - 8.1";
			break;
		case 0x022E0000: // 9.x
			type = '9';
			systemVersion = "Old 3DS V. 9.0 - 9.2";
			break;
		}
	}
	else
	{
		//-----------> NEW 3DS
		switch (kversion)
		{
		case 0x022C0600: // 8.x
			type = 'a';
			systemVersion = "New 3DS V. 8.1";
			break;
		case 0x022E0000: // 9.x
			type = 'b';
			systemVersion = "New 3DS V. 9.0 - 9.2";
			break;
		default:         // Unsupported
			type = '0';
			systemVersion = "unsupported";
			break;
		}
	}
}

void bootCFW_FirstStage()
{
	//Load arm9 payload
	brahma_init();
	quick_boot_firm(1); //Boots the payload
	brahma_exit();
}

void updateFB()
{
	// Flush and swap framebuffers
	gfxFlushBuffers();
	gfxSwapBuffers();

	//Wait for VBlank
	gspWaitForVBlank();
}

int main() {
	// Initialize services
	srvInit();
	aptInit();
	hidInit(NULL);
	gfxInitDefault();
	fsInit();
	sdmcInit();
	hbInit();
	qtmInit();
	hidScanInput();
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();

	//Load the configuration
	loadConfiguration();

    //Check kernel version
	getSystemVersion();
	
	//checks if the CFW has to boot right away or open the GUI
	if(auto_boot == '2') auto_boot = '2'; //here it checks if the "force GUI" is set, and preservs it if it is set
	else if(kHeld & KEY_L) auto_boot = '0'; //here it will start the GUI only this time
	else auto_boot = '1'; //here it won't show the GUI
	
	//Then we save the configuration
	saveConfiguration();

	//Proceeds to launch the loader.bin
	bootCFW_FirstStage();

	}
