#include "rendering.h"
#include "draw.h"

//For ctrulib
#include <3ds.h>

//For sprintF
#include <stdio.h>

//Screen variables
u8* screenBottom = 0;
u8* screenTopLeft = 0;
u8* screenTopRight = 0;

/*----------------------------------------- SCREEN UTILS -------------------------------------------------
*----------------------------------------- by AlbertoSONIC -----------------------------------------------
*/

//Screen rendering (both top and bottom)
void getFB()
{
	screenBottom = gfxGetFramebuffer(GFX_BOTTOM, GFX_BOTTOM, NULL, NULL);
	screenTopLeft = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	screenTopRight = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
}

//Clears the top screen
void clearTop()
{
	screenTopLeft = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	screenTopRight = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	clearScreen(screenTopLeft, GFX_LEFT);
	clearScreen(screenTopRight, GFX_LEFT);
}

//Clears the bottom screen
void clearBottom()
{
	screenBottom = gfxGetFramebuffer(GFX_BOTTOM, GFX_BOTTOM, NULL, NULL);
	clearScreen(screenBottom, GFX_BOTTOM);
}

