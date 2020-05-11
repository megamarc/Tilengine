/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _TILESET_H
#define _TILESET_H

#include "Object.h"
#include "Palette.h"
#include "SequencePack.h"
#include "Bitmap.h"
#include "Animation.h"

/* types of tilesets */
typedef enum
{
	TILESET_NONE,
	TILESET_TILES,
	TILESET_IMAGES,
}
TilesetType;

/* Tileset definition */
struct Tileset
{
	DEFINE_OBJECT;
	TilesetType tstype;		 /* tileset type */
	int		numtiles;		 /* number of tiles */
	int		width;			 /* horizontal tile size */
	int		height;			 /* vertical tile size */
	int		hshift;			 /* horizontal shift */
	int		vshift;			 /* vertical shift */
	int		hmask;			 /* horizontal bitmask */
	int		vmask;			 /* vertical bitmask */
	int		size_tiles;		 /* size of tiles collection section */
	TLN_Palette palette;	 /* palette */
	TLN_SequencePack sp;	 /* associated sequences (if any) */
	Animation* animations;	 /* active tile animations */
	TLN_TileImage* images;	/* image tiles array */
	TLN_TileAttributes* attributes;	/* attribute array */
	bool* color_key;		 /* array telling if each line has color key or is solid */
	uint16_t* tiles;		/* tile indexes for animation */
	uint8_t	data[];			 /* variable size data for images[], attributes[], color_key[] and pixels */
};

#define GetTilesetLine(tileset,index,y) \
	((index << tileset->vshift) + y)

#define GetTilesetPixel(tileset,index,x,y) \
	tileset->data[(((index << tileset->vshift) + y) << tileset->hshift) + x]

TLN_Bitmap GetTilesetBitmap(TLN_Tileset tileset, int tileid);

#endif
