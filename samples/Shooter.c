/******************************************************************************
*
* Tilengine sample
* 2015 Marc Palacios
* http://www.tilengine.org
*
* This full example show a proof-of-concept shoot-em up game. It uses two
* multiplexing layers to simulate more scrolling planes, linescroll for the
* water, transparency for the clouds, and sprite animation for the actors.
*
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Shooter.h"
#include "Tilengine.h"
#include "Boss.h"
#include "Explosion.h"
#include "Enemy.h"
#include "Ship.h"

#define PAL_T0	120
#define PAL_T1	1000

/* linear interploation */
static float lerp (int x, int x0, int x1, float fx0, float fx1)
{
	return (fx0) + ((fx1) - (fx0))*((x) - (x0))/((x1) - (x0));
}

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

TLN_Sequence sequences[MAX_SEQ];
TLN_Spriteset spritesets[MAX_SPRITESET];
TLN_Palette palettes[MAX_LAYER];
layer_t layers[MAX_LAYER];
float pos_foreground[3] = {0};
float pos_background[3] = {0};
float inc_background[3] = {0};
uint32_t _frame;

uint8_t sky1[3] = {107,205,255};
uint8_t sky2[3] = {255,242,167};
uint8_t sky3[3] = {131, 72,148};
uint8_t sky4[3] = {237,219,149};
int sky_hi[3];
int sky_lo[3];

static void raster_callback (int line);
static void main_loop(uint32_t frame);

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
	layer->tilemap = TLN_LoadTilemap (filename, NULL);

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
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, MAX_ACTOR, 0);
	TLN_SetRasterCallback (raster_callback);
	TLN_SetBGColor (136,238,204);

	/* load resources*/
	TLN_SetLoadPath ("assets/tf4");
	spritesets[SPRITESET_MAIN] = TLN_LoadSpriteset ("FireLeo");
	spritesets[SPRITESET_HELLARM] = TLN_LoadSpriteset ("HellArm");
	LoadLayer (LAYER_FOREGROUND, "TF4_fg1");
	LoadLayer (LAYER_BACKGROUND, "TF4_bg1");

	/* create sequences from sprite names */
	sequences[SEQ_CLAW  ] = TLN_CreateSpriteSequence (NULL, spritesets[SPRITESET_MAIN], "claw", 4);
	sequences[SEQ_BLADE1] = TLN_CreateSpriteSequence (NULL, spritesets[SPRITESET_MAIN], "bladeb", 2);
	sequences[SEQ_BLADE2] = TLN_CreateSpriteSequence (NULL, spritesets[SPRITESET_MAIN], "blades", 2);
	sequences[SEQ_EXPLO1] = TLN_CreateSpriteSequence (NULL, spritesets[SPRITESET_MAIN], "explb", 3);
	sequences[SEQ_EXPLO2] = TLN_CreateSpriteSequence (NULL, spritesets[SPRITESET_MAIN], "expls", 2);

	/* create actors */
	CreateActors (MAX_ACTOR);
	CreateShip ();

	/* compute increments for background scroll*/
	inc_background[0] = 1.0f;	/* 1.0 pixels/frame */
	inc_background[1] = 1.2f;	/* 1.2 pixels/frame*/
	inc_background[2] = 8.0f;	/* 8.0 pixels/frame*/

	/* initial colors */
	for (c = 0; c < 3; c += 1)
	{
		sky_hi[c] = sky1[c];
		sky_lo[c] = sky2[c];
	}
	for (c = 0; c < MAX_LAYER; c += 1)
		palettes[c] = TLN_ClonePalette (TLN_GetTilesetPalette (layers[c].tileset));

	/* create window & main loop, block until window closes */
	TLN_CreateWindow(NULL, CWF_FULLSCREEN);
	TLN_SetMainTask(main_loop);	

	/* deinit */
	FreeLayer (LAYER_FOREGROUND);
	FreeLayer (LAYER_BACKGROUND);
	TLN_Deinit ();

	return 0;
}

