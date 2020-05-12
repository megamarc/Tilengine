/******************************************************************************
*
* Tilengine sample
* 2015 Marc Palacios
* http://www.tilengine.org
*
* This example show a barrel distortion effect similar to one of the stages
* in the SNES game "Super Castlevania IV"
*
******************************************************************************/

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Tilengine.h"
#include "Simon.h"

#define WIDTH	400
#define HEIGHT	240
#define MAX_PALETTE 8

#ifndef M_PI
#define M_PI 3.14159265f
#endif

#define DEG2RAD(n) ((n)*M_PI/180)

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

TLN_Palette palette;
TLN_Palette palettes[MAX_PALETTE];
TLN_Affine transform;
int xpos, ypos;

static void raster_callback (int line);

/* entry point */
int main (int argc, char *argv[])
{
	int c;
	TLN_Tilemap tilemaps[MAX_LAYER];

	/* setup engine */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, 1, 1);
	TLN_SetRasterCallback (raster_callback);
	TLN_SetBGColor (115,48,57);

	/* load resources*/
	TLN_SetLoadPath ("assets/sc4");
	tilemaps[LAYER_FOREGROUND] = TLN_LoadTilemap ("castle_fg.tmx", NULL);
	tilemaps[LAYER_BACKGROUND] = TLN_LoadTilemap ("castle_bg.tmx", NULL);
	TLN_SetLayerTilemap (LAYER_FOREGROUND, tilemaps[LAYER_FOREGROUND]);
	TLN_SetLayerTilemap (LAYER_BACKGROUND, tilemaps[LAYER_BACKGROUND]);

	/* tweak palettes */
	palette = TLN_GetLayerPalette (LAYER_BACKGROUND);
	for (c=0; c<MAX_PALETTE; c++)
	{
		int inc = c*7;
		palettes[c] = TLN_ClonePalette (palette);
		TLN_SubPaletteColor (palettes[c], inc,inc,inc, 1, 255);
	}
	
	transform.dx = WIDTH/2;
	transform.dy = 1;
	transform.sy = 1;

	SimonInit ();
	
	/* main loop */
	TLN_CreateWindow (NULL, 0);
	while (TLN_ProcessWindow ())
	{
		ypos++;
		SimonTasks ();

		/* input */
		xpos = SimonGetPosition ();

		/* scroll */
		TLN_SetLayerPosition (LAYER_BACKGROUND, xpos/2, -(ypos>>1));
		TLN_SetLayerPosition (LAYER_FOREGROUND, xpos, 0);

		/* render to window */
		TLN_DrawFrame (0);
	}

	/* deinit */
	SimonDeinit ();
	TLN_DeleteTilemap (tilemaps[LAYER_FOREGROUND]);
	TLN_DeleteTilemap (tilemaps[LAYER_BACKGROUND]);
	TLN_Deinit ();
	return 0;
}

/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	float angle;
	float factor;
	int size;
	int index;
	int dx;

	angle = lerp ((float)line, 0,HEIGHT-1, 0,M_PI);
	factor = (1-sin(angle))*0.4 + 1;
	size = WIDTH * factor;
	dx = ((size - WIDTH) / 2) / factor;
	transform.sx = factor;

	TLN_SetLayerAffineTransform (LAYER_BACKGROUND, &transform);

	if (line < 70)
	{
		index = lerp (line, 0,70, 0,7);
		TLN_SetLayerPalette (LAYER_BACKGROUND, palettes[index]);
	}
	else if (line > 170)
	{
		index = lerp (line, 170,HEIGHT, 7,0);
		TLN_SetLayerPalette (LAYER_BACKGROUND, palettes[index]);
	}
	else
		TLN_SetLayerPalette (LAYER_BACKGROUND, palettes[7]);
}
