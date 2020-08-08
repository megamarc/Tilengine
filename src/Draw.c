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
#include <stdlib.h>
#include "Tilengine.h"
#include "Draw.h"
#include "Engine.h"
#include "Tileset.h"
#include "Tilemap.h"
#include "ObjectList.h"

/* private prototypes */
static void DrawSpriteCollision (int nsprite, uint8_t *srcpixel, uint16_t *dstpixel, int width, int dx);
static void DrawSpriteCollisionScaling (int nsprite, uint8_t *srcpixel, uint16_t *dstpixel, int width, int dx, int srcx);

static bool check_sprite_coverage(Sprite* sprite, int nscan)
{
	/* check sprite coverage */
	if (nscan < sprite->dstrect.y1 || nscan >= sprite->dstrect.y2)
		return false;
	if (sprite->dstrect.x2 < 0 || sprite->srcrect.x2 < 0)
		return false;
	if ((sprite->flags & FLAG_MASKED) && nscan >= engine->sprite_mask_top && nscan <= engine->sprite_mask_bottom)
		return false;
	return true;
}

/* Draws the next scanline of the frame started with TLN_BeginFrame() or TLN_BeginWindowFrame() */
bool DrawScanline(void)
{
	int line = engine->line;
	uint8_t* scan = GetFramebufferLine(line);
	int size = engine->framebuffer.width;
	int c;
	int index;
	bool background_priority = false;
	bool sprite_priority = false;
	List* list;

	/* call raster effect callback */
	if (engine->cb_raster)
		engine->cb_raster(line);

	/* background is bitmap */
	if (engine->bgbitmap && engine->bgpalette)
	{
		if (size > engine->bgbitmap->width)
			size = engine->bgbitmap->width;
		if (line < engine->bgbitmap->height)
			engine->blit_fast(TLN_GetBitmapPtr(engine->bgbitmap, 0, line), engine->bgpalette, scan, size, 1, 0, NULL);
	}

	/* background is solid color */
	else if (engine->bgcolor)
		BlitColor(scan, engine->bgcolor, size);

	background_priority = false;
	memset(engine->priority, 0, engine->framebuffer.pitch);
	memset(engine->collision, -1, engine->framebuffer.width * sizeof(uint16_t));

	/* draw background layers */
	for (c = engine->numlayers - 1; c >= 0; c--)
	{
		Layer* layer = &engine->layers[c];

		/* link layer */
		if (layer->parent != NULL)
		{
			layer->hstart = layer->parent->hstart;
			layer->vstart = layer->parent->vstart;
		}

		if (layer->ok && !layer->priority && line >= layer->clip.y1 && line <= layer->clip.y2)
		{
			if (layer->draw(c, line) == true)
				background_priority = true;
		}
	}

	/* draw regular sprites */
	list = &engine->list_sprites;
	index = list->first;
	while (index != -1)
	{
		Sprite* sprite = &engine->sprites[index];
		if (check_sprite_coverage(sprite, line))
		{
			if (!(sprite->flags & FLAG_PRIORITY))
				sprite->draw(index, line);
			else
				sprite_priority = true;
		}
		index = sprite->list_node.next;
	}

	/* draw background layers with priority */
	for (c = engine->numlayers - 1; c >= 0; c--)
	{
		const Layer* layer = &engine->layers[c];
		if (layer->ok && layer->priority && line >= layer->clip.y1 && line <= layer->clip.y2)
			layer->draw(c, line);
	}

	/* overlay background tiles with priority */
	if (background_priority == true)
	{
		uint32_t* src = (uint32_t*)engine->priority;
		uint32_t* dst = (uint32_t*)scan;
		for (c = 0; c < engine->framebuffer.width; c++)
		{
			if (*src)
				*dst = *src;
			src++;
			dst++;
		}
	}

	/* draw sprites with priority */
	if (sprite_priority == true)
	{
		index = list->first;
		while (index != -1)
		{
			Sprite* sprite = &engine->sprites[index];
			if (check_sprite_coverage(sprite, line) && (sprite->flags & FLAG_PRIORITY))
				sprite->draw(index, line);
			index = sprite->list_node.next;
		}
	}

	/* next scanline */
	engine->line++;
	return engine->line < engine->framebuffer.height;
}

