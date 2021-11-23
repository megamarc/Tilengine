/******************************************************************************
*
* Tilengine sample
* 2021 Marc Palacios
* http://www.tilengine.org

* Demonstrates various API calls to query data about layers
*
******************************************************************************/

#include "Tilengine.h"

#define HRES		424
#define VRES		240
#define NUM_LAYERS	4

/* layer type names */
static const char* layer_types[] = { "undefined", "tiles", "objects", "bitmap" };

/* shows data about a given layer */
static void show_layer_info(int nlayer)
{
	TLN_LayerType type = TLN_GetLayerType(nlayer);
	TLN_ObjectList object_list;
	TLN_ObjectInfo object = { 0 };
	TLN_Tilemap tilemap;
	bool has_object;
	
	if (type == LAYER_NONE)
		return;
	
	/* general info */
	printf("\nLayer %d type: %s\n", nlayer, layer_types[type]);
	printf("  size: %dx%d pixels\n", TLN_GetLayerWidth(nlayer), TLN_GetLayerHeight(nlayer));
	printf("  Palette: %p\n", TLN_GetLayerPalette(nlayer));

	/* specific info */
	switch (type)
	{
	case LAYER_TILE:
		tilemap = TLN_GetLayerTilemap(nlayer);
		printf("  Tileset: %p\n", TLN_GetLayerTileset(nlayer));
		printf("  Tilemap: %p\n", tilemap);
		printf("  Tilemap dimensions: %d rows, %d columns\n", TLN_GetTilemapRows(tilemap), TLN_GetTilemapCols(tilemap));
		break;

	case LAYER_OBJECT:
		object_list = TLN_GetLayerObjects(nlayer);
		printf("  Tileset: %p\n", TLN_GetLayerTileset(nlayer));
		printf("  Objects: %p\n", object_list);

		/* iterate objects and get info on each with TLN_GetListObject() */
		printf("  num_objects = %d\n", TLN_GetListNumObjects(object_list));
		has_object = TLN_GetListObject(object_list, &object);
		while (has_object)
		{
			printf("    id:%d gid:%d name:\"%s\" pos:(%d,%d) size:(%dx%d) type:%d\n", object.id, object.gid, object.name, object.x, object.y, object.width, object.height, object.type);
			has_object = TLN_GetListObject(object_list, NULL);
		}
		break;

	case LAYER_BITMAP:
		printf("  Bitmap: %p\n", TLN_GetLayerBitmap(nlayer));
		break;
	}
}

int main(int argc, char* argv[])
{
	TLN_Init(HRES, VRES, NUM_LAYERS, 0, 0);

	/* load world starting at layer 0 */
	TLN_SetLoadPath("assets/forest");
	TLN_LoadWorld("map.tmx", 0);

	/* retrieve info about layers */
	int c;
	for (c = 0; c < NUM_LAYERS; c += 1)
		show_layer_info(c);

	/* retrieve tile at layer 1, row 13 column 3, flip horizontally and update to the same position */
	Tile tile = { 0 };
	TLN_Tilemap tilemap = TLN_GetLayerTilemap(1);
	TLN_GetTilemapTile(tilemap, 13, 3, &tile);
	tile.flags |= FLAG_FLIPX;
	TLN_SetTilemapTile(tilemap, 13, 3, &tile);

	/* release resources */
	TLN_ReleaseWorld();
	TLN_Deinit();
	return 0;
}
