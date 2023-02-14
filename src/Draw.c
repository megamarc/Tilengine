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
#include "Sprite.h"

/* private prototypes */
static void DrawSpriteCollision(int nsprite, uint8_t *srcpixel, uint16_t *dstpixel, int width, int dx);
static void DrawSpriteCollisionScaling(int nsprite, uint8_t *srcpixel, uint16_t *dstpixel, int width, int dx, int srcx);

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

/* draw background scanline taking into account mosaic and windowing effects */
static bool draw_background_scanline(int nlayer, int line)
{
	/* draw */
	Layer* layer = &engine->layers[nlayer];
	LayerWindow* window = &layer->window;
	uint32_t* mosaic = layer->mosaic.buffer;
	uint32_t* scan = NULL;
	const bool inside = line >= window->y1 && line <= window->y2;
	const int framewidth = engine->framebuffer.width;
	const int windowwidth = layer->window.x2 - layer->window.x1;
	bool priority = false;
	bool build_mosaic = false;

	/* determine target buffer */
	if (layer->mosaic.h != 0)
	{
		if (line % layer->mosaic.h == 0)
		{
			build_mosaic = true;
			scan = engine->linebuffer;
		}
		else
			scan = NULL;
	}
	else if (layer->mode >= MODE_TRANSFORM)
		scan = engine->linebuffer;
	else
		scan = GetFramebufferLine(line);

	if (scan == engine->linebuffer)
		memset(scan, 0, engine->framebuffer.pitch);

	/* regular region */
	if (scan != NULL)
	{
		if (!window->invert)
		{
			if (inside)
				priority |= layer->draw(nlayer, scan, line, window->x1, window->x2);
		}
		else
		{
			if (inside)
			{
				priority |= layer->draw(nlayer, scan, line, 0, layer->window.x1);
				priority |= layer->draw(nlayer, scan, line, layer->window.x2, framewidth);
			}
			else
				priority |= layer->draw(nlayer, scan, line, 0, framewidth);
		}
	}
	scan = GetFramebufferLine(line);

	/* build mosaic to linebuffer */
	if (build_mosaic)
	{
		memset(mosaic, 0, engine->framebuffer.pitch);
		BlitMosaic(engine->linebuffer, mosaic, framewidth, layer->mosaic.w, NULL);
	}

	/* blit mosaic */
	if (layer->mosaic.h != 0)
	{
		if (!window->invert)
		{
			if (inside)
				Blit32_32(mosaic + window->x1, scan + window->x1, windowwidth, layer->blend);
		}
		else
		{
			if (inside)
			{
				Blit32_32(mosaic, scan, windowwidth, layer->blend);
				Blit32_32(mosaic + window->x2, scan + window->x2, framewidth - window->x2, layer->blend);
			}
			else
				Blit32_32(mosaic, scan, framewidth, layer->blend);
		}
	}
	else if (layer->mode >= MODE_TRANSFORM)
		Blit32_32(engine->linebuffer, scan, engine->framebuffer.width, layer->blend);

	/* clipped region */
	if (window->color != 0)
	{
		if (!window->invert)
		{
			if (inside)
			{
				BlitColor(scan, window->color, window->x1, window->blend);
				BlitColor(scan + window->x2, window->color, framewidth - window->x2, window->blend);
			}
			else
				BlitColor(scan, window->color, framewidth, window->blend);
		}
		else if (inside)
			BlitColor(scan + window->x1, window->color, windowwidth, window->blend);
	}

	return priority;
}

