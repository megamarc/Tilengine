/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _PALETTE_H
#define _PALETTE_H

#include "Object.h"

/* entrada de paleta */
struct Palette
{
	DEFINE_OBJECT;
	int entries;
	uint8_t data[0];
};

#define GetPaletteData(palette,index) \
	&palette->data[(index) << 2]

#define PackRGB32(r,g,b) \
	(uint32_t)(0xFF000000 | (r << 16) | (g << 8) | b)

#endif
