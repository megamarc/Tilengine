/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _SPRITE_H
#define _SPRITE_H

#include "Tilengine.h"
#include "Draw.h"
#include "Blitters.h"
#include "Spriteset.h"
#include "List.h"
#include "Animation.h"

/* rectangulo */
typedef struct
{
	int x1,y1,x2,y2;
}
rect_t;

extern void MakeRect(rect_t* rect, int x, int y, int w, int h);

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
	int				x,y;			/* screen space location (TLN_SetSpritePosition) */
	int				dx,dy;
	int				xworld, yworld;	/* world space location (TLN_SetSpriteWorldPosition) */
	float			sx,sy;
	float			ptx, pty;		/* normalized pivot position inside sprite (default = 0,0) */
	rect_t			srcrect;
	rect_t			dstrect;
	draw_t			mode;
	uint8_t*		blend;
	uint32_t		flags;
	ScanDrawPtr		draw;
	ScanBlitPtr		blitter;
	bool			ok;
	bool			do_collision;
	bool			collision;
	bool			world_space;	/* valid position is world space, false = screen space */
	bool			dirty;			/* requires call to UpdatePosition() before drawing */
	TLN_Bitmap		rotation_bitmap;
	ListNode		list_node;
	Animation		animation;
}
Sprite;

extern void UpdateSprite(Sprite* sprite);

#endif