/* draw scanline of tiled background */
static bool DrawLayerScanline (int nlayer, int nscan)
{
	const Layer *layer = &engine->layers[nlayer];
	const TLN_Tileset tileset = layer->tileset;
	const TLN_Tilemap tilemap = layer->tilemap;
	int shift;
	TLN_Tile tile;
	uint8_t *srcpixel;
	int x,x1;
	int xpos, ypos;
	int xtile, ytile;
	int srcx, srcy;
	int direction, width;
	int column;
	int line;
	uint8_t *dstpixel;
	uint8_t *dstpixel_pri;
	uint8_t *dst;
	bool color_key;
	bool priority = false;

	/* mosaic effect */
	if (layer->mosaic.h != 0)
	{
		shift = 0;
		dstpixel = layer->mosaic.buffer;
		if (nscan % layer->mosaic.h == 0)
			memset (dstpixel, 0, engine->framebuffer.width);
		else
			goto draw_end;
	}
	else
	{
		shift = 2;
		dstpixel = GetFramebufferLine (nscan);
	}

	/* target lines */
	x = layer->clip.x1;
	dstpixel += (x << shift);
	dstpixel_pri = engine->priority;

	xpos  = (layer->hstart + x) % layer->width;
	xtile = xpos >> tileset->hshift;
	srcx  = xpos & tileset->hmask;

	/* fill whole scanline */
	column = x % tileset->width;
	while (x < layer->clip.x2)
	{
		int tilewidth;

		/* column offset: update ypos */
		if (layer->column)
		{
			ypos = (layer->vstart + nscan + layer->column[column]) % layer->height;
			if (ypos < 0)
				ypos = layer->height + ypos;
		}
		else
			ypos  = (layer->vstart + nscan) % layer->height;

		ytile = ypos >> tileset->vshift;
		srcy  = ypos & tileset->vmask;

		tile = &tilemap->tiles[ytile*tilemap->cols + xtile];

		/* get effective tile width */
		tilewidth = tileset->width - srcx;
		x1 = x + tilewidth;
		if (x1 > layer->clip.x2)
			x1 = layer->clip.x2;
		width = x1 - x;

		/* paint if not empty tile */
		if (tile->index)
		{
			const uint16_t tile_index = tileset->tiles[tile->index];
			
			/* H/V flip */
			if (tile->flags & FLAG_FLIPX)
			{
				direction = -1;
				srcx = tilewidth - 1;
			}
			else
				direction = 1;
			if (tile->flags & FLAG_FLIPY)
				srcy = tileset->height - srcy - 1;

			/* paint tile scanline */
			srcpixel = &GetTilesetPixel (tileset, tile_index, srcx, srcy);
			if (tile->flags & FLAG_PRIORITY)
			{
				dst = dstpixel_pri;
				priority = true;
			}
			else
			{
				dst = dstpixel;
			}
			line = GetTilesetLine (tileset, tile_index, srcy);
			color_key = *(tileset->color_key + line);
			layer->blitters[color_key] (srcpixel, layer->palette, dst, width, direction, 0, layer->blend);
		}

		/* next tile */
		x += width;
		width <<= shift;
		dstpixel += width;
		dstpixel_pri += width;
		xtile = (xtile + 1) % tilemap->cols;
		srcx = 0;
		column++;
	}

draw_end:
	if (layer->mosaic.h != 0)
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = layer->mosaic.buffer + offset;
		uint8_t* dstptr = GetFramebufferLine (nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		if (layer->blend != NULL)
			BlitMosaicBlend (srcptr, layer->palette, dstptr, width, layer->mosaic.w, layer->blend);
		else
			BlitMosaicSolid (srcptr, layer->palette, dstptr, width, layer->mosaic.w);
	}

	return priority;
}

