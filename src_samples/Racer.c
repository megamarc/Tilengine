/******************************************************************************
*
* Tilengine sample
* 2015 Marc Palacios
* http://www.tilengine.org
*
* This example show a classic pseudo-3D road like the Sega game Super Hang-On
* It uses a single layer with palette cycling to simulate depth, linescroll
* to simulate lateral movement and scaling sprites for the approaching trees
*
******************************************************************************/

#include <malloc.h>
#include <stdio.h>
#include "Racer.h"
#include "Tree.h"

#define MAX_SPEED	6
#define MAX_STEER	58

/* linear interploation */
#define lerp(x, x0,x1, fx0,fx1) \
	(fx0) + ((fx1) - (fx0))*((x) - (x0))/((x1) - (x0))
	
typedef struct
{
	int r,g,b;
}
RGB;

RGB sky[] = 
{
	{0x66, 0x22, 0xEE},
	{0xCC, 0xCC, 0xEE}
};

static void InterpolateColor (int v, int v1, int v2, RGB* color1, RGB* color2, RGB* result);

/* layers */
enum
{
	LAYER_PLAYFIELD,
	MAX_LAYER
};

typedef struct
{
	TLN_Tilemap tilemap;
	TLN_Tileset tileset;
}
layer_t;

layer_t layers[MAX_LAYER];
TLN_Spriteset spritesets[MAX_SPRITESET];
TLN_Palette palettes[2];
int pos;
int speed;
int last_tree;
unsigned int frame;
unsigned int time;
int pan = 0;

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

static void raster_callback (int line);

/* entry point */
int main (int argc, char* argv[])
{
	/* init engine and load resources */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER,MAX_ACTOR, 0);
	TLN_SetRasterCallback (raster_callback);
	
	/* load resources*/
	spritesets[SPRITESET_TREES] = TLN_LoadSpriteset ("trees");
	LoadLayer (LAYER_PLAYFIELD, "racer");
	palettes[0] = TLN_GetLayerPalette (LAYER_PLAYFIELD);
	palettes[1] = TLN_LoadPalette ("racer.act");
		
	/* startup display */
	TLN_CreateWindow ("overlay.bmp", CWF_VSYNC);

	CreateActors (MAX_ACTOR);

	/* main loop */
	while (TLN_ProcessWindow ())
	{
		/* timekeeper */
		time = frame;

		TLN_SetLayerPosition (LAYER_PLAYFIELD, 56,72);
		if (pos - last_tree >= 100)
		{
			CreateTree (240,184,0);
			CreateTree (240,184,1);
			last_tree = pos;
		}

		/* input */
		if ((time & 0x07) == 0)
		{
			if (TLN_GetInput (INPUT_UP) && speed < MAX_SPEED)
				speed++;
		}
		else if (!TLN_GetInput (INPUT_UP) && speed > 0)
			speed--;

		if (TLN_GetInput (INPUT_LEFT) && pan > -MAX_STEER)
			pan-=2;
		else if (TLN_GetInput (INPUT_RIGHT) && pan < MAX_STEER)
			pan+=2;
		
		/* actores */
		pos += speed;
		TasksActors (time);

		/* render to window */
		TLN_DrawFrame (time);

		frame++;
	}

	/* deinit */
	FreeLayer (LAYER_PLAYFIELD);
	TLN_DeleteSpriteset (spritesets[SPRITESET_TREES]);
	TLN_DeleteWindow ();
	TLN_Deinit ();
	return 0;
}
 
/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	/* sky gradient */
	if (line < 56)
	{
		RGB color;
		InterpolateColor (line, 0,56, &sky[0], &sky[1], &color);
		TLN_SetBGColor (color.r, color.g, color.b);
	}
	
	/* road */
	if (line >= 56)
	{
		int depth = lerp (line, 56,240, Z_NEAR,Z_FAR);
		int value = pos + 32768/depth;
		int phase = (value>>5) & 1;
		int dx = lerp(line, 56,240, 0,pan);
		int c = 240 - line - 1;
		int s = ((c*(c + 1))/2)/128;
		TLN_SetLayerPalette (LAYER_PLAYFIELD, palettes[phase]);
		TLN_SetLayerPosition (LAYER_PLAYFIELD, 56 + dx /*+ s*/, 72);
	}
}

static void InterpolateColor (int v, int v1, int v2, RGB* color1, RGB* color2, RGB* result)
{
	result->r = lerp (v, v1,v2, color1->r, color2->r);
	result->g = lerp (v, v1,v2, color1->g, color2->g);
	result->b = lerp (v, v1,v2, color1->b, color2->b);
}
