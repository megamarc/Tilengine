/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _OBJECT_H
#define _OBJECT_H

#include "Tilengine.h"

/* tipos de objeto */
typedef enum
{
	OT_NONE,
	OT_PALETTE,
	OT_TILEMAP,
	OT_TILESET,
	OT_SPRITESET,
	OT_BITMAP,
	OT_SEQUENCE,
	OT_SEQPACK,
	OT_OBJECTLIST,
}
ObjectType;

#define DEFINE_OBJECT \
	ObjectType type; \
	int guid; \
	int size; \
	int owner

typedef struct
{
	DEFINE_OBJECT;	
	uint8_t data[0];
}
object_t;

#define ObjectType(ptr) ((object_t*)ptr)->type
#define ObjectSize(ptr) ((object_t*)ptr)->size
#define ObjectOwner(ptr) ((object_t*)ptr)->owner

/* prototipos */
void* CreateBaseObject (ObjectType type, int size);
void* CloneBaseObject (void* object);
void  DeleteBaseObject (void* object);
bool  CheckBaseObject (void* object, ObjectType type);
void  CopyBaseObject (void* dstobject, void* srcobject);

unsigned int GetNumObjects (void);
unsigned int GetNumBytes (void);

#endif
