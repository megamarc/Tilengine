/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <stdio.h>
#include <string.h>
#include "Engine.h"
#include "Tilengine.h"
#include "Palette.h"
#include "Tables.h"

/*!
 * \brief
 * Creates a new color table
 * 
 * \param entries
 * Number of color entries (typically 256)
 * 
 * \returns
 * Reference to the created palette or NULL if error
 */
TLN_Palette TLN_CreatePalette (int entries)
{
	TLN_Palette palette;
	int size = sizeof(struct Palette) + (4*entries);
	
	palette = (TLN_Palette)CreateBaseObject(OT_PALETTE, size);
	if (palette)
	{
		palette->entries = entries;
		TLN_SetLastError (TLN_ERR_OK);
		return palette;
	}
	else
		return NULL;
}

/*!
 * \brief
 * Creates a duplicate of the specified palette
 * 
 * \param src
 * Reference to the palette to clone
 * 
 * \returns
 * A reference to the newly cloned palette, or NULL if error
 *
 * \see
 * TLN_CreatePalette()
 */
TLN_Palette TLN_ClonePalette (TLN_Palette src)
{
	TLN_Palette palette;

	if (!CheckBaseObject (src, OT_PALETTE))
		return NULL;

	palette = (TLN_Palette)CloneBaseObject (src);
	if (palette)
	{
		TLN_SetLastError (TLN_ERR_OK);
		return palette;
	}
	else
		return NULL;
}

/*!
 * \brief
 * Deletes the specified palette and frees memory
 * 
 * \param palette
 * Reference to the palette to delete
 * 
 * \remarks
 * Don't delete a palette currently attached to a layer or sprite!
 */
