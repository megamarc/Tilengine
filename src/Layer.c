/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <math.h>
#include <string.h>
#include "Engine.h"
#include "Draw.h"
#include "Layer.h"
#include "Tileset.h"
#include "Tilemap.h"
#include "Tables.h"
#include "ObjectList.h"
#include "Bitmap.h"

static void SetBlitter (Layer* layer);

bool TLN_SetLayer(int nlayer, TLN_Tileset tileset, TLN_Tilemap tilemap)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	layer->ok = false;
	if (!CheckBaseObject(tilemap, OT_TILEMAP))
		return false;

	/* select tilemsp's own tileset */
	if (tileset == NULL)
		tileset = tilemap->tilesets[0];

	if (!CheckBaseObject(tileset, OT_TILESET))
		return false;

	layer->tilemap = tilemap;
	layer->width = tilemap->cols*tileset->width;
	layer->height = tilemap->rows*tileset->height;
	layer->bitmap = NULL;
	layer->objects = NULL;
	layer->type = LAYER_TILE;

	/* common operations per tileset */
	int ts;
	for (ts = 0; ts < MAX_TILESETS; ts += 1)
	{
		tileset = tilemap->tilesets[ts];
		if (tileset == NULL)
			break;

		/* apply priority attribute */
		if (tileset->attributes != NULL)
		{
			const int num_tiles = tilemap->rows * tilemap->cols;
			int c;
			Tile* tile = tilemap->tiles;
			for (c = 0; c < num_tiles; c++, tile++)
			{
				if (tile->index != 0 && tile->index < tileset->numtiles)
				{
					if (tileset->attributes[tile->index - 1].priority == true)
						tile->flags |= FLAG_PRIORITY;
					else
						tile->flags &= ~FLAG_PRIORITY;
				}
			}
		}

		/* start animations */
		if (tileset->sp != NULL)
		{
			int c;
			TLN_Sequence sequence;

			c = 0;
			sequence = tileset->sp->sequences;
			while (sequence != NULL)
			{
				SetTilesetAnimation(tileset, c, sequence);
				sequence = sequence->next;
				c += 1;
			}
		}
	}

	if (tilemap->visible)
	{
		layer->ok = true;
		layer->draw = GetLayerDraw(layer);
		SetBlitter(layer);
	}

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_SetLayerTilemap(int nlayer, TLN_Tilemap tilemap)
{
	return TLN_SetLayer(nlayer, NULL, tilemap);
}

bool TLN_SetLayerBitmap(int nlayer, TLN_Bitmap bitmap)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	layer->ok = false;
	if (!CheckBaseObject(bitmap, OT_BITMAP))
		return false;

	layer->tilemap = NULL;
	layer->bitmap = bitmap;
	layer->objects = NULL;
	layer->width = bitmap->width;
	layer->height = bitmap->height;

	/* require palette */
	if (bitmap->palette != NULL)
	{
		layer->type = LAYER_BITMAP;
		layer->ok = true;
		layer->draw = GetLayerDraw(layer);
		SetBlitter(layer);
		TLN_SetLastError(TLN_ERR_OK);
		return true;
	}
	else
	{
		layer->ok = false;
		TLN_SetLastError(TLN_ERR_REF_PALETTE);
		return false;
	}
}

bool TLN_SetLayerObjects(int nlayer, TLN_ObjectList objects, TLN_Tileset tileset)
{
	Layer *layer = NULL;
	TLN_Object* item = NULL;

	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}
	layer = &engine->layers[nlayer];
	layer->ok = false;

	if (!CheckBaseObject(objects, OT_OBJECTLIST))
	{
		TLN_SetLastError(TLN_ERR_REF_LIST);
		return false;
	}

	if (tileset == NULL)
		tileset = objects->tileset;
	if (!CheckBaseObject(tileset, OT_TILESET) || tileset->tstype != TILESET_IMAGES)
	{
		TLN_SetLastError(TLN_ERR_REF_TILESET);
		return false;
	}

	layer->tilemap = NULL;
	layer->bitmap = NULL;
	layer->objects = objects;
	layer->width = objects->width;
	layer->height = objects->height;
	layer->type = LAYER_OBJECT;
	
	/* link objects to actual bitmaps */
	item = objects->list;
	while (item)
	{
		if (item->visible && item->has_gid)
		{
			item->bitmap = GetTilesetBitmap(tileset, item->gid);
			if (item->bitmap)
			{
				item->width = item->bitmap->width;
				item->height = item->bitmap->height;
			}
		}
		item = item->next;
	}

	if (objects->visible)
	{
		layer->ok = true;
		layer->draw = GetLayerDraw(layer);
		SetBlitter(layer);
	}
	TLN_SetLastError(TLN_ERR_OK);
	return true;
}

