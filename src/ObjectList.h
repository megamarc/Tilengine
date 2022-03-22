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
#include "crc32.h"
#include "Spriteset.h"

typedef struct _Object
{
	uint16_t id;
	uint16_t gid;
	uint16_t flags;
	uint8_t type;
	char name[64];
	int x;
	int y;
	int width;
	int height;
	TLN_Bitmap bitmap;	/* computed after calling TLN_SetLayerObjects() */
	bool has_gid;
	bool visible;
	struct _Object* next;
}
TLN_Object;

struct ObjectList
{
	DEFINE_OBJECT;
	int num_items;	/* items in list */
	int width;		/* map width, pixels */
	int height;		/* map height, pixels */
	int id;			/* id property */
	bool visible;	/* visible property */
	TLN_Tileset tileset;	/* attached tileset, if any */
	struct _Object* list;
	struct _Object* last;
	struct _Object* iterator;
	TLN_ObjectInfo* info;
};

extern bool IsObjectInLine(struct _Object* object, int x1, int x2, int y);

#endif