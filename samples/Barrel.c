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

#define WIDTH		400
#define HEIGHT		240
#define MAX_PALETTE 8

/* linear interploation */
#define lerp(x, x0,x1, fx0,fx1) \
	(fx0) + ((fx1) - (fx0))*((x) - (x0))/((x1) - (x0))

TLN_Palette palette;
TLN_Palette palettes[MAX_PALETTE];
TLN_Affine transform;
int xpos, ypos;

static void raster_callback (int line);
static void main_loop(uint32_t frame);

/* entry point */
int main (int argc, char *argv[])
{
	int c;
	TLN_Tilemap tilemap;

	/* setup engine */
	TLN_Init (WIDTH,HEIGHT, 1, 0, 0);
	TLN_SetRasterCallback (raster_callback);
	TLN_SetBGColor (115,48,57);

	/* load resources*/
	TLN_SetLoadPath ("assets/sc4");
	tilemap = TLN_LoadTilemap ("castle_bg.tmx", NULL);
	TLN_SetLayerTilemap (0, tilemap);

	/* tweak palettes */
	palette = TLN_GetLayerPalette (0);
	for (c = 0; c < MAX_PALETTE; c += 1)
	{
		int inc = (7 - c) * 8;
		palettes[c] = TLN_ClonePalette (palette);
		TLN_SubPaletteColor (palettes[c], inc,inc,inc, 1, 255);
	}
	
	transform.dx = WIDTH/2;
	transform.dy = 1;
	transform.sy = 1;

	/* create window & main loop, block until window closes */
	TLN_CreateWindow (NULL, CWF_FULLSCREEN);
	TLN_SetMainTask(main_loop);

	/* deinit */
	TLN_DeleteTilemap(tilemap);
	TLN_Deinit ();
	return 0;
}

/* main loop delegate, called every frame */
static void main_loop(uint32_t frame)
{
	/* rotate barrel up/down */
	if (TLN_GetInput(INPUT_UP))
		ypos -= 1;
	else if (TLN_GetInput(INPUT_DOWN))
		ypos += 1;
	
	/* move left/right */
	if (TLN_GetInput(INPUT_LEFT))
		xpos -= 1;
	else if (TLN_GetInput(INPUT_RIGHT))
		xpos += 1;
	
	/* update view */
	TLN_SetLayerPosition(0, xpos, ypos);
}

/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	float angle;
	float factor;
	int size;
	int index;
	int dx;

	/* barrel distortion */
	angle = lerp ((float)line, 0,HEIGHT - 1, 0, M_PI);
	factor = (1 - sin(angle)) * 0.4 + 1;
	size = WIDTH * factor;
	dx = ((size - WIDTH) / 2) / factor;
	transform.sx = factor;
	TLN_SetLayerAffineTransform (0, &transform);

	/* use secondary palettes near edges to enhance 3D effect */
	if (line < 70)
	{
		index = lerp (line, 0,70, 0,7);
		TLN_SetLayerPalette (0, palettes[index]);
	}
	else if (line > HEIGHT - 70)
	{
		index = lerp (line, HEIGHT - 70, HEIGHT, 7,0);
		TLN_SetLayerPalette (0, palettes[index]);
	}
	else
		TLN_SetLayerPalette (0, palettes[7]);
}
