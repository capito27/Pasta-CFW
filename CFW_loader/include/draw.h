#pragma once

#include "common.h"

#define BYTES_PER_PIXEL 3
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 400

#define SCREEN_SIZE (BYTES_PER_PIXEL * SCREEN_WIDTH * SCREEN_HEIGHT)

#define RGB(r,g,b) (r<<24|b<<16|g<<8|r)

#define TOP_SCREEN0 (u8*)(0x20000000)
#define TOP_SCREEN1 (u8*)(0x20046500)
#define BOT_SCREEN0 (u8*)(0x2008CA00)
#define BOT_SCREEN1 (u8*)(0x200C4E00)
extern int current_y;

void DrawClearScreen(unsigned char *screen, int color);
void DrawClearScreenAll(void);

void DrawCharacter(unsigned char *screen, int character, int x, int y, int color, int bgcolor);
void DrawString(unsigned char *screen, const char *str, int x, int y, int color, int bgcolor);

void Debug(const char *format, ...);
void DebugNoNewLine(const char *format, ...);

//Unused.
void DrawStringFormat(uint8_t newline, uint8_t debug, uint32_t x, uint32_t y, const char *format, ...);

//Un-implemented.
//void DrawHex(uint8_t *screenArea, unsigned int hex, int x, int y, int color, int bgcolor);
//void DrawHexWithName(uint8_t *screenArea, const char *str, unsigned int hex, int x, int y, int color, int bgcolor);