bool TLN_SetLayerPriority(int nlayer, bool enable)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	layer->priority = enable;
	return true;
}

bool TLN_SetLayerParent(int nlayer, int parent)
{
	return true;
}

bool TLN_DisableLayerParent(int nlayer)
{
	return true;
}

int TLN_GetLayerWidth (int nlayer)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	TLN_SetLastError (TLN_ERR_OK);
	return engine->layers[nlayer].width;
}

int TLN_GetLayerHeight (int nlayer)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	TLN_SetLastError (TLN_ERR_OK);
	return engine->layers[nlayer].height;
}

bool TLN_SetLayerBlendMode (int nlayer, TLN_Blend mode, uint8_t factor)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	layer->blend = SelectBlendTable (mode);
	SetBlitter (layer);
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_SetLayerPalette (int nlayer, TLN_Palette palette)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	if (!CheckBaseObject (palette, OT_PALETTE))
	{
		layer->ok = false;
		return false;
	}

	layer->palette = palette;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

TLN_Palette TLN_GetLayerPalette (int nlayer)
{
	if (nlayer < engine->numlayers)
	{
		Layer* layer = &engine->layers[nlayer];
		TLN_SetLastError(TLN_ERR_OK);

		if (layer->palette != NULL)
			return layer->palette;
		else if (layer->bitmap != NULL && layer->bitmap->palette != NULL)
			return layer->bitmap->palette;
		else if (layer->tilemap != NULL && layer->tilemap->tilesets[0] != NULL && layer->tilemap->tilesets[0]->palette != NULL)
			return layer->tilemap->tilesets[0]->palette;

		TLN_SetLastError(TLN_ERR_REF_PALETTE);
		return NULL;
	}

	TLN_SetLastError(TLN_ERR_IDX_LAYER);
	return NULL;
}

TLN_LayerType TLN_GetLayerType(int nlayer)
{
	if (nlayer < engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_OK);
		return engine->layers[nlayer].type;
	}

	TLN_SetLastError(TLN_ERR_IDX_LAYER);
	return LAYER_NONE;
}

TLN_Tileset TLN_GetLayerTileset(int nlayer)
{
	if (nlayer < engine->numlayers && engine->layers[nlayer].tilemap != NULL)
	{
		TLN_SetLastError(TLN_ERR_OK);
		return engine->layers[nlayer].tilemap->tilesets[0];
	}

	TLN_SetLastError(TLN_ERR_IDX_LAYER);
	return NULL;
}

TLN_Tilemap TLN_GetLayerTilemap(int nlayer)
{
	if (nlayer < engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_OK);
		return engine->layers[nlayer].tilemap;
	}

	TLN_SetLastError(TLN_ERR_IDX_LAYER);
	return NULL;
}

TLN_Bitmap TLN_GetLayerBitmap(int nlayer)
{
	if (nlayer < engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_OK);
		return engine->layers[nlayer].bitmap;
	}

	TLN_SetLastError(TLN_ERR_IDX_LAYER);
	return NULL;
}

TLN_ObjectList TLN_GetLayerObjects(int nlayer)
{
	if (nlayer < engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_OK);
		return engine->layers[nlayer].objects;
	}

	TLN_SetLastError(TLN_ERR_IDX_LAYER);
	return NULL;
}

bool TLN_SetLayerPosition (int nlayer, int hstart, int vstart)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	if (layer->width == 0 || layer->height == 0)
	{
		TLN_SetLastError(TLN_ERR_REF_TILEMAP);
		return false;
	}

	/* wrapping */
	layer->hstart = hstart % layer->width;
	layer->vstart = vstart % layer->height;
	if (layer->hstart < 0)
		layer->hstart += layer->width;
	if (layer->vstart < 0)
		layer->vstart += layer->height;

	TLN_SetLastError (TLN_ERR_OK);
	if ((layer->tilemap && layer->tilemap->visible) || (layer->objects && layer->objects->visible))
		layer->ok = true;
	return true;
}

