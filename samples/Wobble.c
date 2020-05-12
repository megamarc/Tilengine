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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tilengine.h"
#include "Sin.h"

#define WIDTH	320
#define HEIGHT	192
#define COLUMNS	WIDTH/8 + 2

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

static unsigned int frame;
int column[COLUMNS];

static void raster_callback (int line);

/* entry point */
int main (int argc, char *argv[])
{
	TLN_Tilemap foreground, background;
	int c;

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

	BuildSinTable ();
	TLN_SetLayerColumnOffset (LAYER_BACKGROUND, column);

	/* main loop */
	TLN_CreateWindow (NULL, 0);
	while (TLN_ProcessWindow ())
	{
		/* scroll */
		TLN_SetLayerPosition (LAYER_FOREGROUND, frame*3, 0);
		TLN_SetLayerPosition (LAYER_BACKGROUND, frame, 0);

		/* update column offset table */
		for (c=0; c<COLUMNS; c++)
			column[c] = CalcSin (frame*5 + c*20, 3);		

		/* render to window */
		TLN_DrawFrame (frame);
		frame++;
	}

	/* deinit */
	TLN_DeleteTilemap (foreground);
	TLN_DeleteTilemap (background);
	TLN_Deinit ();

	return 0;
}

/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	TLN_SetLayerPosition (LAYER_FOREGROUND, frame*2, CalcSin((frame+line)<<1, 8) + 8);
	TLN_SetLayerPosition (LAYER_BACKGROUND, frame + CalcSin((frame + line)<<1, 10), 0);
}
