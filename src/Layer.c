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

static void SelectBlitter (Layer* layer);

/*!
 * \deprecated Use \ref TLN_SetLayerTilemap instead
 * \brief
 * Configures a background layer with the specified tileset and tilemap
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * \param tileset
 * Optional reference to the tileset to assign. If the tilemap has a reference to its own tileset, passing NULL will assign the default tileset.
 * 
 * \param tilemap
 * Reference to the tilemap to assign
 * 
 * \remarks
 * This function doesn't modify the current position nor the blend mode,
 * but assigns the palette of the specified tileset
 *
 * \see
 * TLN_DisableLayer()
 */
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
		tileset = tilemap->tileset;

	if (!CheckBaseObject(tileset, OT_TILESET))
		return false;

	if (tilemap->maxindex <= tileset->numtiles)
	{
		layer->tileset = tileset;
		layer->tilemap = tilemap;
		layer->width = tilemap->cols*tileset->width;
		layer->height = tilemap->rows*tileset->height;
		if (tileset->palette)
			TLN_SetLayerPalette(nlayer, tileset->palette);
	}
	layer->bitmap = NULL;
	layer->objects = NULL;
	layer->type = LAYER_TILE;

	/* apply priority attribute */
	if (tileset->attributes != NULL)
	{
		const int num_tiles = tilemap->rows * tilemap->cols;
		int c;
		Tile* tile = tilemap->tiles;
		for (c = 0; c < num_tiles; c++, tile++)
		{
			if (tile->index != 0)
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

	if (tilemap->visible)
	{
		layer->ok = true;
		layer->draw = GetLayerDraw(layer);
		SelectBlitter(layer);
	}

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief Configures a tiled background layer with the specified tilemap
 * \param nlayer Layer index [0, num_layers - 1]
 * \param tilemap Reference to the tilemap to assign
 * \returns true if success or false if error
 * \see TLN_LoadTilemap()
 */
bool TLN_SetLayerTilemap(int nlayer, TLN_Tilemap tilemap)
{
	return TLN_SetLayer(nlayer, NULL, tilemap);
}

/*!
* \brief
* Configures a background layer with the specified full bitmap
*
* \param nlayer
* Layer index [0, num_layers - 1]
*
* \param bitmap
* Reference to the bitmap to assign
*
* \remarks
* This function doesn't modify the current position nor the blend mode,
* but assigns the palette of the specified bitmap
*
* \see
* TLN_LoadBitmap() TLN_DisableLayer()
*/
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

	layer->tileset = NULL;
	layer->tilemap = NULL;
	layer->bitmap = bitmap;
	layer->objects = NULL;
	layer->width = bitmap->width;
	layer->height = bitmap->height;
	if (bitmap->palette)
		TLN_SetLayerPalette(nlayer, bitmap->palette);

	/* require palette */
	if (layer->palette)
	{
		layer->type = LAYER_BITMAP;
		layer->ok = true;
		layer->draw = GetLayerDraw(layer);
		SelectBlitter(layer);
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

/*!
 * \brief Configures a background layer with a object list and an image-based tileset
 * 
 * \param nlayer Layer index [0, num_layers - 1]
 * \param objects Reference to the TLN_ObjectList to attach
 * \param tileset optional reference to the image-based tileset object. If NULL, object list must have an attached tileset
 * \see TLN_LoadObjectList()
 */
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

	layer->tileset = tileset;
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
		SelectBlitter(layer);
	}
	TLN_SetLastError(TLN_ERR_OK);
	return true;
}

/*!
 * \brief Sets full layer priority, appearing in front of sprites
 * 
 * \param nlayer Layer index [0, num_layers - 1]
 * \param enable Enable (true) or dsiable (false) full priority
 */
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

/* removed, keep for ABI compatibility  */
bool TLN_SetLayerParent(int nlayer, int parent)
{
	return true;
}

/* removed, keep for ABI compatibility  */
bool TLN_DisableLayerParent(int nlayer)
{
	return true;
}

/*!
 * \brief
 * Returns the layer width in pixels
 *
 * \param nlayer
 * Layer index [0, num_layers - 1]
 *
 * \see TLN_SetLayer(), TLN_GetLayerHeight()
 */
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

/*!
 * \brief
 * Returns the layer height in pixels
 *
 * \param nlayer
 * Layer index [0, num_layers - 1]
 *
 * \see TLN_SetLayer(), TLN_GetLayerWidth()
 */
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

/*!
 * \brief
 * Sets the blending mode (transparency effect)
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * \param mode
 * Member of the TLN_Blend enumeration
 * 
 * \param factor
 * Deprecated as of 1.12, left for backwards compatibility but doesn't have effect.
 * 
 * \see
 * Blending
 */
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
	SelectBlitter (layer);
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Sets the color palette to the layer
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * \param palette
 * Reference to the  palette to assign to the layer
 *
 * When a layer is assigned with a tileset with the function TLN_SetLayer(), it
 * automatically sets the palette of the assigned tileset to the layer. 
 * Use this function to override it and set another palette
 * 
 * \remarks
 * Call this function inside a raster callback to change the palette in the middle
 * of the frame to get raster effect colors, like and "underwater" palette below the
 * water line in a partially submerged background, or a gradient palette in an area at
 * the top of the screen to simulate a "depth fog effect" in a pseudo 3d background
 */
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

/*!
 * \brief Returns the active palette of a layer
 * \param nlayer Layer index [0, num_layers - 1]
 * \returns Reference of the palette assigned to the layer
 * \see TLN_SetLayerPalette()
 */
TLN_Palette TLN_GetLayerPalette (int nlayer)
{
	if (nlayer < engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_OK);
		return engine->layers[nlayer].palette;
	}

	TLN_SetLastError(TLN_ERR_IDX_LAYER);
	return NULL;
}

