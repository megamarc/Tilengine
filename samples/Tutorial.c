#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240
#define FOREGROUND	0

/* entry point */
int main (int argc, char* argv[])
{
	/* setup engine */
	TLN_Init (WIDTH, HEIGHT, 1,0,0);
	TLN_SetBGColor (32,32,128);

	/* load resources */
	TLN_Tilemap tilemap = TLN_LoadTilemap ("assets/sonic/Sonic_md_fg1.tmx", NULL);
	
	/* setup the layer */
	TLN_SetLayerTilemap (FOREGROUND, tilemap);
	TLN_SetLayerPosition (FOREGROUND, 32, 0);	/* 32 pixels to the right */

	/* window main loop */
	TLN_CreateWindow (NULL, CWF_FULLSCREEN);
	TLN_SetMainTask(NULL);

	/* release resources */
	TLN_DeleteWindow();
	TLN_DeleteTilemap (tilemap);
	TLN_Deinit ();
	return 0;
}