/* Draws the next scanline of the frame started with TLN_BeginFrame() or TLN_BeginWindowFrame() */
bool DrawScanline(void)
{
	int line = engine->line;
	uint32_t* scan = GetFramebufferLine(line);
	int size = engine->framebuffer.width;
	int c;
	int index;
	bool background_priority = false;	/* at least one tile in priority layer */
	bool sprite_priority = false;		/* at least one sprite in priority layer */
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
		BlitColor(scan, engine->bgcolor, size, NULL);

	/* draw regular background layers */
	if (engine->numlayers > 0)
	{
		background_priority = false;
		memset(engine->priority, 0, engine->framebuffer.pitch);
		for (c = engine->numlayers - 1; c >= 0; c--)
		{
			Layer* layer = &engine->layers[c];
	
			/* update if dirty */
			if (engine->dirty || layer->dirty)
			{
				const int lx = (int)(engine->xworld * layer->world.xfactor) - layer->world.offsetx;
				const int ly = (int)(engine->yworld * layer->world.yfactor) - layer->world.offsety;
				TLN_SetLayerPosition(c, lx, ly);
				layer->dirty = false;
			}

			/* draw */
			if (layer->ok && !layer->priority)
				background_priority |= draw_background_scanline(c, line);
		}
	}

	/* draw regular sprites */
	if (engine->numsprites > 0)
	{
		memset(engine->collision, -1, engine->framebuffer.width * sizeof(uint16_t));
		list = &engine->list_sprites;
		index = list->first;
		while (index != -1)
		{
			Sprite* sprite = &engine->sprites[index];

			/* update if dirty */
			if (sprite->world_space && (sprite->dirty || engine->dirty))
			{
				sprite->x = sprite->xworld - engine->xworld;
				sprite->y = sprite->yworld - engine->yworld;
				UpdateSprite(sprite);
				sprite->dirty = false;
			}

			if (check_sprite_coverage(sprite, line))
			{
				if (!(sprite->flags & FLAG_PRIORITY))
					sprite->draw(index, scan, line, 0, 0);
				else
					sprite_priority = true;
			}
			index = sprite->list_node.next;
		}
	}

	/* draw background layers with priority */
	if (engine->numlayers > 0)
	{
		for (c = engine->numlayers - 1; c >= 0; c--)
		{
			Layer* layer = &engine->layers[c];
			if (layer->ok && layer->priority)
				draw_background_scanline(c, line);
		}
	}

	/* overlay background tiles with priority */
	if (background_priority == true)
	{
		uint32_t* src = engine->priority;
		uint32_t* dst = scan;
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
				sprite->draw(index, scan, line, 0, 0);
			index = sprite->list_node.next;
		}
	}

	/* next scanline */
	engine->dirty = false;
	engine->line++;
	return engine->line < engine->framebuffer.height;
}

typedef struct
{
	int width, height;
	int srcx;
	int srcy;
	int dx;
	int stride;
}
Tilescan;

/* process flip flags */
static inline void process_flip(uint16_t flags, Tilescan* scan)
{
	/* H/V flip */
	if (flags & FLAG_FLIPX)
	{
		scan->dx = -scan->dx;
		scan->srcx = scan->width - 1;
	}
	if (flags & FLAG_FLIPY)
		scan->srcy = scan->height - scan->srcy - 1;
}

/* process flip & rotation flags */
static inline void process_flip_rotation(uint16_t flags, Tilescan* scan)
{
	if (flags & FLAG_ROTATE)
	{
		int tmp = scan->srcx;
		scan->srcx = scan->srcy;
		scan->srcy = tmp;
		scan->dx *= scan->stride;

		/* H/V flip */
		if (flags & FLAG_FLIPX)
		{
			scan->dx = -scan->dx;
			scan->srcy = scan->height - scan->srcy - 1;
		}
		if (flags & FLAG_FLIPY)
			scan->srcx = scan->width - scan->srcx - 1;
	}
	else
	{
		/* H/V flip */
		if (flags & FLAG_FLIPX)
		{
			scan->dx = -scan->dx;
			scan->srcx = scan->width - scan->srcx - 1;
		}
		if (flags & FLAG_FLIPY)
			scan->srcy = scan->height - scan->srcy - 1;
	}
}

