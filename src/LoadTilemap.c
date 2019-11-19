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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tilengine.h"
#include "simplexml.h"
#include "zlib.h"
#include "LoadFile.h"
#include "Base64.h"

#define MAX_TILESETS	8

static int csvdecode (const char* in, int numtiles, uint32_t* data);
static int decompress (uint8_t* in, int in_size, uint8_t* out, int out_size);
static uint32_t ParseHTMLColor (const char* string);

typedef enum
{
	LS_IDLE,	/* waiting for start */
	LS_ACTIVE,	/* loading underway */
	LS_DONE,	/* loading done, ignore everything */
}
LoadState;

/* encoding */
typedef enum
{
	ENCODING_XML,
	ENCODING_BASE64,
	ENCODING_CSV,
}
encoding_t;

/* compression */
typedef enum
{
	COMPRESSION_NONE,
	COMPRESSION_ZLIB,
	COMPRESSION_GZIP,
}
compression_t;

/* load manager */
struct
{
	char layer_name[64];		/* name of layer to load */
	LoadState state;
	int cols, rows;				/* map size */
	encoding_t encoding;		/* encoding */
	compression_t compression;	/* compression */
	uint32_t bgcolor;			/* background color */
	uint32_t* data;				/* map data (rows*cols) */
	struct
	{
		int firstgid;
		TLN_Tileset tileset;
	}
	tilesets[MAX_TILESETS];
	int num_tileset;
}
static loader;

/* XML parser callback */
static void* handler (SimpleXmlParser parser, SimpleXmlEvent evt, 
	const char* szName, const char* szAttribute, const char* szValue)
{
	int intvalue;
	switch (evt)
	{
	case ADD_SUBTAG:
		if (!strcasecmp(szName, "map"))
			return handler;

		else if (!strcasecmp(szName, "tileset") && loader.num_tileset < MAX_TILESETS - 1)
			return handler;

		else if (!strcasecmp(szName, "layer") && loader.state == LS_IDLE)
			return handler;

		else if (!strcasecmp(szName, "data") && loader.state == LS_ACTIVE)
			return handler;

		break;

	case ADD_ATTRIBUTE:

		intvalue = atoi(szValue);
		if (!strcasecmp(szName, "map"))
		{
			if (!strcasecmp(szAttribute, "backgroundcolor"))
				loader.bgcolor = ParseHTMLColor (szValue);
		}

		else if (!strcasecmp(szName, "tileset"))
		{
			if (!strcasecmp(szAttribute, "firstgid"))
				loader.tilesets[loader.num_tileset].firstgid = intvalue;
			else if (!strcasecmp(szAttribute, "source"))
				loader.tilesets[loader.num_tileset].tileset = TLN_LoadTileset(szValue);
		}

		else if (!strcasecmp(szName, "layer") && loader.state != LS_DONE)
		{
			if (!strcasecmp(szAttribute, "name"))
			{
				if (loader.layer_name[0] == 0 || !strcasecmp(szValue, loader.layer_name))
					loader.state = LS_ACTIVE;
			}
			if (loader.state != LS_ACTIVE)
				break;
			else if (!strcasecmp(szAttribute, "width"))
				loader.cols = intvalue;
			else if (!strcasecmp(szAttribute, "height"))
				loader.rows = intvalue;
		}

		else if (!strcasecmp(szName, "data"))
		{
			if (!strcasecmp(szAttribute, "encoding"))
			{
				if (!strcasecmp(szValue, "csv"))
					loader.encoding = ENCODING_CSV;
				else if (!strcasecmp(szValue, "base64"))
					loader.encoding = ENCODING_BASE64;
				else
					loader.state = LS_IDLE;
			}

			else if (!strcasecmp(szAttribute, "compression"))
			{
				if (!strcasecmp(szValue, "gzip"))
					/* loader.compression = COMPRESSION_GZIP; */
					loader.state = LS_IDLE;
				else if (!strcasecmp(szValue, "zlib"))
					loader.compression = COMPRESSION_ZLIB;
			}
		}
		break;

	case FINISH_ATTRIBUTES:
		if (!strcasecmp(szName, "tileset"))
			loader.num_tileset += 1;
		break;

	case ADD_CONTENT:
		if (!strcasecmp(szName, "data") && loader.state == LS_ACTIVE)
		{
			int numtiles = loader.cols * loader.rows;
			int size = numtiles * sizeof(uint32_t);
			uint32_t* data = (uint32_t*)malloc (size);
			
			memset (data, 0, size);
			if (loader.encoding == ENCODING_CSV)
				csvdecode (szValue, numtiles, data);

			else if (loader.encoding == ENCODING_BASE64)
			{
				if (loader.compression == COMPRESSION_NONE)
					base64decode ((uint8_t*)szValue, (int)strlen(szValue), (uint8_t*)data, &size);
				else
				{
					uint8_t* deflated = (uint8_t*)malloc (size);
					int in_size = size;
					base64decode ((uint8_t*)szValue, (int)strlen(szValue), (uint8_t*)deflated, &in_size);
					decompress (deflated, in_size, (uint8_t*)data, size);
					free (deflated);
				}
			}
			loader.data = data;
			loader.state = LS_DONE;
		}
		break;

	case FINISH_TAG:
		break;
	}
	return handler;
}

