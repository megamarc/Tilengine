#include <string.h>
#include <stdlib.h>
#include "Tilengine.h"
#include "LoadTMX.h"
#include "LoadFile.h"
#include "simplexml.h"
#include "Layer.h"

static TMXInfo tmxinfo;

static void init_current_layer(TLN_LayerType type)
{
	TMXLayer* layer = &tmxinfo.layers[tmxinfo.num_layers];
	memset(layer, 0, sizeof(TMXLayer));
	layer->type = type;
	layer->visible = true;
	layer->parallaxx = layer->parallaxy = 1.0f;
}

/* XML parser callback */
static void* handler(SimpleXmlParser parser, SimpleXmlEvent evt,
	const char* szName, const char* szAttribute, const char* szValue)
{
	int intvalue = 0;
	float floatvalue = 0;

	switch (evt)
	{
	case ADD_SUBTAG:
		if (!strcasecmp(szName, "layer"))
			init_current_layer(LAYER_TILE);
		else if (!strcasecmp(szName, "objectgroup"))
			init_current_layer(LAYER_OBJECT);
		else if (!strcasecmp(szName, "imagelayer"))
			init_current_layer(LAYER_BITMAP);
		else if (!strcasecmp(szName, "tileset"))
		{
			TMXTileset* tileset = &tmxinfo.tilesets[tmxinfo.num_tilesets];
			memset(tileset, 0, sizeof(TMXTileset));
		}
		break;

	case ADD_ATTRIBUTE:
		intvalue = atoi(szValue);
		floatvalue = (float)atof(szValue);
		if (!strcasecmp(szName, "map"))
		{
			if (!strcasecmp(szAttribute, "width"))
				tmxinfo.width = intvalue;
			else if (!strcasecmp(szAttribute, "height"))
				tmxinfo.height = intvalue;
			else if (!strcasecmp(szAttribute, "tilewidth"))
				tmxinfo.tilewidth = intvalue;
			else if (!strcasecmp(szAttribute, "tileheight"))
				tmxinfo.tileheight = intvalue;
			else if (!strcasecmp(szAttribute, "backgroundcolor"))
			{
				sscanf(&szValue[1], "%x", &tmxinfo.bgcolor);
				tmxinfo.bgcolor += 0xFF000000;
			}
		}

		else if (!strcasecmp(szName, "tileset"))
		{
			TMXTileset* tileset = &tmxinfo.tilesets[tmxinfo.num_tilesets];
			if (!strcasecmp(szAttribute, "firstgid"))
				tileset->firstgid = intvalue;
			else if (!strcasecmp(szAttribute, "source"))
				strncpy(tileset->source, szValue, sizeof(tileset->source));
		}

		else if (!strcasecmp(szName, "layer") || !strcasecmp(szName, "objectgroup") || !strcasecmp(szName, "imagelayer"))
		{
			TMXLayer* layer = &tmxinfo.layers[tmxinfo.num_layers];
			if (!strcasecmp(szAttribute, "name"))
				strncpy(layer->name, szValue, sizeof(layer->name));
			else if (!strcasecmp(szAttribute, "id"))
				layer->id = intvalue;
			else if (!strcasecmp(szAttribute, "visible"))
				layer->visible = (bool)intvalue;
			else if (!strcasecmp(szAttribute, "width"))
				layer->width = intvalue;
			else if (!strcasecmp(szAttribute, "height"))
				layer->height = intvalue;
			else if (!strcasecmp(szAttribute, "parallaxx"))
				layer->parallaxx = floatvalue;
			else if (!strcasecmp(szAttribute, "parallaxy"))
				layer->parallaxy = floatvalue;
			else if (!strcasecmp(szAttribute, "offsetx"))
				layer->offsetx = floatvalue;
			else if (!strcasecmp(szAttribute, "offsety"))
				layer->offsety = floatvalue;
			else if (!strcasecmp(szAttribute, "opacity"))
				layer->opacity = floatvalue;
			else if (!strcasecmp(szAttribute, "tintcolor"))
				sscanf(&szValue[1], "%x", &layer->tintcolor);
		}

		else if (!strcasecmp(szName, "image"))
		{
			TMXLayer* layer = &tmxinfo.layers[tmxinfo.num_layers];
			if (!strcasecmp(szAttribute, "source"))
				strncpy(layer->image, szValue, sizeof(layer->name));
			else if (!strcasecmp(szAttribute, "width"))
				layer->width = intvalue;
			else if (!strcasecmp(szAttribute, "height"))
				layer->height = intvalue;
		}
		break;

	case FINISH_ATTRIBUTES:
		break;

	case ADD_CONTENT:
		break;

	case FINISH_TAG:
		if (!strcasecmp(szName, "tileset") && tmxinfo.num_tilesets < TMX_MAX_TILESET - 1)
			tmxinfo.num_tilesets += 1;
		else if (!strcasecmp(szName, "layer") && tmxinfo.num_layers < TMX_MAX_LAYER - 1)
			tmxinfo.num_layers += 1;
		else if (!strcasecmp(szName, "objectgroup") && tmxinfo.num_layers < TMX_MAX_LAYER - 1)
			tmxinfo.num_layers += 1;
		else if (!strcasecmp(szName, "imagelayer") && tmxinfo.num_layers < TMX_MAX_LAYER - 1)
			tmxinfo.num_layers += 1;
		else if (!strcasecmp(szName, "object"))
			tmxinfo.layers[tmxinfo.num_layers].num_objects += 1;
		break;
	}
	return handler;
}

