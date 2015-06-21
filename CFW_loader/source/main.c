#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fs.h"
#include "hid.h"
#include "textmenu.h"
#include "platform.h"
#include "draw.h"
#include "crypto.h"

//This contains the System Firmware Version String.
char* cfw_FWString;
//This is the System Firmware Version in system.txt
char cfw_FWValue;
//This is related to the ui autoboot
bool cfw_bootGUI;
//if true, perform firmlaunch
bool firmlaunch;
//if true, the 7X key is needed to perform a firmlaunch
bool key7Xneeded;

firmHdr firm;

//variables needed for menu and settings gui
int menu_idx = 0;
int settings_idx = 0;
#define MENU_ITEMS 7
#define SETTINGS_ITEMS 1
int TOP_Current = 0;

//needed for the nand dumper
#define NAND_SIZE_O3DS 0x3AF00000
#define NAND_SIZE_N3DS 0x4D800000
#define NAND_SECTOR_SIZE 0x200
#define BUF1 0x21000000

void error(void)
{
	int a;
	DrawDebug(0, 1, "Your firm.bin is corrupted!");
	while (1)
	{
		a += 1;
		a -= 1;
	}
}

//[Unused]
//void ioDelay(u32);

// @breif  [Unused]Simply wait for a key, but not use its value.
// @note   Work like a pause.
void CFW_WaitKey(void) {
	DrawDebug(0,1,"");
	DrawDebug(0,1,"Press key to continue");
	HidWaitForInput();
}

// @breif  This reads the system version from the configuration file.
// @note   So please make sure you've have the file in your SD.
//         Also, if we use binary data to store the system version..
void CFW_getSystemVersion(void) {
	char settings[16];
	if (FSFileOpen("/3ds/PastaCFW/system.txt")){
		FSFileRead(settings, 16, 0);
		FSFileClose();
	}
	cfw_FWValue = settings[0];
	switch (settings[0]) {
	case '1': // 4.x
		cfw_FWString = platform_FWStrings[0];
		break;
	case '2': // 5.0
		cfw_FWString = platform_FWStrings[1];
		break;
	case '3': // 5.1
		cfw_FWString = platform_FWStrings[2];
		break;
	case '4': // 6.0
		cfw_FWString = platform_FWStrings[3];
		break;
	case '5': // 6.1 - 6.3
		cfw_FWString = platform_FWStrings[4];
		break;
	case '6': // 7.0-7.1
		cfw_FWString = platform_FWStrings[5];
		break;
	case '7': // 7.2
		cfw_FWString = platform_FWStrings[6];
		break;
	case '8': // 8.x
		cfw_FWString = platform_FWStrings[7];
		break;
	case '9': // 9.x
		cfw_FWString = platform_FWStrings[8];
		break;
	case 'a': // 8.x
		cfw_FWString = platform_FWStrings[9];
		break;
	case 'b': // 9.x
		cfw_FWString = platform_FWStrings[10];
		break;
	}
	//Check if to use the ARM9 Dumper
	if (settings[1] == '0' || settings[1] == '2') cfw_bootGUI = true;
}

// @breif Initialize N3DS keys
void KeyInit(void* source)
{
	int i;
	u8 firm_key[16] = { 0 };
	u8 * _source = (u8*)source;
	u8 firm_key_xored[16] = { 0xDE, 0x0E, 0x42, 0x0F, 0xE5, 0x75, 0x2C, 0xF0, 0x77, 0x4D, 0xA1, 0x87, 0x42, 0x33, 0xB9, 0xAA };
	for (i = 0; i<16; i++)
	{
		firm_key[i] = _source[i] ^ firm_key_xored[i];
	}
	setup_aeskeyX(0x16, firm_key);
}

void Key7X(void)
{
	size_t bytesRead;
	if (FSFileOpen("/slot0x25KeyX.bin")) {
		u8 slot0x25KeyX[16] = { 0 };

		bytesRead = FSFileRead(&slot0x25KeyX, 16, 0);
		FSFileClose();
		if (bytesRead != 16) {
			DrawDebug(0, 1, "slot0x25KeyX.bin is too small!");
		}
		setup_aeskeyX(0x25, slot0x25KeyX);
	}
	else {
		DrawDebug(0, 1, "You can't perform firmlaunch without slot0x25KeyX.bin");
		int a;
		while (1)
		{
			a += 1;
			a -= 1;
		}
	}
}

