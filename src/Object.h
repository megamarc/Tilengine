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