/*!
 * \brief Returns the type of the layer
 * \param nlayer Layer index [0, num_layers - 1]
 * \returns \ref TLN_LayerType enumeration
 * \see TLN_SetLayerTilemap(), TLN_SetLayerObjects(), TLN_SetLayerBitmap()
 */
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

/*!
 * \brief Returns the active tileset on a \ref LAYER_TILE or \ref LAYER_OBJECT layer type
 * \param nlayer Layer index [0, num_layers - 1]
 * \returns Reference to the active tileset
 * \see TLN_SetLayerTilemap(), TLN_SetLayerObjects()
 */
TLN_Tileset TLN_GetLayerTileset(int nlayer)
{
	if (nlayer < engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_OK);
		return engine->layers[nlayer].tileset;
	}

	TLN_SetLastError(TLN_ERR_IDX_LAYER);
	return NULL;
}

/*!
 * \brief Returns the active tilemap on a \ref LAYER_TILE layer type
 * \param nlayer Layer index [0, num_layers - 1]
 * \returns Reference to the active tilemap
 * \see TLN_SetLayerTilemap()
 */
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

/*!
 * \brief Returns the active bitmap on a \ref LAYER_BITMAP layer type
 * \param nlayer Layer index [0, num_layers - 1]
 * \returns Reference to the active bitmap
 * \see TLN_SetLayerBitmap()
 */
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

/*!
 * \brief Returns the active object list on a \ref LAYER_OBJECT layer type
 * \param nlayer Layer index [0, num_layers - 1]
 * \returns Reference to the active objects list
 * \see TLN_SetLayerObjects(), TLN_GetListObject()
 */
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

/*!
 * \brief
 * Sets the position of the tileset that corresponds to the upper left corner
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * \param hstart
 * Horizontal offset in the tileset on the left side
 * 
 * \param vstart
 * Vertical offset in the tileset on the top side
 * 
 * The tileset usually spans an area much bigger than the viewport. Use this
 * function to move the viewport insde the tileset. Change this value progressively
 * for each frame to get a scrolling effect
 * 
 * \remarks
 * Call this function inside a raster callback to get a raster scrolling effect. 
 * Use this to create horizontal strips of the same
 * layer that move at different speeds to simulate depth. The extreme case of this effect, where
 * the position is changed in each scanline, is called "line scroll" and was the technique used by
 * games such as Street Fighter II to simualte a pseudo 3d floor, or many racing games to simulate
 * a 3D road.
 */
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