/* draw scanline of tiled background with scaling */
static bool DrawLayerScanlineScaling (int nlayer, int nscan)
{
	const Layer *layer = &engine->layers[nlayer];
	const TLN_Tileset tileset = layer->tileset;
	const TLN_Tilemap tilemap = layer->tilemap;
	int shift;
	TLN_Tile tile;
	uint8_t *srcpixel;
	int x,x1;
	int xpos, ypos;
	int xtile, ytile;
	int srcx, srcy;
	int direction, width;
	int column;
	int line;
	uint8_t *dstpixel;
	uint8_t *dstpixel_pri;
	uint8_t *dst;
	fix_t fix_tilewidth;
	fix_t fix_x;
	fix_t dx;
	bool color_key;
	bool priority = false;
	
	/* mosaic effect */
	if (layer->mosaic.h != 0)
	{
		shift = 0;
		dstpixel = layer->mosaic.buffer;
		if (nscan % layer->mosaic.h == 0)
			memset (dstpixel, 0, engine->framebuffer.width);
		else
			goto draw_end;
	}
	else
	{
		shift = 2;
		dstpixel = GetFramebufferLine (nscan);
	}

	/* target lines */
	x = layer->clip.x1;
	dstpixel += (x << shift);
	dstpixel_pri = engine->priority;

	xpos  = (layer->hstart + fix2int(x*layer->dx)) % layer->width;
	xtile = xpos >> tileset->hshift;
	srcx  = xpos & tileset->hmask;

	/* fill whole scanline */
	fix_x = int2fix (x);
	column = x % tileset->width;
	while (x < layer->clip.x2)
	{
		int tilewidth;
		int tilescalewidth;

		/* column offset: update ypos */
		ypos = nscan;
		if (layer->column)
			ypos += layer->column[column];

		ypos = layer->vstart + fix2int(ypos*layer->dy);
		if (ypos < 0)
			ypos = layer->height + ypos;
		else
			ypos = ypos % layer->height;

		ytile = ypos >> tileset->vshift;
		srcy  = ypos & tileset->vmask;

		tile = &tilemap->tiles[ytile*tilemap->cols + xtile];

		/* get effective tile width */
		tilewidth = tileset->width - srcx;
		dx = int2fix(tilewidth);
		fix_tilewidth = tilewidth * layer->xfactor;
		fix_x += fix_tilewidth;
		x1 = fix2int (fix_x);
		tilescalewidth = x1 - x;
		if (tilescalewidth)
			dx /= tilescalewidth;
		else
			dx = 0;

		/* right clip */
		if (x1 > layer->clip.x2)
			x1 = layer->clip.x2;
		width = x1 - x;
		
		/* paint if tile is not empty */
		if (tile->index)
		{
			const uint16_t tile_index = tileset->tiles[tile->index];

			/* volteado H/V */
			if (tile->flags & FLAG_FLIPX)
			{
				direction = -dx;
				srcx = tilewidth - 1;
			}
			else
				direction = dx;
			if (tile->flags & FLAG_FLIPY)
				srcy = tileset->height - srcy - 1;

			/* pinta tile scanline */
			srcpixel = &GetTilesetPixel (tileset, tile_index, srcx, srcy);
			if (tile->flags & FLAG_PRIORITY)
			{
				dst = dstpixel_pri;
				priority = true;
			}
			else
			{
				dst = dstpixel;
			}
			line = GetTilesetLine (tileset, tile_index, srcy);
			color_key = *(tileset->color_key + line);
			layer->blitters[color_key] (srcpixel, layer->palette, dst, width, direction, 0, layer->blend);
		}

		/* next tile */
		width <<= shift;
		dstpixel += width;
		dstpixel_pri += width;
		x = x1;
		xtile = (xtile + 1) % tilemap->cols;
		srcx = 0;
		column++;
	}

draw_end:
	if (layer->mosaic.h != 0)
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = layer->mosaic.buffer + offset;
		uint8_t* dstptr = GetFramebufferLine (nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		if (layer->blend != NULL)
			BlitMosaicBlend (srcptr, layer->palette, dstptr, width, layer->mosaic.w, layer->blend);
		else
			BlitMosaicSolid (srcptr, layer->palette, dstptr, width, layer->mosaic.w);
	}

	return priority;
}

/* draw scanline of tiled background with affine transform */
static bool DrawLayerScanlineAffine (int nlayer, int nscan)
{
	Layer *layer = &engine->layers[nlayer];
	const TLN_Tileset tileset = layer->tileset;
	const TLN_Tilemap tilemap = layer->tilemap;
	const TLN_Palette palette = layer->palette;
	int shift;
	TLN_Tile tile;
	int x, width;
	int x1,y1, x2,y2;
	fix_t dx, dy;
	int xpos, ypos;
	int xtile, ytile;
	int srcx, srcy;
	uint8_t *dstpixel;
	Point2D p1,p2;

	/* mosaic effect */
	if (layer->mosaic.h != 0)
	{
		shift = 0;
		dstpixel = layer->mosaic.buffer;
		if (nscan % layer->mosaic.h == 0)
			memset (dstpixel, 0, engine->framebuffer.width);
		else
			goto draw_end;
	}
	else
	{
		shift = 2;
		dstpixel = engine->tmpindex;
		memset (dstpixel, 0, engine->framebuffer.width);
	}

	/* target lines */
	x = layer->clip.x1;
	width = layer->clip.x2;

	xpos = layer->hstart;
	ypos = layer->vstart + nscan;

	Point2DSet (&p1, (math2d_t)xpos,(math2d_t)ypos);
	Point2DSet (&p2, (math2d_t)xpos + width, (math2d_t)ypos);
	Point2DMultiply (&p1, &layer->transform);
	Point2DMultiply (&p2, &layer->transform);

	x1 = float2fix(p1.x);
	y1 = float2fix(p1.y);
	x2 = float2fix(p2.x);
	y2 = float2fix(p2.y);

	dx = (x2 - x1) / width;
	dy = (y2 - y1) / width;

	while (x < width)
	{
		xpos = abs((fix2int(x1) + layer->width)) % layer->width;
		ypos = abs((fix2int(y1) + layer->height)) % layer->height;

		xtile = xpos >> tileset->hshift;
		ytile = ypos >> tileset->vshift;

		srcx = xpos & tileset->hmask;
		srcy = ypos & tileset->vmask;

		tile = &tilemap->tiles[ytile*tilemap->cols + xtile];

		/* paint if not empty tile */
		if (tile->index)
		{
			const uint16_t tile_index = tileset->tiles[tile->index];

			/* H/V flip */
			if (tile->flags & FLAG_FLIPX)
				srcx = tileset->width - srcx - 1;
			if (tile->flags & FLAG_FLIPY)
				srcy = tileset->height - srcy - 1;

			/* pinta scanline tile */
			*dstpixel = GetTilesetPixel (tileset, tile_index, srcx, srcy);
		}

		/* next pixel */
		x++;
		x1 += dx;
		y1 += dy;
		dstpixel++;
	}

draw_end:
	if (layer->mosaic.h != 0)
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = layer->mosaic.buffer + offset;
		uint8_t* dstptr = GetFramebufferLine (nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		if (layer->blend != NULL)
			BlitMosaicBlend (srcptr, layer->palette, dstptr, width, layer->mosaic.w, layer->blend);
		else
			BlitMosaicSolid (srcptr, layer->palette, dstptr, width, layer->mosaic.w);
	}
	else
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = engine->tmpindex + offset;
		uint8_t* dstptr = GetFramebufferLine (nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		layer->blitters[1] (srcptr, layer->palette, dstptr, width, 1, 0, layer->blend);
	}
	return false;
}