/* draw scanline of tiled background */
static bool DrawTiledScanline(int nlayer, uint32_t* dstpixel, int nscan, int tx1, int tx2)
{
	const Layer *layer = (const Layer*)&engine->layers[nlayer];
	bool priority = false;
	Tilescan scan = { 0 };

	/* target lines */
	int x = tx1;
	const TLN_Tilemap tilemap = layer->tilemap;
	const TLN_Tileset tileset = tilemap->tilesets[0];
	int xpos = (layer->hstart + x) % layer->width;
	int xtile = xpos >> tileset->hshift;

	scan.width = scan.height = scan.stride = tileset->width;
	scan.srcx = xpos & tileset->hmask;

	/* fill whole scanline */
	int column = x % tileset->width;
	while (x < tx2)
	{
		/* column offset: update ypos */
		int ypos;
		if (layer->column)
		{
			ypos = (layer->vstart + nscan + layer->column[column]) % layer->height;
			if (ypos < 0)
				ypos = layer->height + ypos;
		}
		else
			ypos = (layer->vstart + nscan) % layer->height;

		int ytile = ypos >> tileset->vshift;
		scan.srcy = ypos & tileset->vmask;

		TLN_Tile tile = &tilemap->tiles[ytile*tilemap->cols + xtile];

		/* get effective tile width */
		int tilewidth = tileset->width - scan.srcx;
		int x1 = x + tilewidth;
		if (x1 > tx2)
			x1 = tx2;
		int width = x1 - x;

		/* paint if not empty tile */
		if (tile->index)
		{
			const TLN_Tileset tileset = tilemap->tilesets[tile->tileset];
			const uint16_t tile_index = tileset->tiles[tile->index];

			/* selects suitable palette */
			TLN_Palette palette = tileset->palette;
			if (layer->palette != NULL)
				palette = layer->palette;
			else if (engine->palettes[tile->palette] != NULL)
				palette = engine->palettes[tile->palette];

			/* process rotate & flip flags */
			scan.dx = 1;
			if ((tile->flags & (FLAG_FLIPX + FLAG_FLIPY + FLAG_ROTATE)) != 0)
				process_flip_rotation(tile->flags, &scan);

			/* paint tile scanline */
			uint8_t *srcpixel = &GetTilesetPixel(tileset, tile_index, scan.srcx, scan.srcy);
			uint32_t *dst = dstpixel;
			if (tile->flags & FLAG_PRIORITY)
			{
				dst = engine->priority;
				priority = true;
			}

			int line = GetTilesetLine(tileset, tile_index, scan.srcy);
			bool color_key = *(tileset->color_key + line);
			layer->blitters[1](srcpixel, palette, dst + x, width, scan.dx, 0, layer->blend);
		}

		/* next tile */
		x += width;
		xtile = (xtile + 1) % tilemap->cols;
		scan.srcx = 0;
		column += 1;
	}
	return priority;
}

