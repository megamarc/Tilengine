#include "Tilengine.h"
#include "../src/sdl/SDL2/SDL_timer.h"

#define WIDTH	400
#define HEIGHT	240

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
	int timeStart = 0;
	int timeFinish = 0;
	float delta = 0.00;

	while (TLN_ProcessWindow ())
	{
		timeStart = SDL_GetTicks();
		delta = timeStart - timeFinish;
		if(delta > 1000 / 60.00) // Capping
		{
			/* scroll the layer, one pixel per frame */
			TLN_SetLayerPosition (0, frame, 0);
			
			/* render to the window */
			TLN_DrawFrame (0);
			frame++;
			timeFinish = timeStart;
		}
		
	}

	/* release resources */
	TLN_DeleteTilemap (tilemap);
	TLN_Deinit ();

	return 0;
}
