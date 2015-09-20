/*
*******************************************************************************
	
	Public Tilengine source code - Megamarc 20 sep 2015
	http://www.tilengine.org
	
	Tileset file loader (.tsx / .png) created with Tiled editor
	http://www.mapeditor.org

*******************************************************************************
*/

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Tilengine.h"
#include "simplexml.h"
#include "LoadFile.h"

/* load manager */
struct
{
	char source[64];
	int tilewidth;
	int tileheight;
	int spacing;
	int margin;
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
		if (!strcmp(szAttribute, "source"))
			strcpy (loader.source, szValue);
		else if (!strcmp(szAttribute, "tilewidth"))
			loader.tilewidth = atoi(szValue);
		else if (!strcmp(szAttribute, "tileheight"))
			loader.tileheight = atoi(szValue);
		else if (!strcmp(szAttribute, "margin"))
			loader.margin = atoi(szValue);
		else if (!strcmp(szAttribute, "spacing"))
			loader.spacing = atoi(szValue);
		break;

	case FINISH_ATTRIBUTES:
		break;

	case ADD_CONTENT:
		break;

	case FINISH_TAG:
		break;
	}
	return handler;
}

/*!
 * \brief
 * Loads a tileset from a Tiled .tsx file
 * 
 * \param filename
 * TSX file to load
 * 
 * \returns
 * Reference to the newly loaded tileset or NULL if error
 *
 * \remarks
 * An associated palette is also created, it can be obtained calling TLN_GetTilesetPalette()
 */
TLN_Tileset TLN_LoadTileset (char *filename)
{
	SimpleXmlParser parser;
	size_t size;
	BYTE *data;
	TLN_Tileset tileset;
	TLN_Bitmap bitmap;
	int htiles, vtiles;
	int x,y,dx,dy;
	int id;
	int pitch;
	
	/* load file */
	data = LoadFile (filename, &size);
	if (!data)
		return NULL;

	/* parse */
	memset (&loader, 0, sizeof(loader));
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

	/* check filename */
	if (!loader.source[0])
		return NULL;
	
	/* load picture */
	bitmap = TLN_LoadBitmap (loader.source);
	if (!bitmap)
		return NULL;

	/* create tileset */
	dx = loader.tilewidth + loader.spacing;
	dy = loader.tileheight + loader.spacing;
	htiles = (TLN_GetBitmapWidth(bitmap) - loader.margin*2 + loader.spacing) / dx;
	vtiles = (TLN_GetBitmapHeight(bitmap) - loader.margin*2 + loader.spacing) / dy;
	tileset = TLN_CreateTileset (htiles*vtiles, loader.tilewidth, loader.tileheight, TLN_ClonePalette(TLN_GetBitmapPalette(bitmap)));
	pitch = TLN_GetBitmapPitch (bitmap);

	/* load tiles */
	for (id=1, y=0; y<vtiles; y++)
	{
		for (x=0; x<htiles; x++, id++)
		{
			BYTE *srcptr = TLN_GetBitmapPtr (bitmap, loader.margin + x*dx, loader.margin + y*dy);
			TLN_SetTilesetPixels (tileset, id, srcptr, pitch);
		}
	}

	TLN_DeleteBitmap (bitmap);

	return tileset;
}
