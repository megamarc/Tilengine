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

/* set de tiles */
struct Tileset
{
	DEFINE_OBJECT;
	int		numtiles;		 /* nº de tiles en el set */
	int		width;			 /* tamaño horizontal de un tile */
	int		height;			 /* tamaño vertical de un tile */
	int		hshift;			 /* tipo horizontal */
	int		vshift;			 /* tipo vertical */
	int		hmask;			 /* mascara horizontal */
	int		vmask;			 /* mascara vertical */
	int		size_tiles;		 /* tamaño de la sección de tiles */
	int		size_color;		 /* tamaño de la sección de color key */
	struct Palette* palette; /* paleta original */
	struct SequencePack* sp; /* secuencias asociadas (si hay) */
	bool*	color_key;		 /* puntero a array indicando si cada línea tiene color key */
	TLN_TileAttributes* attributes;	/* puntero a array de atributos, uno por tile */
	uint8_t	data[];
};

#define GetTilesetLine(tileset,index,y) \
	((index << tileset->vshift) + y)

#define GetTilesetPixel(tileset,index,x,y) \
	tileset->data[(((index << tileset->vshift) + y) << tileset->hshift) + x]

#endif
