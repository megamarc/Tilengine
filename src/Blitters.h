/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _BLITTERS_H
#define _BLITTERS_H

#include "Tilengine.h"

/* blitter callback signature */
typedef void(*ScanBlitPtr) \
	(uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend);

#ifdef __cplusplus
extern "C" {
#endif

	/* returns suitable blitter for specified conditions */
	ScanBlitPtr SelectBlitter(bool key, bool scaling, bool blend);

	/* paints constant color */
	void BlitColor(void* dstptr, uint32_t color, int width);

	/* perfoms direct 32 -> 32 bpp blit with opcional blend */
	void Blit32_32(uint32_t *src, uint32_t* dst, int width, uint8_t* blend);

	/* performs mosaic blit */
	void BlitMosaic(uint32_t *src, uint32_t* dst, int width, int size, uint8_t* blend);

#ifdef __cplusplus
}
#endif

#endif