/* draw scanline of tiled background with per-pixel mapping */
static bool DrawLayerScanlinePixelMapping (int nlayer, int nscan)
{
	Layer *layer = &engine->layers[nlayer];
	const TLN_Tileset tileset = layer->tileset;
	const TLN_Tilemap tilemap = layer->tilemap;
	const TLN_Palette palette = layer->palette;
	const int hstart = layer->hstart + layer->width;
	const int vstart = layer->vstart + layer->height;
	int shift;
	TLN_Tile tile;
	int x, width;
	int xpos, ypos;
	int xtile, ytile;
	int srcx, srcy;
	uint8_t *dstpixel;
	TLN_PixelMap* pixel_map;

	/* mosaic effect */
	if (layer->mosaic.h != 0)
	{
		shift = 0;
		dstpixel = layer->mosaic.buffer;
		if (nscan % layer->mosaic.h == 0)
			memset (dstpixel, 0, engine->framebuffer.width);
		else
			goto draw_end;
	}
	else
	{
		shift = 2;
		dstpixel = engine->tmpindex;
		memset (dstpixel, 0, engine->framebuffer.width);
	}

	/* target lines */
	x = layer->clip.x1;
	width = layer->clip.x2 - layer->clip.x1;

	pixel_map = &layer->pixel_map[nscan*engine->framebuffer.width + x];
	while (x < width)
	{
		xpos = abs(hstart + pixel_map->dx) % layer->width;
		ypos = abs(vstart + pixel_map->dy) % layer->height;

		xtile = xpos >> tileset->hshift;
		ytile = ypos >> tileset->vshift;

		srcx = xpos & tileset->hmask;
		srcy = ypos & tileset->vmask;

		tile = &tilemap->tiles[ytile*tilemap->cols + xtile];

		/* paint if not empty tile */
		if (tile->index)
		{
			const uint16_t tile_index = tileset->tiles[tile->index];

			/* H/V flip */
			if (tile->flags & FLAG_FLIPX)
				srcx = tileset->width - srcx - 1;
			if (tile->flags & FLAG_FLIPY)
				srcy = tileset->height - srcy - 1;

			/* paint tile scanline */
			*dstpixel = GetTilesetPixel (tileset, tile_index, srcx, srcy);
		}

		/* next pixel */
		x++;
		dstpixel++;
		pixel_map++;
	}

draw_end:
	if (layer->mosaic.h != 0)
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = layer->mosaic.buffer + offset;
		uint8_t* dstptr = GetFramebufferLine (nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		if (layer->blend != NULL)
			BlitMosaicBlend (srcptr, layer->palette, dstptr, width, layer->mosaic.w, layer->blend);
		else
			BlitMosaicSolid (srcptr, layer->palette, dstptr, width, layer->mosaic.w);
	}
	else
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = engine->tmpindex + offset;
		uint8_t* dstptr = GetFramebufferLine (nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		layer->blitters[1] (srcptr, layer->palette, dstptr, width, 1, 0, layer->blend);
	}
	return true;
}

