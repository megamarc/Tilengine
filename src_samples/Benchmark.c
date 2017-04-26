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
int pixels;

static TLN_Palette CreateRandomPalette (void);
static TLN_Tileset CreateRandomTileset (int ntiles, int coverage, TLN_Palette palette);
static TLN_Tilemap CreateRandomTilemap (int rows, int cols, int maxtile, int coverage);
static TLN_Spriteset CreateRandomSpriteset (int coverage, TLN_Palette palette);
static void FillRandomData (uint8_t* data, int len, int coverage);
static uint32_t Profile (void);

int main (int argc, char* argv[])
{
	int c;
	bool ok;
	uint8_t* framebuffer;
	uint32_t version;
	int pcen;
	int bpp;

	version = TLN_GetVersion ();
	printf ("\nTilengine benchmark tool\n");
	printf ("Written by Megamarc - %s %s\n", __DATE__, __TIME__);
	printf ("Library version: %d.%d.%d\n", (version >> 16)&0xFF, (version >> 8)&0xFF, version&0xFF);
	printf ("http://www.tilengine.org\n\n");

	if (argc != 2)
	{
		printf ("Syntax: benchmark format\n");
		printf ("  format : pixel format (16 or 32)\n");
		return 0;
	}

	bpp = atoi (argv[1]);
	pcen = 70;

	if (bpp != 16 && bpp != 32)
		return 0;

	/* setup engine */
	TLN_InitBPP (HRES, VRES, bpp, 1, SPRITES, 0);
	framebuffer = malloc(HRES*VRES*bpp/8);
	TLN_SetRenderTarget (framebuffer, HRES*bpp/8);
	//TLN_CreateWindow (NULL, 0);

	/* create assets */
	palette = CreateRandomPalette ();
	//tileset = CreateRandomTileset (1024, pcen, palette);
	//tilemap = CreateRandomTilemap (50,30, 1023, pcen);
	tileset = TLN_LoadTileset ("TF4_bg1.tsx");
	tilemap = TLN_LoadTilemap ("TF4_bg1.tmx", NULL);

	spriteset = CreateRandomSpriteset (pcen, palette);
	
	/* setup layer */
	ok = TLN_SetLayer (0, tileset, tilemap);
	pixels = HRES*VRES;

	printf ("Normal layer..........");
	Profile ();

	printf ("Scaling layer.........");
	TLN_SetLayerScaling (0, 2.0f, 2.0f);
	Profile ();

	printf ("Affine layer..........");
	TLN_SetLayerTransform (0, 45.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	Profile ();

	printf ("Blend layer...........");
	TLN_ResetLayerMode (0);
	TLN_SetLayerBlendMode (0, BLEND_MIX, 128);
	Profile ();

	printf ("Scaling blend layer...");
	TLN_SetLayerScaling (0, 2.0f, 2.0f);
	Profile ();

	printf ("Affine blend layer....");
	TLN_SetLayerTransform (0, 45.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	Profile ();

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
	pixels = SPRITES*SIZE;

	printf ("Normal sprites........");
	Profile ();

	printf ("Colliding sprites.....");
	for (c=0; c<SPRITES; c++)
		TLN_EnableSpriteCollision (c, true);
	Profile ();

	free (framebuffer);
	TLN_DeleteTilemap (tilemap);
	TLN_DeleteTileset (tileset);
	TLN_Deinit ();

	return 0;
}

static uint32_t Profile (void)
{
	uint32_t t0, elapse;
	uint32_t frame = 0;
	uint32_t result;

	t0 = TLN_GetTicks ();
	do
	{
		TLN_UpdateFrame (frame++);
	}
	while (frame < FRAMES);
	elapse = TLN_GetTicks () - t0;
	result = frame*pixels/elapse;

	printf (" %3u.%03u Mpixels/s\n", result/1000, result%1000);
	return result;
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