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

#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "Tilengine.h"
#include "Sequence.h"

#define MAX_COLOR_STRIPS	32

typedef enum
{
	TYPE_NONE,
	TYPE_TILEMAP,
	TYPE_SPRITE,
	TYPE_PALETTE,
	TYPE_TILESET,
}
animation_t;

/* animación */
typedef struct
{
	animation_t type;
	TLN_Sequence sequence;
	bool enabled;
	int loop;
	int pos;
	int timer;
	int delay;
	int idx;	/* indice de capa, sprite */
	bool blend;
	TLN_Palette palette;
	TLN_Palette srcpalette;
}
Animation;

void UpdateAnimations (int time);

#endif
