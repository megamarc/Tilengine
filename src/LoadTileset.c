/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Tilengine.h"
#include "simplexml.h"
#include "LoadFile.h"

/* properties */
typedef enum
{
	PROPERTY_NONE,
	PROPERTY_TYPE,
	PROPERTY_PRIORITY,
}
Property;

/* image context */
typedef enum
{
	CONTEXT_NONE,
	CONTEXT_TILESET,
	CONTEXT_TILE,
}
ImageContext;

/* load manager */
struct
{
	char source[64];
	int tilecount;
	int tilewidth;
	int tileheight;
	int spacing;
	int margin;
	ImageContext context;
	TLN_TileAttributes* attributes;	/* array of attributes */
	TLN_SequencePack sp;
	TLN_SequenceFrame frames[100];
	TLN_TileImage* images;	/* array of images */
	TLN_TileImage* image;	/* current image */
	int frame_count;

	/* tile-specific values */
	struct
	{
		int id;				/* id of tile */
		int type;			/* type of tile */
		Property property;	/* property being read */
		bool priority;		/* value of priority property */
		TLN_Bitmap bitmap;	/* bitmap of image-based tile */
	}
	tile;
}
static loader;

/* XML parser callback */
static void* handler (SimpleXmlParser parser, SimpleXmlEvent evt, 
	const char* szName, const char* szAttribute, const char* szValue)
{
	switch (evt)
	{
	case ADD_SUBTAG:
		/* <animation> */
		if (!strcasecmp(szName, "animation"))
		{
			loader.frame_count = 0;
		}

		else if (!strcasecmp(szName, "tileset"))
			loader.context = CONTEXT_TILESET;
		else if (!strcasecmp(szName, "tile"))
			loader.context = CONTEXT_TILE;
		break;

	case ADD_ATTRIBUTE:
		/* <tileset name="dkc_bg1" tilewidth="8" tileheight="8" tilecount="784" columns="16"> */
		if (!strcasecmp(szName, "tileset"))
		{
			if (!strcasecmp(szAttribute, "tilewidth"))
				loader.tilewidth = atoi(szValue);
			else if (!strcasecmp(szAttribute, "tileheight"))
				loader.tileheight = atoi(szValue);
			else if (!strcasecmp(szAttribute, "margin"))
				loader.margin = atoi(szValue);
			else if (!strcasecmp(szAttribute, "spacing"))
				loader.spacing = atoi(szValue);
			else if (!strcasecmp(szAttribute, "tilecount"))
				loader.tilecount = atoi(szValue);
		}

		/* <image source="dkc_bg1.png" width="128" height="392"/> */
		else if (!strcasecmp(szName, "image"))
		{
			if (!strcasecmp(szAttribute, "source"))
			{
				strncpy(loader.source, szValue, sizeof(loader.source));
				loader.source[sizeof(loader.source) - 1] = '\0';

				/* image for each image-based tileset */
				if (loader.context == CONTEXT_TILE)
				{
					loader.tile.bitmap = TLN_LoadBitmap(loader.source);
					loader.source[0] = 0;
				}
			}
		}

		/* <tile id="314" type="xxx"> */
		else if (!strcasecmp(szName, "tile"))
		{
			if (!strcasecmp(szAttribute, "id"))
				loader.tile.id = atoi(szValue);
			else if (!strcasecmp(szAttribute, "type"))
				loader.tile.type = atoi(szValue);
		}

		/* <property name="type" type="int" value="12"/> */
		else if (!strcasecmp(szName, "property"))
		{
			if (!strcasecmp(szAttribute, "name"))
			{
				if (!strcasecmp(szValue, "type"))
					loader.tile.property = PROPERTY_TYPE;
				else if (!strcasecmp(szValue, "priority"))
					loader.tile.property = PROPERTY_PRIORITY;
				else
					loader.tile.property = PROPERTY_NONE;
			}
			else if (!strcasecmp(szAttribute, "value"))
			{
				if (loader.tile.property == PROPERTY_TYPE)
					loader.attributes[loader.tile.id].type = atoi(szValue);
				else if (loader.tile.property == PROPERTY_PRIORITY)
				{
					if (!strcasecmp(szValue, "true"))
						loader.attributes[loader.tile.id].priority = true;
					else
						loader.attributes[loader.tile.id].priority = false;
				}
			}
		}

		/* <frame tileid="50" duration="100"/> */
		else if (!strcasecmp(szName, "frame"))
		{
			if (!strcasecmp(szAttribute, "tileid"))
				loader.frames[loader.frame_count].index = atoi(szValue) + 1;
			else if (!strcasecmp(szAttribute, "duration"))
				loader.frames[loader.frame_count].delay = atoi(szValue)*60/1000;
		}
		break;

	case FINISH_ATTRIBUTES:
		if (!strcasecmp(szName, "tileset"))
		{
			loader.attributes = (TLN_TileAttributes*)calloc(loader.tilecount, sizeof(TLN_TileAttributes));
			loader.images = (TLN_TileImage*)calloc(loader.tilecount, sizeof(TLN_TileImage));
			loader.image = loader.images;
		}
		break;

	case ADD_CONTENT:
		break;

	case FINISH_TAG:
		if (!strcasecmp(szName, "frame"))
			loader.frame_count++;
		else if (!strcasecmp(szName, "tile"))
		{
			if (loader.context == CONTEXT_TILESET)
			{
				TLN_TileAttributes* attribute = &loader.attributes[loader.tile.id];
				attribute->priority = loader.tile.priority;
				attribute->type = loader.tile.type;
			}
			else if (loader.context == CONTEXT_TILE)
			{
				loader.image->bitmap = loader.tile.bitmap;
				loader.image->id = loader.tile.id;
				loader.image->type = loader.tile.type;
				loader.image += 1;
			}
		}
		else if (!strcasecmp(szName, "animation"))
		{
			char name[16];
			TLN_Sequence sequence;
			sprintf (name, "%d", loader.tile.id);
			sequence = TLN_CreateSequence (name, loader.tile.id + 1, loader.frame_count, loader.frames);
			if (loader.sp == NULL)
				loader.sp = TLN_CreateSequencePack ();
			TLN_AddSequenceToPack (loader.sp, sequence);
		}
		break;
	}
	return handler;
}