/*!
 * \brief
 * Gets info about the tile located in tilemap space
 * 
 * \param nlayer
 * Id of the layer to query [0, num_layers - 1]
 * 
 * \param x
 * x position
 * 
 * \param y
 * y position
 * 
 * \param info
 * Pointer to an application-allocated TLN_TileInfo struct that will get the data
 * 
 * \returns
 * true if success or false if error
 * 
 * \remarks
 * Use this function to implement collision detection between sprites and the main background layer.
 * 
 * \see
 * TLN_TileInfo
 */
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
	if (!CheckBaseObject (layer->tileset, OT_TILESET) || !CheckBaseObject (layer->tilemap, OT_TILEMAP))
		return false;

	tileset = layer->tileset;
	tilemap = layer->tilemap;

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

/*!
 * \brief
 * Enables column offset mode for this layer
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * \param offset
 * Array of offsets to set. Set NULL to disable column offset mode
 * 
 * Column offset is a value that is added or substracted (depeinding on the
 * sign) to the vertical position for that layer (see TLN_SetLayerPosition) for
 * each column in the tilemap assigned to that layer. 
 * 
 * \remarks
 * This feature is tipically used to simulate vertical strips moving at different
 * speeds, or combined with a line scroll effect, to fake rotations where the angle
 * is small. The Sega Genesis games Puggsy and Chuck Rock II used this trick to simulate
 * partially rotating backgrounds
 */
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

/*! \brief Enables a layer previously disabled with \ref TLN_DisableLayer 
 * \param nlayer Layer index [0, num_layers - 1]
 * \remarks The layer must have been previously configured. A layer without a prior configuration can't be enabled 
 */
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
	if (layer->palette && ((layer->tilemap && layer->tileset) || (layer->objects && layer->tileset) || layer->bitmap))
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

/*!
 * \brief
 * Sets affine transform matrix to enable rotating and scaling of this layer
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * \param affine
 * Pointer to an TLN_Affine matrix, or NULL to disable it
 * 
 * Enable the transformation matrix to give the layer the capabilities of the famous 
 * Super Nintendo / Famicom Mode 7. Beware that the rendering of a transformed layer
 * uses more CPU than a regular layer. Unlike the original Mode 7, that could only transform
 * the single layer available, Tilengine can transform all the layers at the same time. The only
 * limitation is the available CPU power.
 *
 * \remarks
 * Call this function inside a raster callback to set the transformation matrix in the middle of
 * the frame. Setting it for each scanline is the trick used by many Super Nintendo games to fake
 * a 3D perspective projection.
 * 
 * \see
 * TLN_SetLayerTransform()
 */
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
		SelectBlitter (layer);

		/*printf ("TLN_SetLayerAffineTransform (ptr=%08Xh, a=%.02f, d=%.02f,%.02f, s=%.02f,%.02f)\n",
			affine, affine->angle, affine->dx, affine->dy, affine->sx, affine->sy);*/

		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return TLN_ResetLayerMode (nlayer);
}

/*!
 * \brief
 * Sets affine transform matrix to enable rotating and scaling of this layer
 * 
 * \param layer
 * Layer index [0, num_layers - 1]
 * 
 * \param angle
 * Rotation angle in degrees
 * 
 * \param dx
 * Horizontal displacement
 * 
 * \param dy
 * Vertical displacement
 * 
 * \param sx
 * Horizontal scaling
 * 
 * \param sy
 * Vertical scaling
 * 
 * \remarks
 * This function is a simple wrapper to TLN_SetLayerAffineTransform() without using the TLN_Affine struct
 * 
 * \see
 * TLN_SetLayerAffineTransform()
 */
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

