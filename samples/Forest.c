/******************************************************************************
*
* Tilengine sample
* 2019 Marc Palacios
* http://www.tilengine.org
*
* This example demosntrates features introduced in Tilengine 2.4.0: 
* layer objects, named sprite animations, layer priorities and sync'd 
* layer movement. It's partially ported from Ansimuz "Sunny Land Forest" 
* asset pack demo.
*
* This code uses two layers of objects for "pops" (or decorations), one in 
* front of sprites and another behind sprites synched to the main layer, 
* a tile-based foreground layer, and two bitmap-based background layers. 
* It also defines two sprite-based named animations.
*
******************************************************************************/

#include "Tilengine.h"

#define HRES	424
#define VRES	240

enum
{
	LAYER_PROPS_FRONT,	/* props in front of sprites */
	LAYER_FOREGROUND,	/* main foreground layer (tiles) */
	LAYER_PROPS,		/* object layer */
	LAYER_MIDDLEGROUND,	/* middle (bitmap) */
	LAYER_BACKGROUND,	/* back ( bitmap) */
	NUM_LAYERS
};

int main(int argc, char* argv[])
{
	int frame = 0;
	TLN_Tilemap foreground;
	TLN_Bitmap middleground, background;
	TLN_ObjectList props_list;
	TLN_Spriteset player;
	TLN_Sequence idle, skip;
	int xworld = 0;
	int xplayer, yplayer;
	int oldx = -1;
	int width, height;
	TLN_ObjectInfo info = {0};
	bool ok;

	TLN_Init(HRES, VRES, NUM_LAYERS, 8, 8);

	/* load assets */
	TLN_SetLogLevel(TLN_LOG_ERRORS);
	TLN_SetLoadPath("assets/forest");
	foreground = TLN_LoadTilemap("map.tmx", "Main Layer");
	middleground = TLN_LoadBitmap("middleground.png");
	background = TLN_LoadBitmap("background.png");
	player = TLN_LoadSpriteset("player");
	props_list = TLN_LoadObjectList("map.tmx", NULL);

	/* setup layers */
	TLN_SetLayer(LAYER_FOREGROUND, NULL, foreground);
	TLN_SetLayerBitmap(LAYER_MIDDLEGROUND, middleground);
	TLN_SetLayerBitmap(LAYER_BACKGROUND, background);
	width = TLN_GetLayerWidth(LAYER_FOREGROUND);
	height = TLN_GetLayerHeight(LAYER_FOREGROUND);

	/* objects layer: add front objects (in front of sprites) */
	printf("pops_list length = %d\n", TLN_GetListNumObjects(props_list));
	ok = TLN_GetListObject(props_list, &info);
	while (ok)
	{
		printf("object id=%d gid=%d x=%d y=%d w=%d h=%d\n", info.id, info.gid, info.x, info.y, info.width, info.height);
		ok = TLN_GetListObject(props_list, NULL);
	}

	/* objects layer: add back objects (behind sprites) */
	TLN_SetLayerObjects(LAYER_PROPS, props_list, NULL);

	/* sync props layer positions to main layer */
	TLN_SetLayerParent(LAYER_PROPS_FRONT, LAYER_FOREGROUND);
	TLN_SetLayerParent(LAYER_PROPS, LAYER_FOREGROUND);

	/* create sprite sequences */
	idle = TLN_CreateSpriteSequence(NULL, player, "player-idle-", 9, 6);
	skip = TLN_CreateSpriteSequence(NULL, player, "player-skip-", 8, 6);
	
	/* setup main player sprite */
	xplayer = 48;
	yplayer = 144;
	TLN_ConfigSprite(0, player, 0);
	TLN_SetSpriteAnimation(0, 0, idle, 0);

	/* create window & main loop */
	TLN_CreateWindow(NULL, 0);
	while (TLN_ProcessWindow())
	{
		TLN_DrawFrame(frame);
		frame += 1;

		/* move 3 pixels right/left main layer */
		if (TLN_GetInput(INPUT_LEFT) && xworld > 0)
			xworld -= 3;
		else if (TLN_GetInput(INPUT_RIGHT) && xworld < width - HRES)
			xworld += 3;

		/* update on change */
		if (xworld != oldx)
		{
			TLN_SetLayerPosition(LAYER_FOREGROUND, xworld, 32);
			TLN_SetLayerPosition(LAYER_MIDDLEGROUND, xworld / 2, 0);
			TLN_SetLayerPosition(LAYER_BACKGROUND, xworld / 3, 0);
			TLN_SetSpritePosition(0, xplayer - xworld, yplayer);
			oldx = xworld;
		}
	}
	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
