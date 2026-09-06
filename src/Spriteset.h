/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _SPRITESET_H
#define _SPRITESET_H

#include "Object.h"
#include "Tilengine.h"
#include "crc32.h"

/* registro de sprite */
typedef struct
{
	uint32_t hash;
	int w,h;
	int offset;
}
SpriteEntry;

struct Spriteset
{
	DEFINE_OBJECT;
	int entries;
	TLN_Bitmap bitmap;
	TLN_Palette palette;
	SpriteEntry data[];
};

TLN_SpriteInfo* GetSpriteInfo (TLN_Spriteset spriteset, int entry);

#endif
