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
#include "LoadTMX.h"

/* properties */
typedef enum
{
	PROPERTY_NONE,
	PROPERTY_TYPE,
	PROPERTY_PRIORITY,
}
Property;

/* load manager */
struct
{
	char layer_name[64];		/* name of layer to load */
	bool state;
	TLN_ObjectList objects;
	TLN_Object object;
	Property property;			/* current property */
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
		if (!strcasecmp(szName, "object"))
			memset(&loader.object, 0, sizeof(struct _Object));
		break;

	case ADD_ATTRIBUTE:

		intvalue = atoi(szValue);
		if (!strcasecmp(szName, "objectgroup") && (!strcasecmp(szAttribute, "name")))
		{
			if (!strcasecmp(szValue, loader.layer_name))
				loader.state = true;
			else
				loader.state = false;
		}

		else if (!strcasecmp(szName, "object"))
		{
			if (!strcasecmp(szAttribute, "id"))
				loader.object.id = intvalue;
			else if (!strcasecmp(szAttribute, "gid"))
			{
				Tile tile;
				tile.value = strtoul(szValue, NULL, 0);
				loader.object.flags = tile.flags;
				loader.object.gid = tile.index;
			}
			else if (!strcasecmp(szAttribute, "x"))
				loader.object.x = intvalue;
			else if (!strcasecmp(szAttribute, "y"))
				loader.object.y = intvalue;
			else if (!strcasecmp(szAttribute, "width"))
				loader.object.width = intvalue;
			else if (!strcasecmp(szAttribute, "height"))
				loader.object.height = intvalue;
		}

		/* <property name="type" type="int" value="12"/> */
		else if (!strcasecmp(szName, "property"))
		{
			if (!strcasecmp(szAttribute, "name"))
			{
				if (!strcasecmp(szValue, "priority"))
					loader.property = PROPERTY_PRIORITY;
				else
					loader.property = PROPERTY_NONE;
			}
			else if (!strcasecmp(szAttribute, "value"))
			{
				if (loader.property == PROPERTY_PRIORITY)
				{
					if (!strcasecmp(szValue, "true"))
						loader.object.flags += FLAG_PRIORITY;
				}
			}
		}
		break;

	case FINISH_ATTRIBUTES:
		if (loader.state == true)
		{
			if (!strcasecmp(szName, "objectgroup"))
				loader.objects = TLN_CreateObjectList();
		}
		break;

	case ADD_CONTENT:
		break;

	case FINISH_TAG:
		if (loader.state == true)
		{
			if (!strcasecmp(szName, "objectgroup"))
				loader.state = false;
			else if (!strcasecmp(szName, "object"))
			{
				loader.object.y -= loader.object.height;
				CloneObjectToList(loader.objects, &loader.object);
			}
		}
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
 * \param id Unique ID of the tileset object
 * \param gid Graphic Id (tile index) of the tileset object
 * \param flags Combination of FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY
 * \param x Layer-space horizontal coordinate of the top-left corner
 * \param y Layer-space bertical coordinate of the top-left corner
 * \return true if success or false if error
 */
bool TLN_AddTileObjectToList(TLN_ObjectList list, uint16_t id, uint16_t gid, uint16_t flags, int x, int y)
{
	struct _Object* object;

	if (!CheckBaseObject(list, OT_OBJECTLIST))
		return false;

	object = (struct _Object*)calloc(1, sizeof(struct _Object));
	object->gid = gid;
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
 * \return Reference to the loaded object or NULL if error
 */
TLN_ObjectList TLN_LoadObjectList(const char* filename, const char* layername)
{
	SimpleXmlParser parser;
	ssize_t size;
	uint8_t *data;
	TMXInfo tmxinfo = { 0 };

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
	TMXLoad(filename, &tmxinfo);
	memset(&loader, 0, sizeof(loader));
	if (layername)
		strncpy(loader.layer_name, layername, sizeof(loader.layer_name));
	else
		strncpy(loader.layer_name, TMXGetFirstLayerName(&tmxinfo, LAYER_OBJECT), sizeof(loader.layer_name));

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

	if (loader.objects != NULL)
	{
		TLN_Tileset tileset = NULL;
		TMXTileset* tmxtileset;
		struct _Object* item;
		int gid = 0;

		/* find suitable tileset */
		item = loader.objects->list;
		while (item && gid == 0)
		{
			if (item->gid > 0)
				gid = item->gid;
			item = item->next;
		}
		tmxtileset = TMXGetSuitableTileset(&tmxinfo, gid);
		tileset = TLN_LoadTileset(tmxtileset->source);

		/* correct with firstgid */
		item = loader.objects->list;
		while (item)
		{
			if (item->gid > 0)
				item->gid = item->gid - tmxtileset->firstgid;
			item = item->next;
		}

		loader.objects->tileset = tileset;
		loader.objects->width = tmxinfo.width*tmxinfo.tilewidth;
		loader.objects->height = tmxinfo.height*tmxinfo.tileheight;
	}
	
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
	list->iterator = NULL;
	return list;
}

/*!
 * \brief Returns number of items in TLN_ObjectList
 * \param list Pointer to TLN_ObjectList to query
 * \return number of items 
 */
int TLN_GetListNumObjects(TLN_ObjectList list)
{
	if (CheckBaseObject(list, OT_OBJECTLIST))
	{
		TLN_SetLastError(TLN_ERR_OK);
		return list->num_items;
	}
	else
	{
		TLN_SetLastError(TLN_ERR_REF_LIST);
		return 0;
	}
}

/*!
 * \brief Iterates over elements in a TLN_ObjectList
 * \param list Reference to TLN_ObjectList to get items
 * \param info Pointer to user-allocated TLN_ObjectInfo struct
 * \return true if item returned, false if no more items left
 * \remarks The info pointer acts as a switch to select first/next element:
 *	* If not NULL, starts the iterator and returns the first item
 *  * If NULL, return the next item
 */
bool TLN_GetListObject(TLN_ObjectList list, TLN_ObjectInfo* info)
{
	if (!CheckBaseObject(list, OT_OBJECTLIST))
	{
		TLN_SetLastError(TLN_ERR_REF_LIST);
		return false;
	}

	/* start iterator */
	if (info != NULL)
	{
		list->iterator = list->list;
		list->info = info;
	}

	if (list->iterator == NULL)
		return false;

	/* copy info */
	list->info->id = list->iterator->id;
	list->info->gid = list->iterator->gid;
	list->info->flags = list->iterator->flags;
	list->info->x = list->iterator->x;
	list->info->y = list->iterator->y;
	list->info->width = list->iterator->width;
	list->info->height = list->iterator->height;

	/* advance */
	list->iterator = list->iterator->next;
	return true;
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
