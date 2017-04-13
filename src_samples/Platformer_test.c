/******************************************************************************
*
* Tilengine sample
* 2015 Marc Palacios
* http://www.tilengine.org
*
* This example show a classic sidescroller. It mimics the actual Sega Genesis
* Sonic game. It uses two layers, where the background one has multiple strips
* and a linescroll effect. It also uses color animation for the water cycle
*
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

/* linear interploation */
#define lerp(x, x0,x1, fx0,fx1) \
	fx0 + (fx1-fx0)*(x-x0)/(x1-x0)

typedef struct
{
	int r,g,b;
}
RGB;

RGB sky[] = 
{
	{0x1B, 0x00, 0x8B},
	{0x00, 0x74, 0xD7},
	{0x24, 0x92, 0xDB},
	{0x1F, 0x7F, 0xBE},
};

static void InterpolateColor (int v, int v1, int v2, RGB* color1, RGB* color2, RGB* result);

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
fix_t pos_foreground = {0};
fix_t pos_background[6] = {0};
fix_t inc_background[6] = {0};
unsigned int frame;
unsigned int time;
float speed;
int ypos;

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
	int pitch, size;
	uint8_t* framebuffer;
	uint32_t t0, t1;	

	/* setup engine */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, 0, 1);
	TLN_SetRasterCallback (raster_callback);
	TLN_SetBGColor (0,128,238);

	/* load resources*/
	LoadLayer (LAYER_FOREGROUND, "Sonic_md_fg1");
	LoadLayer (LAYER_BACKGROUND, "Sonic_md_bg1");

	/* load sequences */
	sp = TLN_LoadSequencePack ("Sonic_md_seq.sqx");
	sequence = TLN_FindSequence (sp, "seq_water");

	/* assign color sequence to various entries in palette */
	palette = TLN_GetTilesetPalette (layers[LAYER_BACKGROUND].tileset);
	TLN_SetPaletteAnimation (0, palette, sequence, false);

	/* compute increments for variable background scrolling speeds */
	inc_background[0] = float2fix(0.562f);
	inc_background[1] = float2fix(0.437f);
	inc_background[2] = float2fix(0.375f);
	inc_background[3] = float2fix(0.625f);
	inc_background[4] = float2fix(1.0f);
	inc_background[5] = float2fix(2.0f);
	
	/* framebuffer */
	pitch = WIDTH*4;
	size = pitch*HEIGHT;
	framebuffer = malloc (size);
	TLN_SetRenderTarget (framebuffer, pitch);

	/* main loop */
	printf ("Rendering 5 seconds... ");
	speed = 1.0;
	t0 = t1 = TLN_GetTicks ();
	while (t1 - t0 < 5000)
	{
		/* timekeeper */
		time = frame;

		/* scroll */
		pos_foreground += (int2fix(3)*speed);
		TLN_SetLayerPosition (LAYER_FOREGROUND, fix2int(pos_foreground), ypos);
		for (c=0; c<6; c++)
			pos_background[c] += (inc_background[c] * speed);

		/* render to window */
		TLN_UpdateFrame (time);
		frame++;
		t1 = TLN_GetTicks ();
	}
	printf ("%d fps\n", frame*1000/(t1 - t0));

	/* deinit */
	free (framebuffer);
	FreeLayer (LAYER_FOREGROUND);
	FreeLayer (LAYER_BACKGROUND);
	TLN_DeleteSequencePack (sp);
	TLN_Deinit ();

	return 0;
}

/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	int pos =- 1;
	RGB color;

	if (line==1)
		pos = pos_background[0];
	else if (line==32)
		pos = pos_background[1];
	else if (line==48)
		pos = pos_background[2];
	else if (line==64)
		pos = pos_background[3];
	else if (line==112)
		pos = pos_background[4];
	else if (line >= 152)
		pos = lerp (line, 152,224, pos_background[4], pos_background[5]);

	if (pos != -1)
		TLN_SetLayerPosition (LAYER_BACKGROUND, fix2int(pos), 0);
	
	/* background color gradients */
	if (line < 112)
	{
		InterpolateColor (line, 0,112, &sky[0], &sky[1], &color);
		TLN_SetBGColor (color.r, color.g, color.b);
	}	
	else if (line >= 144)
	{
		InterpolateColor (line, 144,HEIGHT, &sky[2], &sky[3], &color);
		TLN_SetBGColor (color.r, color.g, color.b);
	}		
}

static void InterpolateColor (int v, int v1, int v2, RGB* color1, RGB* color2, RGB* result)
{
	result->r = lerp (v, v1,v2, color1->r, color2->r);
	result->g = lerp (v, v1,v2, color1->g, color2->g);
	result->b = lerp (v, v1,v2, color1->b, color2->b);
}
