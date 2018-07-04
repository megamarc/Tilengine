/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2018 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

/*!
 * \file spriteset.c
 * Spriteset manager
 */

#include <stdio.h>
#include <string.h>
#include "Tilengine.h"
#include "Spriteset.h"
#include "Palette.h"
#include "Bitmap.h"
#include "Hash.h"

static void set_sprite_entry (TLN_Spriteset spriteset, int entry, TLN_SpriteData* data)
{
	SpriteEntry* dst_data = &spriteset->data[entry];
	dst_data->w = data->w;
	dst_data->h = data->h;
	dst_data->offset = data->y*spriteset->bitmap->pitch + data->x;
	if (data->name[0] != 0)
		dst_data->hash = hash(0, data->name, strlen(data->name));
	else
		dst_data->hash = 0;
}

/*!
 * \brief
 * Creates a new spriteset
 * 
 * \param bitmap
 * Bitmap containing the sprite graphics
 *
 * \param data
 * Array of TLN_SpriteData structures with sprite descriptions
 *
 * \param num_entries
 * Number of entries in data[] array
 * 
 * \returns
 * Reference to the created spriteset, or NULL if error
 * 
 * \see
 * TLN_DeleteSpriteset()
 */
TLN_Spriteset TLN_CreateSpriteset (TLN_Bitmap bitmap, TLN_SpriteData* data, int num_entries)
{
	TLN_Spriteset spriteset = NULL;
	const int size = sizeof(struct Spriteset) + (sizeof(SpriteEntry) * num_entries);
	int c;

	/* crea */
	spriteset = CreateBaseObject (OT_SPRITESET, size);
	if (!spriteset)
	{
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}

	/* copia datos */
	spriteset->bitmap = bitmap;
	spriteset->palette = TLN_GetBitmapPalette (bitmap);
	spriteset->entries = num_entries;
	if (data != NULL)
	{
		for (c=0; c<num_entries; c++)
		{
			set_sprite_entry (spriteset, c, data);
			data++;
		}
	}

	TLN_SetLastError (TLN_ERR_OK);
	return spriteset;
}

/*!
 * \brief
 * Sets attributes and pixels of a given sprite inside a spriteset
 * 
 * \param spriteset
 * Spriteset to set the data
 *
 * \param entry
 * The entry index inside the spriteset to modify [0, num_sprites - 1]
 * 
 * \param data
 * Pointer to a user-provided TLN_SpriteData structure with sprite description
 *
 * \param pixels
 * Pointer to source pixel data
 *
 * \param pitch
 * Number of bytes per scanline of the source pixel data
 *
 * \see
 * TLN_CreateSpriteset()
 */
bool TLN_SetSpritesetData (TLN_Spriteset spriteset, int entry, TLN_SpriteData* data, void* pixels, int pitch)
{
	if (!CheckBaseObject (spriteset, OT_SPRITESET))
	{
		TLN_SetLastError (TLN_ERR_REF_SPRITESET);
		return false;
	}

	if (entry >= spriteset->entries)
	{
		TLN_SetLastError (TLN_ERR_IDX_SPRITE);
		return false;
	}

	set_sprite_entry (spriteset, entry, data);
	if (pixels != NULL && pitch != 0)
	{
		uint8_t* src = pixels;
		uint8_t* dst = TLN_GetBitmapPtr (spriteset->bitmap, data->x, data->y);
		int c;
		for (c=0; c<data->h; c++)
		{
			memcpy (dst, src, data->w);
			src += pitch;
			dst += spriteset->bitmap->pitch;
		}
	}
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Creates a duplicate of the specified spriteset and its associated palette
 * 
 * \param src
 * Spriteset to clone
 * 
 * \returns
 * A reference to the newly cloned spriteset, or NULL if error
 * \see
 * TLN_LoadSpriteset()
 */
TLN_Spriteset TLN_CloneSpriteset (TLN_Spriteset src)
{
	TLN_Spriteset spriteset;

	if (!CheckBaseObject (src, OT_SPRITESET))
	{
		TLN_SetLastError (TLN_ERR_REF_SPRITESET);
		return NULL;
	}

	spriteset = CloneBaseObject (src);
	if (spriteset)
	{
		TLN_SetLastError (TLN_ERR_OK);
		return spriteset;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}
}

/*!
 * \brief
 * Deletes the specified spriteset and frees memory
 * 
 * \param spriteset
 * Spriteset to delete
 * 
 * \remarks
 * Don't delete a spriteset currently attached to a sprite!
 * 
 * \see
 * TLN_LoadSpriteset(), TLN_CloneSpriteset()
 */
bool TLN_DeleteSpriteset (TLN_Spriteset spriteset)
{
	if (CheckBaseObject (spriteset, OT_SPRITESET))
	{
		if (ObjectOwner (spriteset))
			DeleteBaseObject (spriteset->bitmap);
		DeleteBaseObject (spriteset);
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_REF_SPRITESET);
		return false;
	}
}

/*!
 * \brief
 * Query the details about the specified sprite inside a spriteset
 * 
 * \param spriteset
 * Reference to the spriteset to get info about
 * 
 * \param entry
 * The entry index inside the spriteset [0, num_sprites - 1]
 * 
 * \param info
 * Pointer to application-allocated TLN_SpriteInfo structure that will receive the data
 * 
 * \returns
 * true if success or false if error
 */
bool TLN_GetSpriteInfo (TLN_Spriteset spriteset, int entry, TLN_SpriteInfo* info)
{
	if (CheckBaseObject (spriteset, OT_SPRITESET) && info)
	{
		SpriteEntry* sprite = (SpriteEntry*)spriteset->data;
		info->w = sprite[entry].w;
		info->h = sprite[entry].h;
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_REF_SPRITESET);
		return false;
	}
}

/*!
 * \brief
 * Returns a reference to the palette associated to the specified spriteset
 * 
 * \param spriteset
 * Spriteset to obtain the palette
 * 
 * \remarks
 * The palette of a spriteset is created at load time and cannot be modified. When TLN_ConfigSprite
 * function is used to setup a sprite, the palette associated with the specified spriteset is automatically
 * assigned to that sprite, but it can be later replaced with TLN_SetSpritePalette
 * 
 * \see
 * TLN_SetSpritePalette()
 */
TLN_Palette TLN_GetSpritesetPalette (TLN_Spriteset spriteset)
{
	if (CheckBaseObject (spriteset, OT_SPRITESET))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return spriteset->palette;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_REF_SPRITESET);
		return NULL;
	}
}

/*!
 * \brief
 * Returns a reference to the palette associated to the specified spriteset
 * 
 * \param spriteset
 * Spriteset where to find the sprite
 *
 * \param name
 * Name of the sprite to findo
 *
 * \returns
 * sprite index (0 -> num_sprites - 1) if found, or -1 if not found
 */
int TLN_FindSpritesetSprite (TLN_Spriteset spriteset, char* name)
{
	hash_t find;
	int entry = -1;
	int c = 0;

	if (!CheckBaseObject (spriteset, OT_SPRITESET))
	{
		TLN_SetLastError (TLN_ERR_REF_SPRITESET);
		return false;
	}
	if (name == NULL)
		return false;

	/* search by name hash */
	find = hash(0, name, strlen(name));
	for (c=0; c < spriteset->entries; c++)
	{
		const SpriteEntry* info = &spriteset->data[c];
		if (info->hash == find)
			return c;
	}
	return -1;
}