int TLN_GetLayerX(int nlayer)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return 0;
	}

	TLN_SetLastError(TLN_ERR_OK);
	return engine->layers[nlayer].hstart;
}

int TLN_GetLayerY(int nlayer)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return 0;
	}

	TLN_SetLastError(TLN_ERR_OK);
	return engine->layers[nlayer].vstart;
}

bool TLN_GetLayerTile (int nlayer, int x, int y, TLN_TileInfo* info)
{
	Layer *layer;
	TLN_Tileset tileset;
	TLN_Tilemap tilemap;
	TLN_Tile tile;
	int xpos, ypos;
	int xtile, ytile;
	int srcx, srcy;
	int column = 0;
	int column_offset = 0;

	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}
	if (!info)
	{
		TLN_SetLastError (TLN_ERR_NULL_POINTER);
		return false;
	}

	layer = &engine->layers[nlayer];
	if (!CheckBaseObject(layer->tilemap, OT_TILEMAP) || !CheckBaseObject (layer->tilemap->tilesets[0], OT_TILESET))
		return false;

	tilemap = layer->tilemap;
	tileset = tilemap->tilesets[0];

	xpos = x % layer->width;
	if (xpos < 0)
		xpos += layer->width;
	xtile = xpos >> tileset->hshift;
	srcx  = xpos & tileset->hmask;
	
	if (layer->column)
	{
		column = x / tileset->width;
		if (xpos!=0 && x>xpos)
			column++;
		column_offset = layer->column[column];
	}

	ypos  = (y + column_offset) % layer->height;
	if (ypos < 0)
		ypos += layer->height;
	srcy  = ypos & tileset->vmask;

	ytile = ypos >> tileset->vshift;
	tile = &tilemap->tiles[ytile*tilemap->cols + xtile];

	memset (info, 0, sizeof(TLN_TileInfo));
	info->col = xtile;
	info->row = ytile;
	info->xoffset = srcx;
	info->yoffset = srcy;
	if (tile->index != 0)
	{
		tileset = tilemap->tilesets[tile->tileset];
		info->index = tile->index - 1;
		info->flags = tile->flags;
		info->color = GetTilesetPixel (tileset, tile->index, srcx, srcy);
		info->type = tileset->attributes[info->index].type;
	}
	else
		info->empty = true;
	
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_SetLayerColumnOffset (int nlayer, int* offset)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	engine->layers[nlayer].column = offset;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_EnableLayer(int nlayer)
{
	Layer* layer = NULL;

	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];

	/* check proper config */
	if (layer->type == LAYER_TILE && layer->tilemap != NULL || layer->type == LAYER_BITMAP && layer->bitmap != NULL || layer->type == LAYER_OBJECT && layer->objects != NULL)
	{
		layer->ok = true;
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return true;
	}

	TLN_SetLastError(TLN_ERR_NULL_POINTER);
	return false;
}

/*!
 * \brief
 * Disables the specified layer so it is not drawn
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 *
 * \remarks
 * A layer configured with an invalid tileset, tilemap or palette is
 * automatically disabled
 * 
 * \see
 * TLN_SetLayer()
 */
bool TLN_DisableLayer (int nlayer)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	engine->layers[nlayer].ok = false;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_SetLayerAffineTransform (int nlayer, TLN_Affine *affine)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}
	
	layer = &engine->layers[nlayer];
	if (affine)
	{
		Matrix3 transform;
		math2d_t dx = layer->hstart + /*(engine->framebuffer.width>>1)  +*/ affine->dx;
		math2d_t dy = layer->vstart + /*(engine->framebuffer.height>>1) +*/ affine->dy;

		Matrix3SetIdentity (&layer->transform);
		Matrix3SetTranslation (&transform, -dx, -dy);
		Matrix3Multiply (&layer->transform, &transform);
		Matrix3SetRotation (&transform, (math2d_t)fmod(-affine->angle,360.0f));
		Matrix3Multiply (&layer->transform, &transform);
		Matrix3SetScale (&transform, 1/affine->sx, 1/affine->sy);
		Matrix3Multiply (&layer->transform, &transform);
		Matrix3SetTranslation (&transform, dx, dy);
		Matrix3Multiply (&layer->transform, &transform);

		layer->mode = MODE_TRANSFORM;
		layer->draw = GetLayerDraw (layer);
		SetBlitter (layer);

		/*printf ("TLN_SetLayerAffineTransform (ptr=%08Xh, a=%.02f, d=%.02f,%.02f, s=%.02f,%.02f)\n",
			affine, affine->angle, affine->dx, affine->dy, affine->sx, affine->sy);*/

		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return TLN_ResetLayerMode (nlayer);
}

