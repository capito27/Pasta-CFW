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

//variables
char* systemVersion;
int type;
u8 isN3DS = 0;
bool showcredits;
//For clock
#define SECONDS_IN_DAY 86400
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MINUTE 60


s32 quick_boot_firm(s32 load_from_disk) {
	if (load_from_disk)
		load_arm9_payload("/3ds/PastaCFW/loader.bin");
	firm_reboot();
}

void getSystemVersion()
{
	//FIRSTLY, CHECK NEW 3DS
	APT_CheckNew3DS(NULL, &isN3DS);
	if (isN3DS) {
		type = 4;
	}
	else
	{
		//Now we really check system version, based off kversion
		unsigned int kversion = *(unsigned int *)0x1FF80000;
		if (kversion == 0x02220000) {
			type = 1;
		}
		else if (kversion == 0x022C0600){
			type = 2;
		}
		else if (kversion == 0x022E0000) {
			type = 3;
		}
		else {
			type = 0;
		}
	}
	
	//Then we save the detected type to a txt file
	FILE *f = fopen("/3ds/PastaCFW/system.txt", "w");
	fprintf(f, "%d", type);
	fclose(f);

	//And also we save it to a char*
	switch (type)
	{
	case 0: //Unsupported
		systemVersion = "unsupported";
		break;

	case 1:
		systemVersion = "Old 3DS V. 4.1 - 4.5";
		break;

	case 2:
		systemVersion = "Old 3DS V. 8.0 - 8.1";
		break;

	case 3:
		systemVersion = "Old 3DS V. 9.0-9.2";
		break;

	case 4:
		systemVersion = "New 3DS V. 9.0 - 9.2";
		break;
	}
}

void bootCFW_FirstStage()
{
	//Load arm9 payload
	brahma_init();
	quick_boot_firm(1);
	brahma_exit();
}

void drawUI()
{
	//UI DRAWING CODE
	char buffer[100];
	//-------------- TOP --------------
	//WALLPAPER
	gfxDrawSprite(GFX_TOP, GFX_LEFT, (u8*)TOP_bin, 240, 400, 0, 0);
	//STATUSBAR
	drawFillRect(0, 0, 399, 17, 128, 128, 128, screenTopLeft);
	sprintf(buffer, "Pasta CFW Loader                  %s", systemVersion);
	gfxDrawText(GFX_TOP, GFX_LEFT, NULL, buffer, 5, 238 - fontDefault.height * 1);
    //CLOCK
	u64 timeInSeconds = osGetTime() / 1000;
	u64 dayTime = timeInSeconds % SECONDS_IN_DAY;
	sprintf(buffer, "%llu:%llu:%llu", dayTime / SECONDS_IN_HOUR, (dayTime % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE, dayTime % SECONDS_IN_MINUTE);
	gfxDrawText(GFX_TOP, GFX_LEFT, NULL, buffer, 350, 238 - fontDefault.height * 1);

	//------------- BOTTOM ------------
	//Prints a background!
	drawFillRect(0, 0, 319, 239, 128, 128, 128, screenBottom);

	//Prints the buttons
	drawFillRect(14, 6, 92, 33, 64, 64, 64, screenBottom); //EXIT
	drawFillRect(220, 6, 303, 33, 64, 64, 64, screenBottom); //CREDITS
	drawFillRect(14, 149, 302, 212, 255, 0, 0, screenBottom); //REBOOT 

	//Prints the text
	sprintf(buffer, "    Exit");
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, buffer, 30, 245 - fontDefault.height * 2);

	sprintf(buffer, "  Credits");
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, buffer, 235, 245 - fontDefault.height * 2);

	if (type != 0) //Draws "boot" button only if the system is supported
	{
		drawFillRect(14, 68, 302, 131, 0, 204, 0, screenBottom);
		sprintf(buffer, "      BOOT CFW");
		gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, buffer, 108, 240 - fontDefault.height * 6);
		sprintf(buffer, "Apply patches and reboot!");
		gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, buffer, 88, 240 - fontDefault.height * 7);
	}

	sprintf(buffer, "     REBOOT 3DS");
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, buffer, 110, 240 - fontDefault.height * 11);

	sprintf(buffer, "Reboot without applying any patch");
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, buffer, 65, 240 - fontDefault.height * 12);

	//CREDITS!
	if (showcredits)
	{
		//DRAWS A BIG RECT AND WRITES CREDITS INSIDE IT
		drawFillRect(40, 50, 360, 210, 64, 64, 64, screenTopLeft);
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "CREDITS:", 180, 230 - fontDefault.height * 4);
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "-", 50, 230 - fontDefault.height * 5);
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "-", 50, 230 - fontDefault.height * 6);
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "-", 50, 230 - fontDefault.height * 7);
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "-", 50, 230 - fontDefault.height * 8);
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "-", 50, 230 - fontDefault.height * 9);
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "-", 50, 230 - fontDefault.height * 10);
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "-", 50, 230 - fontDefault.height * 11);
		gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "-", 50, 230 - fontDefault.height * 12);
	}
}

void guiPopup(char* title, char* line1, char* line2, char* line3)
{
	//Prints a dark grey rectangle!
	drawFillRect(36, 60, 272, 85, 128, 128, 128, screenBottom);
	//Prints a light grey rectangle!
	drawFillRect(36, 85, 272, 189, 160, 160, 160, screenBottom);
	//Prints text
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, title, 124, 240 - fontDefault.height * 5);
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, line1, 50, 245 - fontDefault.height * 7);
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, line2, 50, 245 - fontDefault.height * 8);
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, NULL, line3, 50, 245 - fontDefault.height * 9);
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

    //Check kernel version first
	getSystemVersion();

	// If L is held, show UI 
	if (kHeld & KEY_L)
	{
		// Main loop
		while (aptMainLoop())
		{
			//As nop90 suggested
			getFB();

			//Gets input (keys and touch)
			getInput();

			//Prints the GUI
			drawUI();

			//Do stuff
			if ((posX >= 14 && posX <= 302) && (posY >= 68 && posY <= 131))
			{
				bootCFW_FirstStage(); //Boot CFW
			}
			else if ((posX >= 14 && posX <= 302) && (posY >= 149 && posY <= 212))
			{
				//Reboot Code
				aptOpenSession();
				APT_HardwareResetAsync(NULL);
				aptCloseSession();
			}
			else if ((posX >= 6 && posX <= 92) && (posY >= 6 && posY <= 33)) break;

			if ((posX >= 220 && posX <= 303) && (posY >= 6 && posY <= 33))
			{
				if (showcredits)showcredits = false;
				else showcredits = true;
			}

			// Flush and swap framebuffers
			gfxFlushBuffers();
			gfxSwapBuffers();

			//Wait for VBlank
			gspWaitForVBlank();
		}
	}
	else
	{
		//QUICK BOOT!
		bootCFW_FirstStage();
	}

	// Exit services
	hbExit();
	sdmcExit();
	fsExit();
	gfxExit();
	hidExit();
	aptExit();
	srvExit();

	// Return to hbmenu
	return 0;
	}