/* draw sprite scanline */
static bool DrawSpriteScanline (int nsprite, int nscan)
{
	int w;
	Sprite *sprite;
	uint8_t *srcpixel;
	uint8_t *dstscan;
	uint32_t *dstpixel;
	int srcx, srcy;
	int direction;

	sprite = &engine->sprites[nsprite];
	dstscan = GetFramebufferLine(nscan);
		
	srcx = sprite->srcrect.x1;
	srcy = sprite->srcrect.y1 + (nscan - sprite->dstrect.y1);
	w = sprite->dstrect.x2 - sprite->dstrect.x1;

	/* H/V flip */
	if (sprite->flags & FLAG_FLIPX)
	{
		direction = -1;
		srcx = sprite->info->w - srcx - 1;
	}
	else
		direction = 1;
	if (sprite->flags & FLAG_FLIPY)
		srcy = sprite->info->h - srcy - 1;

	srcpixel = sprite->pixels + (srcy*sprite->pitch) + srcx;
	dstpixel = (uint32_t*)(dstscan + (sprite->dstrect.x1 << 2));
	sprite->blitter (srcpixel, sprite->palette, dstpixel, w, direction, 0, sprite->blend);

	if (sprite->do_collision)
	{
		uint16_t* dstpixel = engine->collision + sprite->dstrect.x1;
		DrawSpriteCollision (nsprite, srcpixel, dstpixel, w, direction);
	}
	return true;
}

/* draw sprite scanline with scaling */
static bool DrawScalingSpriteScanline (int nsprite, int nscan)
{
	Sprite *sprite;
	uint8_t *srcpixel;
	uint8_t *dstscan;
	uint32_t *dstpixel;
	int srcx, srcy;
	int dstw,dstx,dx;
	struct Palette* palette;

	sprite = &engine->sprites[nsprite];

	dstscan = GetFramebufferLine(nscan);
	srcx = sprite->srcrect.x1;
	srcy = sprite->srcrect.y1 + (nscan - sprite->dstrect.y1)*sprite->dy;
	dstw = sprite->dstrect.x2 - sprite->dstrect.x1;

	/* H/V flip */
	if (sprite->flags & FLAG_FLIPX)
	{
		srcx = int2fix(sprite->info->w) - srcx;
		dstx = sprite->dstrect.x2;
		dx = -sprite->dx;
	}
	else
	{
		dstx = sprite->dstrect.x1;
		dx = sprite->dx;
	}
	if (sprite->flags & FLAG_FLIPY)
		srcy = int2fix(sprite->info->h) - srcy;

	palette = sprite->palette;
	srcpixel = sprite->pixels + (fix2int(srcy)*sprite->pitch);
	dstpixel = (uint32_t*)(dstscan + (sprite->dstrect.x1 << 2));
	sprite->blitter (srcpixel, sprite->palette, dstpixel, dstw, dx, srcx, sprite->blend);

	if (sprite->do_collision)
	{
		uint16_t* dstpixel = engine->collision + sprite->dstrect.x1;
		DrawSpriteCollisionScaling (nsprite, srcpixel, dstpixel, dstw, dx, srcx);
	}
	return true;
}

/* Experimental WIP: blit pre-rotated sprite */
static bool DrawSpriteScanlineRotation(int nsprite, int nscan)
{
	int w;
	Sprite *sprite;
	uint8_t *srcpixel;
	uint8_t *dstscan;
	uint32_t *dstpixel;
	int srcx, srcy;
	int direction;

	sprite = &engine->sprites[nsprite];

	/* comprueba que el sprite está en este scanline y es visible */
	/*
	if (nscan<sprite->dstrect.y1 || nscan >= sprite->dstrect.y2)
		return false;
	if (sprite->dstrect.x2 < 0 || sprite->srcrect.x2 < 0)
		return false;
		*/

	if (nscan < sprite->y || nscan > sprite->y + sprite->rotation_bitmap->height)
		return false;
	dstscan = GetFramebufferLine(nscan);

/*
	srcx = sprite->srcrect.x1;
	srcy = sprite->srcrect.y1 + (nscan - sprite->dstrect.y1);
	w = sprite->dstrect.x2 - sprite->dstrect.x1;
	*/
	srcx = sprite->srcrect.x1;
	srcy = sprite->srcrect.y1 + (nscan - sprite->y);
	w = sprite->dstrect.x2 - sprite->dstrect.x1;

	/* volteado H/V */
	if (sprite->flags & FLAG_FLIPX)
	{
		direction = -1;
		srcx = sprite->rotation_bitmap->width - srcx - 1;
	}
	else
		direction = 1;
	if (sprite->flags & FLAG_FLIPY)
		srcy = sprite->rotation_bitmap->height - srcy - 1;

	srcpixel = sprite->rotation_bitmap->data + (srcy*sprite->rotation_bitmap->pitch) + srcx;
	dstpixel = (uint32_t*)(dstscan + (sprite->dstrect.x1 << 2));
	sprite->blitter(srcpixel, sprite->palette, dstpixel, w, direction, 0, sprite->blend);

	if (sprite->do_collision)
	{
		uint16_t* dstpixel = engine->collision + sprite->dstrect.x1;
		DrawSpriteCollision(nsprite, srcpixel, dstpixel, w, direction);
	}
	return true;
}

