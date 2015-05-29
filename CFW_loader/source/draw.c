#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "font.h"
#include "draw.h"

u32 drawInternalY = 0;

// @breif  Clear the target screen - fill it with pure color.
// @param  screenArea Pointer to the target screen. should be one of 
//               SCREEN_PTR_TOP0,SCREEN_PTR_TOP1,SCREEN_PTR_BOT0,SCREEN_PTR_BOT1.
// @param  color  RGB color, constructed using the macro RGB(r,g,b).
// @note   Painting the pixel one after one would be a little slow, however very easy..
void DrawClearScreen(u8 *screenArea, u32 color) {
    u32 i = 0;
    u8 *screenBuffer = screenArea;
    for (i = 0; i < (SCREEN_HEIGHT * SCREEN_WIDTH); i++) {
        *(screenBuffer++) = color >> 16;  // B
        *(screenBuffer++) = color >> 8;   // G
        *(screenBuffer++) = color & 0xFF; // R
    }
}
// @breif  Clear all the screens.
// @note   This would reset the Y position for text drawing.
void DrawClearScreenAll(void) {
	DrawClearScreen(SCREEN_PTR_TOP0, RGB(0, 0, 0));
	DrawClearScreen(SCREEN_PTR_TOP1, RGB(0, 0, 0));
	DrawClearScreen(SCREEN_PTR_BOT0, RGB(0, 0, 0));
	DrawClearScreen(SCREEN_PTR_BOT1, RGB(0, 0, 0));
	drawInternalY = 0;
}
// @breif  Draw a character to the desired Position, with defined Foreground & Background Color.
// @param  screenArea Pointer to the target screen. should be one of 
//               SCREEN_PTR_TOP0,SCREEN_PTR_TOP1,SCREEN_PTR_BOT0,SCREEN_PTR_BOT1.
// @param  character  the char you wanna draw.
// @param  x, y       where the character supposed to be.
// @param  foreColor, backColor  RGB color, constructed using the macro RGB(r,g,b).
//               One for foreground and another for background.
// @note   screen buffer is filled in column after another not row after row. May look weird.
void DrawCharacter(u8 *screenArea, char character, u32 x, u32 y, u32 foreColor, u32 backColor) {
    u8  XFont = 0, YFont = 0;
    u8  FontCache = 0; // Pick the target character.
    u32 XOffset = 0, YOffset = 0; // The offsets calculated from X, Y.
    u8 *screenBuffer = NULL;
    // Use pre-calculated values.
    u8 ForeR = foreColor & 0xFF, ForeG = foreColor >> 8, ForeB = foreColor >> 16;
    u8 BackR = backColor & 0xFF, BackG = backColor >> 8, BackB = backColor >> 16;
    for (YFont = 0; YFont < 8; YFont++) {
        XOffset = (x * SCREEN_BYTES_PER_PIXEL * SCREEN_WIDTH);
        YOffset = ((SCREEN_WIDTH - (y + YFont) - 1) * SCREEN_BYTES_PER_PIXEL);
        screenBuffer = screenArea + XOffset + YOffset;
        FontCache = font[character * 8 + YFont];
        for (XFont = 7; XFont >= 0; XFont--) {
            if ((FontCache >> XFont) & 1) {
                *(screenBuffer + 0) = ForeB;
                *(screenBuffer + 1) = ForeG;
                *(screenBuffer + 2) = ForeR;
            } else {
                *(screenBuffer + 0) = BackB;
                *(screenBuffer + 1) = BackG;
                *(screenBuffer + 2) = BackR;
            }
            screenBuffer += SCREEN_BYTES_PER_PIXEL * SCREEN_WIDTH;
        }
    }
}
// @breif  Draw a string to the desired Position, with defined Foreground & Background Color.
// @param  screenArea Pointer to the target screen. should be one of 
//               SCREEN_PTR_TOP0,SCREEN_PTR_TOP1,SCREEN_PTR_BOT0,SCREEN_PTR_BOT1.
// @param  str   the string you wanna draw.
// @param  x, y       where the character supposed to be.
// @param  foreColor, backColor  RGB color, constructed using the macro RGB(r,g,b).
//               One for foreground and another for background.
void DrawString(u8 *screenArea, const char *str, u32 x, u32 y, u32 foreColor, u32 backColor) {
    uint16_t i = 0, len = strlen(str); // cahced length
    for (i = 0; i < len; i++) DrawCharacter(screenArea, str[i], x + i * 8, y, foreColor, backColor);
}
// @breif  Draw a string with format to the desired Position, with defined Foreground & Background Color.
// @param  newline print a new line, if not zero.
// @param  x, y   where the character supposed to be.
// @param  format Similar as snprintf, printf..
// @note   Used vsnprintf. So you can use this like printf.
//         You can not use format with more than 256 bytes.
//         Always write to Top screen, Black with White background.
void DrawStringFormat(u8 newline, u8 debug, u32 x, u32 y, const char *format, ...) {
    char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);

    if (debug) {
        DrawString(SCREEN_PTR_TOP0, str, 10, drawInternalY, RGB(255, 255, 255), RGB(0, 0, 0));
        DrawString(SCREEN_PTR_TOP1, str, 10, drawInternalY, RGB(255, 255, 255), RGB(0, 0, 0));
    } else {
        DrawString(SCREEN_PTR_TOP0, str, x, y, RGB(0, 0, 0), RGB(255, 255, 255));
        DrawString(SCREEN_PTR_TOP1, str, x, y, RGB(0, 0, 0), RGB(255, 255, 255));
    }
    if (newline) drawInternalY += 10;
}
// @breif  Draw a string with format to the desired Position, with defined Foreground & Background Color.
// @param  newline print a new line, if not zero.
// @param  format Similar as snprintf, printf..
// @note   All similar as DrawStringFormat.. Maybe we should use that instead one day.
void DrawDebug(u8 newline, const char *format, ...) {
    char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);

    DrawString(SCREEN_PTR_TOP0, str, 10, drawInternalY, RGB(255, 255, 255), RGB(0,0, 0));
    DrawString(SCREEN_PTR_TOP1, str, 10, drawInternalY, RGB(255, 255, 255), RGB(0, 0, 0));

    if (newline) drawInternalY += 10;
}
