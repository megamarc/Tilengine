#ifndef _LOAD_TMX_H
#define _LOAD_TMX_H

#include <stdint.h>
#include <stdbool.h>
#include "Tileset.h"

#define TMX_MAX_LAYER		32
#define TMX_MAX_TILESET		32

typedef struct
{
	TLN_LayerType type;
	char name[64];
	char image[100];	/* bitmap name for bitmap layers */
	int width;			/* layer width (tiles or pixels) */
	int height;			/* layer height (tiles or pixels) */
	int num_objects;	/* number of objects for object layers */
	int id;
	bool visible;
	bool locked;
	float parallaxx;
	float parallaxy;
	float offsetx;
	float offsety;
	float opacity;
	uint32_t tintcolor;
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
	char filename[100];		/* cached tmx file */
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
int TMXGetSuitableTileset(TMXInfo* info, int gid, TMXTileset* tmxtilesets);
TMXLayer* TMXGetFirstLayer(TMXInfo* info, TLN_LayerType type);
TMXLayer* TMXGetLayer(TMXInfo* info, const char* name);

#endif
