#include <stdio.h>
#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

/* linear interploation */
static int lerp (int x, int x0, int x1, int fx0, int fx1)
{
	return (fx0) + ((fx1) - (fx0))*((x) - (x0))/((x1) - (x0));
}

/* RGB color descriptor */
typedef struct
{
	int r,g,b;
}
RGB;

/* sky gradient colors */
static const RGB sky_top = {0x1b, 0x42, 0x7d};
static const RGB sky_bottom = {0xb4, 0x95, 0xaf};

static int xpos;
static const int speed = 2;

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

static void raster_callback (int line);
static void main_loop(uint32_t frame);

/* entry point */
int main (int argc, char* argv[])
{
	TLN_Tilemap foreground, background;
	TLN_Spriteset spriteset;
	TLN_Sequence walk;

	/* setup engine */
	TLN_Init (WIDTH, HEIGHT, 2,1,0);
	TLN_SetRasterCallback (raster_callback);

	/* load resources */
	TLN_SetLoadPath ("assets/sotb");
	foreground = TLN_LoadTilemap ("SOTB_fg.tmx", NULL);
	background = TLN_LoadTilemap ("SOTB_bg.tmx", NULL);
	spriteset = TLN_LoadSpriteset ("SOTB");	
	
	/* setup background layers */
	TLN_SetLayerTilemap (LAYER_FOREGROUND, foreground);
	TLN_SetLayerTilemap (LAYER_BACKGROUND, background);
	
	/* setup main sprite & animation sequence */
	walk = TLN_CreateSpriteSequence (NULL, spriteset, "walk", 6);
	TLN_SetSpriteSet (0, spriteset);
	TLN_SetSpritePosition (0, 200,160);
	TLN_SetSpriteAnimation (0, walk, 0);
	
	/* create window & main loop, block until window closes */
	TLN_CreateWindow(NULL, CWF_FULLSCREEN);
	TLN_SetMainTask(main_loop);	

	/* release resources */
	TLN_DeleteSequence(walk);
	TLN_DeleteTilemap (foreground);
	TLN_DeleteTilemap (background);
	TLN_Deinit ();

	return 0;
}

/* called on each scanline being rendered */
static void raster_callback (int line)
{
	int pos;

	/* sky color: interpolate for smooth gradient */
	if (line == 0)
		TLN_SetBGColor (sky_top.r, sky_top.g, sky_top.b);
	else if (line >= 96 && line < 192)
	{
		RGB color;

		/* interpolate between upper and lower gradient color */
		color.r = lerp (line, 96,191, sky_top.r, sky_bottom.r);
		color.g = lerp (line, 96,191, sky_top.g, sky_bottom.g);
		color.b = lerp (line, 96,191, sky_top.b, sky_bottom.b);
		TLN_SetBGColor (color.r, color.g ,color.b);
	}

	/* horizontal strips in background layer */
	pos = -1;
	if (line == 0 || line == 24 || line == 64 || line == 88 || line == 96)
		pos = (int)lerp (line, 0,96, xpos*0.7f, xpos*0.2f);
	else if (line == 120)
		pos = xpos/2;
	else if (line == 208 || line == 216 || line == 224 || line == 232)
		pos = (int)lerp (line, 208,232, xpos*1.0f, xpos*2.0f);

	if (pos != -1)
		TLN_SetLayerPosition (LAYER_BACKGROUND, pos, 0);

	/* horizontal strips in background layer */
	pos = -1;
	if (line == 0)
		pos = xpos;
	else if (line == 216)
		pos = xpos*3;
	
	if (pos != -1)
		TLN_SetLayerPosition (LAYER_FOREGROUND, pos, 0);
}

/* main loop delegate, called every frame */
static void main_loop(uint32_t frame)
{
	/* update foreground layer position */
	xpos += speed;
	TLN_SetLayerPosition (LAYER_FOREGROUND, xpos, 0);
}
