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

/* capa */
typedef struct Layer
{
	TLN_LayerType	type;		/* layer type */
	TLN_Tilemap		tilemap;	/* pointer to tilemap */
	TLN_Palette		palette;	/* pointer to current color alette */
	TLN_Bitmap		bitmap;		/* pointer to bitmap (bitmap layer mode) */
	TLN_ObjectList	objects;	/* pointer to object list (objects layer mode) */
	int				width;		/* layer width in pixels */
	int				height;		/* layer height in pixels */
	bool			ok;
	bool			affine;
	ScanDrawPtr		draw;
	ScanBlitPtr		blitters[2];
	Matrix3			transform;
	int*			column;		/* column offset (optional) */
	fix_t			xfactor;
	fix_t			dx;
	fix_t			dy;
	uint8_t*		blend;		/* pointer to blend table */
	TLN_PixelMap*	pixel_map;	/* pointer to pixel mapping table */
	draw_t			mode;
	bool			priority;	/* whole layer in front of regular sprites */

	/* world mode related data */
	struct
	{
		int offsetx, offsety;
		float xfactor, yfactor;
	}
	world;
	bool dirty;					/* requires UpdateLayer() before draw */
	
	/* */
	int				hstart;		/* horizontal start offset */
	int				vstart;		/* vertical start offset*/

	/* clip */
	struct
	{
		int x1, y1, x2, y2;
	}
	clip;

	/* mosaic */
	struct
	{
		int w, h;			/* virtual pixel size */
		uint32_t* buffer;	/* line buffer */
	}
	mosaic;
}
Layer;

Layer* GetLayer(int index);
void UpdateLayer(int nlayer);

#endif