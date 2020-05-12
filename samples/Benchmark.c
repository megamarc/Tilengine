#include <stdlib.h>
#include <stdio.h>
#include "Tilengine.h"

#define HRES		400
#define VRES		240
#define NUM_SPRITES	250
#define NUM_FRAMES	2000

static int pixels;

static uint32_t Profile (void);

int main (int argc, char* argv[])
{
	int c;
	bool ok;
	uint8_t* framebuffer;
	uint32_t version;
	TLN_Tilemap tilemap;
	TLN_Spriteset spriteset;
	TLN_SpriteInfo sprite_info;

	version = TLN_GetVersion ();
	printf ("\nTilengine benchmark tool\n");
	printf ("Written by Megamarc - %s %s\n", __DATE__, __TIME__);
	printf ("Library version: %d.%d.%d\n", (version >> 16)&0xFF, (version >> 8)&0xFF, version&0xFF);
	printf ("http://www.tilengine.org\n\n");

	/* setup engine */
	TLN_Init (HRES, VRES, 1, NUM_SPRITES, 0);
	framebuffer = malloc(HRES*VRES*4);
	TLN_SetRenderTarget (framebuffer, HRES*4);
	TLN_DisableBGColor ();

	/* create assets */
	TLN_SetLoadPath ("assets/tf4");
	tilemap = TLN_LoadTilemap ("TF4_bg1.tmx", NULL);
	spriteset = TLN_LoadSpriteset ("FireLeo");

	/* setup layer */
	ok = TLN_SetLayerTilemap (0, tilemap);
	pixels = HRES*VRES;

	printf ("Normal layer..........");
	Profile ();

	printf ("Scaling layer.........");
	TLN_SetLayerScaling (0, 2.0f, 2.0f);
	Profile ();

	printf ("Affine layer..........");
	TLN_SetLayerTransform (0, 45.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	Profile ();

	printf ("Blend layer...........");
	TLN_ResetLayerMode (0);
	TLN_SetLayerBlendMode (0, BLEND_MIX, 128);
	Profile ();

	printf ("Scaling blend layer...");
	TLN_SetLayerScaling (0, 2.0f, 2.0f);
	Profile ();

	printf ("Affine blend layer....");
	TLN_SetLayerTransform (0, 45.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	Profile ();

	TLN_DisableLayer (0);

	/* setup sprites */
	for (c=0; c<NUM_SPRITES; c++)
	{
		int y = c/25;
		int x = c%25;
		ok = TLN_ConfigSprite (c, spriteset, FLAG_NONE);
		ok = TLN_SetSpritePicture (c, 0);
		TLN_SetSpritePosition (c, x*15, y*21);
	}
	TLN_GetSpriteInfo (spriteset, 0, &sprite_info);
	pixels = NUM_SPRITES*sprite_info.w*sprite_info.h;

	printf ("Normal sprites........");
	Profile ();

	printf ("Colliding sprites.....");
	for (c=0; c<NUM_SPRITES; c++)
		TLN_EnableSpriteCollision (c, true);
	Profile ();

	free (framebuffer);
	TLN_DeleteTilemap (tilemap);
	TLN_Deinit ();

	return 0;
}

static uint32_t Profile (void)
{
	uint32_t t0, elapse;
	uint32_t frame = 0;
	uint32_t result;

	t0 = TLN_GetTicks ();
	do
	{
		TLN_UpdateFrame (frame++);
	}
	while (frame < NUM_FRAMES);
	elapse = TLN_GetTicks () - t0;
	result = frame*pixels/elapse;

	printf (" %3u.%03u Mpixels/s\n", result/1000, result%1000);
	return result;
}
