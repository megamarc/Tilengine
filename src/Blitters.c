/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include "Tilengine.h"
#include "Palette.h"
#include "Blitters.h"
#include "Tables.h"
#include "Engine.h"

/* indexes for blitter array table */
#define BLIT_BLEND		0
#define BLIT_SCALING	1
#define BLIT_KEY		2

/* 8 to 32 BPP blitters ----------------------------------------------------- */

/* paints scanline without checking color key (always solid) */
static void blitFast_8_32 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint32_t* dstpixel = (uint32_t*)dstptr;
	uint32_t* color = (uint32_t*)palette->data;
	while (width)
	{
		*dstpixel++ = color[*srcpixel];
		srcpixel += dx;
		width--;
	}
}

/* paints scanline without checking color key (always solid) with blending */
static void blitFastBlend_8_32 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint8_t *src, *dst;
	uint32_t* color = (uint32_t*)palette->data;
	dst = (uint8_t*)dstptr;
	while (width)
	{
		src = (uint8_t*)&color[*srcpixel];
		dst[0] = blendfunc(blend, src[0], dst[0]);
		dst[1] = blendfunc(blend, src[1], dst[1]);
		dst[2] = blendfunc(blend, src[2], dst[2]);
		srcpixel += dx;
		dst += sizeof(uint32_t);
		width--;
	}
}

/* paints scanline without checking color key (always solid) with scaling */
static void blitFastScaling_8_32 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint32_t* dstpixel = (uint32_t*)dstptr;
	uint32_t* color = (uint32_t*)palette->data;
	while (width)
	{
		uint32_t src = *(srcpixel + offset/(1 << FIXED_BITS));
		*dstpixel++ = color[src];
		offset += dx;
		width--;
	}
}

/* paints scanline without checking color key (always solid) with scaling and blending */
static void blitFastBlendScaling_8_32 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint8_t *src, *dst;
	uint32_t* color = (uint32_t*)palette->data;
	dst = (uint8_t*)dstptr;
	while (width)
	{
		uint32_t item = *(srcpixel + offset/(1 << FIXED_BITS));
		src = (uint8_t*)&color[item];
		dst[0] = blendfunc(blend, src[0], dst[0]);
		dst[1] = blendfunc(blend, src[1], dst[1]);
		dst[2] = blendfunc(blend, src[2], dst[2]);
		offset += dx;
		dst += sizeof(uint32_t);
		width--;
	}
}

/* paints scanline skipping empty pixels */
static void blitKey_8_32 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint32_t* dstpixel = (uint32_t*)dstptr;
	uint32_t* color = (uint32_t*)palette->data;
	while (width)
	{
		if (*srcpixel)
			*dstpixel = color[*srcpixel];
		srcpixel += dx;
		dstpixel++;
		width--;
	}
}

/* paints scanline skipping empty pixels with blending */
static void blitKeyBlend_8_32 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint8_t *src, *dst;
	uint32_t* color = (uint32_t*)palette->data;
	dst = (uint8_t*)dstptr;
	while (width)
	{
		if (*srcpixel)
		{
			src = (uint8_t*)&color[*srcpixel];
			dst[0] = blendfunc(blend, src[0], dst[0]);
			dst[1] = blendfunc(blend, src[1], dst[1]);
			dst[2] = blendfunc(blend, src[2], dst[2]);
		}
		srcpixel += dx;
		dst += sizeof(uint32_t);
		width--;
	}
}

/* paints scanline skipping empty pixels with scaling */
static void blitKeyScaling_8_32 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint32_t* dstpixel = (uint32_t*)dstptr;
	uint32_t* color = (uint32_t*)palette->data;
	while (width)
	{
		uint32_t src = *(srcpixel + offset/(1 << FIXED_BITS));
		if (src)
			*dstpixel = color[src];

		offset += dx;
		dstpixel++;
		width--;
	}
}

/* paints scanline skipping empty pixels with scaling and blending */
static void blitKeyBlendScaling_8_32 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint8_t *src, *dst;
	uint32_t* color = (uint32_t*)palette->data;
	dst = (uint8_t*)dstptr;
	while (width)
	{
		uint32_t item = *(srcpixel + offset/(1 << FIXED_BITS));
		if (item)
		{
			src = (uint8_t*)&color[item];
			dst[0] = blendfunc(blend, src[0], dst[0]);
			dst[1] = blendfunc(blend, src[1], dst[1]);
			dst[2] = blendfunc(blend, src[2], dst[2]);
		}
		offset += dx;
		dst += sizeof(uint32_t);
		width--;
	}
}

/* blitter table selector */
static const ScanBlitPtr blitters[]=
{
	blitFast_8_32,
	blitFastBlend_8_32,
	blitFastScaling_8_32,
	blitFastBlendScaling_8_32,
	blitKey_8_32,
	blitKeyBlend_8_32,
	blitKeyScaling_8_32,
	blitKeyBlendScaling_8_32
};

/* returns suitable blitter for specified conditions */
ScanBlitPtr SelectBlitter (bool key, bool scaling, bool blend)
{
	int index = (key << BLIT_KEY) + (scaling << BLIT_SCALING) + (blend << BLIT_BLEND);
	return blitters[index];
}

/* paints constant color */
void BlitColor(void* dstptr, uint32_t color, int width)
{
	uint32_t* dstpixel = (uint32_t*)dstptr;
	while (width)
	{
		*dstpixel++ = color;
		width--;
	}
}

/* perfoms direct 32 -> 32 bpp blit with opcional blend */
void Blit32_32(uint32_t *src, uint32_t* dst, int width, uint8_t* blend)
{
	Color* srcpixel = (Color*)src;
	Color* dstpixel = (Color*)dst;

	/* blending */
	if (blend != NULL)
	{
		while (width > 0)
		{
			if (srcpixel->a != 0)
			{
				dstpixel->r = blendfunc(blend, srcpixel->r, dstpixel->r);
				dstpixel->g = blendfunc(blend, srcpixel->g, dstpixel->g);
				dstpixel->b = blendfunc(blend, srcpixel->b, dstpixel->b);
			}
			srcpixel += 1;
			dstpixel += 1;
			width -= 1;
		}
	}

	/* regular */
	else
	{
		while (width > 0)
		{
			if (srcpixel->a != 0)
				dstpixel->value = srcpixel->value;
			srcpixel += 1;
			dstpixel += 1;
			width -= 1;
		}
	}
}

/* performs mosaic effect with opcional blend */
void BlitMosaic(uint32_t *src, uint32_t* dst, int width, int size, uint8_t* blend)
{
	Color* srcpixel = (Color*)src;
	Color* dstpixel = (Color*)dst;
	
	/* blending */
	if (blend != NULL)
	{
		while (width > 0)
		{
			if (size > width)
				size = width;

			if (srcpixel->a != 0)
			{
				dstpixel->r = blendfunc(blend, srcpixel->r, dstpixel->r);
				dstpixel->g = blendfunc(blend, srcpixel->g, dstpixel->g);
				dstpixel->b = blendfunc(blend, srcpixel->b, dstpixel->b);
			}
			srcpixel += 1;
			dstpixel += 1;
			width -= size;
		}
	}

	/* regular */
	else
	{
		while (width > 0)
		{
			if (size > width)
				size = width;

			if (srcpixel->a != 0)
				dstpixel->value = srcpixel->value;

			srcpixel += 1;
			dstpixel += 1;
			width -= size;
		}
	}
}
