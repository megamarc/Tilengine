#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

static uint8_t framebuffer[WIDTH * HEIGHT * 4];

int main (int argc, char* argv[])
{
	int c;
	TLN_Tilemap tilemap = NULL;
	TLN_Spriteset spriteset = NULL;

	/* basic setup */
	TLN_Init(WIDTH, HEIGHT, 1, 1, 1);
	TLN_SetBGColor (0,128,238);
	TLN_SetRenderTarget(framebuffer, WIDTH * 4);
	TLN_SetLogLevel(TLN_LOG_VERBOSE);
	printf("Tilengine version %06X\n", TLN_GetVersion());
	
	/* test layer */
	tilemap = TLN_LoadTilemap("dummy", NULL);
	for (c = 0; c < 2; c++)
	{
		TLN_SetLayer(0, NULL, tilemap);
		if (tilemap == NULL)
			tilemap = TLN_LoadTilemap("assets/sonic/Sonic_md_bg1.tmx", NULL);
	}
	TLN_SetLayerPosition(1, 0, 0);
	TLN_SetLayerPosition(0, 0, 0);

	/* test sprite */
	spriteset = TLN_LoadSpriteset("dummy");
	for (c = 0; c < 2; c++)
	{
		TLN_ConfigSprite(0, spriteset, 0);
		if (spriteset == NULL)
			spriteset = TLN_LoadSpriteset("smw_sprite");
	}
	TLN_SetSpritePosition(1, 10, 10);
	TLN_SetSpritePosition(0, 10, 10);

	TLN_UpdateFrame(0);

	TLN_DeleteSpriteset(spriteset);
	TLN_DeleteTilemap(tilemap);
	TLN_Deinit ();
	return 0;
}
