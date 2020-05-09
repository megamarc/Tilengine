#ifndef _LOAD_TMX_H
#define _LOAD_TMX_H

#include <stdint.h>
#include <stdbool.h>
#include "Tileset.h"

#define TMX_MAX_LAYER		64
#define TMX_MAX_TILESET		64

typedef enum
{
	LAYER_NONE,
	LAYER_TILE,
	LAYER_OBJECT,
}
LayerType;

typedef struct
{
	LayerType type;
	char name[64];
	int width;			/* layer width (tiles) */
	int height;			/* layer height (tiles) */
	int num_objects;	/* */
	int id;
	bool visible;
}
TMXLayer;

typedef struct
{
	char source[64];
	int firstgid;
}
TMXTileset;

/* general TMX info */
typedef struct
{
	int width;				/* map width (tiles) */
	int height;				/* map height (tiles) */
	int tilewidth;			/* */
	int tileheight;			/* */
	int num_layers;			/* number of layers */
	int num_tilesets;		/* number of tilesets */
	uint32_t bgcolor;		/* background color */
	TMXLayer layers[TMX_MAX_LAYER];			/* array of layers */
	TMXTileset tilesets[TMX_MAX_TILESET];	/* array of tilesets */
}
TMXInfo;

bool TMXLoad(const char* filename, TMXInfo* info);
TMXTileset* TMXGetSuitableTileset(TMXInfo* info, int gid);
TMXLayer* TMXGetFirstLayer(TMXInfo* info, LayerType type);
TMXLayer* TMXGetLayer(TMXInfo* info, const char* name);

#endif