/* updates per-pixel sprite collision buffer */
static void DrawSpriteCollision (int nsprite, uint8_t *srcpixel, uint16_t *dstpixel, int width, int dx)
{
	while (width)
	{
		if (*srcpixel)
		{
			if (*dstpixel != 0xFFFF)
			{
				engine->sprites[nsprite].collision = true;
				engine->sprites[*dstpixel].collision = true;
			}
			*dstpixel = (uint16_t)nsprite;
		}
		srcpixel += dx;
		dstpixel++;
		width--;
	}
}

/* updates per-pixel sprite collision buffer for scaled sprite */
static void DrawSpriteCollisionScaling (int nsprite, uint8_t *srcpixel, uint16_t *dstpixel, int width, int dx, int srcx)
{
	while (width)
	{
		uint32_t src = *(srcpixel + srcx/(1 << FIXED_BITS));
		if (src)
		{
			if (*dstpixel != 0xFFFF)
			{
				engine->sprites[nsprite].collision = true;
				engine->sprites[*dstpixel].collision = true;
			}
			*dstpixel = (uint16_t)nsprite;
		}		

		/* next pixel */
		srcx += dx;
		dstpixel++;
		width--;
	}
}

/* draws regular bitmap scanline for bitmap-based layer */
static bool DrawBitmapScanline(int nlayer, int nscan)
{
	const Layer *layer = &engine->layers[nlayer];
	TLN_Bitmap bitmap = layer->bitmap;
	TLN_Palette palette = layer->palette;
	uint8_t *srcpixel;
	int shift;
	int x, x1;
	int xpos, ypos;
	int direction, width;
	uint8_t *dstpixel;
	bool color_key;

	/* mosaic effect */
	if (layer->mosaic.h != 0)
	{
		shift = 0;
		dstpixel = layer->mosaic.buffer;
		if (nscan % layer->mosaic.h == 0)
			memset(dstpixel, 0, engine->framebuffer.width);
		else
			goto draw_end;
	}
	else
	{
		shift = 2;
		dstpixel = GetFramebufferLine(nscan);
	}

	/* target lines */
	x = layer->clip.x1;
	dstpixel += (x << shift);
	ypos = (layer->vstart + nscan) % layer->height;
	xpos = (layer->hstart + x) % layer->width;
	direction = 1;
	color_key = true;

	/* draws bitmap scanline */
	while (x < layer->clip.x2)
	{
		/* get effective width */
		width = layer->width - xpos;
		x1 = x + width;
		if (x1 > layer->clip.x2)
			x1 = layer->clip.x2;
		width = x1 - x;

		srcpixel = (uint8_t*)get_bitmap_ptr(bitmap, xpos, ypos);
		layer->blitters[color_key](srcpixel, palette, dstpixel, width, direction, 0, layer->blend);
		x += width;
		width <<= shift;
		dstpixel += width;
		xpos = 0;
	}

draw_end:
	if (layer->mosaic.h != 0)
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = layer->mosaic.buffer + offset;
		uint8_t* dstptr = GetFramebufferLine(nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		if (layer->blend != NULL)
			BlitMosaicBlend(srcptr, layer->palette, dstptr, width, layer->mosaic.w, layer->blend);
		else
			BlitMosaicSolid(srcptr, layer->palette, dstptr, width, layer->mosaic.w);
	}

	return false;
}

/* draws regular bitmap scanline for bitmap-based layer with scaling */
static bool DrawBitmapScanlineScaling(int nlayer, int nscan)
{
	const Layer *layer = &engine->layers[nlayer];
	int shift;
	uint8_t *srcpixel;
	int x, x1;
	int xpos, ypos;
	int direction, width;
	uint8_t *dstpixel;
	fix_t fix_tilewidth;
	fix_t fix_x;
	fix_t dx;
	bool color_key;

	/* mosaic effect */
	if (layer->mosaic.h != 0)
	{
		shift = 0;
		dstpixel = layer->mosaic.buffer;
		if (nscan % layer->mosaic.h == 0)
			memset(dstpixel, 0, engine->framebuffer.width);
		else
			goto draw_end;
	}
	else
	{
		shift = 2;
		dstpixel = GetFramebufferLine(nscan);
	}

	/* target line */
	x = layer->clip.x1;
	dstpixel += (x << shift);
	xpos = (layer->hstart + fix2int(x*layer->dx)) % layer->width;

	/* fill whole scanline */
	fix_x = int2fix(x);
	while (x < layer->clip.x2)
	{
		int tilescalewidth;

		ypos = layer->vstart + fix2int(nscan*layer->dy);
		if (ypos < 0)
			ypos = layer->height + ypos;
		else
			ypos = ypos % layer->height;

		/* get effective width */
		width = layer->width - xpos;
		dx = int2fix(width);
		fix_tilewidth = width * layer->xfactor;
		fix_x += fix_tilewidth;
		x1 = fix2int(fix_x);
		tilescalewidth = x1 - x;
		if (tilescalewidth)
			dx /= tilescalewidth;
		else
			dx = 0;

		/* right clipping */
		if (x1 > layer->clip.x2)
			x1 = layer->clip.x2;
		width = x1 - x;

		/* draw bitmap scanline */
		direction = dx;
		srcpixel = (uint8_t*)get_bitmap_ptr(layer->bitmap, xpos, ypos);
		color_key = true;
		layer->blitters[color_key](srcpixel, layer->palette, dstpixel, width, direction, 0, layer->blend);

		/* next */
		width <<= shift;
		dstpixel += width;
		x = x1;
		xpos = 0;
	}

draw_end:
	if (layer->mosaic.h != 0)
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = layer->mosaic.buffer + offset;
		uint8_t* dstptr = GetFramebufferLine(nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		if (layer->blend != NULL)
			BlitMosaicBlend(srcptr, layer->palette, dstptr, width, layer->mosaic.w, layer->blend);
		else
			BlitMosaicSolid(srcptr, layer->palette, dstptr, width, layer->mosaic.w);
	}

	return false;
}

