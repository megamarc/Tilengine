/******************************************************************************
*
* Tilengine sample
* 2015 Marc Palacios
* http://www.tilengine.org
*
* This example show a wobble distortion effect to simulate underwater. It
* combines raster effect to change horizontal position and column offset to
* change vertical position.
*
******************************************************************************/

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Tilengine.h"

#define WIDTH	320
#define HEIGHT	192
#define NUM_COLUMNS	WIDTH/8 + 2

/* linear interploation */
#define lerp(x, x0,x1, fx0,fx1) \
	(fx0) + ((fx1) - (fx0))*((x) - (x0))/((x1) - (x0))

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

static uint32_t _frame;
int column[NUM_COLUMNS];

static void raster_callback (int line);
static void main_loop(uint32_t frame);

/* entry point */
int main (int argc, char *argv[])
{
	TLN_Tilemap foreground, background;

	/* setup engine */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, 0, 5);
	TLN_SetRasterCallback (raster_callback);
	TLN_SetBGColor (0,0,0);

	/* load resources*/
	TLN_SetLoadPath ("assets/tf3");
	foreground = TLN_LoadTilemap ("tf3_bg2.tmx", NULL);
	background = TLN_LoadTilemap ("tf3_bg3.tmx", NULL);
	TLN_SetLayerTilemap (LAYER_FOREGROUND, foreground);
	TLN_SetLayerTilemap (LAYER_BACKGROUND, background);

	/* setup column offset array for vertical tile displacement */
	TLN_SetLayerColumnOffset (LAYER_BACKGROUND, column);

	/* create window & main loop, block until window closes */
	TLN_CreateWindow(NULL, CWF_FULLSCREEN);
	TLN_SetMainTask(main_loop);

	/* deinit */
	TLN_DeleteTilemap (foreground);
	TLN_DeleteTilemap (background);
	TLN_Deinit ();

	return 0;
}

/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	int value = (_frame + line) << 1;
	float angle = (value * M_PI) / 180.0f;
	TLN_SetLayerPosition (LAYER_FOREGROUND, _frame * 2, (sin(angle) * 8) + 8);
	TLN_SetLayerPosition (LAYER_BACKGROUND, _frame + sin(angle) * 10, 0);
}

/* main loop delegate, called every frame */
static void main_loop(uint32_t frame)
{
	int c;
	
	/* scroll */
	TLN_SetLayerPosition (LAYER_FOREGROUND, frame*3, 0);
	TLN_SetLayerPosition (LAYER_BACKGROUND, frame, 0);

	/* update column offset table */
	for (c = 0; c < NUM_COLUMNS; c += 1)
	{
		int value = frame*5 + c*20;
		float angle = (value * M_PI) / 180.0f;
		column[c] = (int)(sin(angle) * 3);
	}
	_frame = frame;
}
