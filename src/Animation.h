/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "Tilengine.h"
#include "Sequence.h"
#include "List.h"

#define MAX_COLOR_STRIPS	64

typedef enum
{
	TYPE_NONE,
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
	TLN_Tileset tileset;	/* tileset for tileset animations */
	bool enabled;
	int loop;
	int pos;
	int timer;
	int nsprite;			/* sprite number for sprite animation */
	bool blend;
	TLN_Palette palette;
	TLN_Palette srcpalette;
	ListNode list_node;
}
Animation;

bool SetTilesetAnimation(TLN_Tileset tileset, int index, TLN_Sequence sequence);
void UpdateAnimation(Animation* animation, int time);

#endif
