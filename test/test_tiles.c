#include <stdlib.h>
#include "Tilengine.h"

#define WIDTH	440
#define HEIGHT	260

static void apply_palette(TLN_Tilemap palette_layer, TLN_Tilemap target_layer)
{
	const int numtiles = TLN_GetTilemapCols(palette_layer) * TLN_GetTilemapRows(palette_layer);
	TLN_Tile srctile = TLN_GetTilemapTiles(palette_layer, 0, 0);
	TLN_Tile dsttile = TLN_GetTilemapTiles(target_layer, 0, 0);
	int c;
	for (c = 0; c < numtiles; c += 1)
	{
		if (srctile->index != 0)
			dsttile->palette = srctile->index - 1;
		srctile += 1;
		dsttile += 1;
	}
}

int main(int argc, char* argv[])
{
	int x = 0;

	TLN_Init(WIDTH, HEIGHT, 2, 4, 0);
	TLN_SetLoadPath("assets/cliffs");
	TLN_SetBGColor(0x88, 0xDA, 0xF2);	

	TLN_Tilemap layer_foreground = TLN_LoadTilemap("layer_foreground.tmx", NULL);
	TLN_Tilemap layer_background = TLN_LoadTilemap("layer_background.tmx", NULL);
	TLN_SetLayer(0, NULL, layer_foreground);
	TLN_SetLayer(1, NULL, layer_background);
	TLN_SetLayerPosition(0, 16, 32);

	TLN_Spriteset spriteset = TLN_LoadSpriteset("enemy_eagle.png");
	
	TLN_ConfigSprite(0, spriteset, 0);
	TLN_ConfigSprite(1, spriteset, FLAG_ROTATE | FLAG_FLIPX);
	TLN_ConfigSprite(2, spriteset, FLAG_FLIPX | FLAG_FLIPY);
	TLN_ConfigSprite(3, spriteset, FLAG_ROTATE | FLAG_FLIPY);
	TLN_SetSpritePosition(0, 32, 212);
	TLN_SetSpritePosition(1, 128, 212);
	TLN_SetSpritePosition(2, 224, 212);
	TLN_SetSpritePosition(3, 320, 212);

	TLN_Palette global_palette1 = TLN_ClonePalette(TLN_GetTilesetPalette(TLN_GetTilemapTileset(layer_foreground)));
	TLN_Palette global_palette2 = TLN_ClonePalette(global_palette1);
	TLN_AddPaletteColor(global_palette1, 0, 0, 80, 1, 255);
	TLN_AddPaletteColor(global_palette2, 80, 0, 0, 1, 255);
	TLN_SetGlobalPalette(1, global_palette1);
	TLN_SetGlobalPalette(2, global_palette2);

	/* getters for 2.13.2 */
	printf("Layer at %d,%d\n", TLN_GetLayerX(0), TLN_GetLayerY(0));
	printf("Sprite at %d,%d\n", TLN_GetSpriteX(0), TLN_GetSpriteY(0));
	printf("Palette[1] = %p\n", TLN_GetGlobalPalette(1));

	const int numtiles = TLN_GetTilemapCols(layer_foreground) * TLN_GetTilemapRows(layer_foreground);
	TLN_Tile tile = TLN_GetTilemapTiles(layer_foreground, 0, 0);
	int c;
	for (c = 0; c < numtiles; c += 1)
	{
		if (c % 6 == 0)
			tile->palette = 1;
		else if (c % 3 == 0)
			tile->palette = 2;
		tile += 1;
	}

	TLN_CreateWindow(NULL, CWF_S2);
	//TLN_DisableCRTEffect();
	//TLN_SetLayerTransform(0, 30.0f, WIDTH / 4, HEIGHT / 4, 1.0f, 1.0f);
	//TLN_SetLayerScaling(0, 2.0f, 1.0f);
	TLN_SetLayerPosition(1, 64, 0);
	while (TLN_ProcessWindow())
	{
		x += 1;
		TLN_DrawFrame(0);
	}

	//TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
