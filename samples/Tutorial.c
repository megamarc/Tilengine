#include "Tilengine.h"

/* entry point */
int main (int argc, char* argv[])
{
	/* setup engine: 400x240, 1 layer */
	TLN_Init (400, 240, 1, 0, 0);
	TLN_SetBGColor (32, 32, 128);

	/* load resources */
	TLN_Tilemap tilemap = TLN_LoadTilemap ("assets/sonic/Sonic_md_fg1.tmx", NULL);
	
	/* setup the layer (index 0) */
	TLN_SetLayerTilemap (0, tilemap);
	TLN_SetLayerPosition (0, 32, 0);	/* 32 pixels to the right */

	/* window main loop, block until closed */
	TLN_CreateWindow (NULL, CWF_FULLSCREEN);
	TLN_SetMainTask(NULL);

	/* release resources */
	TLN_DeleteTilemap (tilemap);
	TLN_Deinit ();
	return 0;
}
