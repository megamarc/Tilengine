/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <string.h>
#include <stdlib.h>
#include "Tilengine.h"
#include "ObjectList.h"
#include "Sprite.h"
#include "simplexml.h"
#include "LoadFile.h"

typedef enum
{
	LS_IDLE,	/* waiting for start */
	LS_ACTIVE,	/* loading underway */
	LS_DONE,	/* loading done, ignore everything */
}
LoadState;

/* load manager */
struct
{
	char layer_name[64];		/* name of layer to load */
	LoadState state;
	int firstgid;
	int width;
	int height;
	int tilewidth;
	int tileheight;
	TLN_ObjectList objects;
	TLN_Object object;
}
static loader;

/* XML parser callback */
static void* handler(SimpleXmlParser parser, SimpleXmlEvent evt,
	const char* szName, const char* szAttribute, const char* szValue)
{
	int intvalue;
	switch (evt)
	{
	case ADD_SUBTAG:
		if (!strcasecmp(szName, "map"))
			return handler;

		else if (loader.state == LS_IDLE && !strcasecmp(szName, "objectgroup"))
			return handler;
		
		/* create new object and add to list */
		else if (loader.state == LS_ACTIVE && !strcasecmp(szName, "object"))
		{
			return handler;
		}
		return NULL;

	case ADD_ATTRIBUTE:

		intvalue = atoi(szValue);
		if (!strcasecmp(szName, "map"))
		{
			if (!strcasecmp(szAttribute, "width"))
				loader.width = intvalue;
			else if (!strcasecmp(szAttribute, "height"))
				loader.height = intvalue;
			else if (!strcasecmp(szAttribute, "tilewidth"))
				loader.tilewidth = intvalue;
			else if (!strcasecmp(szAttribute, "tileheight"))
				loader.tileheight = intvalue;
		}

		else if (!strcasecmp(szName, "objectgroup") && !strcasecmp(szAttribute, "name"))
		{
			if (loader.layer_name[0] == 0 || !strcasecmp(szValue, loader.layer_name))
				loader.state = LS_ACTIVE;
		}

		else if (!strcasecmp(szName, "object"))
		{
			if (!strcasecmp(szAttribute, "id"))
				loader.object.gid = intvalue;
			else if (!strcasecmp(szAttribute, "gid"))
				loader.object.gid = intvalue - loader.firstgid;
			else if (!strcasecmp(szAttribute, "x"))
				loader.object.x = intvalue;
			else if (!strcasecmp(szAttribute, "y"))
				loader.object.y = intvalue;
			else if (!strcasecmp(szAttribute, "width"))
				loader.object.width = intvalue;
			else if (!strcasecmp(szAttribute, "height"))
				loader.object.height = intvalue;
		}
		break;

	case FINISH_ATTRIBUTES:
		if (!strcasecmp(szName, "objectgroup"))
		{
			/* create */
			loader.width *= loader.tilewidth;
			loader.height *= loader.tileheight;
			loader.objects = TLN_CreateObjectList(loader.width, loader.height);
		}
		else if (!strcasecmp(szName, "object"))
			TLN_AddObjectToList(loader.objects, &loader.object);
		break;

	case ADD_CONTENT:
		break;

	case FINISH_TAG:
		if (!strcasecmp(szName, "objectgroup"))
			loader.state = LS_DONE;
		break;
	}
	return handler;
}

static bool intersetcs(rect_t* rect1, rect_t* rect2)
{
	// TODO
	return false;
}

TLN_ObjectList TLN_CreateObjectList(int width, int height)
{
	TLN_ObjectList list = NULL;
	const int size = sizeof(struct ObjectList);

	/* create */
	list = CreateBaseObject(OT_OBJECTLIST, size);
	if (!list)
		return NULL;

	/* copy data */
	list->width = width;
	list->height = height;

	TLN_SetLastError(TLN_ERR_OK);
	return list;
}

/* adds entry to linked list */
static void add_to_list(TLN_ObjectList list, struct _Object* object)
{
	if (list->list == NULL)
		list->list = object;
	else
		list->last->next = object;
	list->last = object;
	list->num_items += 1;
}

TLNAPI bool TLN_AddObjectToList(TLN_ObjectList list, TLN_Object* data)
{
	struct _Object* object;

	if (!CheckBaseObject(list, OT_OBJECTLIST))
		return false;

	object = calloc(1, sizeof(struct _Object));
	memcpy(&object->data, data, sizeof(TLN_Object));
	add_to_list(list, object);
	return true;
}