/* draws regular bitmap scanline for bitmap-based layer with affine transform */
static bool DrawBitmapScanlineAffine(int nlayer, int nscan)
{
	Layer *layer = &engine->layers[nlayer];
	const TLN_Palette palette = layer->palette;
	const TLN_Bitmap bitmap = layer->bitmap;
	int shift;
	int x, width;
	int x1, y1, x2, y2;
	fix_t dx, dy;
	int xpos, ypos;
	uint8_t *dstpixel;
	Point2D p1, p2;

	/* mosaic effect */
	if (layer->mosaic.h != 0)
	{
		shift = 0;
		dstpixel = layer->mosaic.buffer;
		if (nscan % layer->mosaic.h == 0)
			memset(dstpixel, 0, engine->framebuffer.width);
		else
			goto draw_end;
	}
	else
	{
		shift = 2;
		dstpixel = engine->tmpindex;
		memset(dstpixel, 0, engine->framebuffer.width);
	}

	/* target lines */
	x = layer->clip.x1;
	width = layer->clip.x2;

	xpos = layer->hstart;
	ypos = layer->vstart + nscan;

	Point2DSet(&p1, (math2d_t)xpos, (math2d_t)ypos);
	Point2DSet(&p2, (math2d_t)xpos + width, (math2d_t)ypos);
	Point2DMultiply(&p1, &layer->transform);
	Point2DMultiply(&p2, &layer->transform);

	x1 = float2fix(p1.x);
	y1 = float2fix(p1.y);
	x2 = float2fix(p2.x);
	y2 = float2fix(p2.y);

	dx = (x2 - x1) / width;
	dy = (y2 - y1) / width;

	while (x < width)
	{
		xpos = abs((fix2int(x1) + layer->width)) % layer->width;
		ypos = abs((fix2int(y1) + layer->height)) % layer->height;
		*dstpixel = *get_bitmap_ptr(bitmap, xpos, ypos);

		/* next pixel */
		x++;
		x1 += dx;
		y1 += dy;
		dstpixel++;
	}

draw_end:
	if (layer->mosaic.h != 0)
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = layer->mosaic.buffer + offset;
		uint8_t* dstptr = GetFramebufferLine(nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		if (layer->blend != NULL)
			BlitMosaicBlend(srcptr, layer->palette, dstptr, width, layer->mosaic.w, layer->blend);
		else
			BlitMosaicSolid(srcptr, layer->palette, dstptr, width, layer->mosaic.w);
	}
	else
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = engine->tmpindex + offset;
		uint8_t* dstptr = GetFramebufferLine(nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		layer->blitters[1](srcptr, layer->palette, dstptr, width, 1, 0, layer->blend);
	}
	return false;
}

