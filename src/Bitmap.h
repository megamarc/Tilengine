/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _BITMAP_H
#define _BITMAP_H

#include "Object.h"

/* bitmap para sprite */
struct Bitmap
{
	DEFINE_OBJECT;
	int		width;
	int		height;
	int		bpp;
	int		pitch;
	TLN_Palette palette;
	uint8_t	data[];
};

#define get_bitmap_ptr(bitmap, x, y) \
	(bitmap->data + (y) * bitmap->pitch + (x))

#endif
