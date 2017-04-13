/*
Tilengine - 2D Graphics library with raster effects
Copyright (c) 2015-2017 Marc Palacios Domenech (megamarc@hotmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*!
 ******************************************************************************
 *
 * \file
 * \brief Bitmap loading support
 * \author Megamarc
 * \date 20 sep 2015
 *
 * Public Tilengine source code
 * http://www.tilengine.org
 *
 ******************************************************************************
 */

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
	char header[8];
	int y;

	fp = FileOpen (filename);
	if (!fp)
		return NULL;

	fread (header, 8, 1, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		fclose (fp);
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
    
    png_read_update_info(png, info);

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

	fclose(fp);
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
		fclose (pf);
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
		fclose (pf);
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

	fclose (pf);
	return bitmap;
}