bool TLN_SetLayerTransform (int layer, float angle, float dx, float dy, float sx, float sy)
{
	TLN_Affine affine;

	affine.angle = angle;
	affine.dx = dx;
	affine.dy = dy;
	affine.sx = sx;
	affine.sy = sy;

	return TLN_SetLayerAffineTransform (layer, &affine);
}

bool TLN_SetLayerScaling (int nlayer, float xfactor, float yfactor)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}
	
	layer = &engine->layers[nlayer];
	layer->xfactor = float2fix(xfactor);
	layer->dx = float2fix((1.0f / xfactor));
	layer->dy = float2fix((1.0f / yfactor));
	layer->mode = MODE_SCALING;
	layer->draw = GetLayerDraw (layer);
	SetBlitter (layer);
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_SetLayerPixelMapping (int nlayer, TLN_PixelMap* table)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	layer->pixel_map = table;
	if (table != NULL)
		layer->mode = MODE_PIXEL_MAP;
	else
		layer->mode = MODE_NORMAL;
	layer->draw = GetLayerDraw (layer);
	return true;
}

bool TLN_ResetLayerMode (int nlayer)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}
	
	layer = &engine->layers[nlayer];
	layer->mode = MODE_NORMAL;
	layer->draw = GetLayerDraw (layer);
	SetBlitter (layer);
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_SetLayerClip (int nlayer, int x1, int y1, int x2, int y2)
{
	return TLN_SetLayerWindow(nlayer, x1, y1, x2, y2, false);
}

bool TLN_DisableLayerClip (int nlayer)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}
	
	LayerWindow* window = &engine->layers[nlayer].window;
	window->x1 = 0;
	window->x2 = engine->framebuffer.width;
	window->y1 = 0;
	window->y2 = engine->framebuffer.height;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_SetLayerWindow(int nlayer, int x1, int y1, int x2, int y2, bool invert)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}

	LayerWindow* window = &engine->layers[nlayer].window;
	window->x1 = x1 >= 0 && x1 <= engine->framebuffer.width ? x1 : 0;
	window->x2 = x2 >= 0 && x2 <= engine->framebuffer.width ? x2 : engine->framebuffer.width;
	window->y1 = y1 >= 0 && y1 <= engine->framebuffer.height ? y1 : 0;
	window->y2 = y2 >= 0 && y2 <= engine->framebuffer.height ? y2 : engine->framebuffer.height;
	window->invert = invert;
	TLN_SetLastError(TLN_ERR_OK);
	return true;
}

bool TLN_SetLayerWindowColor(int nlayer, uint8_t r, uint8_t g, uint8_t b, TLN_Blend blend)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}

	LayerWindow* window = &engine->layers[nlayer].window;
	window->color = PackRGB32(r, g, b);
	window->blend = SelectBlendTable(blend);
	TLN_SetLastError(TLN_ERR_OK);
	return true;
}

bool TLN_DisableLayerWindow(int nlayer)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}

	LayerWindow* window = &engine->layers[nlayer].window;
	window->x1 = 0;
	window->x2 = engine->framebuffer.width;
	window->y1 = 0;
	window->y2 = engine->framebuffer.height;
	window->invert = false;
	TLN_SetLastError(TLN_ERR_OK);
	return true;
}

bool TLN_DisableLayerWindowColor(int nlayer)
{
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}

	LayerWindow* window = &engine->layers[nlayer].window;
	window->color = 0;
	window->blend = NULL;
	return true;
}

bool TLN_SetLayerMosaic (int nlayer, int width, int height)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	layer->mosaic.w = width;
	layer->mosaic.h = height;
	SetBlitter (layer);
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_DisableLayerMosaic (int nlayer)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	layer->mosaic.h = 0;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

Layer* GetLayer(int index)
{
	return &engine->layers[index];
}

static void SetBlitter (Layer* layer)
{
	bool scaling = layer->mode == MODE_SCALING;
	bool blend = layer->blend != NULL && layer->mosaic.h == 0;

	layer->blitters[0] = SelectBlitter (false, scaling, blend);
	layer->blitters[1] = SelectBlitter(true, scaling, blend);
}
