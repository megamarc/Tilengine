/******************************************************************************
*
* Tilengine sample
* 2015 Marc Palacios
* http://www.tilengine.org
*
* This example shows a classic Mode 7 perspective projection plane like the 
* one seen in SNES games like Super Mario Kart. It uses a single transformed
* layer with a raster effect for setting the scaling factor on each line
*
******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Tilengine.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIDTH	400
#define HEIGHT	240

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

enum
{
	MAP_HORIZON,
	MAP_TRACK,
	MAX_MAP
};

static TLN_Tilemap road, horizon;

static void raster_callback (int line);
static void main_loop(uint32_t frame);

int angle = 0;			/* angle */
float x = -136;			/* horizontal position */
float y = 336;			/* vertical position */
float speed = 0;		/* speed */
float accel = 0.2f;		/* acceleration */
float maxspeed = 2.0f;	/* max speed */

/* entry point */
int main (int argc, char* argv[])
{
	/* setup engine */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, 0, 0);
	TLN_SetRasterCallback (raster_callback);
	TLN_SetBGColor (0,0,0);

	/* load resources*/
	TLN_SetLoadPath ("assets/smk");
	road = TLN_LoadTilemap ("track1.tmx", NULL);
	horizon = TLN_LoadTilemap ("track1_bg.tmx", NULL);

	/* create window & main loop, block until window closes */
	TLN_CreateWindow(NULL, CWF_FULLSCREEN);
	TLN_SetMainTask(main_loop);

	/* deinit */
	TLN_DeleteTilemap (road);
	TLN_DeleteTilemap (horizon);
	TLN_DeleteWindow ();
	TLN_Deinit ();
	return 0;
}

/* main loop delegate, called every frame */
static void main_loop(uint32_t frame)
{
	/* draws top horizon based on viewing angle */
	TLN_SetLayerTilemap (LAYER_FOREGROUND, horizon);
	TLN_SetLayerTilemap (LAYER_BACKGROUND, horizon);
	TLN_SetLayerPosition (LAYER_FOREGROUND, lerp(angle*2, 0,360, 0,256), 24);
	TLN_SetLayerPosition (LAYER_BACKGROUND, lerp(angle, 0,360, 0,256), 0);
	TLN_ResetLayerMode (LAYER_BACKGROUND);

	/* rotate left/right */		
	if (TLN_GetInput (INPUT_LEFT))
		angle -= 2;
	else if (TLN_GetInput (INPUT_RIGHT))
		angle += 2;
	
	/* move forward */
	if (TLN_GetInput (INPUT_UP))
	{
		speed += accel;
		if (speed > maxspeed)
			speed = maxspeed;
	}
	else if (speed >= accel)
		speed -= accel;
	
	/* move backwards */
	if (TLN_GetInput (INPUT_DOWN))
	{
		speed -= accel;
		if (speed < -maxspeed)
			speed = -maxspeed;
	}
	else if (speed <= -accel)
		speed += accel;
	
	if (fabs(speed) < 0.2f)
		speed = 0;

	/* normalize angle in range 0 - 360 */
	angle = (angle + 360) % 360;	

	/* move according to speed and direction */
	if (speed != 0)
	{
		float radians = (angle * M_PI) / 180.0f;
		x += sin(radians) * speed;
		y -= cos(radians) * speed;
	}
}

/* raster callback (virtual HBLANK) */
static void raster_callback (int line)
{
	if (line == 24)
	{
		TLN_SetLayerTilemap (LAYER_BACKGROUND, road);
		TLN_SetLayerPosition (LAYER_BACKGROUND, (int)x, (int)y);
		TLN_DisableLayer (LAYER_FOREGROUND);
	}

	if (line >= 24)
	{
		float scale = lerp (line, 24,HEIGHT, 0.2f,5.0f);
		TLN_SetLayerTransform (LAYER_BACKGROUND, (float)angle, WIDTH/2, HEIGHT, scale, scale);
	}
}
