#include "Tilengine.h"

#define HRES	(29 * 16)
#define VRES	(16 * 16)

int main(int argc, char* argv[])
{
	TLN_Init(HRES, VRES, 1, 0, 0);
	TLN_SetLoadPath("assets/test");
	TLN_Tilemap tilemap = TLN_LoadTilemap("test.tmx", NULL);
	TLN_SetLayerTilemap(0, tilemap);

	for (int row = 0; row < 2; row += 1)
	{
		for (int col = 0; col < 5; col += 1)
		{
			TLN_TileInfo info = { 0 };
			TLN_GetLayerTile(0, col * 32, row * 32, &info);
			printf("%d ", info.type);
		}
		printf("\n");		
	}
	return 0;
}