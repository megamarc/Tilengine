#include <stdio.h>
#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

#define MIN_SCALE	 50
#define MAX_SCALE	200

/* linear interploation */
#define lerp(x, x0,x1, fx0,fx1) \
	(fx0) + ((fx1) - (fx0))*((x) - (x0))/((x1) - (x0))
	
typedef struct
{
	int r,g,b;
}
RGB;

/* RGB sky colors */
static RGB sky[] = 
{
	{0x19, 0x54, 0x75},
	{0x2C, 0xB0, 0xDC},
};

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

static int xpos, ypos;
static int scale;

static void raster_callback (int line);

/* entry point */
int main (int argc, char* argv[])
{
	TLN_Tilemap tilemaps[MAX_LAYER];

	/* setup engine */
	TLN_Init (WIDTH, HEIGHT, MAX_LAYER,0,0);
	TLN_SetBGColor (34,136,170);
	TLN_SetRasterCallback (raster_callback);

	/* load resources */
	TLN_SetLoadPath ("assets/fox");
	tilemaps[LAYER_FOREGROUND] = TLN_LoadTilemap ("psycho.tmx", NULL);
	tilemaps[LAYER_BACKGROUND] = TLN_LoadTilemap ("rolo.tmx", NULL);
	TLN_SetLayer (LAYER_FOREGROUND, NULL, tilemaps[LAYER_FOREGROUND]);
	TLN_SetLayer (LAYER_BACKGROUND, NULL, tilemaps[LAYER_BACKGROUND]);
	
	/* initial values */
	xpos = 0;
	ypos = 192;
	scale = 100;
	
	/* main loop */
	TLN_CreateWindow (NULL, 0);
	while (TLN_ProcessWindow ())
	{
		float fgscale;
		float bgscale;
		int bgypos;
		int maxy;

		/* user input */
		if (TLN_GetInput (INPUT_LEFT))
			xpos--;
		if (TLN_GetInput (INPUT_RIGHT))
			xpos++;
		if (TLN_GetInput (INPUT_UP) && ypos > 0)
			ypos--;
		if (TLN_GetInput (INPUT_DOWN))
			ypos++;
		if (TLN_GetInput (INPUT_A) && scale < MAX_SCALE)
			scale += 1;
		if (TLN_GetInput (INPUT_B) && scale > MIN_SCALE)
			scale -= 1;

		/* calculate scale factor from fixed point base */
		fgscale = (float)scale/100.0f;
		bgscale = lerp((float)scale, MIN_SCALE,MAX_SCALE, 0.75f,1.5f);

		/* scale dependant lower clipping */
		maxy = 640 - (240*100/scale);
		if (ypos > maxy)
			ypos = maxy;
		
		/* update position */
		bgypos = lerp(scale,MIN_SCALE,MAX_SCALE, 0,80);
		TLN_SetLayerPosition (LAYER_FOREGROUND, xpos*2, ypos);
		TLN_SetLayerPosition (LAYER_BACKGROUND, xpos, bgypos);
		TLN_SetLayerScaling (LAYER_FOREGROUND, fgscale, fgscale);
		TLN_SetLayerScaling (LAYER_BACKGROUND, bgscale, bgscale);

		/* render to the window */
		TLN_DrawFrame (0);
	}

	/* release resources */
	TLN_DeleteTilemap (tilemaps[LAYER_FOREGROUND]);
	TLN_DeleteTilemap (tilemaps[LAYER_BACKGROUND]);
	TLN_Deinit ();

	return 0;
}

/* sky color gradient with raster effect */
static void raster_callback (int line)
{
	if (line <= 152)
	{
		RGB color;
		color.r = lerp (line, 0,152, sky[0].r, sky[1].r);
		color.g = lerp (line, 0,152, sky[0].g, sky[1].g);
		color.b = lerp (line, 0,152, sky[0].b, sky[1].b);
		TLN_SetBGColor (color.r, color.g, color.b);
	}
}
