#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

static int frame;
static TLN_Tileset tileset;
static TLN_Tilemap tilemap;

/* entry point */
int main (int argc, char* argv[])
{
	/* setup engine */
	TLN_Init (WIDTH, HEIGHT, 1,0,0);
	TLN_SetBGColor (0,128,238);
	TLN_CreateWindow ("overlay.bmp", CWF_VSYNC);

	/* load layer (tileset + tilemap) */
	tileset = TLN_LoadTileset ("Sonic_md_bg1.tsx");
	tilemap = TLN_LoadTilemap ("Sonic_md_bg1.tmx", "Layer 1");
	
	/* setup the layer */
	if (tileset && tilemap)
		TLN_SetLayer (0, tileset, tilemap);

	/* main loop */
	while (TLN_ProcessWindow ())
	{
		/* scroll the layer, one pixel per frame */
		TLN_SetLayerPosition (0, frame, 0);
		
		/* render to the window */
		TLN_DrawFrame (frame);
		frame++;
	}

	/* release resources */
	TLN_DeleteTileset (tileset);
	TLN_DeleteTilemap (tilemap);
	TLN_DeleteWindow ();
	TLN_Deinit ();

	return 0;
}