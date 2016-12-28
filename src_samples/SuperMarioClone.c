#include <stdio.h>
#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

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
Layer;

static Layer layers[MAX_LAYER];

/* helper for loading a related tileset + tilemap and configure the appropiate layer */
static void LoadLayer (int index, char* name)
{
	char filename[64];
	Layer* layer = &layers[index];

	/* load tileset */
	sprintf (filename, "%s.tsx", name);
	layer->tileset = TLN_LoadTileset (filename);

	/* load tilemap */
	sprintf (filename, "%s.tmx", name);
	layer->tilemap = TLN_LoadTilemap (filename, NULL);

	TLN_SetLayer (index, layer->tileset, layer->tilemap);
}

/* helper for freeing a tileset + tilemap */ 
static void FreeLayer (int index)
{
	Layer* layer = &layers[index];
	
	TLN_DeleteTileset (layer->tileset);
	TLN_DeleteTilemap (layer->tilemap);
}

int main (int argc, char* argv[])
{
	TLN_SequencePack sp;
	TLN_Sequence seq_coin;
	TLN_Sequence seq_question;
	TLN_Sequence seq_walking;
	TLN_Spriteset spriteset;
	int frame = 0;
	int player_x = -16;
	int player_y = 160;

	/* basic setup */
	TLN_Init (WIDTH, HEIGHT, MAX_LAYER,1,3);
	TLN_CreateWindow ("overlay.bmp", CWF_VSYNC);
	TLN_SetBGColor (0, 96, 184);
	TLN_SetLoadPath ("assets");

	/* setup layers */
	LoadLayer (LAYER_FOREGROUND, "smw_foreground");
	LoadLayer (LAYER_BACKGROUND, "smw_background");
	TLN_SetLayerPosition (LAYER_FOREGROUND, 0,48);
	TLN_SetLayerPosition (LAYER_BACKGROUND, 0,80);

	/* setup sprite */
	spriteset = TLN_LoadSpriteset ("smw_sprite");
	TLN_SetSpriteSet (0, spriteset);
	TLN_SetSpritePicture (0, 0);
	TLN_SetSpritePosition (0, player_x, player_y);

	/* setup animations */
	sp = TLN_LoadSequencePack ("sequences.sqx");
	seq_coin = TLN_FindSequence (sp, "seq_coin");
	seq_question = TLN_FindSequence (sp, "seq_question");
	seq_walking = TLN_FindSequence (sp, "seq_walking");
	TLN_SetTilesetAnimation (0, LAYER_FOREGROUND, seq_coin);
	TLN_SetTilesetAnimation (1, LAYER_FOREGROUND, seq_question);
	TLN_SetSpriteAnimation (2, 0, seq_walking, 0);

	/* main loop */
	while (TLN_ProcessWindow())
	{
		player_x += 1;
		if (player_x >= WIDTH)
			player_x = -16;
		TLN_SetSpritePosition (0, player_x, player_y);
		TLN_DrawFrame (frame++);
	}

	/* deinit */
	FreeLayer (LAYER_FOREGROUND);
	FreeLayer (LAYER_BACKGROUND);
	TLN_DeleteSequencePack (sp);
	TLN_DeleteWindow ();
	TLN_Deinit ();

	return 0;
}
