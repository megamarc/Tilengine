#include <stdlib.h>
#include <stdio.h>
#include "Tilengine.h"

#define HRES	400
#define VRES	240
#define SIZE	(32*48)
#define SPRITES	250
#define FRAMES	1000

TLN_Palette palette;
TLN_Tileset tileset;
TLN_Tilemap tilemap;
TLN_Spriteset spriteset;
int frame;
int pixels;
uint32_t t0, t1;

static TLN_Palette CreateRandomPalette (void);
static TLN_Tileset CreateRandomTileset (int ntiles, int coverage, TLN_Palette palette);
static TLN_Tilemap CreateRandomTilemap (int rows, int cols, int maxtile, int coverage);
static TLN_Spriteset CreateRandomSpriteset (int coverage, TLN_Palette palette);
static void FillRandomData (uint8_t* data, int len, int coverage);
static int Profile (void);

int main (int argc, char* argv[])
{
	int c;
	int result;
	bool ok;
	uint8_t* framebuffer;
	int pcen;
	int bpp;

	printf ("\nTilengine benchmark tool\n");
	printf ("Written by Megamarc - %s %s\n", __DATE__, __TIME__);
	printf ("http://www.tilengine.org\n\n");

	if (argc != 3)
	{
		printf ("Syntax: benchmark format percent\n");
		printf ("  format : pixel format (16 or 32)\n");
		printf ("  percent: pixel coverage percentage (0-100)\n");
		printf ("    Real game is about 70%%\n");
		return 0;
	}

	bpp = atoi (argv[1]);
	pcen = atoi (argv[2]);

	if (bpp != 16 && bpp != 32)
		return 0;

	/* setup engine */
	TLN_InitBPP (HRES, VRES, bpp, 1, SPRITES, 0);
	framebuffer = malloc(HRES*VRES*bpp/8);
	TLN_SetRenderTarget (framebuffer, HRES*bpp/8);
	//TLN_CreateWindow (NULL, 0);

	/* create assets */
	palette = CreateRandomPalette ();
	tileset = CreateRandomTileset (1024, pcen, palette);
	tilemap = CreateRandomTilemap (50,30, 1023, pcen);
	spriteset = CreateRandomSpriteset (pcen, palette);
	
	/* setup layer */
	ok = TLN_SetLayer (0, tileset, tilemap);
	pixels = FRAMES*HRES*VRES;

	printf ("Normal layer..........");
	result = Profile ();
	printf ("%10d pixels/s\n", result);

	printf ("Scaling layer.........");
	TLN_SetLayerScaling (0, 2.0f, 2.0f);
	result = Profile ();
	printf ("%10d pixels/s\n", result);

	printf ("Affine layer..........");
	TLN_SetLayerTransform (0, 45.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	result = Profile ();
	printf ("%10d pixels/s\n", result);

	printf ("Blend layer...........");
	TLN_ResetLayerMode (0);
	TLN_SetLayerBlendMode (0, BLEND_MIX, 128);
	result = Profile ();
	printf ("%10d pixels/s\n", result);

	printf ("Scaling blend layer...");
	TLN_SetLayerScaling (0, 2.0f, 2.0f);
	result = Profile ();
	printf ("%10d pixels/s\n", result);

	printf ("Affine blend layer....");
	TLN_SetLayerTransform (0, 45.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	result = Profile ();
	printf ("%10d pixels/s\n", result);

	TLN_DisableLayer (0);

	/* setup sprites */
	for (c=0; c<SPRITES; c++)
	{
		int y = c/25;
		int x = c%25;
		ok = TLN_ConfigSprite (c, spriteset, FLAG_NONE);
		ok = TLN_SetSpritePicture (c, 0);
		TLN_SetSpritePosition (c, x*15, y*21);
	}
	pixels = FRAMES*SPRITES*SIZE;

	printf ("Normal sprites........");
	result = Profile ();
	printf ("%10d pixels/s\n", result);

	printf ("Colliding sprites.....");
	for (c=0; c<SPRITES; c++)
		TLN_EnableSpriteCollision (c, true);
	result = Profile ();
	printf ("%10d pixels/s\n", result);

	free (framebuffer);
	TLN_DeleteTilemap (tilemap);
	TLN_DeleteTileset (tileset);
	TLN_Deinit ();

	return 0;
}

static int Profile (void)
{
	frame = 0;

	t0 = TLN_GetTicks ();
	while (TLN_ProcessWindow () && frame<FRAMES)
		TLN_UpdateFrame (frame++);
		//TLN_DrawFrame (frame++);
	t1 = TLN_GetTicks () - t0;

	return pixels/t1*1000;
}

static void FillRandomData (uint8_t* data, int len, int coverage)
{
	int c;

	for (c=0; c<len; c++)
	{
		if (rand()%100 < coverage)
			data[c] = (rand()%255) + 1;
		else
			data[c] = 0;
	}
}

static TLN_Palette CreateRandomPalette (void)
{
	TLN_Palette palette;
	int c;

	palette = TLN_CreatePalette (256);
	for (c=0; c<256; c++)
		TLN_SetPaletteColor (palette, c, rand()&255, rand()&255, rand()&255);

	return palette;
}

static TLN_Tileset CreateRandomTileset (int ntiles, int coverage, TLN_Palette palette)
{
	TLN_Tileset tileset;
	int c;
	uint8_t* data;

	data = malloc (8*8);

	tileset = TLN_CreateTileset (ntiles, 8,8, palette);

	for (c=0; c<ntiles; c++)
	{
		FillRandomData (data, 64, coverage);
		TLN_SetTilesetPixels (tileset, c, data, 8);
	}

	free (data);
	return tileset;
}

static TLN_Tilemap CreateRandomTilemap (int rows, int cols, int maxtile, int coverage)
{
	TLN_Tilemap tilemap;
	Tile tile = {0, FLAG_NONE};
	int r,c;
	int size;

	tilemap = TLN_CreateTilemap (rows, cols, NULL);
	size = rows*cols;
	for (r=0; r<rows; r++)
	{
		for (c=0; c<cols; c++)
		{
			if (rand()%100 < coverage)
				tile.index = (rand()%maxtile) + 1;
			else
				tile.index = 0;
			TLN_SetTilemapTile (tilemap, r,c, &tile);
		}
	}
	return tilemap;
}

static TLN_Spriteset CreateRandomSpriteset (int coverage, TLN_Palette palette)
{
	TLN_Spriteset spriteset;
	uint8_t* data;
	TLN_Rect rect;

	data = malloc (32*48);
	FillRandomData (data, 32*48, coverage);
	rect.x = 0;
	rect.y = 0;
	rect.w = 32;
	rect.h = 48;
	spriteset = TLN_CreateSpriteset (1, &rect, data, 32,48, 32, palette);
	free (data);

	return spriteset;
}