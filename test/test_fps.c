#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240
#define FPS		200

/* entry point */
int main(int argc, char* argv[])
{
	TLN_Init(WIDTH, HEIGHT, 1, 0, 0);
	TLN_Tilemap tilemap = TLN_LoadTilemap("assets/sonic/Sonic_md_fg1.tmx", NULL);

	TLN_SetLayer(0, NULL, tilemap);
	TLN_SetBGColor(32, 32, 128);

	/* main loop */
	int frame = 0;
	
	/* 
	 * Creates window, will automatically adjust target FPS to monitor Hz.
	 * Use CWF_NOVSYNC when forcing a different target FPS than monitor rate 
	*/
	TLN_CreateWindow(NULL, CWF_NOVSYNC);
	
	/* override monitor refresh rate */
	TLN_SetTargetFps(FPS);

	while (TLN_ProcessWindow())
	{
		/* adjust game logic pacing to target fps */
		int pos = (frame * 60) / TLN_GetTargetFps();

		TLN_SetLayerPosition(0, pos, 0);
		TLN_DrawFrame(0);
		frame++;
	}

	/* release resources */
	TLN_DeleteTilemap(tilemap);
	TLN_Deinit();

	return 0;
}
