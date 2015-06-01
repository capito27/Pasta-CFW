#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "font.h"
#include "draw.h"

int current_y = 0;

void ClearScreen(unsigned char *screen, int color)
{
    int i;
    u8 colorR = color & 0xFF, colorG = color >> 8, colorB = color >> 16;
    unsigned char *screenPos = screen;
    for (i = 0; i < (SCREEN_HEIGHT * SCREEN_WIDTH); i++) {
        *(screenPos++) = colorB;  // B
        *(screenPos++) = colorG;   // G
        *(screenPos++) = colorR; // R
    }
}

void DrawCharacter(unsigned char *screen, int character, int x, int y, int color, int bgcolor)
{
    int yy, xx;
    u8 foreColorR = color & 0xFF, foreColorG = color >> 8, foreColorB = color >> 16;
    u8 backColorR = bgcolor & 0xFF, backColorG = bgcolor >> 8, backColorB = bgcolor >> 16;
    for (yy = 0; yy < 8; yy++) {
        int xDisplacement = (x * BYTES_PER_PIXEL * SCREEN_WIDTH);
        int yDisplacement = ((SCREEN_WIDTH - (y + yy) - 1) * BYTES_PER_PIXEL);
        unsigned char *screenPos = screen + xDisplacement + yDisplacement;

        unsigned char charPos = font[character * 8 + yy];
        for (xx = 7; xx >= 0; xx--) {
            if ((charPos >> xx) & 1) {
                *(screenPos + 0) = foreColorB;  // B
                *(screenPos + 1) = foreColorG;   // G
                *(screenPos + 2) = foreColorR; // R
            } else {
                *(screenPos + 0) = backColorB;  // B
                *(screenPos + 1) = backColorG;   // G
                *(screenPos + 2) = backColorR; // R
            }
            screenPos += BYTES_PER_PIXEL * SCREEN_WIDTH;
        }
    }
}

void DrawString(unsigned char *screen, const char *str, int x, int y, int color, int bgcolor)
{
    int i;
    for (i = 0; i < strlen(str); i++)
        DrawCharacter(screen, str[i], x + i * 8, y, color, bgcolor);
}

void DrawStringF(int x, int y, const char *format, ...)
{
    char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);

    DrawString(TOP_SCREEN0, str, x, y, RGB(0, 0, 0), RGB(255, 255, 255));
    DrawString(TOP_SCREEN1, str, x, y, RGB(0, 0, 0), RGB(255, 255, 255));
}

void Debug(const char *format, ...)
{
    char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);

    DrawString(TOP_SCREEN0, str, 10, current_y, RGB(255, 255, 255), RGB(0,0, 0));
    DrawString(TOP_SCREEN1, str, 10, current_y, RGB(255, 255, 255), RGB(0, 0, 0));

    current_y += 10;
}

void DebugNoNewLine(const char *format, ...)
{
	char str[256];
	va_list va;

	va_start(va, format);
	vsnprintf(str, 256, format, va);
	va_end(va);

	DrawString(TOP_SCREEN0, str, 10, current_y, RGB(255, 255, 255), RGB(0, 0, 0));
	DrawString(TOP_SCREEN1, str, 10, current_y, RGB(255, 255, 255), RGB(0, 0, 0));
}
