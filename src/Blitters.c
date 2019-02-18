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
#define BLIT_BPP		3

/* 8 to 8 BPP blitters ----------------------------------------------------- */

static void blitFast_8_8 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint8_t* dstpixel = (uint8_t*)dstptr;
	while (width)
	{
		*dstpixel++ = *srcpixel;
		srcpixel += dx;
		width--;
	}
}

static void blitFastScaling_8_8 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint8_t* dstpixel = (uint8_t*)dstptr;
	while (width)
	{
		uint32_t src = *(srcpixel + offset/(1 << FIXED_BITS));
		*dstpixel++ = src;
		offset += dx;
		width--;
	}
}

static void blitKey_8_8 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint8_t* dstpixel = (uint8_t*)dstptr;
	while (width)
	{
		if (*srcpixel)
			*dstpixel = *srcpixel;
		srcpixel += dx;
		dstpixel++;
		width--;
	}
}

static void blitKeyScaling_8_8 (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int dx, int offset, uint8_t* blend)
{
	uint8_t* dstpixel = (uint8_t*)dstptr;
	while (width)
	{
		uint32_t src = *(srcpixel + offset/(1 << FIXED_BITS));
		if (src)
			*dstpixel = src;

		offset += dx;
		dstpixel++;
		width--;
	}
}

/* 8 to 32 BPP blitters ----------------------------------------------------- */

static void blitColor_8_32 (void* dstptr, uint32_t color, int width)
{
	uint32_t* dstpixel = (uint32_t*)dstptr;
	while (width)
	{
		*dstpixel++ = color;
		width--;
	}
}

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

static const ScanBlitPtr blitters[]=
{
	blitFast_8_8,
	NULL,
	blitFastScaling_8_8,
	NULL,
	blitKey_8_8,
	NULL,
	blitKeyScaling_8_8,
	NULL,

	blitFast_8_32,
	blitFastBlend_8_32,
	blitFastScaling_8_32,
	blitFastBlendScaling_8_32,
	blitKey_8_32,
	blitKeyBlend_8_32,
	blitKeyScaling_8_32,
	blitKeyBlendScaling_8_32
};

ScanBlitPtr GetBlitter (int bpp, bool key, bool scaling, bool blend)
{
	int index;

	if (bpp == 32)
		bpp = 1;
	else
		bpp = 0;

	index = (bpp << BLIT_BPP) + (key << BLIT_KEY) + (scaling << BLIT_SCALING) + (blend << BLIT_BLEND);
	return blitters[index];
}

void BlitColor (void* dstptr, uint32_t color, int width)
{
	blitColor_8_32 (dstptr, color, width);
}

void BlitMosaicSolid (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int size)
{
	uint32_t* dstpixel = (uint32_t*)dstptr;
	uint32_t* color = (uint32_t*)palette->data;
	while (width)
	{
		if (size > width)
			size = width;

		if (*srcpixel)
		{
			const uint32_t value = color[*srcpixel];
			int c;
			for (c=0; c<size; c++)
				*dstpixel++ = value;
		}
		else
			dstpixel += size;
		srcpixel += size;
		width -= size;
	}
}

void BlitMosaicBlend (uint8_t *srcpixel, TLN_Palette palette, void* dstptr, int width, int size, uint8_t* blend)
{
	uint8_t* dstpixel = (uint8_t*)dstptr;
	uint32_t* color = (uint32_t*)palette->data;
	while (width)
	{
		if (size > width)
			size = width;

		if (*srcpixel)
		{
			const uint8_t* value = (uint8_t*)&color[*srcpixel];
			int c;
			for (c=0; c<size; c++)
			{
				dstpixel[0] = blendfunc(blend, value[0], dstpixel[0]);
				dstpixel[1] = blendfunc(blend, value[1], dstpixel[1]);
				dstpixel[2] = blendfunc(blend, value[2], dstpixel[2]);
				dstpixel += sizeof(uint32_t);
			}
		}
		else
			dstpixel += (sizeof(uint32_t)*size);
		srcpixel += size;
		width -= size;
	}
}
