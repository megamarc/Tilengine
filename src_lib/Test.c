/* Compile test without windowing component, not for real execution */
#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

static TLN_Tilemap tilemap;
static uint8_t framebuffer[WIDTH * HEIGHT * 4];

/* entry point */
int main (int argc, char* argv[])
{
	/* setup engine */
	TLN_Init (WIDTH, HEIGHT, 1,1,0);
	printf ("Tilengine version %06X\n", TLN_GetVersion());
	TLN_SetBGColor (0,128,238);
	TLN_SetRenderTarget (framebuffer, WIDTH * 4);
	TLN_SetLoadPath ("../assets/sonic");


	tilemap = TLN_LoadTilemap ("Sonic_md_bg1.tmx", NULL);
	TLN_SetLayer (0, NULL, tilemap);
	TLN_SetLayerPosition (0, 100, 0);
		
	/* render one frame */
	TLN_UpdateFrame (0);

	/* release resources */
	TLN_DeleteTilemap (tilemap);
	TLN_Deinit ();

	return 0;
}
