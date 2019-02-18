/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _LAYER_H
#define _LAYER_H

#include "Tilengine.h"
#include "Draw.h"
#include "Blitters.h"
#include "Math2D.h"

/* tipo de capa */
typedef enum
{
	LAYER_NORMAL,
	LAYER_AFFINE,
}
layer_t;

/* capa */
typedef struct Layer
{
	/* configuración */
	layer_t		type;		/* tipo de capa */
	TLN_Tileset	tileset;	/* puntero al tileset */
	TLN_Tilemap	tilemap;	/* puntero al array de memoria con el mapa de tiles */
	TLN_Palette	palette;	/* puntero a la paleta */
	TLN_Bitmap  bitmap;		/* puntero al bitmap (modo bitmap) */
	int			width;		/* anchura total en píxeles */
	int			height;		/* altura total en píxeles */
	bool		ok;
	bool		affine;
	ScanDrawPtr draw;
	ScanBlitPtr blitters[2];
	Matrix3		transform;
	int*		column;		/* offset de columna (opcional) */
	fix_t		xfactor;
	fix_t		dx;
	fix_t		dy;
	uint8_t*	blend;		/* puntero a tabla de transparencia (NULL = no hay) */
	TLN_PixelMap* pixel_map;	/* puntero a tabla de pixel map (NULL = no hay) */
	draw_t		mode;
	
	/* */
	int			hstart;		/* offset de inicio horizontal */
	int			vstart;		/* offset de inicio vertical */

	/* clip */
	struct
	{
		int x1, y1, x2, y2;
	}
	clip;

	/* mosaico */
	struct
	{
		int w,h;			/* tamaño del pixel */
		uint8_t* buffer;	/* linea temporal */
	}
	mosaic;
}
Layer;

#endif