static int compare(void const* d1, void const* d2)
{
	TMXTileset* t1 = (TMXTileset*)d1;
	TMXTileset* t2 = (TMXTileset*)d2;
	return t1->firstgid > t2->firstgid;
}

/* loads common info about a .tmx file */
bool TMXLoad(const char* filename, TMXInfo* info)
{
	SimpleXmlParser parser;
	ssize_t size;
	uint8_t *data;
	TLN_Tilemap tilemap = NULL;
	bool retval = false;

	/* already cached: return as is */
	if (!strcasecmp(filename, tmxinfo.filename))
	{
		memcpy(info, &tmxinfo, sizeof(TMXInfo));
		return true;
	}

	/* load file */
	data = (uint8_t*)LoadFile(filename, &size);
	if (!data)
	{
		if (size == 0)
			TLN_SetLastError(TLN_ERR_FILE_NOT_FOUND);
		else if (size == -1)
			TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);
		return retval;
	}

	/* parse */
	memset(&tmxinfo, 0, sizeof(TMXInfo));
	parser = simpleXmlCreateParser((char*)data, (long)size);
	if (parser != NULL)
	{
		if (simpleXmlParse(parser, handler) != 0)
		{
			printf("parse error on line %li:\n%s\n",
				simpleXmlGetLineNumber(parser), simpleXmlGetErrorDescription(parser));
		}
		else
		{
			strncpy(tmxinfo.filename, filename, sizeof(tmxinfo.filename));
			TLN_SetLastError(TLN_ERR_OK);
			retval = true;
		}
	}
	else
		TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);

	/* sort tilesets by gid */
	qsort(&tmxinfo.tilesets, tmxinfo.num_tilesets, sizeof(TMXTileset), compare);

	simpleXmlDestroyParser(parser);
	free(data);
	if (retval)
		memcpy(info, &tmxinfo, sizeof(TMXInfo));
	return retval;
}

/* returns index of suitable tileset acoording to gid range */
int TMXGetSuitableTileset(TMXInfo* info, int gid, TMXTileset* tmxtilesets)
{
	/* if no tilesets list provided, use internal one */
	if (tmxtilesets == NULL)
		tmxtilesets = info->tilesets;

	int c;
	for (c = 0; c < info->num_tilesets; c += 1, tmxtilesets += 1)
	{
		if (gid >= tmxtilesets->firstgid && (gid < tmxtilesets[1].firstgid || tmxtilesets[1].firstgid == 0))
			return c;
	}
	return c;
}

/*returns first layer of requested type */
TMXLayer* TMXGetFirstLayer(TMXInfo* info, TLN_LayerType type)
{
	int c;
	for (c = 0; c < info->num_layers; c += 1)
	{
		if (info->layers[c].type == type)
			return &info->layers[c];
	}
	return NULL;
}

/* returns specified layer */
TMXLayer* TMXGetLayer(TMXInfo* info, const char* name)
{
	int c;
	for (c = 0; c < info->num_layers; c += 1)
	{
		if (!strcasecmp(info->layers[c].name, name))
			return &info->layers[c];
	}
	return NULL;
}