#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "draw.h"
#include "hid.h"
#include "textmenu.h"

void ioDelay(u32);

void ClearTop() {
	ClearScreen(TOP_SCREEN0, RGB(255, 255, 255));
	ClearScreen(TOP_SCREEN1, RGB(255, 255, 255));
	current_y = 0;
}

void newline(int count) {
	int i;
	for(i=0; i<count; i++)
		Debug("");
}

void wait_key() {
	Debug("");
	Debug("Press key to continue");
	InputWait();
}

int who_am_i(int idx, void *param) {
	newline(1);		
	Debug("Hi, from menu item %d", idx+1);
	if (param)
		Debug("Number of Tetris games played %d", *(u32 *)param);
	return 0;
}

int fake_tetris(int idx, void *param) {
	if (param)
		(*(u32 *)param)++;
	int i;
	for(i=0; i<12; i++) {
		ClearTop();
		newline(i);
		Debug(" _ _");
		Debug("|_|_|");
		Debug("  |_|");
		Debug("  |_|");
		ioDelay(100000);
	}	
	ClearTop();
	newline(1);
	Debug("         GAME OVER        :-D");
	
	return 0;
}

int print_menu (int idx, struct menu_t *menu) {
	int i;
	int newidx;
	int count = menu_get_element_count(menu);
	
	newidx = menu_update_index(idx, menu);

	for (i=0; i<count; i++) {
		if (newidx == i)
			Debug("[ %s ]", menu_get_element_name(i, menu));
		else
			Debug("  %s  ", menu_get_element_name(i, menu));
	}
	
	return newidx;	
}

int print_main_menu(int idx, struct menu_t *menu) {
	int newidx = 0;

	newline(1);
	Debug("Pasta CFW loader by capito27 V 0.1.3");
	newline(2);
	Debug("===========================");
	newidx = print_menu(idx, menu);
	Debug("===========================");	
	newline(1);
	Debug("START: Boot selected custom firmware");
	Debug("B: Reboot console without entering the CFW");
	newline(2);
	Debug("Credits :");
	Debug("- Me (Capito27) for the loader menu.");
	Debug("- Patois for the Brahma solution.");
	Debug("- motezazer for the N3DS offsets.");
	Debug("- Unknown for the O3DS offsets. and the method.");
	
	
	return newidx;
}

struct menu_t main_menu = {
	4,
	{
		{"Old 3DS V. 4.1-4.5 CFW.", &fake_tetris},
		{"Old 3DS V. 8.0-8.1 CFW.", &who_am_i},
		{"Old 3DS V. 9.0-9.2 CFW.", &who_am_i},
		{"New 3DS V. 9.0-9.2 CFW.", &who_am_i}
	}
};

int main() {
	u32 pad_state;
	int menuidx = 0;
	u32 num_calls = 0;

	while (true) {
		ClearTop();

		menuidx = print_main_menu(menuidx, &main_menu);		
		pad_state = InputWait();

		if (pad_state & BUTTON_START) {
			if(menuidx == 0){
				u8 patch[]={0x00, 0x20, 0x3B, 0xE0};
				u32 *dest=0x080549C4;
				memcpy(dest,patch,4);
				u8 patch1[]={0x00, 0x20, 0x08, 0xE0};
				u32 *dest1=0x0804239C;
				memcpy(dest1,patch1,4);
			}
			if(menuidx == 1){
				u8 patch[]={0x00, 0x20, 0x3B, 0xE0};
				u32 *dest=0x080523C4;
				memcpy(dest,patch,4);
				u8 patch1[]={0x00, 0x20, 0x08, 0xE0};
				u32 *dest1=0x08058098;
				memcpy(dest1,patch1,4);
			}
			if(menuidx == 2){
				u8 patch[]={0x00, 0x20, 0x3B, 0xE0};
				u32 *dest=0x0805235C;
				memcpy(dest,patch,4);
				u8 patch1[]={0x00, 0x20, 0x08, 0xE0};
				u32 *dest1=0x08058100;
				memcpy(dest1,patch1,4);
			}
			if(menuidx == 3){
				u8 patch[]={0x6D, 0x20, 0xCE, 0x77};
				u32 *dest=0x08052FD8;
				memcpy(dest,patch,4);
				u8 patch1[]={0x5A, 0xC5, 0x73, 0xC1};
				u32 *dest1=0x08058804;
				memcpy(dest1,patch1,4);
			}
			break;
		}
		else if (pad_state & BUTTON_B){
			break;
		}
		else if (menuidx == 999) {
			ClearTop();
			menu_execute_function(menuidx, &main_menu, &num_calls);
			wait_key();
		}
		else if (pad_state & BUTTON_UP) {
			menuidx--;
		}
		else if (pad_state & BUTTON_DOWN) {
			menuidx++;
		}
	}

	// return control to FIRM ARM9 code (performs firmlaunch)
	return 0;
}