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
	LAYER_PROPS_BACK,	/* props behind sprites */
	LAYER_MIDDLEGROUND,	/* middle (bitmap) */
	LAYER_BACKGROUND,	/* back ( bitmap) */
	NUM_LAYERS
};

/* ported from PhaserEngine demo code: x and y are tile coordinates (16 pixels side)  */
static void addProp(TLN_ObjectList list, TLN_Spriteset spriteset, double x, double y, char* name)
{
	TLN_AddSpriteToList(list, spriteset, name, 0, (int)(x * 16), (int)(y * 16));
}

int main(int argc, char* argv[])
{
	int frame = 0;
	TLN_Tilemap foreground;
	TLN_Bitmap middleground, background;
	TLN_ObjectList props_list_front, props_list;
	TLN_Spriteset atlas_props;
	TLN_Spriteset player;
	TLN_Sequence idle, skip;
	int xworld = 0;
	int xplayer, yplayer;
	int oldx = -1;
	int width, height;

	TLN_Init(HRES, VRES, NUM_LAYERS, 8, 8);

	/* load assets */
	TLN_SetLoadPath("assets/forest");
	atlas_props = TLN_LoadSpriteset("atlas-props");
	foreground = TLN_LoadTilemap("map.tmx", "Main Layer");
	middleground = TLN_LoadBitmap("middleground.png");
	background = TLN_LoadBitmap("background.png");
	player = TLN_LoadSpriteset("player");

	/* setup layers */
	TLN_SetLayer(LAYER_FOREGROUND, NULL, foreground);
	TLN_SetLayerBitmap(LAYER_MIDDLEGROUND, middleground);
	TLN_SetLayerBitmap(LAYER_BACKGROUND, background);
	width = TLN_GetLayerWidth(LAYER_FOREGROUND);
	height = TLN_GetLayerHeight(LAYER_FOREGROUND);

	/* objects layer: add front objects (in front of sprites) */
	props_list_front = TLN_CreateObjectList();
	addProp(props_list_front, atlas_props,  16, 12.7, "rock");
	addProp(props_list_front, atlas_props,   2, 12.0, "plant");
	addProp(props_list_front, atlas_props,  23, 12.0, "plant");
	addProp(props_list_front, atlas_props,  53, 11.7, "rock");
	addProp(props_list_front, atlas_props, 150, 12.0, "plant");
	addProp(props_list_front, atlas_props, 152, 12.0, "plant");
	addProp(props_list_front, atlas_props, 143, 12.0, "plant");
	addProp(props_list_front, atlas_props, 119, 12.0, "plant");
	addProp(props_list_front, atlas_props, 122, 12.5, "rock");
	TLN_SetLayerObjects(LAYER_PROPS_FRONT, props_list_front, atlas_props, width, height);
	TLN_SetLayerPriority(LAYER_PROPS_FRONT, true);

	/* objects layer: add back objects (behind sprites) */
	props_list = TLN_CreateObjectList();
	addProp(props_list, atlas_props,   1,  0.2, "tree");
	addProp(props_list, atlas_props,  11, 10.3, "mushroom-red");
	addProp(props_list, atlas_props,   3,  0.0, "vine");
	addProp(props_list, atlas_props,  25,  0.0, "vine");
	addProp(props_list, atlas_props,  17, 11.0, "mushroom-brown");
	addProp(props_list, atlas_props, 120,  0.2, "tree");
	addProp(props_list, atlas_props, 146,  2.7, "house");
	addProp(props_list, atlas_props, 130,  0.0, "vine");
	addProp(props_list, atlas_props, 136,  0.0, "vine");
	addProp(props_list, atlas_props, 144, 11.3, "mushroom-red");
	addProp(props_list, atlas_props, 140, 11.3, "mushroom-brown");
	TLN_SetLayerObjects(LAYER_PROPS_BACK, props_list, atlas_props, width, height);

	/* sync props layer positions to main layer */
	TLN_SetLayerParent(LAYER_PROPS_FRONT, LAYER_FOREGROUND);
	TLN_SetLayerParent(LAYER_PROPS_BACK, LAYER_FOREGROUND);

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