/*!
 * \brief
 * Sets simple scaling
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * \param sx
 * Horizontal scale factor
 * 
 * \param sy
 * Vertical scale factor
 * 
 * By default the scaling factor of a given layer is 1.0f, 1.0f, which means
 * no scaling. Use values below 1.0 to downscale (shrink) and above 1.0 to upscale (enlarge).
 * Call TLN_ResetLayerMode() to disable scaling
 * 
 * Write detailed description for TLN_SetLayerScaling here.
 * 
 * \see TLN_ResetLayerMode()
 * 
 */
bool TLN_SetLayerScaling (int nlayer, float sx, float sy)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}
	
	layer = &engine->layers[nlayer];
	layer->xfactor = float2fix(sx);
	layer->dx = float2fix((1.0f/sx));
	layer->dy = float2fix((1.0f/sy));
	layer->mode = MODE_SCALING;
	layer->draw = GetLayerDraw (layer);
	SelectBlitter (layer);
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Sets the table for pixel mapping render mode
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * \param table
 * User-provided array of hres*vres sized TLN_PixelMap items 
 * 
 * \see
 * TLN_SetLayerScaling(), TLN_SetLayerAffineTransform()
 */
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

/*!
 * \brief
 * Disables scaling or affine transform for the layer
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * Write detailed description for TLN_ResetLayerMode here.
 * 
 * \see
 * TLN_SetLayerScaling(), TLN_SetLayerAffineTransform()
 */
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
	SelectBlitter (layer);
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Enables clipping rectangle on selected layer
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * \param x1
 * left coordinate
 *
 * \param y1
 * top coordinate
 *
 * \param x2
 * right coordinate
 *
 * \param y2
 * bottom coordinate
 *
 * \see
 * TLN_DisableLayerClip()
 */
bool TLN_SetLayerClip (int nlayer, int x1, int y1, int x2, int y2)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}
	
	layer = &engine->layers[nlayer];
	layer->clip.x1 = x1 >= 0 && x1 <= engine->framebuffer.width? x1 : 0;
	layer->clip.x2 = x2 >= 0 && x2 <= engine->framebuffer.width? x2 : engine->framebuffer.width;
	layer->clip.y1 = y1 >= 0 && y1 <= engine->framebuffer.height? y1 : 0;
	layer->clip.y2 = y2 >= 0 && y2 <= engine->framebuffer.height? y2 : engine->framebuffer.height;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Disables clipping rectangle on selected layer
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 * 
 * \see
 * TLN_SetLayerClip()
 */
bool TLN_DisableLayerClip (int nlayer)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}
	
	layer = &engine->layers[nlayer];
	layer->clip.x1 = 0;
	layer->clip.x2 = engine->framebuffer.width;
	layer->clip.y1 = 0;
	layer->clip.y2 = engine->framebuffer.height;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Enables mosaic effect (pixelation) for selected layer
 *
 * \param nlayer
 * Layer index [0, num_layers - 1]
 *
 * \param width
 * horizontal pixel size
 *
 * \param height
 * vertical pixel size
 *
 * \see
 * TLN_DisableLayerMosaic()
 */
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
	SelectBlitter (layer);
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Disables mosaic effect for selected layer
 * 
 * \param nlayer
 * Layer index [0, num_layers - 1]
 *
 * \see
 * TLN_SetLayerMosaic()
 */
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

/* updates layer from world position, accounting offset and parallax */
void UpdateLayer(int nlayer)
{
	Layer* layer = GetLayer(nlayer);
	const int lx = (int)(engine->xworld*layer->world.xfactor) - layer->world.offsetx;
	const int ly = (int)(engine->yworld*layer->world.yfactor) - layer->world.offsety;
	TLN_SetLayerPosition(nlayer, lx, ly);
}

static void SelectBlitter (Layer* layer)
{
	bool scaling = layer->mode == MODE_SCALING;
	bool blend;
	int bpp;

	/* without mosaic effect */
	if (layer->mosaic.h == 0)
	{
		blend = layer->blend != NULL;
		bpp = 32;
	}
	/* with mosaic effect */
	else
	{
		blend = false;
		bpp = 8;
	}

	layer->blitters[0] = GetBlitter (bpp, false, scaling, blend);
	layer->blitters[1] = GetBlitter (bpp, true, scaling, blend);
}
