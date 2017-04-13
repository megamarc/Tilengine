/*
Tilengine - 2D Graphics library with raster effects
Copyright (c) 2015-2017 Marc Palacios Domenech (megamarc@hotmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*!
 ******************************************************************************
 *
 * \file
 * \brief Tilemap file loader (.tmx) created with Tiled editor
 * http://www.mapeditor.org
 *
 * \author Megamarc
 * \date 20 sep 2015
 *
 * Public Tilengine source code
 * http://www.tilengine.org
 *
 ******************************************************************************
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
	TLN_Tilemap tilemap;		/* tilemap being built */
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
		if (!strcasecmp(szName, "layer") && loader.done == false)
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
			loader.tilemap = TLN_CreateTilemap (loader.rows, loader.cols, NULL);
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
 * \param layername
 * name of the layer inside the tmx file to load. NULL to load the first layer
 * 
 * \returns
 * Reference to the newly loaded tilemap or NULL if error
 *
 * \remarks
 * A tmx map file from Tiled can contain one or more layers, each with its own name. TLN_LoadTilemap()
 * doesn't load a full tmx file, only the specified layer
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
		strcpy (loader.layer_name, layername);
	parser = simpleXmlCreateParser (data, (long)size);
	if (parser != NULL)
	{
		if (simpleXmlParse(parser, handler) != 0)
		{
			printf("parse error on line %li:\n%s\n", 
				simpleXmlGetLineNumber(parser), simpleXmlGetErrorDescription(parser));
			free (data);
			TLN_SetLastError (TLN_ERR_WRONG_FORMAT);
			return NULL;
		}
		else
			TLN_SetLastError (TLN_ERR_OK);
	}
	else
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);

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
