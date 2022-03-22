/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Tilengine.h"
#include "Tileset.h"
#include "Palette.h"
#include "simplexml.h"
#include "Bitmap.h"

static bool HasTransparentPixels (uint8_t* src, int width);

/*!
 * \brief
 * Creates a tile-based tileset
 * 
 * \param numtiles
 * Number of tiles that the tileset will hold
 * 
 * \param width
 * Width of each tile (must be multiple of 8)
 * 
 * \param height
 * Height of each tile (must be multiple of 8)
 * 
 * \param palette
 * Reference to the palette to assign
 * 
 * \param sp
 * Optional reference to the optional sequence pack with associated tileset animations, can be NULL
 *
 * \param attributes
 * Optional array of attributes, one for each tile. Can be NULL
 *
 * \returns
 * Reference to the created tileset, or NULL if error
 * 
 * \see
 * TLN_SetTilesetPixels()
 */
TLN_Tileset TLN_CreateTileset (int numtiles, int width, int height, TLN_Palette palette, TLN_SequencePack sp, TLN_TileAttributes* attributes)
{
	TLN_Tileset tileset;
	int hshift = 0;
	int vshift = 0;
	int c;
	int size;
	int size_tiles;
	int size_attributes;

	for (c=0; c<=8; c++)
	{
		int mask = 1<<c;
		if (mask==width)
			hshift = c;
		if (mask==height)
			vshift = c;
	}
	if (!hshift || !vshift)
	{
		TLN_SetLastError (TLN_ERR_WRONG_SIZE);
		return NULL;
	}

	numtiles++;
	size_tiles = width * height * numtiles;
	size_attributes = numtiles * sizeof(TLN_TileAttributes);
	size = sizeof(struct Tileset) + size_tiles;
	tileset = (TLN_Tileset)CreateBaseObject (OT_TILESET, size);
	if (!tileset)
	{
		TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}

	tileset->tstype = TILESET_TILES;
	tileset->width = width;
	tileset->height = height;
	tileset->hshift = hshift;
	tileset->vshift = vshift;
	tileset->hmask = width - 1;
	tileset->vmask = height - 1;
	tileset->numtiles = numtiles;
	tileset->palette = palette;
	tileset->sp = sp;
	tileset->color_key = (bool*)calloc(numtiles, height);
	tileset->attributes = (TLN_TileAttributes*)malloc(size_attributes);
	if (attributes != NULL)
		memcpy (tileset->attributes, attributes, size_attributes);
	tileset->tiles = (uint16_t*)calloc(numtiles, sizeof(uint16_t));
	for (c = 0; c < numtiles; c += 1)
		tileset->tiles[c] = c;

	/* create animations */
	if (sp != NULL)
		tileset->animations = (Animation*)calloc(sp->num_sequences, sizeof(Animation));
	
	TLN_SetLastError (TLN_ERR_OK);
	return tileset;
}

/*!
 * \brief
 * Creates a multiple image-based tileset
 *
 * \param numtiles
 * Number of tiles that the tileset will hold
 *
 * \param images
 * Array of image structures, one for each tile. Can be NULL
 *
 * \returns
 * Reference to the created tileset, or NULL if error
  */

TLN_Tileset TLN_CreateImageTileset(int numtiles, TLN_TileImage* images)
{
	TLN_Tileset tileset;
	const int images_size = numtiles * sizeof(TLN_TileImage);
	const int size = sizeof(struct Tileset) + images_size;

	tileset = (TLN_Tileset)CreateBaseObject(OT_TILESET, size);
	if (tileset == NULL)
	{
		TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}

	tileset->tstype = TILESET_IMAGES;
	tileset->numtiles = numtiles;
	tileset->images = (TLN_TileImage*)tileset->data;
	memcpy(tileset->images, images, images_size);
	return tileset;
}

/*!
 * \brief
 * Sets pixel data for a tile in a tile-based tileset
 * 
 * \param tileset
 * Reference to the tileset
 * 
 * \param entry
 * Number of tile to set [0, num_tiles - 1]
 * 
 * \param srcdata
 * Pointer to pixel data to set
 * 
 * \param srcpitch
 * Bytes per line of source data
 * 
 * \returns
 * true if success, or false if error
 * 
 * \remarks
 * Care must be taken in providing pixel data and pitch as it can crash the aplication
 * 
 * \see
 * TLN_CreateTileset()
 */
