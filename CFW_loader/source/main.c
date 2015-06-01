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
bool dump_arm9;

void wait_key() {
	DrawDebug(1,"");
	DrawDebug(1,"Press key to continue");
	InputWait();
}

void getSystemVersion()
{
	char settings[16];
	if (FSFileOpen("/3ds/PastaCFW/system.txt")){
		FSFileRead(settings, 16, 0);
		FSFileClose();
	}

	switch (settings[0])
	{
	case '1': // 4.x
		type = '1';
		systemVersion = platform_FWStrings[0];
		break;
	case '2': // 5.0
		type = '2';
		systemVersion = platform_FWStrings[1];
		break;
	case '3': // 5.1
		type = '3';
		systemVersion = platform_FWStrings[2];
		break;
	case '4': // 6.0
		type = '4';
		systemVersion = platform_FWStrings[3];
		break;
	case '5': // 6.1 - 6.3
		type = '5';
		systemVersion = platform_FWStrings[4];
		break;
	case '6': // 7.0-7.1
		type = '6';
		systemVersion = platform_FWStrings[5];
		break;
	case '7': // 7.2
		type = '7';
		systemVersion = platform_FWStrings[6];
		break;
	case '8': // 8.x
		type = '8';
		systemVersion = platform_FWStrings[7];
		break;
	case '9': // 9.x
		type = '9';
		systemVersion = platform_FWStrings[8];
		break;
	case 'a': // 8.x
		type = 'a';
		systemVersion = platform_FWStrings[9];
		break;
	case 'b': // 9.x
		type = 'b';
		systemVersion = platform_FWStrings[10];
		break;
	}

	//Check if to use the ARM9 Dumper
	if (settings[2] == '1') dump_arm9 = true;
}

void bootCFW_SecondStage()
{
	u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
	u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
	u8 patch2[] = { 0x01, 0x27, 0x1E, 0xF5 };
	u8 patch3[] = { 0xB4, 0xF9, 0xD0, 0xAB };
	u8 patch4[] = { 0x6D, 0x20, 0xCE, 0x77 };
	u8 patch5[] = { 0x5A, 0xC5, 0x73, 0xC1 };
	//Apply patches
	DrawDebug(0,"Apply patch for type %c...", type);
	if (type == '1'){ // 4.x
		memcpy((u32*)0x080549C4, patch, 4);
		memcpy((u32*)0x0804239C, patch1, 4);
	}
	else if (type == '2'){ // 5.0
		memcpy((u32*)0x08051650, patch, 4);
		memcpy((u32*)0x0803C838, patch1, 4);
	}
	else if (type == '3'){ // 5.1
		memcpy((u32*)0x0805164C, patch, 4);
		memcpy((u32*)0x0803C838, patch1, 4);
	}
	else if (type == '4'){ // 6.0
		memcpy((u32*)0x0805235C, patch, 4);
		memcpy((u32*)0x08057FE4, patch1, 4);
	}
	else if (type == '5'){ // 6.1 - 6.3
		memcpy((u32*)0x08051B5C, patch, 4);
		memcpy((u32*)0x08057FE4, patch1, 4);
	}
	else if (type == '6'){ // 7.0-7.1
		memcpy((u32*)0x080521C4, patch, 4);
		memcpy((u32*)0x08057E98, patch1, 4);
	}
	else if (type == '7'){ // 7.2
		memcpy((u32*)0x080521C8, patch, 4);
		memcpy((u32*)0x08057E9C, patch1, 4);
	}
	else if (type == '8'){ // 8.x
		memcpy((u32*)0x080523C4, patch, 4);
		memcpy((u32*)0x08058098, patch1, 4);
	}
	else if (type == '9'){ // 9.x
		memcpy((u32*)0x0805235C, patch, 4);
		memcpy((u32*)0x08058100, patch1, 4);
	}
	else if (type == 'a'){ // 8.x
		memcpy((u32*)0x08053114, patch2, 4);
		memcpy((u32*)0x080587E0, patch3, 4);
	}
	else if (type == 'b'){ // 9.x
		memcpy((u32*)0x08052FD8, patch4, 4);
		memcpy((u32*)0x08058804, patch5, 4);
	}
	DrawDebug(1,"Apply patch for type %c...                  Done!", type);
}

void arm9dumper()
{
	DrawDebug(1,"");
	DrawDebug(1,"");
	DrawDebug(1,"");
	DrawDebug(1,"---------------- ARM9 RAM DUMPER ---------------");
	DrawDebug(1,"");
	DrawDebug(0,"Press A to DUMP, B to skip.");

	u32 pad_state = InputWait();


	if (pad_state & BUTTON_B)DrawDebug(1,"Skipping...");
	else
	{
		u32 written = 0;
		u32 total = 0;
		u32 result = 0;
		u32 num = 0;
		void *addr = (void*)0x08000000;
		u32 size = 0x00100000;
		const u32 sz_chunk = 0x10000;

		if (FSFileCreate("/3ds/PastaCFW/RAM.bin", true)) {
			while (total < size) {
				num = size - total < sz_chunk ? size - total : sz_chunk;
				written = FSFileWrite((u8 *)addr + total, num, total);
				if (written != num) break;
				total += written;
				DrawDebug(0,"Dumping:                         %07d/%d", total, size);
			}
			FSFileClose();
			result = (size == total);
		}
		DrawDebug(1,"");
		DrawDebug(1,"");
		DrawDebug(1,"Dump %s! Press any key to boot CFW.", result ? "finished" : "failed");
		InputWait();
	}
}

int main() {

	//BOOT
	DrawClearScreenAll();
	DrawDebug(1,"--------------- PASTA CFW LOADER ---------------");
	DrawDebug(1,"");
	DrawDebug(0,"Initializing FS...");
	FSInit();
	DrawDebug(1,"Initializing FS...                         Done!");
	DrawDebug(1,"");
	DrawDebug(0,"Getting system information...");
	getSystemVersion();
	DrawDebug(1,"Getting system information...              Done!");
	DrawDebug(1,"");
	DrawDebug(1,"Your system is %s", systemVersion);
	DrawDebug(1,"");
	bootCFW_SecondStage();
	if (dump_arm9 == true)arm9dumper();

	// return control to FIRM ARM9 code (performs firmlaunch)
	return 0;
}

