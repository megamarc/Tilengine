/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _DRAW_H
#define _DRAW_H

/* modos de render */
typedef enum
{
	MODE_NORMAL,
	MODE_SCALING,
	MODE_TRANSFORM,
	MODE_PIXEL_MAP,
	MAX_DRAW_MODE
}
draw_t;

typedef bool (*ScanDrawPtr)(int,uint32_t*,int,int,int);
typedef struct Layer Layer;

ScanDrawPtr GetLayerDraw (Layer* layer);
ScanDrawPtr GetSpriteDraw (draw_t mode);

extern bool DrawScanline(void);

#endif
