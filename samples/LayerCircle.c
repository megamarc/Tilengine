/******************************************************************************
*
* Tilengine sample
* 2023 Marc Palacios
* http://www.tilengine.org
*
* Uses raster effects to create circle-shaped, scalable sliding window
* - button 1 / Z : decrease radius
* - button 2 / X : increase radius
* - d-pad / curs : moves the window
*
******************************************************************************/

#include "Tilengine.h"

#define HRES		480
#define VRES		360
#define RADIUS		200		/* build 400 line circle*/

/* layers */
enum
{
	LAYER_FOREGROUND,
	NUM_LAYERS
};

/* sliding window */
int radius = RADIUS / 2;	/* default radius: half display size */
int x_center = HRES / 2;	/* default center: center of display*/
int y_center = VRES / 2;

/* window edges */
typedef struct
{
	int x1, x2;
}
Edge;

/* holds edges lateral for high resolution circle */
Edge edges[RADIUS*2];

/* forward declarations */
void raster_callback(int line);
void setup_circle(Edge* edges, int r);

/* entry point */
int main(int arg, char* argv[])
{
	/* init & load assets */ 
	TLN_Init(HRES, VRES, NUM_LAYERS, 0, 0);
	TLN_SetLoadPath("assets/shots");
	TLN_SetLayerBitmap(LAYER_FOREGROUND, TLN_LoadBitmap("zss1.png"));
	TLN_SetRasterCallback(raster_callback);
	setup_circle(edges, RADIUS);

	/* setup basic window layer properties */
	TLN_SetLayerWindowColor(LAYER_FOREGROUND, 0, 128, 224, BLEND_NONE);

	/* create window & main loop */
	TLN_CreateWindow(NULL, 0);
	while (TLN_ProcessWindow())
	{
		/* change radius */
		if (TLN_GetInput(INPUT_BUTTON1) && radius > 2)
			radius -= 2;
		else if (TLN_GetInput(INPUT_BUTTON2))
			radius += 2;

		/* slide */
		if (TLN_GetInput(INPUT_LEFT))
			x_center -= 2;
		else if (TLN_GetInput(INPUT_RIGHT))
			x_center += 2;
		if (TLN_GetInput(INPUT_UP))
			y_center -= 2;
		else if (TLN_GetInput(INPUT_DOWN))
			y_center += 2;

		TLN_DrawFrame(0);
	}

	/* release resources */
	TLN_ReleaseWorld();
	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}

/* auxiliar for Bresenham's setup_circle() */
void setup_edge(Edge* edges, int xc, int yc, int x, int y)
{
	edges[yc+y].x1 = edges[yc-y].x1 = xc - x;
	edges[yc+y].x2 = edges[yc-y].x2 = xc + x;
	edges[yc+x].x1 = edges[yc-x].x1 = xc-y;
	edges[yc+x].x2 = edges[yc-x].x2 = xc+y;
}

/* Bresenham circle: 
	https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/ 
*/
void setup_circle(Edge* edges, int r)
{
	int x = 0, y = r;
	int d = 3 - 2 * r;
	setup_edge(edges, r, r, x, y);
	while (y >= x)
	{
		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
			d = d + 4 * x + 6;
		setup_edge(edges, r, r, x, y);
	}
}

/* linear scaling */
int scale(int x, int fx0, int fx1)
{
	return (x * fx1) / fx0;
}

/* called for every scanline. Updates layer window with scaled circle edges */
void raster_callback(int line)
{
	const int y1 = y_center - radius;
	const int y2 = y_center + radius;
	if (line < y1 || line > y2)
		TLN_SetLayerWindow(LAYER_FOREGROUND, 0, 0, 0, 0, false);
	else
	{
		const int row = scale(line - y1, radius, RADIUS);
		TLN_SetLayerWindow
		(
			LAYER_FOREGROUND, 
			x_center - radius + scale(edges[row].x1, RADIUS, radius),
			0, 
			x_center - radius + scale(edges[row].x2, RADIUS, radius),
			VRES, 
			false
		);
	}
}