/* draw scanline of tiled background with scaling */
static bool DrawTiledScanlineScaling(int nlayer, uint32_t* dstpixel, int nscan, int tx1, int tx2)
{
	const Layer *layer = (const Layer*)&engine->layers[nlayer];
	bool priority = false;
	Tilescan scan = { 0 };

	/* target lines */
	int x = tx1;
	const TLN_Tilemap tilemap = layer->tilemap;
	const TLN_Tileset tileset = tilemap->tilesets[0];
	int xpos = (layer->hstart + fix2int(x*layer->dx)) % layer->width;
	int xtile = xpos >> tileset->hshift;

	scan.width = scan.height = scan.stride = tileset->width;
	scan.srcx = xpos & tileset->hmask;

	/* fill whole scanline */
	fix_t fix_x = int2fix(x);
	int column = x % tileset->width;
	while (x < tx2)
	{
		/* column offset: update ypos */
		int ypos = nscan;
		if (layer->column)
			ypos += layer->column[column];

		ypos = layer->vstart + fix2int(ypos*layer->dy);
		if (ypos < 0)
			ypos = layer->height + ypos;
		else
			ypos = ypos % layer->height;

		int ytile = ypos >> tileset->vshift;
		scan.srcy = ypos & tileset->vmask;

		TLN_Tile tile = &tilemap->tiles[ytile*tilemap->cols + xtile];

		/* get effective tile width */
		int tilewidth = tileset->width - scan.srcx;
		fix_t dx = int2fix(tilewidth);
		fix_t fix_tilewidth = tilewidth * layer->xfactor;
		fix_x += fix_tilewidth;
		int x1 = fix2int(fix_x);
		int tilescalewidth = x1 - x;
		if (tilescalewidth)
			dx /= tilescalewidth;
		else
			dx = 0;

		/* right clip */
		if (x1 > tx2)
			x1 = tx2;
		int width = x1 - x;

		/* paint if tile is not empty */
		if (tile->index)
		{
			const TLN_Tileset tileset = tilemap->tilesets[tile->tileset];
			const uint16_t tile_index = tileset->tiles[tile->index];

			/* selects suitable palette */
			TLN_Palette palette = tileset->palette;
			if (layer->palette != NULL)
				palette = layer->palette;
			else if (engine->palettes[tile->palette] != NULL)
				palette = engine->palettes[tile->palette];

			/* process flip flags */
			scan.dx = dx;
			if ((tile->flags & (FLAG_FLIPX + FLAG_FLIPY)) != 0)
				process_flip(tile->flags, &scan);
				//process_flip_rotation(tile->flags, &scan);

			/* paint tile scanline */
			uint8_t* srcpixel = &GetTilesetPixel(tileset, tile_index, scan.srcx, scan.srcy);
			uint32_t *dst = dstpixel;
			if (tile->flags & FLAG_PRIORITY)
			{
				dst = engine->priority;
				priority = true;
			}

			int line = GetTilesetLine(tileset, tile_index, scan.srcy);
			bool color_key = *(tileset->color_key + line);
			layer->blitters[color_key](srcpixel, palette, dst + x, width, scan.dx, 0, layer->blend);
		}

		/* next tile */
		x = x1;
		xtile = (xtile + 1) % tilemap->cols;
		scan.srcx = 0;
		column += 1;
	}
	return priority;
}

/* draw scanline of tiled background with affine transform */
static bool DrawTiledScanlineAffine(int nlayer, uint32_t* dstpixel, int nscan, int tx1, int tx2)
{
	const Layer *layer = (const Layer*)&engine->layers[nlayer];
	bool priority = false;
	Tilescan scan = { 0 };

	const TLN_Tilemap tilemap = layer->tilemap;
	const TLN_Tileset tileset = tilemap->tilesets[0];
	int xpos = layer->hstart;
	int ypos = layer->vstart + nscan;

	Point2D p1, p2;
	Point2DSet(&p1, (math2d_t)xpos + tx1, (math2d_t)ypos);
	Point2DSet(&p2, (math2d_t)xpos + tx2, (math2d_t)ypos);
	Point2DMultiply(&p1, (Matrix3*)&layer->transform);
	Point2DMultiply(&p2, (Matrix3*)&layer->transform);

	int x1 = float2fix(p1.x);
	int y1 = float2fix(p1.y);
	int x2 = float2fix(p2.x);
	int y2 = float2fix(p2.y);

	const int twidth = tx2 - tx1;
	const int dx = (x2 - x1) / twidth;
	const int dy = (y2 - y1) / twidth;

	scan.width = scan.height = scan.stride = tileset->width;
	dstpixel += tx1;

	while (tx1 < tx2)
	{
		xpos = abs((fix2int(x1) + layer->width)) % layer->width;
		ypos = abs((fix2int(y1) + layer->height)) % layer->height;

		int xtile = xpos >> tileset->hshift;
		int ytile = ypos >> tileset->vshift;

		scan.srcx = xpos & tileset->hmask;
		scan.srcy = ypos & tileset->vmask;
		TLN_Tile tile = &tilemap->tiles[ytile*tilemap->cols + xtile];

		/* paint if not empty tile */
		if (tile->index != 0)
		{
			const TLN_Tileset tileset = tilemap->tilesets[tile->tileset];
			const uint16_t tile_index = tileset->tiles[tile->index];

			/* process flip & rotation flags */
			if ((tile->flags & (FLAG_FLIPX + FLAG_FLIPY + FLAG_ROTATE)) != 0)
				process_flip_rotation(tile->flags, &scan);

			/* paint RGB pixel value */
			const TLN_Palette palette = layer->palette != NULL ? layer->palette : tileset->palette;
			*dstpixel = palette->data[GetTilesetPixel(tileset, tile_index, scan.srcx, scan.srcy)];
		}

		/* next pixel */
		tx1 += 1;
		x1 += dx;
		y1 += dy;
		dstpixel += 1;
	}
	return false;
}

