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
char type;
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
	case '1': // 4.x
		type = '1';
		systemVersion = "Old 3DS V. 4.1 - 4.5";
		break;
	case '2': // 5.0
		type = '2';
		systemVersion = "Old 3DS V. 5.0";
		break;
	case '3': // 5.1
		type = '3';
		systemVersion = "Old 3DS V. 5.1"; 
		break;
	case '4': // 6.0
		type = '4';
		systemVersion = "Old 3DS V. 6.0";
		break;
	case '5': // 6.1 - 6.3
		type = '5';
		systemVersion = "Old 3DS V. 6.1 - 6.3";
		break;
	case '6': // 7.0-7.1
		type = '6';
		systemVersion = "Old 3DS V. 7.0 - 7.1";
		break;
	case '7': // 7.2
		type = '7';
		systemVersion = "Old 3DS V. 7.2";
		break;
	case '8': // 8.x
		type = '8';
		systemVersion = "Old 3DS V. 8.0 - 8.1";
		break;
	case '9': // 9.x
		type = '9';
		systemVersion = "Old 3DS V. 9.0 - 9.2";
		break;
	case 'a': // 8.x
		type = 'a';
		systemVersion = "New 3DS V. 8.1";
		break;
	case 'b': // 9.x
		type = 'b';
		systemVersion = "New 3DS V. 9.0 - 9.2";
		break;
	}
}

void bootCFW_SecondStage()
{
	//Apply patches
	Debug("Apply patch for type %c...", type);
	if (type == '1'){ // 4.x
		u32 *dest = 0x080549C4;
		u32 *dest1 = 0x0804239C;
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '2'){ // 5.0
		u32 *dest = 0x08051650;
		u32 *dest1 = 0x0803C838;
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '3'){ // 5.1
		u32 *dest = 0x0805164C;
		u32 *dest1 = 0x0803C838;
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '4'){ // 6.0
		u32 *dest = 0x0805235C;
		u32 *dest1 = 0x0803D774;
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '5'){ // 6.1 - 6.3
		u32 *	dest = 0x08051B5C;
		u32 *	dest1 = 0x0803CF74;
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '6'){ // 7.0-7.1
		u32 *dest = 0x080521C4;
		u32 *dest1 = 0x0803D568;
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '7'){ // 7.2
		u32 *dest = 0x080521C8;
		u32 *dest1 = 0x0803D56C;
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '8'){ // 8.x
		u32 *dest = 0x080523C4;
		u32 *dest1 = 0x08058098;
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '9'){ // 9.x
		u32 *dest = 0x0805235C;
		u32 *dest1 = 0x08058100;
		u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
		u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == 'a'){ // 8.x
		u32 *dest = 0x08053114;
		u32 *dest1 = 0x080587E0;
		u8 patch[] = { 0x01, 0x27, 0x1E, 0xF5 };
		u8 patch1[] = { 0xB4, 0xF9, 0xD0, 0xAB };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == 'b'){ // 9.x
		u32 *dest = 0x08052FD8;
		u32 *dest1 = 0x08058804;
		u8 patch[] = { 0x6D, 0x20, 0xCE, 0x77 };
		u8 patch1[] = { 0x5A, 0xC5, 0x73, 0xC1 };
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}

	Debug("Done!");
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
	bootCFW_SecondStage();

	// return control to FIRM ARM9 code (performs firmlaunch)
	return 0;
}
