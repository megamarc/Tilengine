#include "Tilengine.h"

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
	TLN_SetLoadPath ("assets/sonic");
	tilemap = TLN_LoadTilemap ("Sonic_md_fg1.tmx", NULL);
	
	/* setup the layer */
	TLN_SetLayer (0, NULL, tilemap);
	TLN_SetBGColor (32,32,128);

	/* main loop */
	TLN_CreateWindow (NULL, 0);
	while (TLN_ProcessWindow ())
	{
		/* scroll the layer, one pixel per frame */
		TLN_SetLayerPosition (0, frame, 0);
		
		/* render to the window */
		TLN_DrawFrame (frame);
		frame++;
	}

	/* release resources */
	TLN_DeleteTilemap (tilemap);
	TLN_Deinit ();

	return 0;
}
