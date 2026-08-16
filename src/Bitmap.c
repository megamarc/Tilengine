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

/*!
 * \brief
 * Creates a memory bitmap
 * 
 * \param width
 * Width in pixels
 * 
 * \param height
 * Height in pixels
 * 
 * \param bpp
 * Bits per pixel
 * 
 * \returns
 * Reference to the created bitmap, or NULL if error
 * 
 * \see
 * TLN_SetBGBitmap()
 */
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

/*!
 * \brief
 * Creates a copy of a bitmap
 * 
 * \param src
 * Reference to the original bitmap
 * 
 * \returns
 * Reference to the created bitmap, or NULL if error
 * 
 * \see
 * TLN_SetBGBitmap()
 */
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

/*!
 * \brief
 * Deletes bitmap and frees resources
 * 
 * \param bitmap
 * Reference to bitmap to delete
 * 
 * \see
 * TLN_CreateBitmap89, TLN_CloneBitmap()
 */
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

/*!
 * \brief
 * Gets memory access for direct pixel manipulation
 * 
 * \param bitmap
 * Reference to bitmap
 * 
 * \param x
 * Starting x position [0, width - 1]
 * 
 * \param y
 * Starting y position [0, height - 1]
 * 
 * \returns
 * Pointer to pixel data starting at x,y
 * 
 * \remarks
 * Care must be taken in manipulating memory directly as it can crash the application
 */
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

/*!
 * \brief
 * Gets the associated palete of a bitmap
 * 
 * \param bitmap
 * Reference to bitmap
 * 
 * \returns
 * Reference to the bitmap palette
 *
 * \see
 * TLN_SetBitmapPalette()
 */
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

/*!
 * \brief
 * Assigns a new palette to the bitmap
 * 
 * \param bitmap
 * Reference to the bitmap
 * 
 * \param palette
 * Reference to the palette to assign
 * 
 * \see
 * TLN_GetBitmapPalette()
 */
bool TLN_SetBitmapPalette (TLN_Bitmap bitmap, TLN_Palette palette)
{
	if (!CheckBaseObject (bitmap, OT_BITMAP) || !CheckBaseObject (palette, OT_PALETTE))
		return false;

	bitmap->palette = palette;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Returns the width in pixels
 * 
 * \param bitmap
 * Reference to the bitmap
 */
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

/*!
 * \brief
 * Returns the height in pixels
 * 
 * \param bitmap
 * Reference to the bitmap
 */
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

/*!
 * \brief
 * Returns the bits per pixel
 * 
 * \param bitmap
 * Reference to the bitmap
 */
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

/*!
 * \brief
 * Returns the number of bytes per scaline (also known a stride)
 * 
 * \param bitmap
 * Reference to the bitmap
 */
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
