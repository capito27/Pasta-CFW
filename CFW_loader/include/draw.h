#pragma once

#include "common.h"

//It used 24 bits color, so 1 pixel = 3 Bytes.
#define SCREEN_BYTES_PER_PIXEL 3
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 400
//This is the buffer area size of a screen in bytes.
#define SCREEN_SIZE (BYTES_PER_PIXEL * SCREEN_WIDTH * SCREEN_HEIGHT)

// @breif  Construct a RGB value.
// @param  r, g, b. Red, Green, Blue.
// @retval a u32 RGB value.
#define RGB(r,g,b) (r<<24|b<<16|g<<8|r)
//Frame buffer offsets fixed by Capito27
#define SCREEN_AREA_TOP0 (u8*)(0x20000017)
#define SCREEN_AREA_TOP1 (u8*)(0x20046515)
#define SCREEN_AREA_BOT0 (u8*)(0x2008CA36)
#define SCREEN_AREA_BOT1 (u8*)(0x200C4E34)
extern u32 drawInternalY;

void DrawClearScreen(u8 *screenArea, int color);
void DrawClearScreenAll(void);

//void DrawCharacter(u8 *screenArea, const char character, u32 x, u32 y, u32 foreColor, u32 backColor); //buggy
void DrawCharacter(unsigned char *screen, int character, int x, int y, int color, int bgcolor);
void DrawString(u8 *screenArea, const char *str, u32 x, u32 y, u32 foreColor, u32 backColor);
void DrawDebug(u8 bottom, u8 newline, const char *format, ...);
void DrawSettingsDebug(u8 newline, const char *format, ...);

void DrawTopSplash(char splash_file[]);
void DrawBottomSplash(char splash_file[]);

//Unused.
void DrawStringFormat(u8 newline, u8 debug, u32 x, u32 y, const char *format, ...);

//Un-implemented.
//void DrawHex(uint8_t *screenArea, unsigned int hex, u32 x, u32 y, u32 foreColor, u32 backColor);
//void DrawHexWithName(uint8_t *screenArea, const char *str, unsigned int hex, u32 x, u32 y, u32 foreColor, u32 backColor);
