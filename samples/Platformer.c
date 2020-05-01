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

float pos_foreground = {0};
float pos_background[6] = {0};
float inc_background[6] = {0};
unsigned int frame;
unsigned int time;
float speed;
int ypos;

static void raster_callback (int line);

/* entry point */
int main (int argc, char *argv[])
{
	int c;
	TLN_Tilemap tilemaps[MAX_LAYER];
	TLN_SequencePack sp;
	TLN_Sequence sequence;
	TLN_Palette palette;

	/* setup engine */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, 0, 1);
	TLN_SetRasterCallback (raster_callback);
	TLN_SetBGColor (0,128,238);

	/* load resources*/
	TLN_SetLoadPath ("assets/sonic");
	tilemaps[LAYER_FOREGROUND] = TLN_LoadTilemap ("Sonic_md_fg1.tmx", NULL);
	tilemaps[LAYER_BACKGROUND] = TLN_LoadTilemap ("Sonic_md_bg1.tmx", NULL);
	TLN_SetLayer (LAYER_FOREGROUND, NULL, tilemaps[LAYER_FOREGROUND]);
	TLN_SetLayer (LAYER_BACKGROUND, NULL, tilemaps[LAYER_BACKGROUND]);
	sp = TLN_LoadSequencePack ("Sonic_md_seq.sqx");
	sequence = TLN_FindSequence (sp, "seq_water");

	/* assign color sequence to various entries in palette */
	palette = TLN_GetLayerPalette (LAYER_BACKGROUND);
	TLN_SetPaletteAnimation (TLN_GetAvailableAnimation(), palette, sequence, true);

	/* compute increments for variable background scrolling speeds */
	inc_background[0] = 0.562f;
	inc_background[1] = 0.437f;
	inc_background[2] = 0.375f;
	inc_background[3] = 0.625f;
	inc_background[4] = 1.0f;
	inc_background[5] = 2.0f;

	/* startup display */
	TLN_CreateWindow (NULL, 0);

	/* main loop */
	while (TLN_ProcessWindow ())
	{
		/* timekeeper */
		time = frame;

		if (TLN_GetInput (INPUT_RIGHT))
		{
			speed += 0.02f;
			if (speed > 1.0f)
				speed = 1.0f;
		}
		else if (speed > 0.0f)
		{
			speed -= 0.02f;
			if (speed < 0.0f)
				speed = 0.0f;
		}
			 
		if (TLN_GetInput (INPUT_LEFT))
		{
			speed -= 0.02f;
			if (speed < -1.0f)
				speed = -1.0f;
		}
		else if (speed < 0.0f)
		{
			speed += 0.02f;
			if (speed > 0.0f)
				speed = 0.0f;
		}

		/* scroll */
		pos_foreground += 3.0f*speed;
		TLN_SetLayerPosition (LAYER_FOREGROUND, (int)pos_foreground, ypos);
		for (c=0; c<6; c++)
			pos_background[c] += (inc_background[c] * speed);

		/* render to window */
		TLN_DrawFrame (time);

		frame++;
	}

	/* deinit */
	TLN_DeleteTilemap (tilemaps[LAYER_FOREGROUND]);
	TLN_DeleteTilemap (tilemaps[LAYER_BACKGROUND]);
	TLN_DeleteSequencePack (sp);
	TLN_Deinit ();

	return 0;
}

/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	float pos =- 1;
	RGB color;
	
	if (line==0)
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
		TLN_SetLayerPosition (LAYER_BACKGROUND, (int)pos, 0);
	
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
