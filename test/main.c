#include "Tilengine.h"
#include "sdl2\SDL_events.h"

#define WIDTH	400
#define HEIGHT	240

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

static void sdl_callback(SDL_Event* evt)
{
	if (evt->type == SDL_MOUSEBUTTONDOWN)
	{
		printf("ratonnnn\n");
	}
}

/* helper for loading a related tileset + tilemap and configure the appropiate layer */
static void LoadLayer (int index, char* filename)
{
	TLN_Tilemap tilemap = TLN_LoadTilemap (filename, NULL);
	TLN_SetLayer (index, NULL, tilemap);
	TLN_SetBGColorFromTilemap (tilemap);
}

int main (int argc, char* argv[])
{
	int frame = 0;
	int c = 0;
	TLN_Spriteset spriteset;
	TLN_Tileset tileset;
	TLN_Bitmap bitmap;

	TLN_Init (WIDTH,HEIGHT, 2,1,5);
	TLN_SetLoadPath("D:/Tilengine/samples/assets/smw");

	tileset = TLN_LoadTileset("smw_background.tsx");
	if (tileset == NULL)
		printf(TLN_GetErrorString(TLN_GetLastError()));
	spriteset = TLN_LoadSpriteset ("smw_sprite");
	TLN_SetSpriteSet (0, spriteset);
	bitmap = TLN_LoadBitmap("dancougar.png");
	TLN_SetSpritePosition (0,100,100);
	TLN_SetLayerBitmap(0, bitmap);
	TLN_SetLayerPosition(0, 640, 480);

	TLN_SetWindowTitle("Ventana de carapapas");
	TLN_CreateWindow (NULL, CWF_VSYNC);
	TLN_SetSDLCallback(sdl_callback);
	TLN_EnableCRTEffect(TLN_OVERLAY_APERTURE, 128, 128, 128, 128, 128, 128, false, 128);
	while (TLN_ProcessWindow())
	{
		//TLN_SetSpriteRotation(0, frame);
		TLN_DrawFrame(frame++);
		TLN_SetLayerTransform(0, frame/2.0, WIDTH/2, HEIGHT/2, 0.5, 0.5);
	}

	TLN_Deinit ();
	return 0;
}