/* draw scanline of tiled background with per-pixel mapping */
static bool DrawTiledScanlinePixelMapping(int nlayer, uint32_t* dstpixel, int nscan, int tx1, int tx2)
{
	const Layer *layer = (const Layer*)&engine->layers[nlayer];
	bool priority = false;
	Tilescan scan = { 0 };

	/* target lines */
	int x = tx1;
	dstpixel += x;

	const TLN_Tilemap tilemap = layer->tilemap;
	const TLN_Tileset tileset = tilemap->tilesets[0];
	const int hstart = layer->hstart + layer->width;
	const int vstart = layer->vstart + layer->height;
	TLN_PixelMap* pixel_map = &layer->pixel_map[nscan*engine->framebuffer.width + x];

	scan.width = scan.height = scan.stride = tileset->width;

	while (x < tx2)
	{
		int xpos = abs(hstart + pixel_map->dx) % layer->width;
		int ypos = abs(vstart + pixel_map->dy) % layer->height;

		int xtile = xpos >> tileset->hshift;
		int ytile = ypos >> tileset->vshift;

		scan.srcx = xpos & tileset->hmask;
		scan.srcy = ypos & tileset->vmask;
		TLN_Tile tile = &tilemap->tiles[ytile*tilemap->cols + xtile];

		/* paint if not empty tile */
		if (tile->index)
		{
			const TLN_Tileset tileset = tilemap->tilesets[tile->tileset];
			const uint16_t tile_index = tileset->tiles[tile->index];

			/* process flip & rotation flags */
			if ((tile->flags & (FLAG_FLIPX + FLAG_FLIPY + FLAG_ROTATE)) != 0)
				process_flip_rotation(tile->flags, &scan);

			/* paint RGB pixel value */
			const TLN_Palette palette = layer->palette != NULL ? layer->palette : tileset->palette;
			*dstpixel = palette->data[GetTilesetPixel(tileset, tile_index, scan.srcx, scan.srcy)];
		}

		/* next pixel */
		x += 1;
		dstpixel += 1;
		pixel_map += 1;
	}
	return false;
}

/* draw sprite scanline */
static bool DrawSpriteScanline(int nsprite, uint32_t* dstscan, int nscan, int tx1, int tx2)
{
	Sprite* sprite = (Sprite*)&engine->sprites[nsprite];

	Tilescan scan = { 0 };
	scan.srcx = sprite->srcrect.x1;
	scan.srcy = sprite->srcrect.y1 + (nscan - sprite->dstrect.y1);
	scan.width = sprite->info->w;
	scan.height = sprite->info->h;
	scan.stride = sprite->pitch;

	/* disable rotation for non-squared sprites */
	uint16_t flags = sprite->flags;
	if ((flags & FLAG_ROTATE) && sprite->info->w != sprite->info->h)
		flags &= ~FLAG_ROTATE;

	const int w = sprite->dstrect.x2 - sprite->dstrect.x1;

	/* process rotate & flip flags */
	scan.dx = 1;
	if ((flags & (FLAG_FLIPX + FLAG_FLIPY + FLAG_ROTATE)) != 0)
		process_flip_rotation(flags, &scan);

	/* blit scanline */
	uint8_t* srcpixel = sprite->pixels + (scan.srcy*sprite->pitch) + scan.srcx;
	uint32_t *dstpixel = dstscan + sprite->dstrect.x1;
	sprite->blitter(srcpixel, sprite->palette, dstpixel, w, scan.dx, 0, sprite->blend);

	if (sprite->do_collision)
	{
		uint16_t* dstpixel = engine->collision + sprite->dstrect.x1;
		DrawSpriteCollision(nsprite, srcpixel, dstpixel, w, scan.dx);
	}
	return true;
}

