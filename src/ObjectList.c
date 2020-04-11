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

static bool CloneObjectToList(TLN_ObjectList list, TLN_Object* data);

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
			if (!strcasecmp(szAttribute, "gid"))
				loader.object.tileid = intvalue - loader.firstgid;
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
			loader.objects = TLN_CreateObjectList();
		}
		else if (!strcasecmp(szName, "object"))
			CloneObjectToList(loader.objects, &loader.object);
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
	return !(rect1->x2 < rect2->x1 || rect1->x1 > rect2->x2 || rect1->y2 < rect2->y1 || rect1->y1 > rect2->y2);
}

/*!
 * \brief Creates a TLN_ObjectList
 * The list is created empty, it must be populated with TLN_AddSpriteToList()
 * and assigned to a layer with TLN_SetLayerObjects()
 * \return Reference to new object or NULL if error
 */
TLN_ObjectList TLN_CreateObjectList(void)
{
	TLN_ObjectList list = NULL;
	const int size = sizeof(struct ObjectList);

	/* create */
	list = (TLN_ObjectList)CreateBaseObject(OT_OBJECTLIST, size);
	if (!list)
		return NULL;

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
	object->next = NULL;
}

/*!
 * \brief Adds a TLN_Object item to a given TLN_ObjectList
 * \param list Reference to the list
 * \param data Pointer to a user-provided TLN_Object. This object is internally copied to the list, so it's safe to discard the user-provided one after addition.
 * \return true if success or false if error
 */
static bool CloneObjectToList(TLN_ObjectList list, TLN_Object* data)
{
	struct _Object* object;

	if (!CheckBaseObject(list, OT_OBJECTLIST))
		return false;

	object = (struct _Object*)calloc(1, sizeof(struct _Object));
	memcpy(object, data, sizeof(struct _Object));
	add_to_list(list, object);
	return true;
}
/*!
 * \brief Adds an image-based tileset item to given TLN_ObjectList
 * 
 * \param list Reference to TLN_ObjectList
 * \param tileid Id of the tileset object to insert
 * \param x Layer-space horizontal coordinate of the top-left corner
 * \param y Layer-space bertical coordinate of the top-left corner
 * \return true if success or false if error
 */
bool TLN_AddTileObjectToList(TLN_ObjectList list, int tileid, int x, int y)
{
	struct _Object* object;

	if (!CheckBaseObject(list, OT_OBJECTLIST))
		return false;

	object = (struct _Object*)calloc(1, sizeof(struct _Object));
	object->tileid = tileid;
	object->x = x;
	object->y = y;
	add_to_list(list, object);
	return true;
}

/*!
 * \brief Loads an object list from a Tiled object layer
 * 
 * \param filename Name of the .tmx file containing the list
 * \param layername Name of the layer to load
 * \param firstgid First graphic id (gid) of the tileset used by layer, must match Tiled value!
 * \return Reference to the loaded object or NULL if error
 */
TLN_ObjectList TLN_LoadObjectList(const char* filename, const char* layername, int firstgid)
{
	SimpleXmlParser parser;
	ssize_t size;
	uint8_t *data;

	/* load file */
	data = (uint8_t*)LoadFile(filename, &size);
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

/*!
 * \brief Creates a duplicate of a given TLN_ObjectList object
  * \param src Reference to the source object to clone
 * \return A reference to the newly cloned object list, or NULL if error
 */
TLN_ObjectList TLN_CloneObjectList(TLN_ObjectList src)
{
	TLN_ObjectList list;
	struct _Object* object;

	if (!CheckBaseObject(src, OT_OBJECTLIST))
		return NULL;

	list = (TLN_ObjectList)CloneBaseObject(src);
	object = src->list;
	while (object != NULL)
	{
		CloneObjectToList(list, object);
		object = object->next;
	}
	return list;
}

bool IsObjectInLine(struct _Object* object, int x1, int x2, int y)
{
	rect_t rect;
	MakeRect(&rect, object->x, object->y, object->width, object->height);
	if (y >= rect.y1 && y < rect.y2 && !(x1 > rect.x2 || x2 < rect.x1))
		return true;
	else
		return false;
}

/*!
 * \brief Deletes object list
 * 
 * \param list Reference to list to delete
* \return true if success or false if error
 */
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
