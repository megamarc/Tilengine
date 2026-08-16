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

/* color definition */
typedef union
{
	struct
	{
		uint8_t b, g, r, a;
	};
	uint32_t value;
}
Color;

/* palette object */
struct Palette
{
	DEFINE_OBJECT;
	int entries;		/* number of colors */
	uint32_t data[0];	/* variable size Color array */
};

/* returns pointer to specified index color definition */
#define GetPaletteData(palette,index) \
	&palette->data[index]

#define PackRGB32(r,g,b) \
	(uint32_t)(0xFF000000 | (r << 16) | (g << 8) | b)

#endif
