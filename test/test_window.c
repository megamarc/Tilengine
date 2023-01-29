#include "Tilengine.h"

#define WIDTH	398
#define HEIGHT	224

int main(int argc, char* argv[])
{
	int x = 0;

	TLN_Init(WIDTH, HEIGHT, 2, 4, 0);
	TLN_SetLoadPath("assets/cliffs");
	TLN_SetBGColor(0x88, 0xDA, 0xF2);

	TLN_Tilemap layer_foreground = TLN_LoadTilemap("layer_foreground.tmx", NULL);
	TLN_Tilemap layer_background = TLN_LoadTilemap("layer_background.tmx", NULL);
	TLN_SetLayer(0, NULL, layer_foreground);
	TLN_SetLayer(1, NULL, layer_background);

	TLN_SetLayerTransform(0, 45, 0, 0, 1.0f, 1.0f);

	TLN_CreateWindow(NULL, 0);
	while (TLN_ProcessWindow())
	{
		TLN_DrawFrame(0);
	}

	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
