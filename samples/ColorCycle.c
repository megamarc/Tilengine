#include <stdio.h>
#include <string.h>
#include "Tilengine.h"

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
	while (TLN_ProcessWindow ())
		TLN_DrawFrame (0);

	TLN_DeleteBitmap (background);
	TLN_DeleteSequencePack (sp);
	TLN_Deinit ();
	return 0;
}