bool TLN_SetTilesetPixels (TLN_Tileset tileset, int entry, uint8_t* srcdata, int srcpitch)
{
	int c, line;
	uint8_t* dstdata;

	if (!CheckBaseObject (tileset, OT_TILESET))
		return false;

	if (tileset->tstype != TILESET_TILES || entry<1 || entry>tileset->numtiles)
	{
		TLN_SetLastError(TLN_ERR_IDX_PICTURE);
		return false;
	}

	line = entry * tileset->height;
	dstdata = tileset->data + (entry * tileset->width * tileset->height);
	for (c=0; c<tileset->height; c++)
	{
		memcpy (dstdata, srcdata, tileset->width);
		tileset->color_key[line++] = HasTransparentPixels (srcdata, tileset->width);
		srcdata += srcpitch;
		dstdata += tileset->width;
	}

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Creates a duplicate of the specified tileset and its associated palette
 * 
 * \param src
 * Tileset to clone
 * 
 * \returns
 * A reference to the newly cloned tileset, or NULL if error
 *
 * \see
 * TLN_LoadTileset()
 */
TLN_Tileset TLN_CloneTileset (TLN_Tileset src)
{
	TLN_Tileset tileset;

	if (!CheckBaseObject (src, OT_TILESET))
		return NULL;

	tileset = (TLN_Tileset)CloneBaseObject (src);
	if (tileset)
	{
		const int size_tiles = src->numtiles * sizeof(uint16_t);
		const int size_color = src->numtiles * src->height;
		const int size_attributes = src->numtiles * sizeof(TLN_TileAttributes);
		
		TLN_SetLastError (TLN_ERR_OK);
		tileset->tiles = (uint16_t*)malloc(size_tiles);
		memcpy(tileset->tiles, src->tiles, size_tiles);
		tileset->color_key = (bool*)malloc(size_color);
		memcpy(tileset->color_key, src->color_key, size_color);
		tileset->attributes = (TLN_TileAttributes*)malloc(size_attributes);
		memcpy(tileset->attributes, src->attributes, size_attributes);
		return tileset;
	}
	else
		return NULL;
}

/*!
 * \brief
 * Deletes the specified tileset and frees memory
 * 
 * \param tileset
 * Tileset to delete
 * 
 * \remarks
 * Don't delete a tileset currently attached to a layer!
 * 
 * \see
 * TLN_LoadTileset(), TLN_CloneTileset()
 */
bool TLN_DeleteTileset (TLN_Tileset tileset)
{
	// TODO fix crash on exit, only on Release build, can't be debugged
	return true;

	if (CheckBaseObject (tileset, OT_TILESET))
	{
		if (ObjectOwner (tileset))
		{
			TLN_DeletePalette (tileset->palette);
			TLN_DeleteSequencePack (tileset->sp);
		}
		free(tileset->tiles);
		free(tileset->color_key);
		free(tileset->attributes);
		if (tileset->animations)
			free(tileset->animations);

		DeleteBaseObject (tileset);
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return false;
}

/*!
 * \brief
 * Returns the width in pixels of each individual tile in the tileset
 * 
 * \param tileset
 * Reference to the tileset to get info from
 * 
 * \see
 * TLN_GetTileHeight()
 */
int TLN_GetTileWidth (TLN_Tileset tileset)
{
	if (CheckBaseObject (tileset, OT_TILESET))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tileset->width;
	}
	else
		return 0;
}

/*!
 * \brief
 * Returns the height in pixels of each individual tile in the tileset
 * 
 * \param tileset
 * Reference to the tileset to get info from
 * 
 * \see
 * TLN_GetTileWidth()
 */
int TLN_GetTileHeight (TLN_Tileset tileset)
{
	if (CheckBaseObject (tileset, OT_TILESET))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tileset->height;
	}
	else
		return 0;
}

/*!
 * \brief
 * Returns the number of different tiles in tileset
 *
 * \param tileset
 * Reference to the tileset to get info from
 */
int TLN_GetTilesetNumTiles(TLN_Tileset tileset)
{
	if (CheckBaseObject(tileset, OT_TILESET))
	{
		TLN_SetLastError(TLN_ERR_OK);
		return tileset->numtiles;
	}
	else
		return 0;
}

/*!
 * \brief
 * Returns a reference to the palette associated to the specified tileset
 * 
 * \param tileset
 * Reference to the tileset to get the palette
 * 
 * \remarks
 * The palette of a tileset is created at load time and cannot be modified. When TLN_SetLayer
 * function is used to attach a tileset to a layer, the palette associated with the specified tileset is automatically
 * assigned to that layer, but it can be later replaced with TLN_SetLayerPalette
 * 
 * \see
 * TLN_LoadTileset(), TLN_SetLayerPalette()
 */
TLN_Palette TLN_GetTilesetPalette (TLN_Tileset tileset)
{
	if (CheckBaseObject (tileset, OT_TILESET))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tileset->palette;
	}
	else
		return NULL;
}

/*!
 * \brief
 * Returns a reference to the optional sequence pack associated to the specified tileset
 * 
 * \param tileset
 * Reference to the tileset to get the palette
 *
 * \see
 * TLN_LoadTileset(), TLN_CreateTileset()
 */
TLN_SequencePack TLN_GetTilesetSequencePack (TLN_Tileset tileset)
{
	if (CheckBaseObject (tileset, OT_TILESET))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tileset->sp;
	}
	else
		return NULL;
}

/* for image-based tilesets: returns bitmap with matching tileid */
TLN_Bitmap GetTilesetBitmap(TLN_Tileset tileset, int tileid)
{
	int c;
	if (!CheckBaseObject(tileset, OT_TILESET) || tileset->tstype != TILESET_IMAGES)
		return NULL;

	for (c = 0; c < tileset->numtiles; c += 1)
	{
		if (tileset->images[c].id == tileid)
			return tileset->images[c].bitmap;
	}
	return NULL;
}

/* devuelve si la línea usa color key */
static bool HasTransparentPixels (uint8_t* src, int width)
{
	register uint8_t* end = src + width;
	do {
		if (*src++ == 0) return true;
	} while (src < end);

	return false;
}