/*!
 * \brief
 * Loads a tilemap layer from a Tiled .tmx file
 * 
 * \param filename
 * TMX file with the tilemap
 *
 * \param layername
 * Optional name of the layer inside the tmx file to load. NULL to load the first layer
 * 
 * \returns
 * Reference to the newly loaded tilemap or NULL if error
 *
 * \remarks
 * A tmx map file from Tiled can contain one or more layers, each with its own name. TLN_LoadTilemap()
 * doesn't load a full tmx file, only the specified layer. The associated *external* tileset (TSX file) is
 * also loaded and associated to the tilemap
 */
TLN_Tilemap TLN_LoadTilemap (const char *filename, const char *layername)
{
	SimpleXmlParser parser;
	ssize_t size;
	uint8_t *data;
	TLN_Tilemap tilemap = NULL;
	
	/* load file */
	data = (uint8_t*)LoadFile (filename, &size);
	if (!data)
	{
		if (size == 0)
			TLN_SetLastError (TLN_ERR_FILE_NOT_FOUND);
		else if (size == -1)
			TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}

	/* parse */
	memset (&loader, 0, sizeof(loader));
	if (layername)
	{
		strncpy (loader.layer_name, layername, sizeof(loader.layer_name));
		loader.layer_name[sizeof(loader.layer_name) - 1] = '\0';
	}
	parser = simpleXmlCreateParser ((char*)data, (long)size);
	if (parser != NULL)
	{
		if (simpleXmlParse(parser, handler) != 0)
		{
			printf("parse error on line %li:\n%s\n", 
				simpleXmlGetLineNumber(parser), simpleXmlGetErrorDescription(parser));
		}
		else
			TLN_SetLastError (TLN_ERR_OK);
	}
	else
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);

	simpleXmlDestroyParser(parser);
	free (data);

	if (loader.state == LS_DONE)
	{
		TLN_Tileset tileset = NULL;
		int numtiles = loader.cols*loader.rows;
		int c;
		uint32_t* data = loader.data;
		uint32_t firstgid = 9999999;
		uint32_t lastgid = 0;

		/* find range of used tiles */
		for (c = 0; c < numtiles; c++)
		{
			Tile* tile = (Tile*)&data[c];
			if (tile->index > 0)
			{
				if (firstgid > tile->index)
					firstgid = tile->index;
				if (lastgid < tile->index)
					lastgid = tile->index;
			}
		}

		/* find suitable tileset */
		for (c = 0; c < loader.num_tileset && tileset == NULL; c++)
		{
			uint32_t tilefirst = loader.tilesets[c].firstgid;
			uint32_t tilelast = tilefirst + TLN_GetTilesetNumTiles(loader.tilesets[c].tileset);
			if (firstgid >= tilefirst && lastgid <= tilelast)
			{
				tileset = loader.tilesets[c].tileset;
				firstgid = loader.tilesets[c].firstgid;
			}
		}

		/* correct with firstgid */
		for (c = 0; c < numtiles; c++)
		{
			Tile* tile = (Tile*)&data[c];
			if (tile->index > 0)
				tile->index = tile->index - firstgid + 1;
		}

		/* create */
		tilemap = TLN_CreateTilemap(loader.rows, loader.cols, (TLN_Tile)data, loader.bgcolor, tileset);

		/* delete loaded but unused tilesets */
		for (c = 0; c < loader.num_tileset; c++)
		{
			if (loader.tilesets[c].tileset != tileset)
				TLN_DeleteTileset(loader.tilesets[c].tileset);
		}
	}
	return tilemap;
}

/* read CSV string */
static int csvdecode (const char* in, int numtiles, uint32_t *data)
{
	int c;
	char *token = strtok ((char*)in, ",\n");

	c = 0;
	do
	{
		if (token[0] != 0x0D)
			sscanf (token, "%u", &data[c++]);
		token = strtok (NULL, ",\n");
	}
	while (c < numtiles);

	return 1;
}

/* decompress a zipped string */
static int decompress (uint8_t* in, int in_size, uint8_t* out, int out_size)
{
	int ret;
	z_stream strm;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	/* decompress until deflate stream ends or end of file */
	do
	{
		strm.avail_in = in_size;
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do
		{
			strm.avail_out = out_size;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
			}
		}
		while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	}
	while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

static uint8_t ParseHexChar (char data)
{
	if (data >= '0' && data <= '9')
		return data - '0';
	else if (data >= 'A' && data <= 'F')
		return data - 'A' + 10;
	else if (data >= 'a' && data <= 'f')
		return data - 'a' + 10;
	else
		return 0;
}

static uint8_t ParseHexByte (const char* string)
{
	return (ParseHexChar(string[0]) << 4) + ParseHexChar(string[1]);
}

static uint32_t ParseHTMLColor (const char* string)
{
	int r,g,b;

	if (string[0] != '#')
		return 0;

	r = ParseHexByte (&string[1]);
	g = ParseHexByte (&string[3]);
	b = ParseHexByte (&string[5]);

	return (uint32_t)(0xFF000000 | (r << 16) | (g << 8) | b);
}