// @breif Copy and initialize FIRM
void PrepareFirmLaunch(void)
{
	uint32_t magic = 0x4D524946;
	size_t bytesRead;
	if (key7Xneeded)
		Key7X();
	FSFileOpen("/3ds/PastaCFW/firm.bin");
	bytesRead = FSFileRead(&firm, 0x200, 0);
	if (bytesRead != 0x200)
		error();
	if (firm.magic != magic)
		error();
	FSFileRead(firm.sect[0].p, firm.sect[0].size, firm.sect[0].offset);
	FSFileRead(firm.sect[1].p, firm.sect[1].size, firm.sect[1].offset);
	FSFileRead(firm.sect[2].p, firm.sect[2].size, firm.sect[2].offset);
	if (cfw_FWValue == 'd')
	{
		u8 key[16] = { 0 };
		FSFileRead(&key, 0x10, (firm.sect[2].offset + 0x60));
		KeyInit(key);
	}

}

// @breif  Patch the offsets to pass the signs.
void CFW_SecondStage(void) {
	//Firm launch part
	//Check if firm.bin exists
	if (FSFileOpen("/firm.bin"))
	{
		FSFileClose();
		if (Platform_CheckUnit() == PLATFORM_N3DS)
		{
			cfw_FWValue = 'd';
		}
		else
		{
			cfw_FWValue = 'c';
		}
		PrepareFirmLaunch();
		firmlaunch = 1;
	}

	u8 patchO0[] = { 0x00, 0x20, 0x3B, 0xE0 };
	u8 patchO1[] = { 0x00, 0x20, 0x08, 0xE0 };
	u8 patchN0[] = { 0x01, 0x27, 0x1E, 0xF5 };
	u8 patchN1[] = { 0xB4, 0xF9, 0xD0, 0xAB };
	u8 patchN2[] = { 0x6D, 0x20, 0xCE, 0x77 };
	u8 patchN3[] = { 0x5A, 0xC5, 0x73, 0xC1 };
	u8 patchN4[] = { 0xE7, 0x08, 0x3F, 0x22 };
	u8 patchN5[] = { 0x6F, 0x94, 0x72, 0x09 };
	//Apply patches
	DrawDebug(0, 0, "Apply patch for type %c...", cfw_FWValue);
	switch (cfw_FWValue) {
		//Old-3DS
	case '1': // 4.x
		memcpy((u32*)0x080549C4, patchO0, 4);
		memcpy((u32*)0x0804239C, patchO1, 4);
		break;
	case '2': // 5.0
		memcpy((u32*)0x08051650, patchO0, 4);
		memcpy((u32*)0x0803C838, patchO1, 4);
		break;
	case '3': // 5.1
		memcpy((u32*)0x0805164C, patchO0, 4);
		memcpy((u32*)0x0803C838, patchO1, 4);
		break;
	case '4': // 6.0
		memcpy((u32*)0x0805235C, patchO0, 4);
		memcpy((u32*)0x08057FE4, patchO1, 4);
		break;
	case '5': // 6.1 - 6.3
		memcpy((u32*)0x08051B5C, patchO0, 4);
		memcpy((u32*)0x08057FE4, patchO1, 4);
		break;
	case '6': // 7.0-7.1
		memcpy((u32*)0x080521C4, patchO0, 4);
		memcpy((u32*)0x08057E98, patchO1, 4);
		break;
	case '7': // 7.2
		memcpy((u32*)0x080521C8, patchO0, 4);
		memcpy((u32*)0x08057E9C, patchO1, 4);
		break;
	case '8': // 8.x
		memcpy((u32*)0x080523C4, patchO0, 4);
		memcpy((u32*)0x08058098, patchO1, 4);
		break;
	case '9': // 9.x
		memcpy((u32*)0x0805235C, patchO0, 4);
		memcpy((u32*)0x08058100, patchO1, 4);
		break;
		//New-3DS
	case 'a': // 8.x
		memcpy((u32*)0x08053114, patchN0, 4);
		memcpy((u32*)0x080587E0, patchN1, 4);
		break;
	case 'b': // 9.x
		memcpy((u32*)0x08052FD8, patchN2, 4);
		memcpy((u32*)0x08058804, patchN3, 4);
		break;
		//FIRM_LAUNCH
		//Old 3DS
	case 'c': // 9.6 - 9.8
		memcpy((u32*)0x0805252C, patchO0, 4);
		memcpy((u32*)0x08058094, patchO1, 4);
		break;
		//New 3DS
	case 'd': // 9.5
		memcpy((u32*)0x0805219C, patchN4, 4);
		memcpy((u32*)0x08057F50, patchN5, 4);
		break;
	}
	DrawDebug(0, 1, "Apply patch for type %c...                  Done!", cfw_FWValue);
}