/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	/* sky color gradient */
	if (line < 64)
	{
		uint8_t r = lerp(line, 0,63, sky_hi[0], sky_lo[0]);
		uint8_t g = lerp(line, 0,63, sky_hi[1], sky_lo[1]);
		uint8_t b = lerp(line, 0,63, sky_hi[2], sky_lo[2]);
		TLN_SetBGColor (r, g, b);
	}

	/* foreground */
	if (line == 32)
		TLN_SetLayerPosition (LAYER_BACKGROUND, _frame/4, 160);

	if (line == 64)
	{
		/* swap fore/background layers */
		TLN_SetLayer (LAYER_BACKGROUND, layers[LAYER_BACKGROUND].tileset, layers[LAYER_BACKGROUND].tilemap);
		TLN_SetLayer (LAYER_FOREGROUND, layers[LAYER_FOREGROUND].tileset, layers[LAYER_FOREGROUND].tilemap);
		TLN_SetLayerPosition (LAYER_BACKGROUND, (int)pos_background[0], 64);
		TLN_SetLayerPalette (LAYER_FOREGROUND, palettes[LAYER_FOREGROUND]);
		TLN_SetLayerPalette (LAYER_BACKGROUND, palettes[LAYER_BACKGROUND]);

		/* foreground: cloud layer */
		TLN_SetLayerPosition (LAYER_FOREGROUND, (_frame << 2) / 3, 192 - line);
	}

	if (line == 64)
		TLN_SetLayerBlendMode (LAYER_FOREGROUND, BLEND_MIX50, 0);

	if (line == 112)
		TLN_DisableLayer (LAYER_FOREGROUND);

	if (line == 192)
	{
		TLN_SetLayerBlendMode (LAYER_FOREGROUND, BLEND_NONE, 0);
		TLN_SetLayerPosition (LAYER_FOREGROUND, _frame*10, 448 - line);
	}

	/* sea linescroll */
	if (line >= 112)
	{
		int pos = (int)lerp (line, 112,240, pos_background[1], pos_background[2]);
		TLN_SetLayerPosition (LAYER_BACKGROUND, pos, 224 - 112);
	}
}

/* main loop delegate, called every frame */
static void main_loop(uint32_t frame)
{
	int c;
	
	/* bg color (sky) */
	if (frame >= PAL_T0 && frame <= PAL_T1 && (frame & 0x07) == 0)
	{
		/* sky color */
		for (c = 0; c < 3; c += 1)
		{
			sky_hi[c] = lerp(frame, PAL_T0,PAL_T1, sky1[c], sky3[c]);
			sky_lo[c] = lerp(frame, PAL_T0,PAL_T1, sky2[c], sky4[c]);
		}

		for (c = 0; c < MAX_LAYER; c += 1)
		{
			if (palettes[c])
				TLN_DeletePalette (palettes[c]);
			palettes[c] = TLN_ClonePalette (TLN_GetTilesetPalette (layers[c].tileset));
		}
	}

	/* scroll */
	for (c = 0; c < 3; c += 1)
		pos_background[c] += inc_background[c];

	/* layers */
	TLN_SetLayer (LAYER_BACKGROUND, layers[LAYER_FOREGROUND].tileset, layers[LAYER_FOREGROUND].tilemap);
	TLN_SetLayer (LAYER_FOREGROUND, layers[LAYER_BACKGROUND].tileset, layers[LAYER_BACKGROUND].tilemap);
	TLN_SetLayerPosition (LAYER_BACKGROUND, frame/3, 160);
	TLN_SetLayerPosition (LAYER_FOREGROUND, (int)pos_background[0], 64);
	TLN_SetLayerPalette (LAYER_FOREGROUND, palettes[LAYER_BACKGROUND]);
	TLN_SetLayerPalette (LAYER_BACKGROUND, palettes[LAYER_FOREGROUND]);
	
	/* during first 500 frames, spawn new enemy every 30 frames */
	if (frame < 500)
	{
		if (rand() % 30 == 1)
			CreateEnemy ();
	}
	
	/* create boss at frame 600 */
	else if (frame == 600)
		CreateBoss ();

	/* process actors */
	TasksActors (frame);
	_frame = frame;
}
