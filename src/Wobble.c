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

typedef struct
{
	TLN_Tilemap tilemap;
	TLN_Tileset tileset;
}
layer_t;

TLN_SequencePack sp;
TLN_Sequence sequence;
TLN_Palette palette;
layer_t layers[MAX_LAYER];
unsigned int frame;
unsigned int time;
int column[COLUMNS];

static void raster_callback (int line);

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
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, 0, 5);
	TLN_SetRasterCallback (raster_callback);
	TLN_SetBGColor (0,0,0);

	/* load resources*/
	LoadLayer (LAYER_FOREGROUND, "tf3_bg2");
	LoadLayer (LAYER_BACKGROUND, "tf3_bg3");

	/* startup display */
	TLN_CreateWindow ("overlay.bmp", CWF_VSYNC);
	
	BuildSinTable ();

	/* main loop */
	while (TLN_ProcessWindow ())
	{
		/* timekeeper */
		time = frame;

		/* scroll */
		TLN_SetLayerPosition (LAYER_FOREGROUND, frame*3, 0);
		TLN_SetLayerPosition (LAYER_BACKGROUND, frame, 0);

		for (c=0; c<COLUMNS; c++)
			column[c] = CalcSin (frame*5 + c*20, 3);
		TLN_SetLayerColumnOffset (LAYER_BACKGROUND, column);

		/* render to window */
		TLN_DrawFrame (time);
		frame++;
	}

	/* deinit */
	FreeLayer (LAYER_FOREGROUND);
	FreeLayer (LAYER_BACKGROUND);
	TLN_DeleteWindow ();
	TLN_Deinit ();

	return 0;
}

/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	TLN_SetLayerPosition (LAYER_FOREGROUND, frame*2, CalcSin((frame+line)<<1, 8) + 8);
	TLN_SetLayerPosition (LAYER_BACKGROUND, frame + CalcSin((frame + line)<<1, 10), 0);
}