/* cache section: keeps already loaded tilesets so it doesnt spawn multiple instances of the same */
#define CACHE_SIZE	16
static cache_entries = 0;
struct
{
	char name[200];
	TLN_Tileset tileset;
}
static cache[16];

static TLN_Tileset search_cache(const char* name)
{
	int c;
	for (c = 0; c < cache_entries; c += 1)
	{
		if (!strcmp(cache[c].name, name))
			return cache[c].tileset;
	}
	return NULL;
}

static void add_to_cache(const char* name, TLN_Tileset tileset)
{
	if (cache_entries < CACHE_SIZE - 1)
	{
		strncpy(cache[cache_entries].name, name, sizeof(cache[0].name));
		cache[cache_entries].tileset = tileset;
		cache_entries += 1;
	}
}

/*!
 * \brief
 * Loads a tileset from a Tiled .tsx file
 * 
 * \param filename
 * TSX file to load
 * 
 * \returns
 * Reference to the newly loaded tileset or NULL if error
 *
 * \remarks
 * An associated palette is also created, it can be obtained calling TLN_GetTilesetPalette()
 */
TLN_Tileset TLN_LoadTileset (const char* filename)
{
	SimpleXmlParser parser;
	ssize_t size = 0;
	uint8_t *data = NULL;
	TLN_Tileset tileset = NULL;

	/* find in cache */
	tileset = search_cache(filename);
	if (tileset)
		return tileset;
	
	/* load file */
	data = (uint8_t*)LoadFile (filename, &size);
	if (!data)
	{
		if (size == 0)
			TLN_SetLastError (TLN_ERR_FILE_NOT_FOUND);
		else if (size == -1)
			TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}

	/* parse */
	memset (&loader, 0, sizeof(loader));
	parser = simpleXmlCreateParser ((char*)data, (long)size);
	if (parser != NULL)
	{
		if (simpleXmlParse(parser, handler) != 0)
		{
			printf("parse error on line %li:\n%s\n", 
				simpleXmlGetLineNumber(parser), simpleXmlGetErrorDescription(parser));
			simpleXmlDestroyParser(parser);
			free (data);
			TLN_SetLastError (TLN_ERR_WRONG_FORMAT);
			return NULL;
		}
	}
	else
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);

	simpleXmlDestroyParser(parser); 
	free(data);

	/* tile based tileset (classic) */
	if (loader.source[0] != 0)
	{
		TLN_Bitmap bitmap;
		int htiles, vtiles;
		int x, y, dx, dy;
		int id;
		int pitch;

		bitmap = TLN_LoadBitmap(loader.source);
		if (!bitmap)
		{
			TLN_SetLastError(TLN_ERR_FILE_NOT_FOUND);
			return NULL;
		}

		/* create */
		dx = loader.tilewidth + loader.spacing;
		dy = loader.tileheight + loader.spacing;
		htiles = (TLN_GetBitmapWidth(bitmap) - loader.margin * 2 + loader.spacing) / dx;
		vtiles = (TLN_GetBitmapHeight(bitmap) - loader.margin * 2 + loader.spacing) / dy;
		tileset = TLN_CreateTileset(htiles*vtiles, loader.tilewidth, loader.tileheight, TLN_ClonePalette(TLN_GetBitmapPalette(bitmap)), loader.sp, loader.attributes);
		if (tileset == NULL)
		{
			TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);
			return NULL;
		}

		/* load tile data */
		pitch = TLN_GetBitmapPitch(bitmap);
		for (id = 1, y = 0; y < vtiles; y++)
		{
			for (x = 0; x < htiles; x++, id++)
			{
				uint8_t *srcptr = TLN_GetBitmapPtr(bitmap, loader.margin + x * dx, loader.margin + y * dy);
				TLN_SetTilesetPixels(tileset, id, srcptr, pitch);
			}
		}
		TLN_DeleteBitmap(bitmap);
	}

	/* +2.5.0 image-based tileset */
	else
	{
		tileset = TLN_CreateImageTileset(loader.tilecount, loader.images);
		if (tileset == NULL)
		{
			TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);
			return NULL;
		}
	}

	if (loader.attributes != NULL)
		free(loader.attributes);
	if (loader.images != NULL)
		free(loader.images);

	add_to_cache(filename, tileset);
	TLN_SetLastError (TLN_ERR_OK);
	return tileset;
}
