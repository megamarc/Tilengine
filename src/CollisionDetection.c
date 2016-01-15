#include <stdlib.h>
#include <stdio.h>
#include "Tilengine.h"

#define HRES	400
#define VRES	240
#define SIZE	(32*48)
#define SPRITES	250
#define FRAMES	1000

int main (int argc, char* argv[])
{
	TLN_Tileset tileset;
	TLN_Tilemap tilemap;
	TLN_Spriteset spriteset;
	int frame = 0;
	int c;
	int pixels;
	DWORD t0, t1;
	bool ok;
	BYTE* framebuffer;

	TLN_Init (HRES, VRES, 1, SPRITES, 0);
	framebuffer = malloc(HRES*VRES*4);
	TLN_SetRenderTarget (framebuffer, HRES*4);

	tileset = TLN_LoadTileset ("SOTB_fg.tsx");
	tilemap = TLN_LoadTilemap ("SOTB_fg.tmx", "Layer 1");
	ok = TLN_SetLayer (0, tileset, tilemap);
	pixels = FRAMES*HRES*VRES;

	printf ("Capa de tiles... ");
	frame = 0;
	t0 = TLN_GetTicks ();
	while (TLN_ProcessWindow () && frame<FRAMES)
		TLN_UpdateFrame (frame++);
	t1 = TLN_GetTicks () - t0;
	printf ("%10d pixels/s\n", pixels/t1*1000);
	TLN_DisableLayer (0);
	TLN_DeleteTilemap (tilemap);
	TLN_DeleteTileset (tileset);

	spriteset = TLN_LoadSpriteset ("Simon");
	for (c=0; c<SPRITES; c++)
	{
		TLN_ConfigSprite (c, spriteset, FLAG_NONE);
		TLN_SetSpritePicture (c, 0);
		TLN_SetSpritePosition (c, rand()%(HRES-32), rand()%(VRES-48));
	}
	pixels = FRAMES*SPRITES*SIZE;

	printf ("Sprites sin colision... ");
	for (c=0; c<SPRITES; c++)
		TLN_EnableSpriteCollision (c, false);
	frame = 0;
	t0 = TLN_GetTicks ();
	while (TLN_ProcessWindow () && frame<FRAMES)
		TLN_UpdateFrame (frame++);
	t1 = TLN_GetTicks () - t0;
	printf ("%10d pixels/s\n", pixels/t1*1000);

	printf ("Sprites con colision... ");
	for (c=0; c<SPRITES; c++)
		TLN_EnableSpriteCollision (c, true);
	frame = 0;
	t0 = TLN_GetTicks ();
	while (TLN_ProcessWindow () && frame<FRAMES)
		TLN_UpdateFrame (frame++);
	t1 = TLN_GetTicks () - t0;
	printf ("%10d pixels/s\n", pixels/t1*1000);

	free (framebuffer);
	TLN_DeleteSpriteset (spriteset);
	TLN_Deinit ();

	return 0;
}