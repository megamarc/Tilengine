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

/*!
 * \file
 * \brief Tilemap file loader (.tmx) created with Tiled editor
 * http://www.mapeditor.org
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tilengine.h"
#include "simplexml.h"
#include "zlib.h"
#include "LoadFile.h"

extern int base64decode (const char* in, int inLen, unsigned char *out, int *outLen);
static int csvdecode (const char* in, int numtiles, uint32_t* data);
static int decompress (unsigned char* in, int in_size, unsigned char* out, int out_size);
static uint32_t ParseHTMLColor (const char* string);

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
	bool load;					/* loading in progress */
	bool done;					/* loading complete */
	int cols, rows;				/* map size */
	encoding_t encoding;		/* encoding */
	compression_t compression;	/* compression */
	uint32_t bgcolor;			/* background color */
	TLN_Tilemap tilemap;		/* tilemap being built */
	TLN_Tileset tileset;		/* optional associated tileset */
}
static loader;

/* XML parser callback */
static void* handler (SimpleXmlParser parser, SimpleXmlEvent evt, 
	const char* szName, const char* szAttribute, const char* szValue)
{
	switch (evt)
	{
	case ADD_SUBTAG:
		break;

	case ADD_ATTRIBUTE:
		
		if (!strcasecmp(szName, "map"))
		{
			if (!strcasecmp(szAttribute, "backgroundcolor"))
				loader.bgcolor = ParseHTMLColor (szValue);
		}
		else if (!strcasecmp(szName, "tileset") && loader.tileset == NULL)
		{
			if (!strcasecmp(szAttribute, "source"))
				loader.tileset = TLN_LoadTileset (szValue);
		}
		else if (!strcasecmp(szName, "layer") && loader.done == false)
		{
			if (!strcasecmp(szAttribute, "name"))
			{
				if (loader.layer_name[0] != 0)
				{
					if (!strcasecmp(szValue, loader.layer_name))
						loader.load = true;
					else
						loader.load = false;
				}
				else
					loader.load = true;
			}
			else if (!strcasecmp(szAttribute, "width"))
				loader.cols = atoi(szValue);
			else if (!strcasecmp(szAttribute, "height"))
				loader.rows = atoi(szValue);
		}
		else if (!strcasecmp(szName, "data") && loader.load)
		{
			if (!strcasecmp(szAttribute, "encoding"))
			{
				if (!strcasecmp(szValue, "csv"))
					loader.encoding = ENCODING_CSV;
				else if (!strcasecmp(szValue, "base64"))
					loader.encoding = ENCODING_BASE64;
				else
					loader.load = false;
			}

			else if (!strcasecmp(szAttribute, "compression"))
			{
				if (!strcasecmp(szValue, "gzip"))
					/* loader.compression = COMPRESSION_GZIP; */
					loader.load = false;

				if (!strcasecmp(szValue, "zlib"))
					loader.compression = COMPRESSION_ZLIB;
			}
		}
		break;

	case FINISH_ATTRIBUTES:
		if (!strcasecmp(szName, "data") && loader.load)
		{
			loader.tilemap = TLN_CreateTilemap (loader.rows, loader.cols, NULL, loader.bgcolor, loader.tileset);
			loader.done = true;
		}
		break;

	case ADD_CONTENT:
		if (!strcasecmp(szName, "data") && loader.load)
		{
			int numtiles = loader.cols * loader.rows;
			int size = numtiles * sizeof(uint32_t);
			uint32_t* data = malloc (size);
			int c;
			
			memset (data, 0, size);
			if (loader.encoding == ENCODING_CSV)
				csvdecode (szValue, numtiles, data);

			else if (loader.encoding == ENCODING_BASE64)
			{
				if (loader.compression == COMPRESSION_NONE)
					base64decode (szValue, (int)strlen(szValue), (unsigned char*)data, &size);
				else
				{
					uint8_t* deflated = malloc (size);
					int in_size = size;
					base64decode (szValue, (int)strlen(szValue), (unsigned char*)deflated, &in_size);
					decompress (deflated, in_size, (uint8_t*)data, size);
					free (deflated);
				}
			}

			for (c=0; c<numtiles; c++)
			{
				uint32_t val = data[c];
				Tile tile;
				
				int col = c % loader.cols;
				int row = c / loader.cols;

				tile.flags = (uint16_t)((val & 0xFFFF0000) >> 16);
				tile.index = (uint16_t) (val & 0x000FFFFF);
				TLN_SetTilemapTile (loader.tilemap, row, col, &tile);
			}

			free (data);
			loader.load = false;
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
	size_t size;
	uint8_t *data;
	
	/* load file */
	data = LoadFile (filename, &size);
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
	loader.tilemap = NULL;
	if (layername)
		strncpy (loader.layer_name, layername, 64);
	parser = simpleXmlCreateParser (data, (long)size);
	if (parser != NULL)
	{
		if (simpleXmlParse(parser, handler) != 0)
		{
			printf("parse error on line %li:\n%s\n", 
				simpleXmlGetLineNumber(parser), simpleXmlGetErrorDescription(parser));
			simpleXmlDestroyParser(parser);
			free(data);
			TLN_SetLastError (TLN_ERR_WRONG_FORMAT);
			return NULL;
		}
		else
			TLN_SetLastError (TLN_ERR_OK);
	}
	else
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);

	simpleXmlDestroyParser(parser);
	free (data);
	return loader.tilemap;
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
		{
			sscanf (token, "%u", &data[c]);
			c++;
		}
		token = strtok (NULL, ",\n");
	}
	while (c < numtiles);

	return 1;
}

/* decompress a zipped string */
static int decompress (unsigned char* in, int in_size, unsigned char* out, int out_size)
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