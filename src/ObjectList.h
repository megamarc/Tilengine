/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _OBJECT_LIST_H
#define _OBJECT_LIST_H

#include "Tilengine.h"
#include "Object.h"
#include "Hash.h"
#include "Spriteset.h"

struct _Object
{
	TLN_Object data;
	SpriteEntry* sprite;
	struct _Object* next;
};

struct ObjectList
{
	DEFINE_OBJECT;
	int num_items;
	struct _Object* list;
	struct _Object* last;
};

extern bool IsObjectInLine(struct _Object* object, int x1, int x2, int y);

#endif