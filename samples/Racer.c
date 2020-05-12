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

#include <stdlib.h>
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


TLN_Spriteset spritesets[MAX_SPRITESET];
TLN_Palette palettes[2];
int pos;
int speed;
int last_tree;
unsigned int frame;
unsigned int time;
int pan = 0;

static void raster_callback (int line);

/* entry point */
int main (int argc, char* argv[])
{
	TLN_Tilemap tilemap;

	/* init engine and load resources */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER,MAX_ACTOR, 0);
	TLN_SetRasterCallback (raster_callback);
	
	/* load resources*/
	TLN_SetLoadPath ("assets/racer");
	tilemap = TLN_LoadTilemap ("racer.tmx", NULL);
	TLN_SetLayerTilemap (LAYER_PLAYFIELD, tilemap);
	palettes[0] = TLN_GetLayerPalette (LAYER_PLAYFIELD);
	palettes[1] = TLN_LoadPalette ("racer.act");
	spritesets[SPRITESET_TREES] = TLN_LoadSpriteset ("trees");
		
	/* startup display */
	TLN_CreateWindow (NULL, 0);

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
	TLN_DeleteTilemap (tilemap);
	TLN_DeleteSpriteset (spritesets[SPRITESET_TREES]);
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
