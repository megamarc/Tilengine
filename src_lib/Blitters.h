/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2018 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BLITTERS_H
#define _BLITTERS_H

#include "Tilengine.h"

typedef void (*ScanBlitPtr) \
	(uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend);

ScanBlitPtr GetBlitter (int bpp, bool key, bool scaling, bool blend);

void BlitColor (void* dstptr, uint32_t color, int width);
void BlitMosaicSolid (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int size);
void BlitMosaicBlend (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int size, uint8_t* blend);

#endif