#include "Tilengine.h"
#include "..\Common\Sin.h"

#define WIDTH	256
#define HEIGHT	224

/* linear interploation */
#define lerp(x, x0,x1, fx0,fx1) \
	fx0 + (fx1-fx0)*(x-x0)/(x1-x0)

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

layer_t layers[MAX_LAYER];
static int frame;
static int xpos, ypos;
static int width,height;

static void raster_callback (int line);
static void Scroll (int dx, int dy, int idblock);

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
int main (int argc, char* argv[])
{
	/* setup engine */
	TLN_Init (WIDTH, HEIGHT, 2,0,0);
	TLN_SetBGColor (0,128,238);
	TLN_CreateWindow ("overlay.bmp", CWF_VSYNC);
	TLN_SetRasterCallback (raster_callback);

	/* load resources*/
	LoadLayer (LAYER_FOREGROUND, "DKC_fg");
	LoadLayer (LAYER_BACKGROUND, "DKC_bg");

	xpos = 0;
	ypos = 1600;
	width = TLN_GetTilemapCols (layers[LAYER_FOREGROUND].tilemap) * TLN_GetTileWidth (layers[LAYER_FOREGROUND].tileset);
	height = TLN_GetTilemapRows (layers[LAYER_FOREGROUND].tilemap) * TLN_GetTileHeight (layers[LAYER_FOREGROUND].tileset);
	BuildSinTable ();
	
	/* main loop */
	while (TLN_ProcessWindow ())
	{
		int dx = 0;
		int dy = 0;

		TLN_SetLayerPosition (LAYER_FOREGROUND, xpos, ypos);

		if (TLN_GetInput (INPUT_LEFT) && xpos > 0)
			dx = -1;
		else if (TLN_GetInput (INPUT_RIGHT) && xpos + WIDTH < width)
			dx = 1;
		if (TLN_GetInput (INPUT_UP) && ypos > 0)
			dy = -1;
		else if (TLN_GetInput (INPUT_DOWN) && ypos + HEIGHT < height)
			dy = 1;

		Scroll (dx, dy, 536);
		TLN_SetLayerPosition (LAYER_FOREGROUND, xpos, ypos);
		
		/* render to the window */
		TLN_DrawFrame (frame);
		frame++;
	}

	/* release resources */
	FreeLayer (LAYER_FOREGROUND);
	FreeLayer (LAYER_BACKGROUND);
	TLN_DeleteWindow ();
	TLN_Deinit ();

	return 0;
}

static void raster_callback (int line)
{
	int x = -1;
	int x1 = xpos/2;
	int x2 = (x1*3)/2;

	/* background layer */
	if (line < 72)
		x = lerp (line, 0,72, x2,x1);
	else if (line == 72)
		x = x1;
	else if (line >= 144)
		x = lerp (line, 144,256, x1,x2);

	if (x != -1)
		TLN_SetLayerPosition (LAYER_BACKGROUND, x,0);

	/* foreground layer */
	x1 = CalcSin(frame+line*3,5);
	x2 = CalcSin(frame*2+line,3);
	TLN_SetLayerPosition (LAYER_FOREGROUND, xpos+x1, ypos+x2);
}

void Scroll (int dx, int dy, int idblock)
{
	int dstx, dsty, c;
	TLN_TileInfo info;
	int tilew = TLN_GetTileWidth (layers[LAYER_FOREGROUND].tileset);
	int tileh = TLN_GetTileHeight (layers[LAYER_FOREGROUND].tileset);

	/* left */
	if (dx < 0)
	{
		dstx = dx;
		for (c=0; c<HEIGHT; c+=tileh)
		{
			TLN_GetLayerTile (LAYER_FOREGROUND, dstx, c, &info);
			if (info.index==idblock)
			{
				dx = 0;
				break;
			}
		}
	}

	/* right */
	if (dx > 0)
	{
		dstx = dx + WIDTH;
		for (c=0; c<HEIGHT; c+=tileh)
		{
			TLN_GetLayerTile (LAYER_FOREGROUND, dstx, c, &info);
			if (info.index==idblock)
			{
				dx = 0;
				break;
			}
		}
	}

	/* up */
	if (dy < 0)
	{
		dsty = dy;
		for (c=0; c<WIDTH; c+=tilew)
		{
			TLN_GetLayerTile (LAYER_FOREGROUND, c, dsty, &info);
			if (info.index==idblock)
			{
				dy = 0;
				break;
			}
		}
	}

	/* down */
	if (dy > 0)
	{
		dsty = dy + HEIGHT;
		for (c=0; c<WIDTH; c+=tilew)
		{
			TLN_GetLayerTile (LAYER_FOREGROUND, c, dsty, &info);
			if (info.index==idblock)
			{
				dy = 0;
				break;
			}
		}
	}

	xpos += dx;
	ypos += dy;
}