/* draw sprite scanline with scaling */
static bool DrawScalingSpriteScanline(int nsprite, uint32_t* dstscan, int nscan, int tx1, int tx2)
{
	Sprite* sprite = (Sprite*)&engine->sprites[nsprite];

	int srcx = sprite->srcrect.x1;
	int srcy = sprite->srcrect.y1 + (nscan - sprite->dstrect.y1)*sprite->dy;
	int dstw = sprite->dstrect.x2 - sprite->dstrect.x1;

	/* H/V flip */
	int dstx, dx;
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

	/* blit scanline */
	uint8_t* srcpixel = sprite->pixels + (fix2int(srcy)*sprite->pitch);
	uint32_t* dstpixel = dstscan + sprite->dstrect.x1;
	sprite->blitter(srcpixel, sprite->palette, dstpixel, dstw, dx, srcx, sprite->blend);

	if (sprite->do_collision)
	{
		uint16_t* dstpixel = engine->collision + sprite->dstrect.x1;
		DrawSpriteCollisionScaling(nsprite, srcpixel, dstpixel, dstw, dx, srcx);
	}
	return true;
}

/* updates per-pixel sprite collision buffer */
static void DrawSpriteCollision(int nsprite, uint8_t *srcpixel, uint16_t *dstpixel, int width, int dx)
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
		dstpixel += 1;
		width -= 1;
	}
}

/* updates per-pixel sprite collision buffer for scaled sprite */
static void DrawSpriteCollisionScaling(int nsprite, uint8_t *srcpixel, uint16_t *dstpixel, int width, int dx, int srcx)
{
	while (width)
	{
		uint32_t src = *(srcpixel + srcx / (1 << FIXED_BITS));
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
		dstpixel += 1;
		width -= 1;
	}
}

/* draws regular bitmap scanline for bitmap-based layer */
static bool DrawBitmapScanline(int nlayer, uint32_t* dstpixel, int nscan, int tx1, int tx2)
{
	const Layer *layer = (const Layer*)&engine->layers[nlayer];

	/* target lines */
	int x = tx1;
	dstpixel += x;
	int ypos = (layer->vstart + nscan) % layer->height;
	int xpos = (layer->hstart + x) % layer->width;

	/* draws bitmap scanline */
	TLN_Bitmap bitmap = layer->bitmap;
	TLN_Palette palette = layer->palette != NULL ? layer->palette : bitmap->palette;
	while (x < tx2)
	{
		/* get effective width */
		int width = layer->width - xpos;
		int x1 = x + width;
		if (x1 > tx2)
			x1 = tx2;
		width = x1 - x;

		uint8_t* srcpixel = (uint8_t*)get_bitmap_ptr(bitmap, xpos, ypos);
		layer->blitters[1](srcpixel, palette, dstpixel, width, 1, 0, layer->blend);
		x += width;
		dstpixel += width;
		xpos = 0;
	}
	return false;
}

