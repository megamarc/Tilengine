#include <string.h>
#include <stdlib.h>
#include "Tilengine.h"
#include "LoadTMX.h"
#include "LoadFile.h"
#include "simplexml.h"

static TMXInfo* tmxinfo = NULL;

/* XML parser callback */
static void* handler(SimpleXmlParser parser, SimpleXmlEvent evt,
	const char* szName, const char* szAttribute, const char* szValue)
{
	int intvalue = 0;
	switch (evt)
	{
	case ADD_SUBTAG:
		if (!strcasecmp(szName, "layer"))
			tmxinfo->layers[tmxinfo->num_layers].type = LAYER_TILE;
		else if (!strcasecmp(szName, "objectgroup"))
			tmxinfo->layers[tmxinfo->num_layers].type = LAYER_OBJECT;
		break;

	case ADD_ATTRIBUTE:
		intvalue = atoi(szValue);
		if (!strcasecmp(szName, "map"))
		{
			if (!strcasecmp(szAttribute, "width"))
				tmxinfo->width = intvalue;
			else if (!strcasecmp(szAttribute, "height"))
				tmxinfo->height = intvalue;
			else if (!strcasecmp(szAttribute, "tilewidth"))
				tmxinfo->tilewidth = intvalue;
			else if (!strcasecmp(szAttribute, "tileheight"))
				tmxinfo->tileheight = intvalue;
		}

		else if (!strcasecmp(szName, "tileset"))
		{
			if (!strcasecmp(szAttribute, "firstgid"))
				tmxinfo->tilesets[tmxinfo->num_tilesets].firstgid = intvalue;
			else if (!strcasecmp(szAttribute, "source"))
				strncpy(tmxinfo->tilesets[tmxinfo->num_tilesets].source, szValue, 64);
		}

		else if (!strcasecmp(szName, "layer") || !strcasecmp(szName, "objectgroup"))
		{
			if (!strcasecmp(szAttribute, "name"))
				strncpy(tmxinfo->layers[tmxinfo->num_layers].name, szValue, 64);
			else if (!strcasecmp(szAttribute, "width"))
				tmxinfo->layers[tmxinfo->num_layers].width = intvalue;
			else if (!strcasecmp(szAttribute, "height"))
				tmxinfo->layers[tmxinfo->num_layers].height = intvalue;
		}
		break;

	case FINISH_ATTRIBUTES:
		break;

	case ADD_CONTENT:
		break;

	case FINISH_TAG:
		if (!strcasecmp(szName, "tileset") && tmxinfo->num_tilesets < TMX_MAX_TILESET - 1)
			tmxinfo->num_tilesets += 1;
		else if (!strcasecmp(szName, "layer") && tmxinfo->num_layers < TMX_MAX_LAYER - 1)
			tmxinfo->num_layers += 1;
		else if (!strcasecmp(szName, "objectgroup") && tmxinfo->num_layers < TMX_MAX_LAYER - 1)
			tmxinfo->num_layers += 1;
		else if (!strcasecmp(szName, "object"))
			tmxinfo->layers[tmxinfo->num_layers].num_objects += 1;
		break;
	}
	return handler;
}

/* loads common info about a .tmx file */
bool TMXLoad(const char* filename, TMXInfo* info)
{
	SimpleXmlParser parser;
	ssize_t size;
	uint8_t *data;
	TLN_Tilemap tilemap = NULL;
	bool retval = false;

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
	tmxinfo = info;
	memset(info, 0, sizeof(TMXInfo));
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
			TLN_SetLastError(TLN_ERR_OK);
			retval = true;
		}
	}
	else
		TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);

	simpleXmlDestroyParser(parser);
	free(data);
	return retval;
}

/* loads suitable tileset acoording to gid range */
TMXTileset* TMXGetSuitableTileset(TMXInfo* info, int gid)
{
	int c;
	for (c = 0; c < info->num_tilesets - 1; c += 1)
	{
		if (gid >= info->tilesets[c].firstgid && gid < info->tilesets[c + 1].firstgid)
			return &info->tilesets[c];
	}
	return &info->tilesets[c];
}

/*returns first layer of requested type */
char* TMXGetFirstLayerName(TMXInfo* info, LayerType type)
{
	int c;
	for (c = 0; c < info->num_layers; c += 1)
	{
		if (info->layers[c].type == type)
			return info->layers[c].name;
	}
	return NULL;
}
