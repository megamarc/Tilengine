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
#include "Tilengine.h"
#include "Object.h"
#include "Palette.h"
#include "Bitmap.h"

TLN_Bitmap TLN_CreateBitmap (int width, int height, int bpp)
{
	TLN_Bitmap bitmap;
	int pitch, size;

	pitch = (((width * bpp)>>3) + 3) & ~0x03;
	size = sizeof(struct Bitmap) + (pitch * height);
	bitmap = (TLN_Bitmap)CreateBaseObject (OT_BITMAP, size);
	if (bitmap)
	{
		bitmap->width = width;
		bitmap->height = height;
		bitmap->bpp = bpp;
		bitmap->pitch = pitch;
		TLN_SetLastError (TLN_ERR_OK);
		return bitmap;
	}
	else
		return NULL;
}

TLN_Bitmap TLN_CloneBitmap (TLN_Bitmap src)
{
	TLN_Bitmap bitmap;

	if (!CheckBaseObject (src, OT_BITMAP))
		return NULL;

	bitmap = (TLN_Bitmap)CloneBaseObject (src);
	if (bitmap)
	{
		TLN_SetLastError (TLN_ERR_OK);
		return bitmap;
	}
	else
		return NULL;
}

bool TLN_DeleteBitmap (TLN_Bitmap bitmap)
{
	if (CheckBaseObject (bitmap, OT_BITMAP))
	{
		if (ObjectOwner (bitmap) && bitmap->palette)
			TLN_DeletePalette (bitmap->palette);
		DeleteBaseObject (bitmap);
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return false;
}

uint8_t* TLN_GetBitmapPtr (TLN_Bitmap bitmap, int x, int y)
{
	uint8_t *srcptr;

	if (!CheckBaseObject (bitmap, OT_BITMAP))
		return NULL;

	if (x>=bitmap->width || y>=bitmap->height)
	{
		TLN_SetLastError (TLN_ERR_WRONG_SIZE);
		return NULL;
	}
	
	TLN_SetLastError (TLN_ERR_OK);
	srcptr = get_bitmap_ptr(bitmap, x, y);
	return srcptr;
}

TLN_Palette TLN_GetBitmapPalette (TLN_Bitmap bitmap)
{
	if (CheckBaseObject (bitmap, OT_BITMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return bitmap->palette;
	}
	else
		return NULL;
}

bool TLN_SetBitmapPalette (TLN_Bitmap bitmap, TLN_Palette palette)
{
	if (!CheckBaseObject (bitmap, OT_BITMAP) || !CheckBaseObject (palette, OT_PALETTE))
		return false;

	bitmap->palette = palette;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

int TLN_GetBitmapWidth (TLN_Bitmap bitmap)
{
	if (CheckBaseObject (bitmap, OT_BITMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return bitmap->width;
	}
	else
		return 0;
}

int TLN_GetBitmapHeight (TLN_Bitmap bitmap)
{
	if (CheckBaseObject (bitmap, OT_BITMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return bitmap->height;
	}
	else
		return 0;
}

int TLN_GetBitmapDepth (TLN_Bitmap bitmap)
{
	if (CheckBaseObject (bitmap, OT_BITMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return bitmap->bpp;
	}
	else
		return 0;
}

int TLN_GetBitmapPitch (TLN_Bitmap bitmap)
{
	if (CheckBaseObject (bitmap, OT_BITMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return bitmap->pitch;
	}
	else
		return 0;
}