void CFW_NandDumper(void){
	//Nand dumper
	unsigned char* buf = 0x21000000;
	unsigned int nsectors = 0x200;  //sectors in a row

	//Here we draw the gui
	TOP_Current = 0;
	DrawBottomSplash("/3ds/PastaCFW/UI/nand0.bin");
	u32 pad_state = HidWaitForInput();
	if (pad_state & BUTTON_A)
	{
		int PERCENTAGE = 0;
		int NAND_SIZE;
		if (cfw_FWValue == 'a' || cfw_FWValue == 'b') NAND_SIZE = NAND_SIZE_N3DS;
		else  NAND_SIZE = NAND_SIZE_O3DS;
		DrawBottomSplash("/3ds/PastaCFW/UI/nand1.bin");
		if (FSFileCreate("/NAND.bin", true))
		{
			for (int count = 0; count < NAND_SIZE / NAND_SECTOR_SIZE / nsectors; count++)
			{
				sdmmc_nand_readsectors(count*nsectors, nsectors, buf);

				FSFileWrite(buf, nsectors*NAND_SECTOR_SIZE, count*NAND_SECTOR_SIZE*nsectors);
				if ((count % (int)(NAND_SIZE / NAND_SECTOR_SIZE / nsectors / 100)) == 0 && count != 0)
				{
					char str[100];
					sprintf(str, "%d%%", PERCENTAGE);
					DrawString(SCREEN_AREA_BOT0, str, 150, 195, RGB(255, 255, 255), RGB(187, 223, 249));
					DrawString(SCREEN_AREA_BOT1, str, 150, 195, RGB(255, 255, 255), RGB(187, 223, 249));
					PERCENTAGE++;
				}
			}
			FSFileClose();
			DrawBottomSplash("/3ds/PastaCFW/UI/nand2O.bin");
		}
		else DrawBottomSplash("/3ds/PastaCFW/UI/nand2E.bin");
	}
	HidWaitForInput();
}

// @breif  Dump ARM9 Ram to file.
void CFW_ARM9Dumper(void) {
	TOP_Current = 0;
	DrawBottomSplash("/3ds/PastaCFW/UI/arm90.bin");
	u32 pad_state = HidWaitForInput();
	if (pad_state & BUTTON_A)
	{
		DrawBottomSplash("/3ds/PastaCFW/UI/arm91.bin");
		u32 bytesWritten = 0;
		u32 currentWritten = 0;
		u32 result = 0;
		u32 currentSize = 0;
		void *dumpAddr = (void*)0x08000000;
		u32 fullSize = 0x00100000;
		const u32 chunkSize = 0x10000;

		if (FSFileCreate("/3ds/PastaCFW/RAM.bin", true)) {
			while (currentWritten < fullSize) {
				currentSize = fullSize - currentWritten < chunkSize ? fullSize - currentWritten : chunkSize;
				bytesWritten = FSFileWrite((u8 *)dumpAddr + currentWritten, currentSize, currentWritten);
				if (bytesWritten != currentSize) break;
				currentWritten += bytesWritten;
				char str[100];
				sprintf(str, "%lu%%", (currentWritten * 100) / fullSize);
				DrawString(SCREEN_AREA_BOT0, str, 150, 195, RGB(255, 255, 255), RGB(187, 223, 249));
				DrawString(SCREEN_AREA_BOT1, str, 150, 195, RGB(255, 255, 255), RGB(187, 223, 249));
			}
			FSFileClose();
			result = (fullSize == currentWritten);
		}
		if(result == 1){
			DrawBottomSplash("/3ds/PastaCFW/UI/arm92OK.bin");
		}
		else{
			DrawBottomSplash("/3ds/PastaCFW/UI/arm92E.bin");
		}
		HidWaitForInput();
	}
}