TLNAPI bool TLN_AddSpriteToList(TLN_ObjectList list, TLN_Spriteset spriteset, const char* name, int id, int x, int y)
{
	struct _Object* object;
	int index;

	if (!CheckBaseObject(list, OT_OBJECTLIST) || !CheckBaseObject(spriteset, OT_SPRITESET))
		return false;

	index = TLN_FindSpritesetSprite(spriteset, name);
	if (index == -1)
		return false;

	object = calloc(1, sizeof(struct _Object));
	object->sprite = &spriteset->data[index];
	object->data.x = x;
	object->data.y = y;
	object->data.width = object->sprite->w;
	object->data.height = object->sprite->h;
	object->data.gid = index;
	object->data.id = id;
	add_to_list(list, object);
	return true;
}

TLN_ObjectList TLN_LoadObjectList(const char* filename, const char* layername, int firstgid)
{
	SimpleXmlParser parser;
	ssize_t size;
	uint8_t *data;

	/* load file */
	data = LoadFile(filename, &size);
	if (!data)
	{
		if (size == 0)
			TLN_SetLastError(TLN_ERR_FILE_NOT_FOUND);
		else if (size == -1)
			TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}

	/* parse */
	memset(&loader, 0, sizeof(loader));
	loader.firstgid = firstgid;
	if (layername)
	{
		strncpy(loader.layer_name, layername, sizeof(loader.layer_name));
		loader.layer_name[sizeof(loader.layer_name) - 1] = '\0';
	}
	parser = simpleXmlCreateParser((char*)data, (long)size);
	if (parser != NULL)
	{
		if (simpleXmlParse(parser, handler) != 0)
		{
			printf("parse error on line %li:\n%s\n",
				simpleXmlGetLineNumber(parser), simpleXmlGetErrorDescription(parser));
			TLN_SetLastError(TLN_ERR_WRONG_FORMAT);
		}
		else
			TLN_SetLastError(TLN_ERR_OK);
	}
	else
		TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);

	simpleXmlDestroyParser(parser);
	free(data);
	
	return loader.objects;
}

TLN_ObjectList TLN_CloneObjectList(TLN_ObjectList src)
{
	TLN_ObjectList list;
	struct _Object* object;

	if (!CheckBaseObject(src, OT_OBJECTLIST))
		return NULL;

	list = CloneBaseObject(src);
	object = src->list;
	while (object != NULL)
	{
		TLN_AddObjectToList(list, &object->data);
		object = object->next;
	}
	return list;
}

bool TLN_GetObjectInfo(TLN_ObjectList list, int entry, TLN_Object* info)
{
	return false;
}

int TLN_GetObjectsInReigion(TLN_ObjectList list, int x, int y, int width, int height, int array_size, TLN_Object* objects)
{
	struct _Object* object;
	int current = 0;
	rect_t rect1;
	if (!CheckBaseObject(list, OT_OBJECTLIST))
		return false;

	MakeRect(&rect1, x, y, width, height);
	object = list->list;
	while (object != NULL)
	{
		rect_t rect2;
		TLN_Object* data = &object->data;
		MakeRect(&rect2, data->x, data->y, data->width, data->height);
		if (intersetcs(&rect1, &rect2))
		{
			if (current < array_size)
				memcpy(&objects[current], data, sizeof(TLN_Object));
			current += 1;
		}
		object = object->next;
	}
	return current;
}

bool IsObjectInLine(struct _Object* object, int x1, int x2, int y)
{
	rect_t rect;
	TLN_Object* data = &object->data;
	MakeRect(&rect, data->x, data->y, data->width, data->height);
	if (y >= rect.y1 && y < rect.y2 && !(x1 > rect.x2 || x2 < rect.x1))
		return true;
	else
		return false;
}

bool TLN_DeleteObjectList(TLN_ObjectList list)
{
	struct _Object* object;
	if (!CheckBaseObject(list, OT_OBJECTLIST))
		return false;

	/* delete nodes */
	object = list->list;
	while (object != NULL)
	{
		struct _Object* next;
		next = object->next;
		free(object);
		object = next;
	}

	DeleteBaseObject(list);
	return true;
}
