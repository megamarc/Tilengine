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

#ifndef _SPRITE_H
#define _SPRITE_H

#include "Tilengine.h"
#include "Draw.h"
#include "Blitters.h"
#include "Spriteset.h"

/* rectangulo */
typedef struct
{
	int x1,y1,x2,y2;
}
rect_t;

/* sprite */
typedef struct Sprite
{
	TLN_Spriteset	spriteset;
	TLN_Palette		palette;
	SpriteEntry*	info;
	uint8_t*		pixels;
	int				pitch;
	int				num;
	int				index;
	int				x,y;
	int				dx,dy;
	float			sx,sy;
	rect_t			srcrect;
	rect_t			dstrect;
	draw_t			mode;
	uint8_t*		blend;
	TLN_TileFlags	flags;
	ScanDrawPtr		draw;
	ScanBlitPtr		blitter;
	bool			ok;
	bool			do_collision;
	bool			collision;
	TLN_Bitmap		rotation_bitmap;
}
Sprite;

#endif