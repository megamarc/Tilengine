/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <stdlib.h>
#include <string.h>
#include "Tilengine.h"
#include "LoadFile.h"
#include "png.h"
#include "DIB.h"

static TLN_Bitmap LoadPNG (const char* filename);
static TLN_Bitmap LoadBMP (const char* filename);

/*!
 * \brief
 * Load image file (8-bit BMP or PNG)
 * 
 * \param filename
 * File name with the image
 * 
 * \returns
 * Handler to the loaded image or NULL if error
 * 
 * \see
 * TLN_DeleteBitmap()
 */
TLN_Bitmap TLN_LoadBitmap (const char* filename)
{
	TLN_Bitmap bitmap;

	if (!CheckFile (filename))
	{
		TLN_SetLastError (TLN_ERR_FILE_NOT_FOUND);
		return NULL;
	}

	/* try png, else bmp*/
	bitmap = LoadPNG (filename);
	if (bitmap == NULL)
		bitmap = LoadBMP (filename);

	/* bitmap loaded */
	if (bitmap)
	{
		/* accept only 8 bpp */
		int bpp = TLN_GetBitmapDepth (bitmap);
		if (bpp == 8)
			TLN_SetLastError (TLN_ERR_OK);
		else
		{
			TLN_DeleteBitmap (bitmap);
			bitmap = NULL;
		}
	}
	
	if (!bitmap)
		TLN_SetLastError (TLN_ERR_WRONG_FORMAT);

	return bitmap;
}

/* Loads PNG using libpng 1.2 */
static TLN_Bitmap LoadPNG (const char* filename)
{
	TLN_Bitmap bitmap = NULL;
	FILE* fp;
	png_struct* png;
	png_info* info;
	int width, height;
	png_byte color_type;
	png_byte bit_depth;
	png_bytep *row_pointers;
	png_byte header[8];
	int channels;
	int y;

	fp = FileOpen (filename);
	if (!fp)
		return NULL;

	fread (header, 8, 1, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		FileClose (fp);
		return NULL;
	}

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);
	
	setjmp(png_jmpbuf(png));
	png_init_io(png, fp);
	png_set_sig_bytes(png, 8);
	png_read_info(png, info);

	width      = png_get_image_width(png, info);
	height     = png_get_image_height(png, info);
	color_type = png_get_color_type(png, info);
	bit_depth  = png_get_bit_depth(png, info);
	channels   = png_get_channels(png, info);
    
    png_read_update_info(png, info);

	/* adjust actual bit depth */
	bit_depth *= channels;

	setjmp(png_jmpbuf(png));
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	bitmap = TLN_CreateBitmap (width, height, bit_depth);
	for (y=0; y<height; y++)
		row_pointers[y] = (png_byte*) TLN_GetBitmapPtr (bitmap, 0,y);
	png_read_image(png, row_pointers);
	free (row_pointers);

	/* 8 bpp indexed palette */
	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_colorp png_palette = NULL;
		int palette_entries = 0;
		TLN_Palette palette;
		int c;

		png_get_PLTE(png,info, &png_palette, &palette_entries);

		palette = TLN_CreatePalette (palette_entries);
		for (c=0; c<palette_entries; c++)
		{
			TLN_SetPaletteColor (palette, c, 
				png_palette[c].red, png_palette[c].green, png_palette[c].blue);
		}
		TLN_SetBitmapPalette (bitmap, palette);
	}

	FileClose(fp);
	png_destroy_read_struct (&png, &info, NULL);
	return bitmap;
}

/* loads BMP */
static TLN_Bitmap LoadBMP (const char* filename)
{
	BITMAPFILEHEADER bfh;
	BITMAPV5HEADER bv5;
	uint32_t StructSize;
	FILE* pf;
	TLN_Bitmap bitmap = NULL;
	unsigned int c;
	int pitch;

	/* open file */
	pf = FileOpen (filename);
	if (!pf)
		return NULL;

	/* read BMP header */
	fread (&bfh, sizeof(bfh), 1, pf);
	if (bfh.Type != 0x4D42)
	{
		FileClose (pf);
		return NULL;
	}

	/* load info structure */
	memset (&bv5, 0, sizeof(bv5));
	fread (&StructSize, 4, 1, pf);
	fseek (pf, sizeof(bfh), SEEK_SET);
	fread (&bv5, StructSize, 1, pf);

	/* create */
	bitmap = TLN_CreateBitmap (bv5.bV5Width, bv5.bV5Height, bv5.bV5BitCount);
	if (!bitmap)
	{
		FileClose (pf);
		return NULL;
	}

	/* load scanlines */
	pitch = TLN_GetBitmapPitch (bitmap);
	fseek (pf, bfh.OffsetData, SEEK_SET);
	for (c=0; c<bv5.bV5Height; c++)
	{
		uint8_t* line = TLN_GetBitmapPtr (bitmap, 0, bv5.bV5Height - c - 1);
		fread (line, pitch, 1, pf);
	}

	/* load palette */
	if (bv5.bV5BitCount == 8)
	{
		TLN_Palette palette;

		/* HACK: some editors don't set the bV5ClrUsed field, compute from size */
		if (bv5.bV5ClrUsed == 0)
			bv5.bV5ClrUsed = (bfh.OffsetData - sizeof(bfh) - bv5.bV5Size) / sizeof(RGBQUAD);

		fseek (pf, sizeof(BITMAPFILEHEADER) + bv5.bV5Size, SEEK_SET);
		palette = TLN_CreatePalette (bv5.bV5ClrUsed);
		for (c=0; c<(int)bv5.bV5ClrUsed; c++)
		{
			RGBQUAD color;
			fread (&color, sizeof(RGBQUAD), 1, pf);
			TLN_SetPaletteColor (palette, c, color.r, color.g, color.b);
		}
		TLN_SetBitmapPalette (bitmap, palette);
	}

	FileClose (pf);
	return bitmap;
}
