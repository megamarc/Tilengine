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

#ifndef _SPRITESET_H
#define _SPRITESET_H

#include "Object.h"
#include "Tilengine.h"
#include "Hash.h"

/* registro de sprite */
typedef struct
{
	hash_t hash;
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