/* draws regular bitmap scanline for bitmap-based layer with per-pixel mapping */
static bool DrawBitmapScanlinePixelMapping(int nlayer, int nscan)
{
	Layer *layer = &engine->layers[nlayer];
	const TLN_Bitmap bitmap = layer->bitmap;
	const TLN_Palette palette = layer->palette;
	const int hstart = layer->hstart + layer->width;
	const int vstart = layer->vstart + layer->height;
	int shift;
	int x, width;
	int xpos, ypos;
	uint8_t *dstpixel;
	TLN_PixelMap* pixel_map;

	/* mosaic effect */
	if (layer->mosaic.h != 0)
	{
		shift = 0;
		dstpixel = layer->mosaic.buffer;
		if (nscan % layer->mosaic.h == 0)
			memset(dstpixel, 0, engine->framebuffer.width);
		else
			goto draw_end;
	}
	else
	{
		shift = 2;
		dstpixel = engine->tmpindex;
		memset(dstpixel, 0, engine->framebuffer.width);
	}

	/* target lines */
	x = layer->clip.x1;
	width = layer->clip.x2 - layer->clip.x1;

	pixel_map = &layer->pixel_map[nscan*engine->framebuffer.width + x];
	while (x < width)
	{
		xpos = abs(hstart + pixel_map->dx) % layer->width;
		ypos = abs(vstart + pixel_map->dy) % layer->height;
		*dstpixel = *get_bitmap_ptr(bitmap, xpos, ypos);

		/* next pixel */
		x++;
		dstpixel++;
		pixel_map++;
	}

draw_end:
	if (layer->mosaic.h != 0)
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = layer->mosaic.buffer + offset;
		uint8_t* dstptr = GetFramebufferLine(nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		if (layer->blend != NULL)
			BlitMosaicBlend(srcptr, layer->palette, dstptr, width, layer->mosaic.w, layer->blend);
		else
			BlitMosaicSolid(srcptr, layer->palette, dstptr, width, layer->mosaic.w);
	}
	else
	{
		int offset = (layer->clip.x1 << shift);
		uint8_t* srcptr = engine->tmpindex + offset;
		uint8_t* dstptr = GetFramebufferLine(nscan) + offset;
		int width = layer->clip.x2 - layer->clip.x1;

		layer->blitters[1](srcptr, layer->palette, dstptr, width, 1, 0, layer->blend);
	}
	return false;
}

/* draws regular object layer scanline */
static bool DrawLayerObjectScanline(int nlayer, int nscan)
{
	const Layer* layer = &engine->layers[nlayer];
	struct _Object* object = layer->objects->list;
	int x1 = layer->hstart + layer->clip.x1;
	int x2 = layer->hstart + layer->clip.x2;
	int y = layer->vstart + nscan;
	uint8_t* dstscan = GetFramebufferLine(nscan);
	bool priority = false;

	while (object != NULL)
	{
		if (IsObjectInLine(object, x1, x2, y) && object->visible && object->bitmap != NULL)
		{
			int w;
			uint8_t *srcpixel;
			uint8_t *target;
			uint32_t *dstpixel;
			TLN_Bitmap bitmap = object->bitmap;
			int srcx, srcy;
			int dstx1, dstx2;
			int direction = 1;

			srcx = 0;
			srcy = y - object->y;
			dstx1 = object->x - x1;
			dstx2 = dstx1 + object->width;
			if (dstx1 < layer->clip.x1)
			{
				int w = layer->clip.x1 - dstx1;
				srcx = w;
				dstx1 = 0;
			}
			if (dstx2 > layer->clip.x2)
			{
				dstx2 = layer->clip.x2;
			}
			w = dstx2 - dstx1;

			/* H/V flip */
			if (object->flags & FLAG_FLIPX)
			{
				direction = -1;
				srcx = object->width - srcx - 1;
			}
			if (object->flags & FLAG_FLIPY)
				srcy = object->height - srcy - 1;

			/* paint tile scanline */
			srcpixel = get_bitmap_ptr(bitmap, srcx, srcy);
			if (object->flags & FLAG_PRIORITY)
			{
				target = engine->priority;
				priority = true;
			}
			else
				target = dstscan;
			dstpixel = (uint32_t*)(target + (dstx1 << 2));
			layer->blitters[1] (srcpixel, bitmap->palette, dstpixel, w, direction, 0, layer->blend);
		}
		object = object->next;
	}

	return priority;
}

/* draw modes */
enum
{
	DRAW_SPRITE,
	DRAW_TILED_LAYER,
	DRAW_BITMAP_LAYER,
	DRAW_OBJECT_LAYER,
	MAX_DRAW_TYPE,
};

/* table of function pointers to draw procedures */
static const ScanDrawPtr drawers[MAX_DRAW_TYPE][MAX_DRAW_MODE] =
{
	{ DrawSpriteScanline, DrawScalingSpriteScanline, NULL, NULL},
	{ DrawLayerScanline, DrawLayerScanlineScaling,	DrawLayerScanlineAffine, DrawLayerScanlinePixelMapping },
	{ DrawBitmapScanline, DrawBitmapScanlineScaling, DrawBitmapScanlineAffine, DrawBitmapScanlinePixelMapping },
	{ DrawLayerObjectScanline, NULL, NULL, NULL },
};

/* returns suitable draw procedure based on layer configuration */
ScanDrawPtr GetLayerDraw(Layer* layer)
{
	if (layer->tilemap != NULL)
		return drawers[DRAW_TILED_LAYER][layer->mode];
	else if (layer->bitmap != NULL)
		return drawers[DRAW_BITMAP_LAYER][layer->mode];
	else if (layer->objects != NULL)
		return drawers[DRAW_OBJECT_LAYER][layer->mode];
	else
		return NULL;
}

/* returns suitable draw procedure based on sprite configuration */
ScanDrawPtr GetSpriteDraw (draw_t mode)
{
	return drawers[DRAW_SPRITE][mode];
}
