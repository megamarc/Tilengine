#include "Tilengine.h"
// #include "../src/sdl/SDL2/SDL_timer.h"

#define WIDTH	400
#define HEIGHT	240

#define FPS	60

const DELAY = 1000.0f/FPS;

/* entry point */
int main (int argc, char* argv[])
{
	TLN_Tilemap tilemap;
	int frame = 0;

	/* setup engine */
	TLN_Init (WIDTH, HEIGHT, 1,0,0);

	/* load layer */
	tilemap = TLN_LoadTilemap ("assets/sonic/Sonic_md_fg1.tmx", NULL);
	
	/* setup the layer */
	TLN_SetLayer (0, NULL, tilemap);
	TLN_SetBGColor (32,32,128);

	/* main loop */
	TLN_CreateWindow (NULL, 0);

	// We will cap the FPS to 60 for people having a screen with a refresh rate greater than 60Hz
	float timeStart = 0.00;
	float timeFinish = 0.00;
	float delta = 0.00;

	while (TLN_ProcessWindow ())
	{
		timeStart = TLN_GetTicks();
		delta = timeStart - timeFinish;
		/* scroll the layer, one pixel per frame */
		TLN_SetLayerPosition (0, frame, 0);
			
		/* render to the window */
		TLN_DrawFrame (0);
		frame++;
		timeFinish = TLN_GetTicks();
		delta = timeFinish - timeStart;
		// Capping FPS to 60
		if(delta < DELAY)
			TLN_Delay(DELAY - delta);
		
	}

	/* release resources */
	TLN_DeleteTilemap (tilemap);
	TLN_Deinit ();

	return 0;
}
