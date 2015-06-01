#pragma once

#include "common.h"

#define SCREEN_BYTES_PER_PIXEL 3
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 400

#define SCREEN_SIZE (BYTES_PER_PIXEL * SCREEN_WIDTH * SCREEN_HEIGHT)

#define RGB(r,g,b) (r<<24|b<<16|g<<8|r)

#define SCREEN_AREA_TOP0 (u8*)(0x20000000)
#define SCREEN_AREA_TOP1 (u8*)(0x20046500)
#define SCREEN_AREA_BOT0 (u8*)(0x2008CA00)
#define SCREEN_AREA_BOT1 (u8*)(0x200C4E00)
extern u32 drawInternalY;

void DrawClearScreen(u8 *screenArea, int color);
void DrawClearScreenAll(void);

void DrawCharacter(u8 *screenArea, const char character, u32 x, u32 y, u32 foreColor, u32 backColor);
void DrawString(u8 *screenArea, const char *str, u32 x, u32 y, u32 foreColor, u32 backColor);
void DrawDebug(u8 newline, const char *format, ...);

//Unused.
void DrawStringFormat(u8 newline, u8 debug, u32 x, u32 y, const char *format, ...);

//Un-implemented.
//void DrawHex(uint8_t *screenArea, unsigned int hex, u32 x, u32 y, u32 foreColor, u32 backColor);
//void DrawHexWithName(uint8_t *screenArea, const char *str, unsigned int hex, u32 x, u32 y, u32 foreColor, u32 backColor);