/* draws regular bitmap scanline for bitmap-based layer with scaling */
static bool DrawBitmapScanlineScaling(int nlayer, uint32_t* dstpixel, int nscan, int tx1, int tx2)
{
	const Layer *layer = (const Layer*)&engine->layers[nlayer];

	/* target line */
	int x = tx1;
	dstpixel += x;
	int xpos = (layer->hstart + fix2int(x*layer->dx)) % layer->width;

	/* fill whole scanline */
	const TLN_Bitmap bitmap = layer->bitmap;
	const TLN_Palette palette = layer->palette != NULL ? layer->palette : bitmap->palette;
	fix_t fix_x = int2fix(x);
	while (x < tx2)
	{
		int ypos = layer->vstart + fix2int(nscan*layer->dy);
		if (ypos < 0)
			ypos = layer->height + ypos;
		else
			ypos = ypos % layer->height;

		/* get effective width */
		int width = layer->width - xpos;
		fix_t dx = int2fix(width);
		fix_t fix_tilewidth = width * layer->xfactor;
		fix_x += fix_tilewidth;
		int x1 = fix2int(fix_x);
		int tilescalewidth = x1 - x;
		if (tilescalewidth)
			dx /= tilescalewidth;
		else
			dx = 0;

		/* right clipping */
		if (x1 > tx2)
			x1 = tx2;
		width = x1 - x;

		/* draw bitmap scanline */
		uint8_t* srcpixel = (uint8_t*)get_bitmap_ptr(bitmap, xpos, ypos);
		layer->blitters[1](srcpixel, palette, dstpixel, width, dx, 0, layer->blend);

		/* next */
		dstpixel += width;
		x = x1;
		xpos = 0;
	}
	return false;
}

/* draws regular bitmap scanline for bitmap-based layer with affine transform */
static bool DrawBitmapScanlineAffine(int nlayer, uint32_t* dstpixel, int nscan, int tx1, int tx2)
{
	const Layer *layer = (const Layer*)&engine->layers[nlayer];
	bool priority = false;

	int xpos = layer->hstart;
	int ypos = layer->vstart + nscan;

	Point2D p1, p2;
	Point2DSet(&p1, (math2d_t)xpos + tx1, (math2d_t)ypos);
	Point2DSet(&p2, (math2d_t)xpos + tx2, (math2d_t)ypos);
	Point2DMultiply(&p1, (Matrix3*)&layer->transform);
	Point2DMultiply(&p2, (Matrix3*)&layer->transform);

	int x1 = float2fix(p1.x);
	int y1 = float2fix(p1.y);
	int x2 = float2fix(p2.x);
	int y2 = float2fix(p2.y);

	const int twidth = tx2 - tx1;
	const int dx = (x2 - x1) / twidth;
	const int dy = (y2 - y1) / twidth;

	const TLN_Bitmap bitmap = layer->bitmap;
	const TLN_Palette palette = layer->palette != NULL ? layer->palette : bitmap->palette;
	while (tx1 < tx2)
	{
		xpos = abs((fix2int(x1) + layer->width)) % layer->width;
		ypos = abs((fix2int(y1) + layer->height)) % layer->height;
		*dstpixel = palette->data[*get_bitmap_ptr(bitmap, xpos, ypos)];

		/* next pixel */
		tx1 += 1;
		x1 += dx;
		y1 += dy;
		dstpixel += 1;
	}
	return priority;
}

/* draws regular bitmap scanline for bitmap-based layer with per-pixel mapping */
static bool DrawBitmapScanlinePixelMapping(int nlayer, uint32_t* dstpixel, int nscan, int tx1, int tx2)
{
	const Layer *layer = (const Layer*)&engine->layers[nlayer];
	bool priority = false;

	/* target lines */
	int x = tx1;
	dstpixel += x;

	const int hstart = layer->hstart + layer->width;
	const int vstart = layer->vstart + layer->height;
	const TLN_Bitmap bitmap = layer->bitmap;
	const TLN_Palette palette = layer->palette != NULL ? layer->palette : bitmap->palette;
	const TLN_PixelMap* pixel_map = &layer->pixel_map[nscan*engine->framebuffer.width + x];
	while (x < tx2)
	{
		int xpos = abs(hstart + pixel_map->dx) % layer->width;
		int ypos = abs(vstart + pixel_map->dy) % layer->height;
		*dstpixel = layer->palette->data[*get_bitmap_ptr(bitmap, xpos, ypos)];

		/* next pixel */
		x += 1;
		dstpixel += 1;
		pixel_map += 1;
	}
	return priority;
}

