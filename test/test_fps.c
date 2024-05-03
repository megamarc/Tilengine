#include "Tilengine.h"

#define WIDTH		400
#define HEIGHT		240

static const int	MONITOR_HZ =  60;	/* set native monitor hz */
static const int	TARGET_FPS = 144;	/* set desired refresh rate */

/* entry point */
int main(int argc, char* argv[])
{
	TLN_Init(WIDTH, HEIGHT, 1, 0, 0);
	TLN_Tilemap tilemap = TLN_LoadTilemap("assets/sonic/Sonic_md_fg1.tmx", NULL);

	TLN_SetLayer(0, NULL, tilemap);
	TLN_SetBGColor(32, 32, 128);

	/* main loop */
	int frame = 0;
	int average_fps = 0;
	
	/* 
	 * Creates window, will automatically adjust target FPS to monitor Hz.
	 * Use CWF_NOVSYNC when forcing a different target FPS than monitor rate 
	*/
	if (MONITOR_HZ == TARGET_FPS)
	{
		TLN_CreateWindow(NULL, 0);
	}
	else
	{
		TLN_CreateWindow(NULL, CWF_NOVSYNC);
		TLN_SetTargetFps(TARGET_FPS);
	}

	/* main loop */
	while (TLN_ProcessWindow())
	{
		/* adjust game logic pacing to target fps */
		int pos = (frame * 60) / TLN_GetTargetFps();

		TLN_SetLayerPosition(0, pos, 0);
		TLN_DrawFrame(0);
		frame++;

		int fps = TLN_GetAverageFps();
		if (fps != average_fps)
		{
			printf("fps: %d   \r", fps);
			average_fps = fps;
		}
	}

	/* release resources */
	TLN_DeleteTilemap(tilemap);
	TLN_Deinit();

	return 0;
}
