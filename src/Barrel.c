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

typedef struct
{
	TLN_Tilemap tilemap;
	TLN_Tileset tileset;
}
layer_t;

TLN_Palette palette;
TLN_Palette palettes[MAX_PALETTE];
layer_t layers[MAX_LAYER];
unsigned int frame;
unsigned int time;
TLN_Affine transform;
int xpos, ypos;

static void raster_callback (int line);
static void AddPaletteColor (TLN_Palette palette, BYTE r, BYTE g, BYTE b);
static void MulPaletteColor (TLN_Palette palette, BYTE r, BYTE g, BYTE b);
static void SubPaletteColor (TLN_Palette palette, BYTE r, BYTE g, BYTE b);

/* helper for loading a related tileset + tilemap and configure the appropiate layer */
static void LoadLayer (int index, char* name)
{
	char filename[64];
	layer_t* layer = &layers[index];

	/* load tileset */
	sprintf (filename, "%s.tsx", name);
	layer->tileset = TLN_LoadTileset (filename);

	/* load tilemap */
	sprintf (filename, "%s.tmx", name);
	layer->tilemap = TLN_LoadTilemap (filename, "Layer 1");

	TLN_SetLayer (index, layer->tileset, layer->tilemap);
}

/* helper for freeing a tileset + tilemap */ 
static void FreeLayer (int index)
{
	layer_t* layer = &layers[index];
	
	TLN_DeleteTileset (layer->tileset);
	TLN_DeleteTilemap (layer->tilemap);
}

/* entry point */
int main (int argc, char *argv[])
{
	int c;

	/* setup engine */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, 1, 1);
	TLN_SetRasterCallback (raster_callback);
	TLN_SetBGColor (115,48,57);

	/* load resources*/
	LoadLayer (LAYER_FOREGROUND, "castle_fg");
	LoadLayer (LAYER_BACKGROUND, "castle_bg");

	/* startup display */
	TLN_CreateWindow ("overlay.bmp", CWF_VSYNC);

	/* tweak palettes */
	palette = TLN_GetTilesetPalette (layers[LAYER_BACKGROUND].tileset);
	for (c=0; c<MAX_PALETTE; c++)
	{
		int inc = c*7;
		palettes[c] = TLN_ClonePalette (palette);
		SubPaletteColor (palettes[c], inc,inc,inc);
	}
	
	transform.dx = WIDTH/2;
	transform.dy = 1;
	transform.sy = 1;

	SimonInit ();
	
	/* main loop */
	while (TLN_ProcessWindow ())
	{
		/* timekeeper */
		time = frame;
		ypos++;
		SimonTasks ();

		/* input */
		xpos = SimonGetPosition ();

		/* scroll */
		TLN_SetLayerPosition (LAYER_BACKGROUND, xpos/2, -(ypos>>1));
		TLN_SetLayerPosition (LAYER_FOREGROUND, xpos, 0);

		/* render to window */
		TLN_DrawFrame (time);

		frame++;
	}

	/* deinit */
	SimonDeinit ();
	FreeLayer (LAYER_FOREGROUND);
	FreeLayer (LAYER_BACKGROUND);
	TLN_DeleteWindow ();
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

static void AddPaletteColor (TLN_Palette palette, BYTE r, BYTE g, BYTE b)
{
	int c;
	int tmp;

	if (!palette)
		return;

	for (c=0; c<32; c++)
	{
		BYTE* data = (BYTE*)TLN_GetPaletteData (palette, c);
		tmp = data[0] + g;
		data[0] = tmp<255? tmp:255;
		tmp = data[1] + b;
		data[1] = tmp<255? tmp:255;
		tmp = data[2] + r;
		data[2] = tmp<255? tmp:255;
	}
}

static void SubPaletteColor (TLN_Palette palette, BYTE r, BYTE g, BYTE b)
{
	int c;
	int tmp;

	if (!palette)
		return;

	for (c=0; c<32; c++)
	{
		BYTE* data = (BYTE*)TLN_GetPaletteData (palette, c);
		tmp = data[0] - g;
		data[0] = tmp>0? tmp:0;
		tmp = data[1] - b;
		data[1] = tmp>0? tmp:0;
		tmp = data[2] - r;
		data[2] = tmp>0? tmp:0;
	}
}

static void MulPaletteColor (TLN_Palette palette, BYTE r, BYTE g, BYTE b)
{
	int c;

	if (!palette)
		return;

	for (c=0; c<32; c++)
	{
		BYTE* data = (BYTE*)TLN_GetPaletteData (palette, c);
		data[0] = (BYTE)(int)b*data[0]/255;
		data[1] = (BYTE)(int)g*data[1]/255;
		data[2] = (BYTE)(int)r*data[2]/255;
	}
}