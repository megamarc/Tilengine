#include <stdio.h>
#include "Tilengine.h"
// #include "../src/sdl/SDL2/SDL_timer.h"

#define WIDTH	400
#define HEIGHT	240

#define FPS	60

const DELAY = 1000.0f/FPS;

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

int main (int argc, char* argv[])
{
	TLN_Sequence seq_walking;
	TLN_Spriteset spriteset;
	TLN_Tilemap foreground, background;
	int player_x = -16;
	int player_y = 160;

	/* basic setup */
	TLN_Init (WIDTH, HEIGHT, MAX_LAYER,1,0);
	TLN_SetBGColor (0, 96, 184);

	/* load resources */
	TLN_SetLoadPath ("assets/smw");
	foreground = TLN_LoadTilemap ("smw_foreground.tmx", NULL);
	background = TLN_LoadTilemap ("smw_background.tmx", NULL);
	TLN_SetLayerTilemap (LAYER_FOREGROUND, foreground);
	TLN_SetLayerTilemap (LAYER_BACKGROUND, background);
	TLN_SetLayerPosition (LAYER_FOREGROUND, 0,48);
	TLN_SetLayerPosition (LAYER_BACKGROUND, 0,80);

	/* setup sprite */
	spriteset = TLN_LoadSpriteset ("smw_sprite");
	TLN_SetSpriteSet (0, spriteset);
	TLN_SetSpritePicture (0, 0);
	TLN_SetSpritePosition (0, player_x, player_y);

	/* setup animations */
	seq_walking = TLN_CreateSpriteSequence (NULL, spriteset, "walking", 6);
	TLN_SetSpriteAnimation (0, seq_walking, 0);

	/* main loop */
	TLN_CreateWindow (NULL, 0);

	// We will cap the FPS to 60 for people having a screen with a refresh rate greater than 60Hz
	float timeStart = 0.00;
	float timeFinish = 0.00;
	float delta = 0.00;

	while (TLN_ProcessWindow())
	{
		timeStart = TLN_GetTicks();
		delta = timeStart - timeFinish;
		player_x += 1;
		if (player_x >= WIDTH)
			player_x = -16;
		TLN_SetSpritePosition (0, player_x, player_y);
		TLN_DrawFrame (0);
		timeFinish = TLN_GetTicks();
		delta = timeFinish - timeStart;
		// Capping FPS to 60
		if(delta < DELAY)
			TLN_Delay(DELAY - delta);
		
	}

	/* deinit */
	TLN_DeleteTilemap (foreground);
	TLN_DeleteTilemap (background);
	TLN_Deinit ();

	return 0;
}
