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

#include <malloc.h>
#include <string.h>
#include "Tilengine.h"
#include "LoadFile.h"
#include "png.h"
#include "DIB.h"

static TLN_Bitmap LoadPNG (char *filename);
static TLN_Bitmap LoadBMP (char *filename);

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
TLN_Bitmap TLN_LoadBitmap (char *filename)
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

/* Loads PNG using libpng */
static TLN_Bitmap LoadPNG (char *filename)
{
	png_image image;
	TLN_Bitmap bitmap = NULL;

	/* Only the image structure version number needs to be set. */
	memset (&image, 0, sizeof image);
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file (&image, filename))
	{
		int src_line_size = image.width * PNG_IMAGE_PIXEL_SIZE(image.format);
		int lut_size = PNG_IMAGE_COLORMAP_SIZE(image);
		BYTE* data = malloc (src_line_size * image.height);
		BYTE* lut = malloc (lut_size);
		
		bitmap = TLN_CreateBitmap (image.width, image.height, PNG_IMAGE_PIXEL_SIZE(image.format)<<3);

		/* Change this to try different formats!  If you set a colormap format
		* then you must also supply a colormap below.
		*/
		image.format = PNG_FORMAT_RGB_COLORMAP;
		if (png_image_finish_read (&image, NULL, data, 0, lut))
		{
			BYTE *src, *dst;
			unsigned int c;
			
			png_image_free (&image);

			/* copy scanlines */
			src = data;
			for (c=0; c<image.height; c++)
			{
				dst = TLN_GetBitmapPtr (bitmap, 0, c);
				memcpy (dst, src, src_line_size);
				src += src_line_size;
			}

			/* get palette */
			{
				BYTE *src = lut;
				png_uint_32 c;
				TLN_Palette palette;
				palette = TLN_CreatePalette (image.colormap_entries);
				for (c=0; c<image.colormap_entries; c++)
				{
					TLN_SetPaletteColor (palette, c, src[0], src[1], src[2]);
					src += 3;
				}
				TLN_SetBitmapPalette (bitmap, palette);
			}
		}
		free (lut);
		free (data);
	}
	return bitmap;
}

/* loads BMP */
static TLN_Bitmap LoadBMP (char *filename)
{
	BITMAPFILEHEADER bfh;
	BITMAPV5HEADER bv5;
	DWORD StructSize;
	FILE* pf;
	TLN_Bitmap bitmap = NULL;
	unsigned int c;
	int pitch;

	/* open file */
	pf = fopen (filename, "rb");
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
		BYTE* line = TLN_GetBitmapPtr (bitmap, 0, bv5.bV5Height - c - 1);
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
