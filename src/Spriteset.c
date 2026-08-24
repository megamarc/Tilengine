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
#include "Tilengine.h"
#include "Spriteset.h"
#include "Palette.h"
#include "Bitmap.h"
#include "crc32.h"

static void set_sprite_entry (TLN_Spriteset spriteset, int entry, TLN_SpriteData* data)
{
	SpriteEntry* dst_data = &spriteset->data[entry];
	dst_data->w = data->w;
	dst_data->h = data->h;
	dst_data->offset = data->y*spriteset->bitmap->pitch + data->x;
	if (data->name[0] != 0)
		dst_data->hash = _crc32(0, data->name, strlen(data->name));
	else
		dst_data->hash = 0;
}

TLN_Spriteset TLN_CreateSpriteset (TLN_Bitmap bitmap, TLN_SpriteData* data, int num_entries)
{
	TLN_Spriteset spriteset = NULL;
	const int size = sizeof(struct Spriteset) + (sizeof(SpriteEntry) * num_entries);
	int c;

	/* crea */
	spriteset = (TLN_Spriteset)CreateBaseObject (OT_SPRITESET, size);
	if (!spriteset)
		return NULL;

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

bool TLN_SetSpritesetData (TLN_Spriteset spriteset, int entry, TLN_SpriteData* data, void* pixels, int pitch)
{
	if (!CheckBaseObject (spriteset, OT_SPRITESET))
		return false;

	if (entry >= spriteset->entries)
	{
		TLN_SetLastError (TLN_ERR_IDX_SPRITE);
		return false;
	}

	set_sprite_entry (spriteset, entry, data);
	if (pixels != NULL && pitch != 0)
	{
		uint8_t* src = (uint8_t*)pixels;
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

TLN_Spriteset TLN_CloneSpriteset (TLN_Spriteset src)
{
	TLN_Spriteset spriteset;

	if (!CheckBaseObject (src, OT_SPRITESET))
		return NULL;

	spriteset = (TLN_Spriteset)CloneBaseObject (src);
	if (spriteset)
	{
		TLN_SetLastError (TLN_ERR_OK);
		return spriteset;
	}
	else
		return NULL;
}

bool TLN_DeleteSpriteset (TLN_Spriteset spriteset)
{
	if (CheckBaseObject (spriteset, OT_SPRITESET))
	{
		if (ObjectOwner (spriteset))
			TLN_DeleteBitmap (spriteset->bitmap);
		DeleteBaseObject (spriteset);
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return false;
}

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
		return false;
}

TLN_Palette TLN_GetSpritesetPalette (TLN_Spriteset spriteset)
{
	if (CheckBaseObject (spriteset, OT_SPRITESET))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return spriteset->palette;
	}
	else
		return NULL;
}

int TLN_FindSpritesetSprite (TLN_Spriteset spriteset, const char* name)
{
	uint32_t find;
	int entry = -1;
	int c = 0;

	if (!CheckBaseObject (spriteset, OT_SPRITESET))
		return false;

	if (name == NULL)
		return false;

	/* search by name hash */
	find = _crc32(0, name, strlen(name));
	for (c=0; c < spriteset->entries; c++)
	{
		const SpriteEntry* info = &spriteset->data[c];
		if (info->hash == find)
			return c;
	}
	return -1;
}