void CFW_Settings(void)
{
	TOP_Current = 0;
	int settings_idx = 0;
	bool autobootgui = false;
	char settings[2];
	if (FSFileOpen("/3ds/PastaCFW/system.txt")){
		FSFileRead(settings, 16, 0);
		FSFileClose();
		if (settings[1] == '2')autobootgui = true;
	}
	while (true)
	{
		//DRAW GUI
		DrawBottomSplash("/3ds/PastaCFW/UI/options.bin");
		TOP_Current = 0;
		drawInternalY = 0;
		DrawDebug(1, 1, "");
		DrawDebug(1, 1, "");
		DrawDebug(1, 1, "");
		DrawDebug(1, 1, "");
		int i;
		char* beg;
		for (i = 0; i < SETTINGS_ITEMS; i++)
		{
			if (i == settings_idx) beg = "->";
			else beg = "  ";

			       if (i == 0)DrawSettingsDebug(1, "%s Always boot the GUI         <%s>", beg, autobootgui ? "YES" : "NO ");
			//else if (i == 1)DrawSettingsDebug(1, "%s Option 2                    <%s>", beg, option2 ? "YES" : "NO ");
		}

		//APP CONTROLS
		u32 pad_state = HidWaitForInput();
		if (pad_state & BUTTON_DOWN && settings_idx != SETTINGS_ITEMS - 1) settings_idx++; //MOVE DOWN
		else if (pad_state & BUTTON_UP && settings_idx != 0) settings_idx--; //MOVE UP
		else if (pad_state & BUTTON_LEFT || pad_state & BUTTON_RIGHT)
		{
			if (settings_idx == 0) autobootgui = !autobootgui; //autobootgui settings
		}
		else if (pad_state & BUTTON_A)
		{
			//SAVE SETTINGS
			FSFileOpen("/3ds/PastaCFW/system.txt");
			char tobewritten[2];
			tobewritten[0] = cfw_FWValue;
			tobewritten[1] = autobootgui ? '2' : '1';
			FSFileWrite(tobewritten, 2, 0);
			FSFileClose();
			break;
		}
		else if (pad_state & BUTTON_B) break; //EXIT WITHOUT SAVING 
	}
}

void CFW_Boot(void){
	//BOOT
	DrawClearScreenAll();
	DrawDebug(0,1, "--------------- PASTA CFW LOADER ---------------");
	DrawDebug(0,1, "");
	DrawDebug(0,1, "Your system is %s", cfw_FWString);
	DrawDebug(0,1, "");
	CFW_SecondStage();
}

// @breif FirmLaunch!
void FirmLaunch(void)
{
	//Prepare ARM11
	*(uint32_t *)0x1FFFFFF8 = *(uint32_t *)(firm.arm11Ent);
	//Jump
	((void(*)())firm.arm9Ent)();
}

// @breif  Main routine surely.
int main(void) {

	FSInit();
	CFW_getSystemVersion();	
	if (cfw_bootGUI==true) //If gui autoboot is enabled or L is held, show the ui //L held not working
	{
		DrawClearScreenAll();
		//MENU
		while (true)
		{
			//DRAW GUI
			if (menu_idx == MENU_ITEMS - 1)
			{
				DrawTopSplash("/3ds/PastaCFW/UI/creditsTOP.bin");
				DrawBottomSplash("/3ds/PastaCFW/UI/menu6.bin");
				TOP_Current = 0;
			}
			else
			{
				char path[] = "/3ds/PastaCFW/UI/menu0.bin";
				path[21] = menu_idx + 48;
				DrawBottomSplash(path); //BOTTOM SCREEN
				if (TOP_Current == 0){
					DrawTopSplash("/3ds/PastaCFW/UI/menuTOP.bin"); //TOP SCREEN
					TOP_Current = 1;
				}
			}
			//MENU CONTROLS
			u32 pad_state = HidWaitForInput();
			if (pad_state & BUTTON_RIGHT && menu_idx != MENU_ITEMS - 1) menu_idx++; //MOVE RIGHT
			else if (pad_state & BUTTON_LEFT && menu_idx != 0)menu_idx--; //MOVE LEFT
			else if (pad_state & BUTTON_A)//SELECT
			{
				if (menu_idx == 0){ CFW_Boot(); break; }//BOOT CFW
				else if (menu_idx == 1)break; //REBOOT
				else if (menu_idx == 2)CFW_NandDumper(); //NAND DUMPER
				else if (menu_idx == 4)CFW_ARM9Dumper(); //ARM9 RAM DUMPER
				else if (menu_idx == 5)CFW_Settings(); //SETTINGS
			}
		}
	}
	else CFW_Boot(); //else directly boot the cfw
	if (firmlaunch == true) FirmLaunch();

	// return control to FIRM ARM9 code (performs firmlaunch)
	return 0;
}
