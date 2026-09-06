/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include <string.h>
#include <stdio.h>
#include "Tilengine.h"
#include "Tilemap.h"

typedef struct
{
	int x,y,w,h;
}
Rect;

TLN_Tilemap TLN_CreateTilemap (int rows, int cols, TLN_Tile tiles, uint32_t bgcolor, TLN_Tileset tileset)
{
	TLN_Tilemap tilemap = NULL;
	int size = sizeof(struct Tilemap) + (rows * cols * sizeof(Tile));

	tilemap = (TLN_Tilemap)CreateBaseObject (OT_TILEMAP, size);
	if (!tilemap)
		return NULL;

	tilemap->rows = rows;
	tilemap->cols = cols;
	tilemap->bgcolor = bgcolor;
	tilemap->tilesets[0] = tileset;
	tilemap->visible = true;

	if (tiles)
		memcpy (tilemap->tiles, tiles, tilemap->size - sizeof(struct Tilemap));

	TLN_SetLastError (TLN_ERR_OK);
	return tilemap;
}

TLN_Tilemap TLN_CloneTilemap (TLN_Tilemap src)
{
	TLN_Tilemap tilemap;

	if (!CheckBaseObject (src, OT_TILEMAP))
		return NULL;

	tilemap = (TLN_Tilemap)CloneBaseObject (src);
	if (tilemap)
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tilemap;
	}
	else
		return NULL;
}

int TLN_GetTilemapRows (TLN_Tilemap tilemap)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tilemap->rows;
	}
	else
		return 0;
}

int TLN_GetTilemapCols (TLN_Tilemap tilemap)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tilemap->cols;
	}
	else
		return 0;
}

TLN_Tileset TLN_GetTilemapTileset (TLN_Tilemap tilemap)
{
	return TLN_GetTilemapTileset2(tilemap, 0);
}

TLN_Tileset TLN_GetTilemapTileset2(TLN_Tilemap tilemap, int index)
{
	if (CheckBaseObject(tilemap, OT_TILEMAP))
	{
		TLN_SetLastError(TLN_ERR_OK);
		return tilemap->tilesets[index];
	}
	else
		return NULL;
}

bool TLN_SetTilemapTileset(TLN_Tilemap tilemap, TLN_Tileset tileset)
{
	return TLN_SetTilemapTileset2(tilemap, tileset, 0);
}

bool TLN_SetTilemapTileset2(TLN_Tilemap tilemap, TLN_Tileset tileset, int index)
{
	if (!CheckBaseObject(tilemap, OT_TILEMAP))
	{
		TLN_SetLastError(TLN_ERR_REF_TILEMAP);
		return false;
	}
	if (!CheckBaseObject(tileset, OT_TILESET))
	{
		TLN_SetLastError(TLN_ERR_REF_TILESET);
		return false;
	}

	tilemap->tilesets[index] = tileset;
	TLN_SetLastError(TLN_ERR_OK);
	return true;
}

static TLN_Tile GetTilemapPtr (TLN_Tilemap tilemap, int row, int col)
{
	if (row<tilemap->rows && col<tilemap->cols)
		return &tilemap->tiles[row*tilemap->cols + col];
	else
		return NULL;
}

bool TLN_GetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP) && tile)
	{
		TLN_Tile srctile = GetTilemapPtr (tilemap, row, col);
		if (srctile)
		{
			tile->flags = srctile->flags;
			tile->index = srctile->index;
			TLN_SetLastError (TLN_ERR_OK);
			return true;
		}
		else
		{
			TLN_SetLastError (TLN_ERR_WRONG_SIZE);
			return false;
		}
	}
	else
		return false;
}

bool TLN_SetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP) && tile)
	{
		TLN_Tile dsttile = GetTilemapPtr (tilemap, row, col);
		if (dsttile != NULL)
		{
			dsttile->value = tile != NULL ? tile->value : 0;
			TLN_SetLastError (TLN_ERR_OK);
			return true;
		}
		else
		{
			TLN_SetLastError (TLN_ERR_WRONG_SIZE);
			return false;
		}
	}
	else
		return false;
}

TLN_Tile TLN_GetTilemapTiles(TLN_Tilemap tilemap, int row, int col)
{
	if (!CheckBaseObject(tilemap, OT_TILEMAP))
		return NULL;

	return GetTilemapPtr(tilemap, row, col);
}

bool TLN_DeleteTilemap (TLN_Tilemap tilemap)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP))
	{
		if (ObjectOwner (tilemap))
			TLN_DeleteTileset (tilemap->tilesets[0]);
		DeleteBaseObject (tilemap);
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return false;
}

static void ClipRect (Rect* src, Rect* dst)
{
	if (src->x + src->w >= dst->w)
		src->w = dst->w - src->x;
	if (src->y + src->h >= dst->h)
		src->h = dst->h - src->y;
}

bool TLN_CopyTiles (TLN_Tilemap src, int srcrow, int srccol, int rows, int cols, TLN_Tilemap dst, int dstrow, int dstcol)
{
	int y, size;

	if (!CheckBaseObject (src, OT_TILEMAP) || !CheckBaseObject (dst, OT_TILEMAP))
		return false;

	/* setup rects */
	{
		Rect tgtrect = {srccol,srcrow, cols,rows};	/* area a copiar */
		Rect srcrect = {0,0, src->rows,src->cols};	/* tilemap de origen */
		Rect dstrect = {0,0, dst->rows,dst->cols};	/* tilemap de destino */

		/* clipping */
		ClipRect (&tgtrect, &srcrect);
		ClipRect (&tgtrect, &dstrect);

		size = tgtrect.w * sizeof(Tile);
		for (y=0; y<tgtrect.h; y++)
		{
			Tile* srctile = GetTilemapPtr (src, y + srcrow, srccol);
			Tile* dsttile = GetTilemapPtr (dst, y + dstrow, dstcol);
			if (srctile && dsttile)
				memcpy (dsttile, srctile, size);
			else
			{
				TLN_SetLastError (TLN_ERR_WRONG_SIZE);
				return false;
			}
		}
	}

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}
