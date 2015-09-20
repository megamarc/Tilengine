/*
*******************************************************************************
	
	Public Tilengine source code - Megamarc 20 sep 2015
	http://www.tilengine.org
	
	Tilemap file loader (.tmx) created with Tiled editor
	http://www.mapeditor.org

*******************************************************************************
*/

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tilengine.h"
#include "simplexml.h"
#include "zlib.h"
#include "LoadFile.h"

extern int base64decode (const char* in, size_t inLen, unsigned char *out, size_t *outLen);
static int csvdecode (const char* in, int numtiles, DWORD* data);
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
		if (!strcmp(szName, "layer"))
		{
			if (!strcmp(szAttribute, "name"))
			{
				if (!strcmp(szValue, loader.layer_name))
					loader.load = true;
				else
					loader.load = false;
			}
			else if (!strcmp(szAttribute, "width"))
				loader.cols = atoi(szValue);
			else if (!strcmp(szAttribute, "height"))
				loader.rows = atoi(szValue);
		}
		else if (!strcmp(szName, "data") && loader.load)
		{
			if (!strcmp(szAttribute, "encoding"))
			{
				if (!strcmp(szValue, "csv"))
					loader.encoding = ENCODING_CSV;
				else if (!strcmp(szValue, "base64"))
					loader.encoding = ENCODING_BASE64;
				else
					loader.load = false;
			}

			else if (!strcmp(szAttribute, "compression"))
			{
				if (!strcmp(szValue, "gzip"))
					/* loader.compression = COMPRESSION_GZIP; */
					loader.load = false;

				if (!strcmp(szValue, "zlib"))
					loader.compression = COMPRESSION_ZLIB;
			}
		}
		break;

	case FINISH_ATTRIBUTES:
		if (!strcmp(szName, "data") && loader.load)
			loader.tilemap = TLN_CreateTilemap (loader.rows, loader.cols, NULL);
		break;

	case ADD_CONTENT:
		if (!strcmp(szName, "data") && loader.load)
		{
			int numtiles = loader.cols * loader.rows;
			int size = numtiles * sizeof(DWORD);
			DWORD* data = malloc (size);
			int c;
			
			memset (data, 0, size);
			if (loader.encoding == ENCODING_CSV)
				csvdecode (szValue, numtiles, data);

			else if (loader.encoding == ENCODING_BASE64)
			{
				if (loader.compression == COMPRESSION_NONE)
					base64decode (szValue, strlen(szValue), (unsigned char*)data, &size);
				else
				{
					BYTE* deflated = malloc (size);
					int in_size = size;
					base64decode (szValue, strlen(szValue), (unsigned char*)deflated, &in_size);
					decompress (deflated, in_size, (BYTE*)data, size);
					free (deflated);
				}
			}

			for (c=0; c<numtiles; c++)
			{
				DWORD val = data[c];
				Tile tile;
				
				int col = c % loader.cols;
				int row = c / loader.cols;

				tile.flags = (WORD)((val & 0xFFFF0000) >> 16);
				tile.index = (WORD) (val & 0x000FFFFF);
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
 * \param name
 * name of the layer inside the tmx file to load
 * 
 * \returns
 * Reference to the newly loaded tilemap or NULL if error
 *
 * \remarks
 * A tmx map file from Tiled can contain one or more layers, each with its own name. TLN_LoadTilemap()
 * doesn't load a full tmx file, only the specified layer
 */
TLN_Tilemap TLN_LoadTilemap (char *filename, char *name)
{
	SimpleXmlParser parser;
	size_t size;
	BYTE *data;
	
	/* load file */
	data = LoadFile (filename, &size);
	if (!data)
		return NULL;

	/* parse */
	loader.tilemap = NULL;
	strcpy (loader.layer_name, name);
	parser = simpleXmlCreateParser (data, (long)size);
	if (parser != NULL)
	{
		if (simpleXmlParse(parser, handler) != 0)
		{
			printf("parse error on line %li:\n%s\n", 
				simpleXmlGetLineNumber(parser), simpleXmlGetErrorDescription(parser));
			free (data);
			return NULL;
		}
	}

	free (data);
	return loader.tilemap;
}

/* read CSV string */
static int csvdecode (const char* in, int numtiles, DWORD *data)
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