/* draws regular object layer scanline */
static bool DrawObjectScanline(int nlayer, uint32_t* dstpixel, int nscan, int tx1, int tx2)
{
	const Layer *layer = (const Layer*)&engine->layers[nlayer];
	struct _Object* object = layer->objects->list;
	struct _Object tmpobject = { 0 };
	
	int x1 = layer->hstart + tx1;
	int x2 = layer->hstart + tx2;
	int y = layer->vstart + nscan;
	uint32_t* dstscan = GetFramebufferLine(nscan);
	bool priority = false;

	while (object != NULL)
	{
		/* swap width & height for rotated objects */
		memcpy(&tmpobject, object, sizeof(struct _Object));
		if (tmpobject.flags & FLAG_ROTATE)
		{
			tmpobject.width = object->height;
			tmpobject.height = object->width;
		}

		if (IsObjectInLine(&tmpobject, x1, x2, y) && tmpobject.visible && tmpobject.bitmap != NULL)
		{
			Tilescan scan = { 0 };
			scan.srcx = 0;
			scan.srcy = y - tmpobject.y;

			int dstx1 = tmpobject.x - x1;
			int dstx2 = dstx1 + tmpobject.width;
			if (dstx1 < tx1)
			{
				int w = tx1 - dstx1;
				scan.srcx = w;
				dstx1 = 0;
			}
			if (dstx2 > tx2)
				dstx2 = tx2;
			int w = dstx2 - dstx1;

			TLN_Bitmap bitmap = tmpobject.bitmap;
			scan.width = bitmap->width;
			scan.height = bitmap->height;
			scan.stride = bitmap->pitch;

			/* process rotate & flip flags */
			scan.dx = 1;
			if ((tmpobject.flags & (FLAG_FLIPX + FLAG_FLIPY + FLAG_ROTATE)) != 0)
				process_flip_rotation(tmpobject.flags, &scan);

			/* paint tile scanline */
			uint8_t* srcpixel = get_bitmap_ptr(bitmap, scan.srcx, scan.srcy);
			uint32_t *target = dstscan;
			if (tmpobject.flags & FLAG_PRIORITY)
			{
				target = engine->priority;
				priority = true;
			}
			uint32_t* dstpixel = target + dstx1;
			layer->blitters[1](srcpixel, bitmap->palette, dstpixel, w, scan.dx, 0, layer->blend);
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
static const ScanDrawPtr draw_delegates[MAX_DRAW_TYPE][MAX_DRAW_MODE] =
{
	{ DrawSpriteScanline,	DrawScalingSpriteScanline,	NULL,						NULL},
	{ DrawTiledScanline,	DrawTiledScanlineScaling,	DrawTiledScanlineAffine,	DrawTiledScanlinePixelMapping },
	{ DrawBitmapScanline,	DrawBitmapScanlineScaling,	DrawBitmapScanlineAffine,	DrawBitmapScanlinePixelMapping },
	{ DrawObjectScanline,	NULL,						NULL,						NULL },
};

/* returns suitable draw procedure based on layer configuration */
ScanDrawPtr GetLayerDraw(Layer* layer)
{
	if (layer->tilemap != NULL)
		return draw_delegates[DRAW_TILED_LAYER][layer->mode];
	else if (layer->bitmap != NULL)
		return draw_delegates[DRAW_BITMAP_LAYER][layer->mode];
	else if (layer->objects != NULL)
		return draw_delegates[DRAW_OBJECT_LAYER][layer->mode];
	else
		return NULL;
}

/* returns suitable draw procedure based on sprite configuration */
ScanDrawPtr GetSpriteDraw(draw_t mode)
{
	return draw_delegates[DRAW_SPRITE][mode];
}
