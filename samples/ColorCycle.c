#include <stdio.h>
#include <string.h>
#include "Tilengine.h"
// #include "../src/sdl/SDL2/SDL_timer.h"

int main (int argc, char* argv[])
{
	TLN_Bitmap background;
	TLN_SequencePack sp;
	TLN_Sequence sequence;
	TLN_Palette palette;

	/* engine init */
	TLN_Init (640,480,0,0,1);

	/* load resources */
	TLN_SetLoadPath ("assets/color");
	background = TLN_LoadBitmap ("beach.png");
	palette    = TLN_GetBitmapPalette (background);
	sp		   = TLN_LoadSequencePack ("beach.sqx");
	sequence   = TLN_FindSequence (sp, "beach");

	/* setup */
	TLN_SetBGBitmap (background);
	TLN_SetPaletteAnimation (0, palette, sequence, true);

	/* main loop */
	TLN_CreateWindow (NULL, 0);

	// We will cap the FPS to 60 for people having a screen with a refresh rate greater than 60Hz
	int timeStart = 0;
	int timeFinish = 0;
	float delta = 0.00;

	while (TLN_ProcessWindow ())
	{
		// Calculating the Delta
		timeStart = TLN_GetTicks();
		delta = timeStart - timeFinish;

		if(delta > 1000 / 60.00) // Capping
		{
			TLN_DrawFrame (0);
			timeFinish = timeStart;
		}
	}

	TLN_DeleteBitmap (background);
	TLN_DeleteSequencePack (sp);
	TLN_Deinit ();
	return 0;
}
