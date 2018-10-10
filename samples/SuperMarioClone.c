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

int main (int argc, char* argv[])
{
	TLN_SequencePack sp;
	TLN_Sequence seq_walking;
	TLN_Spriteset spriteset;
	TLN_Tilemap tilemaps[MAX_LAYER];
	int frame = 0;
	int player_x = -16;
	int player_y = 160;

	/* basic setup */
	TLN_Init (WIDTH, HEIGHT, MAX_LAYER,1,3);
	TLN_SetBGColor (0, 96, 184);

	/* load resources */
	TLN_SetLoadPath ("assets/smw");
	tilemaps[LAYER_FOREGROUND] = TLN_LoadTilemap ("smw_foreground.tmx", NULL);
	tilemaps[LAYER_BACKGROUND] = TLN_LoadTilemap ("smw_background.tmx", NULL);
	TLN_SetLayer (LAYER_FOREGROUND, NULL, tilemaps[LAYER_FOREGROUND]);
	TLN_SetLayer (LAYER_BACKGROUND, NULL, tilemaps[LAYER_BACKGROUND]);
	TLN_SetLayerPosition (LAYER_FOREGROUND, 0,48);
	TLN_SetLayerPosition (LAYER_BACKGROUND, 0,80);

	/* setup sprite */
	spriteset = TLN_LoadSpriteset ("smw_sprite");
	TLN_SetSpriteSet (0, spriteset);
	TLN_SetSpritePicture (0, 0);
	TLN_SetSpritePosition (0, player_x, player_y);

	/* setup animations */
	sp = TLN_LoadSequencePack ("sequences.sqx");
	seq_walking = TLN_FindSequence (sp, "seq_walking");
	TLN_SetSpriteAnimation (2, 0, seq_walking, 0);

	/* main loop */
	TLN_CreateWindow ("overlay.bmp", CWF_VSYNC);
	while (TLN_ProcessWindow())
	{
		player_x += 1;
		if (player_x >= WIDTH)
			player_x = -16;
		TLN_SetSpritePosition (0, player_x, player_y);
		TLN_DrawFrame (frame++);
	}

	/* deinit */
	TLN_DeleteTilemap (tilemaps[LAYER_FOREGROUND]);
	TLN_DeleteTilemap (tilemaps[LAYER_BACKGROUND]);
	TLN_DeleteSequencePack (sp);
	TLN_Deinit ();

	return 0;
}
