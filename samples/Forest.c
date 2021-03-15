/******************************************************************************
*
* Tilengine sample
* 2021 Marc Palacios
* http://www.tilengine.org

*
******************************************************************************/

#include "Tilengine.h"

#define HRES	424
#define VRES	240

/* layers, must mach "map.tmx" layer structure! */
enum
{
	LAYER_PROPS,		/* object layer */
	LAYER_FOREGROUND,	/* main foreground layer (tiles) */
	LAYER_MIDDLEGROUND,	/* middle (bitmap) */
	LAYER_BACKGROUND,	/* back ( bitmap) */
	NUM_LAYERS
};

int main(int argc, char* argv[])
{
	TLN_Spriteset atlas;
	TLN_Sequence idle, skip;
	int xworld = 0;
	int xplayer, yplayer;
	int oldx = -1;
	int width, height;
	char* respack = NULL;
	char* passkey = NULL;
	
	/* get arguments */
	if (argc > 1)
		respack = argv[1];
	if (argc > 2)
		passkey = argv[2];

	TLN_Init(HRES, VRES, NUM_LAYERS, 8, 0);

	/* load assets */
	TLN_SetLogLevel(TLN_LOG_ERRORS);
	if (respack != NULL)
	{
		bool ok = TLN_OpenResourcePack(respack, passkey);
		if (!ok)
		{
			printf("Cannot open resource pack!\n");
			TLN_Deinit();
			return 0;
		}
		TLN_SetLoadPath("forest");
	}
	else
		TLN_SetLoadPath("assets/forest");
	
	/* load world starting at layer 0 and get dimensions */ 
	TLN_LoadWorld("map.tmx", 0);
	width = TLN_GetLayerWidth(LAYER_FOREGROUND);
	height = TLN_GetLayerHeight(LAYER_FOREGROUND);

	/* create sprite sequences */
	atlas = TLN_LoadSpriteset("atlas.png");
	idle = TLN_CreateSpriteSequence(NULL, atlas, "player-idle/player-idle-", 6);
	skip = TLN_CreateSpriteSequence(NULL, atlas, "player-skip/player-skip-", 6);
	
	/* setup main player sprite */
	xplayer = 48;
	yplayer = 144;
	TLN_ConfigSprite(0, atlas, 0);
	TLN_SetSpriteAnimation(0, idle, 0);
	TLN_SetSpriteWorldPosition(0, xplayer, yplayer);

	/* create window & main loop */
	TLN_CreateWindow(NULL, 0);
	while (TLN_ProcessWindow())
	{
		TLN_DrawFrame(0);

		/* move 3 pixels right/left main layer */
		if (TLN_GetInput(INPUT_LEFT) && xworld > 0)
			xworld -= 3;
		else if (TLN_GetInput(INPUT_RIGHT) && xworld < width - HRES)
			xworld += 3;

		/* update on change */
		if (xworld != oldx)
		{
			TLN_SetWorldPosition(xworld, 0);
			oldx = xworld;
		}
	}

	/* release resources */
	TLN_ReleaseWorld();
	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
