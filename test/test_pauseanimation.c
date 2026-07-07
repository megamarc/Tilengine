/******************************************************************************
*
* Tilengine sample
* 2026 Marc Palacios
* https://www.tilengine.org

* Demonstrates how to pause and resume aniamtions, both in sprites and in tilesets
*
******************************************************************************/

#include "Tilengine.h"

int main(int argc, char* argv[])
{
	TLN_Init(400, 240, 1, 1, 0);

	/* sprite animation */
	TLN_Spriteset ss = TLN_LoadSpriteset("assets/forest/atlas");
	TLN_Sequence sequence = TLN_CreateSpriteSequence(NULL, ss, "player-idle/player-idle-", 5);
	TLN_ConfigSprite(0, ss, 0);
	TLN_SetSpriteAnimation(0, sequence, 0);

	/* tile animation */
	TLN_Tilemap tm = TLN_LoadTilemap("assets/sonic/Sonic_md_fg1.tmx", NULL);
	TLN_Tileset ts = TLN_GetTilemapTileset(tm);
	TLN_SetLayerTilemap(0, tm);

	/* get number of animations embedded in the tileset */
	int numAnimations = TLN_GetTilesetNumAnimations(ts);

	/* after 2 seconds: pause sprite animation and all animations in main tileset */
	/* after 4 seconds: pause sprite animation and all animations in main tileset */

	bool paused = false;
	bool pressed = false;

	uint32_t t0 = TLN_GetTicks();
	TLN_CreateWindow(NULL, CWF_NEAREST);
	while (TLN_ProcessWindow())
	{
		/* toggle animations by pressing button 1 */
		if (TLN_GetInput(INPUT_BUTTON1))
		{
			if (!pressed)
			{
				pressed = true;
				paused = !paused;
				if (paused)
				{
					TLN_PauseSpriteAnimation(0);
					for (int c = 0; c < numAnimations; c++)
						TLN_PauseTilesetAnimation(ts, c);
				}
				else
				{
					TLN_ResumeSpriteAnimation(0);
					for (int c = 0; c < numAnimations; c++)
						TLN_ResumeTilesetAnimation(ts, c);
				}
			}
		}
		else
			pressed = false;

		TLN_DrawFrame(0);
	}

	TLN_DeleteTilemap(tm);
	TLN_DeleteSpriteset(ss);
	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