bool TLN_DeletePalette (TLN_Palette palette)
{
	if (CheckBaseObject (palette, OT_PALETTE))
	{
		DeleteBaseObject (palette);
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return false;
}

/*!
 * \brief
 * Sets the RGB color value of a palette entry
 * 
 * \param palette
 * Reference to the palette to modify
 * 
 * \param index
 * Index of the palette entry to modify (0-255)
 * 
 * \param r
 * Red component of the color (0-255)
 * 
 * \param g
 * Green component of the color (0-255)
 * 
 * \param b
 * Blue component of the color (0-255)
 */
bool TLN_SetPaletteColor (TLN_Palette palette, int index, uint8_t r, uint8_t g, uint8_t b)
{
	if (CheckBaseObject (palette, OT_PALETTE))
	{
		uint32_t* data = (uint32_t*)GetPaletteData (palette, index);
		*data = PackRGB32(r,g,b);
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return false;
}

/*!
 * \brief
 * Returns the color value of a palette entry
 * 
 * \param palette
 * Reference to the palette to get the color
 * 
 * \param index
 * Index of the palette entry to obtain (0-255)
 * 
 * \returns
 * 32-bit integer with the packed color in internal pixel format RGBA
 */
uint8_t* TLN_GetPaletteData (TLN_Palette palette, int index)
{
	if (!CheckBaseObject (palette, OT_PALETTE))
		return NULL;
	else if (index >= palette->entries)
	{
		TLN_SetLastError (TLN_ERR_IDX_PICTURE);
		return false;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_OK);
		return GetPaletteData (palette, index);
	}
}

/*!
 * \brief
 * Mixes two palettes to create a third one
 * 
 * \param src1
 * Reference to the first source palette
 * 
 * \param src2
 * Reference to the second source palette
 * 
 * \param dst
 * Reference to the target palette
 * 
 * \param factor
 * Integer with mixing factor. 0=100% src1, 255=100% src2, 128=50%/50%
 */
bool TLN_MixPalettes (TLN_Palette src1, TLN_Palette src2, TLN_Palette dst, uint8_t factor)
{
	int c;
	const uint8_t invfactor = 255 - factor;
	const uint8_t* blend_table = engine->blend_table;
	uint8_t* src1ptr;
	uint8_t* src2ptr;
	uint8_t* dstptr;
	int count;

	if (!CheckBaseObject (src1, OT_PALETTE) || !CheckBaseObject (src2, OT_PALETTE) || !CheckBaseObject (dst, OT_PALETTE))
		return false;

	src1ptr = TLN_GetPaletteData (src1, 0);
	src2ptr = TLN_GetPaletteData (src2, 0);
	dstptr  = TLN_GetPaletteData (dst, 0);
	blend_table = SelectBlendTable (BLEND_MOD);

	if (src1->entries > src2->entries)
		count = src1->entries;
	else
		count = src2->entries;

	for (c=0; c<count; c++)
	{
		dstptr[0] = blendfunc(blend_table,src2ptr[0],factor) + blendfunc(blend_table,src1ptr[0],invfactor);
		dstptr[1] = blendfunc(blend_table,src2ptr[1],factor) + blendfunc(blend_table,src1ptr[1],invfactor);
		dstptr[2] = blendfunc(blend_table,src2ptr[2],factor) + blendfunc(blend_table,src1ptr[2],invfactor);
		src1ptr += sizeof(uint32_t);
		src2ptr += sizeof(uint32_t);
		dstptr  += sizeof(uint32_t);
	}

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/* edita rango de colores según tabla de mezcla */
static bool EditPaletteColor (TLN_Palette palette, uint8_t* blend_table, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num)
{
	int end;
	int c;
	uint8_t* color_ptr;

	if (!CheckBaseObject (palette, OT_PALETTE))
		return false;

	if (start >= palette->entries)
	{
		TLN_SetLastError (TLN_ERR_IDX_PICTURE);
		return false;
	}

	end = start + num - 1;
	if (end >= palette->entries)
		end = palette->entries - 1;

	color_ptr = TLN_GetPaletteData (palette, start);
	for (c=start; c<=end; c++)
	{
		color_ptr[0] = blendfunc(blend_table, color_ptr[0], r);
		color_ptr[1] = blendfunc(blend_table, color_ptr[1], g);
		color_ptr[2] = blendfunc(blend_table, color_ptr[2], b);
		color_ptr += sizeof(uint32_t);
	}

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Modifies a range of colors by adding the provided color value to the selected range. The result is always a brighter color.
 * 
 * \param palette
 * Reference to the palette to modify
 * 
 * \param r
 * Red component of the color (0-255)
 * 
 * \param g
 * Green component of the color (0-255)
 * 
 * \param b
 * Blue component of the color (0-255)
 *
 * \param start
 * index of the first color entry to modify
 * 
 * \param num
 * number of colors from start to modify
 */
bool TLN_AddPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num)
{
	return EditPaletteColor (palette, SelectBlendTable(BLEND_ADD), r,g,b, start,num);
}

/*!
 * \brief
 * Modifies a range of colors by subtracting the provided color value to the selected range. The result is always a darker color.
 * 
 * \param palette
 * Reference to the palette to modify
 * 
 * \param r
 * Red component of the color (0-255)
 * 
 * \param g
 * Green component of the color (0-255)
 * 
 * \param b
 * Blue component of the color (0-255)
 *
 * \param start
 * index of the first color entry to modify
 * 
 * \param num
 * number of colors from start to modify
 */
bool TLN_SubPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num)
{
	return EditPaletteColor (palette, SelectBlendTable(BLEND_SUB), r,g,b, start,num);
}

/*!
 * \brief
 * Modifies a range of colors by modulating (normalized product) the provided color value to the selected range. The result is always a darker color.
 * 
 * \param palette
 * Reference to the palette to modify
 * 
 * \param r
 * Red component of the color (0-255)
 * 
 * \param g
 * Green component of the color (0-255)
 * 
 * \param b
 * Blue component of the color (0-255)
 *
 * \param start
 * index of the first color entry to modify
 * 
 * \param num
 * number of colors from start to modify
 */
bool TLN_ModPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num)
{
	return EditPaletteColor (palette, SelectBlendTable(BLEND_MOD), r,g,b, start,num);
}
