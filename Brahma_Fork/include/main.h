#ifndef MAIN_H
#define MAIN_H


int main();

s32 quick_boot_firm(s32 load_from_disk);
void getSystemVersion();
void bootCFW_FirstStage();
void drawUI();
void guiPopup(char* title, char* line1, char* line2, char* line3);

#endif
