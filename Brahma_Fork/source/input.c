#include "input.h"

//For ctrulib
#include <3ds.h>

//For sprintF
#include <stdio.h>

//Variables
u32 input;
u16 posX;
u16 posY;
u16 old_posX;
u16 old_posY;

void getInput()
{
	//Touch input
	touchPosition myTouchPosition;
	hidTouchRead(&myTouchPosition);

	posX = myTouchPosition.px;
	posY = myTouchPosition.py;

	if (old_posX == posX && old_posY == posY)
	{ 
		posX = 0; 
		posY = 0; 
	}
	else
	{
		old_posX = posX;
		old_posY = posY;
	}

	//HID Input
	hidScanInput();
	input = hidKeysDown();
}