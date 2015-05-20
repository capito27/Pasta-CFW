#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fs.h"
#include "hid.h"
#include "textmenu.h"
#include "platform.h"
#include "draw.h"

//VARIABLES
char* systemVersion;
int type;
void ioDelay(u32);

void ClearTop() {
	ClearScreen(TOP_SCREEN0, RGB(0, 0, 0));
	ClearScreen(TOP_SCREEN1, RGB(0, 0, 0));
	ClearScreen(BOT_SCREEN0, RGB(0, 0, 0));
	ClearScreen(BOT_SCREEN1, RGB(0, 0, 0));
	current_y = 0;
}

void wait_key() {
	Debug("");
	Debug("Press key to continue");
	InputWait();
}

void getSystemVersion()
{
	File VersionFile;
	char sysver[16];
	if (FileOpen(&VersionFile, "/3ds/PastaCFW/system.txt", 0)){
		FileRead(&VersionFile, sysver, 16, 0);
		FileClose(&VersionFile);
	}
	switch (sysver[0])
	{
	case '0': //Unsupported
		systemVersion = "unsupported"; type = 0;
		break;

	case '1':
		systemVersion = "Old 3DS V. 4.1 - 4.5";
		type = 1;
		break;

	case '2':
		systemVersion = "Old 3DS V. 8.0 - 8.1";
		type = 2;
		break;

	case '3':
		systemVersion = "Old 3DS V. 9.0-9.2";
		type = 3;
		break;

	case '4':
		systemVersion = "New 3DS V. 9.0 - 9.2";
		type = 4;
		break;
	}
}

void bootCFW_SecondStage(int type)
{
	Debug("Apply patch for type %d...", type);
	if (type == 1){
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u32 *dest = 0x080549C4;
		memcpy(dest, patch, 4);
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		u32 *dest1 = 0x0804239C;
		memcpy(dest1, patch1, 4);
	}
	if (type == 2){
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u32 *dest = 0x080523C4;
		memcpy(dest, patch, 4);
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		u32 *dest1 = 0x08058098;
		memcpy(dest1, patch1, 4);
	}
	if (type == 3){
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u32 *dest = 0x0805235C;
		memcpy(dest, patch, 4);
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		u32 *dest1 = 0x08058100;
		memcpy(dest1, patch1, 4);
	}
	if (type == 4){
		u8 patch[] = { 0x6D, 0x20, 0xCE, 0x77 };
		u32 *dest = 0x08052FD8;
		memcpy(dest, patch, 4);
		u8 patch1[] = { 0x5A, 0xC5, 0x73, 0xC1 };
		u32 *dest1 = 0x08058804;
		memcpy(dest1, patch1, 4);
	}
}

int main() {

	//BOOT
	ClearTop();
	Debug("PASTA CFW LOADER");
	Debug("");
	Debug("Initializing FS");
	InitFS();
	Debug("");
	Debug("Ready!");
	Debug("");
	Debug("Getting system information...");
	Debug("");
	getSystemVersion();
	Debug("Your system is %s", systemVersion);
	Debug("");
	bootCFW_SecondStage(type);
	Debug("Done!");

	// return control to FIRM ARM9 code (performs firmlaunch)
	return 0;
}