#pragma once

#include "common.h"
//It used 24 bits color, so 1 pixel = 3 Bytes.
#define SCREEN_BYTES_PER_PIXEL 3
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 400
//This is the area size of a screen in bytes.
#define SCREEN_SIZE (BYTES_PER_PIXEL * SCREEN_WIDTH * SCREEN_HEIGHT)

// @breif  Construct a RGB value.
// @param  r, g, b. Red, Green, Blue.
// @retval a uint32_t RGB value.
#define RGB(r,g,b) (r<<24|b<<16|g<<8|r)

#define SCREEN_PTR_TOP0 (u8*)(0x20000000)
#define SCREEN_PTR_TOP1 (u8*)(0x20046500)
#define SCREEN_PTR_BOT0 (u8*)(0x2008CA00)
#define SCREEN_PTR_BOT1 (u8*)(0x200C4E00)
extern int drawInternalY;

void DrawClearScreen(uint8_t *screenArea, uint32_t color);
void DrawClearScreenAll(void);

void DrawCharacter(uint8_t *screenArea, char character, uint32_t x, uint32_t y, uint32_t foreColor, uint32_t backColor);
void DrawString(uint8_t *screenArea, const char *str, uint32_t x, uint32_t y, uint32_t foreColor, uint32_t backColor);
void DrawDebug(uint8_t newline, const char *format, ...);

//Unused.
void DrawStringFormat(uint8_t newline, uint8_t debug, uint32_t x, uint32_t y, const char *format, ...);

//Un-implemented.
//void DrawHex(uint8_t *screenArea, unsigned int hex, int x, int y, int color, int bgcolor);
//void DrawHexWithName(uint8_t *screenArea, const char *str, unsigned int hex, int x, int y, int color, int bgcolor);

