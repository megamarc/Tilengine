#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

static uint8_t framebuffer[WIDTH * HEIGHT * 4];

void rasters(int line)
{
	float factor = line / 120.0f;
	TLN_SetLayerParallaxFactor(3, -factor, 1);
}

int main (int argc, char* argv[])
{
	int x = 0;

	TLN_Init(WIDTH, HEIGHT, 8, 80, 0);
	TLN_SetLoadPath("assets/forest");
	TLN_LoadWorld("map.tmx", 0);
	TLN_CreateWindow(NULL, 0);
	TLN_SetRasterCallback(rasters);
	while (TLN_ProcessWindow())
	{
		TLN_SetWorldPosition(x, 0);
		TLN_DrawFrame(0);
		x += 2;
	}

	TLN_DeleteWindow();
	TLN_Deinit ();
	return 0;
}
