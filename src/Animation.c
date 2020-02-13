/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. 
* */

#ifdef _MSC_VER
#define inline __inline
#endif

#include <string.h>
#include "Tilengine.h"
#include "Tilemap.h"
#include "Sequence.h"
#include "Animation.h"
#include "Engine.h"
#include "Palette.h"
#include "Tables.h"

/* linear interploation */
static int lerp (int x, int x0, int x1, int fx0, int fx1)
{
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0);
}

static inline void blendColors (uint8_t* srcptr0, uint8_t* srcptr1, uint8_t* dstptr, uint8_t f0, uint8_t f1)
{
	dstptr[0] = blendfunc(engine->mod_table, srcptr0[0], f0) + blendfunc(engine->mod_table, srcptr1[0], f1);
	dstptr[1] = blendfunc(engine->mod_table, srcptr0[1], f0) + blendfunc(engine->mod_table, srcptr1[1], f1);
	dstptr[2] = blendfunc(engine->mod_table, srcptr0[2], f0) + blendfunc(engine->mod_table, srcptr1[2], f1);
}

static void SetAnimation (Animation* animation, TLN_Sequence sequence, animation_t type);
static void ColorCycle (TLN_Palette srcpalette, TLN_Palette dstpalette, struct Strip* strip);
static void ColorCycleBlend (TLN_Palette srcpalette, TLN_Palette dstpalette, struct Strip* strip, int t);
static void ReplaceTiles (TLN_Tilemap tilemap, int srctile, int dsttile);

/* main loop tasks */
void UpdateAnimations (int time)
{
	int c;
	TLN_Sequence sequence;
	TLN_SequenceFrame* frames;
	struct Strip* strips;
	
	for (c=0; c<engine->numanimations; c++)
	{
		Animation* animation = &engine->animations[c];
		if (animation->enabled==false)
			continue;

		sequence = animation->sequence;
		if (animation->type == TYPE_PALETTE)
		{
			int i;
			strips = (struct Strip*)&sequence->data;
			for (i=0; i<sequence->count; i++)
			{
				struct Strip* strip = &strips[i];
				/* next frame */
				if (time >= strip->timer)
				{
					strip->timer = time + strip->delay;
					strip->pos = (strip->pos + 1) % strip->count;
					strip->t0 = time;
					if (!animation->blend)
						ColorCycle (animation->srcpalette, animation->palette, strip);
				}

				/* interpolate */
				if (animation->blend)
					ColorCycleBlend (animation->srcpalette, animation->palette, strip, time);
			}
			continue;
		}

		if (time < animation->timer)
			continue;

		frames = (TLN_SequenceFrame*)&sequence->data;
		animation->timer = time + frames[animation->pos].delay;
		switch (animation->type)
		{
		case TYPE_TILEMAP:
			ReplaceTiles (engine->layers[animation->idx].tilemap, 
				frames[animation->pos].index, 
				frames[animation->pos + 1].index % sequence->count);
			break;

		case TYPE_SPRITE:
			TLN_SetSpritePicture (animation->idx, frames[animation->pos].index);
			break;

		case TYPE_TILESET:
			TLN_CopyTile (engine->layers[animation->idx].tileset, frames[animation->pos].index, sequence->target);
			break;

		/* Fall through									*/
		/* Stop warning GNU C compiler	*/
		case TYPE_NONE:
		case TYPE_PALETTE:
			break;
		}

		/* next frame */
		animation->pos++;
		if (animation->pos == sequence->count)
		{
			if (animation->loop > 1)
			{
				animation->loop--;
				animation->pos = 0;
			}
			else if (animation->loop == 1)
				animation->enabled = false;
			else if (animation->loop == 0)
				animation->pos = 0;
		}
	}
}

/**
 * \brief
 * Checks the state of the specified animation
 * 
 * \param index
 * Id of the animation to check (0 <= id < num_animations)
 * 
 * \returns
 * true if animation is running, false if it's finished or inactive
 */
bool TLN_GetAnimationState (int index)
{
	if (index >= engine->numanimations)
	{
		TLN_SetLastError (TLN_ERR_IDX_ANIMATION);
		return false;
	}

	TLN_SetLastError (TLN_ERR_OK);
	return engine->animations[index].enabled;
}

/*!
 * \brief
 * Starts a palette animation
 * 
 * \param index
 * Id of the animation to set (0 <= id < num_animations)
 * 
 * \param palette
 * Reference of the palette to be animated
 * 
 * \param sequence
 * Reference of the sequence to assign
 * 
 * \param blend
 * true for smooth frame interpolation, false for classic, discrete mode
 */
bool TLN_SetPaletteAnimation (int index, TLN_Palette palette, TLN_Sequence sequence, bool blend)
{
	Animation* animation;
	int c;
	struct Strip* strips;

	TLN_SetLastError (TLN_ERR_OK);
	
	if (index >= engine->numanimations)
	{
		TLN_SetLastError (TLN_ERR_IDX_ANIMATION);
		return false;
	}
	
	animation = &engine->animations[index];

	if (animation->sequence == sequence)
		return true;

	/* validate type */
	if (!CheckBaseObject (palette, OT_PALETTE) || !CheckBaseObject (sequence, OT_SEQUENCE))
		return false;

	SetAnimation (animation, sequence, TYPE_PALETTE);
	animation->palette = palette;
	animation->blend = blend;

	/* start timers */
	strips = (struct Strip*)&sequence->data;
	for (c=0; c<sequence->count; c++)
	{
		strips[c].timer = 0;
		strips[c].t0 = 0;
	}

	/* create auxiliary palette */
	if (animation->srcpalette == NULL)
		animation->srcpalette = TLN_CreatePalette (256);
	CopyBaseObject (animation->srcpalette, palette);

	return true;
}

/*!
 * \brief
 * Sets the source palette of a color cycle animation
 * 
 * \param index
 * Id of the animation to set (0 <= id < num_animations)
 * 
 * \param palette
 * Reference of the palette to assign
 * 
 * \remarks
 * Use this function to change the palette assigned to a color cycle animation running.
 * This is useful to combine color cycling and palette interpolation at the same time
 */
bool TLN_SetPaletteAnimationSource (int index, TLN_Palette palette)
{
	Animation* animation;

	if (index >= engine->numanimations)
	{
		TLN_SetLastError (TLN_ERR_IDX_ANIMATION);
		return false;
	}

	if (!CheckBaseObject (palette, OT_PALETTE))
		return false;

	animation = &engine->animations[index];
	CopyBaseObject (animation->srcpalette, palette);
	CopyBaseObject (animation->palette, palette);

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Starts a tileset animation
 * 
 * \param index
 * Id of the animation to set (0 <= id < num_animations)
 * 
 * \param nlayer
 * Id of the layer to animate (0 <= id < num_layers)
 * 
 * \param sequence
 * Reference of the sequence to assign
 * 
 * \see
 * Animations
 */
bool TLN_SetTilesetAnimation (int index, int nlayer, TLN_Sequence sequence)
{
	Animation* animation;
	
	if (index >= engine->numanimations)
	{
		TLN_SetLastError (TLN_ERR_IDX_ANIMATION);
		return false;
	}
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	/* validate type */
	if (!CheckBaseObject (sequence, OT_SEQUENCE))
		return false;
	
	animation = &engine->animations[index];
	SetAnimation (animation, sequence, TYPE_TILESET);
	animation->idx = nlayer;

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Starts a tilemap animation
 * 
 * \param index
 * Id of the animation to set (0 <= id < num_animations)
 * 
 * \param nlayer
 * Id of the layer to animate (0 <= id < num_layers)
 * 
 * \param sequence
 * Reference of the sequence to assign
 * 
 * \see
 * Animations
 */
bool TLN_SetTilemapAnimation (int index, int nlayer, TLN_Sequence sequence)
{
	Animation* animation;
	
	if (index >= engine->numanimations)
	{
		TLN_SetLastError (TLN_ERR_IDX_ANIMATION);
		return false;
	}
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError (TLN_ERR_IDX_LAYER);
		return false;
	}

	/* validate type */
	if (!CheckBaseObject (sequence, OT_SEQUENCE))
		return false;
	
	animation = &engine->animations[index];
	SetAnimation (animation, sequence, TYPE_TILEMAP);
	animation->idx = nlayer;

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Starts a sprite animation
 * 
 * \param index
 * Id of the animation to set (0 <= id < num_animations)
 * 
 * \param nsprite
 * If of the sprite to animate (0 <= id < num_sprites)
 * 
 * \param sequence
 * Reference of the sequence to assign
 * 
 * \param loop
 * amount of times to loop, 0=infinite
 * 
 * \see
 * Animations
 */
bool TLN_SetSpriteAnimation (int index, int nsprite, TLN_Sequence sequence, int loop)
{
	Animation* animation;
	
	if (index >= engine->numanimations)
	{
		TLN_SetLastError (TLN_ERR_IDX_ANIMATION);
		return false;
	}
	if (nsprite >= engine->numsprites)
	{
		TLN_SetLastError (TLN_ERR_IDX_SPRITE);
		return false;
	}

	/* validate type */
	if (!CheckBaseObject (sequence, OT_SEQUENCE))
		return false;
	
	animation = &engine->animations[index];
	SetAnimation (animation, sequence, TYPE_SPRITE);
	animation->idx = nsprite;
	animation->loop = loop;

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Deprecated, each frame has its own delay
 */
bool TLN_SetAnimationDelay (int index, int delay)
{
	return true;
}

/*!
 * \brief
 * Finds an available (unused) animation
 * 
 * \returns
 * Index of the first unused animation (starting from 0) or -1 if none found
 */
int TLN_GetAvailableAnimation (void)
{
	int c;

	TLN_SetLastError (TLN_ERR_OK);
	for (c=0; c<engine->numanimations; c++)
	{
		if (!engine->animations[c].enabled)
			return c;
	}
	return -1;
}

/*!
 * \brief
 * Disables the animation so it stops playing and returns it to the list of available animations
 * 
 * \param index
 * Id of the animation to set (0 <= id < num_animations)
 * 
 * \see
 * Animations
 */
bool TLN_DisableAnimation (int index)
{
	Animation* animation;
	
	if (index >= engine->numanimations)
	{
		TLN_SetLastError (TLN_ERR_IDX_ANIMATION);
		return false;
	}
	
	animation = &engine->animations[index];
	animation->enabled = false;
	animation->type = TYPE_NONE;
	animation->sequence = NULL;

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/* animation commons */
static void SetAnimation (Animation* animation, TLN_Sequence sequence, animation_t type)
{
	animation->timer = 0;
	animation->enabled = true;
	animation->sequence = sequence;
	animation->type = type;
	animation->loop = 0;
	animation->pos = 0;
}

/* regular color cycle */
static void ColorCycle (TLN_Palette srcpalette, TLN_Palette dstpalette, struct Strip* strip)
{
	int c;
	uint32_t* srcptr = (uint32_t*)GetPaletteData (srcpalette, strip->first);
	uint32_t* dstptr = (uint32_t*)GetPaletteData (dstpalette, strip->first);
	int count = strip->count;
	int steps = strip->pos;

	if (strip->dir)
	{
		for (c=0; c<count; c++)
			dstptr[c] = srcptr[(c - steps + count) % count];
	}
	else
	{
		for (c=0; c<count; c++)
			dstptr[c] = srcptr[(c + steps) % count];
	}
}

/* blended color cycle */
static void ColorCycleBlend (TLN_Palette srcpalette, TLN_Palette dstpalette, struct Strip* strip, int t)
{
	int c;
	int idx0, idx1;
	uint8_t *srcptr0, *srcptr1, *dstptr;

	int t0 = strip->t0;
	int t1 = strip->timer;
	int count = strip->count;
	int steps = strip->pos;

	/* map [t0 - t1] to [0 - 255] */
	int f1 = lerp (t, t0,t1, 0,255);
	int f0 = 255 - f1;

	for (c=0; c<count; c++)
	{
		if (strip->dir)
		{
			idx0 = (c - steps + count) % count;
			idx1 = (c - steps - 1 + count) % count;
		}
		else
		{
			idx0 = (c + steps) % count;
			idx1 = (c + steps + 1) % count;
		}

		srcptr0 = GetPaletteData (srcpalette, strip->first + idx0);
		srcptr1 = GetPaletteData (srcpalette, strip->first + idx1);
		dstptr  = GetPaletteData (dstpalette, strip->first + c);
		blendColors (srcptr0, srcptr1, dstptr, f0, f1);
	}
}

/* tile substitution */
static void ReplaceTiles (TLN_Tilemap tilemap, int srctile, int dsttile)
{
	int c;
	int size = tilemap->rows * tilemap->cols;

	for (c=0; c<size; c++)
	{
		if (tilemap->tiles[c].index == srctile)
			tilemap->tiles[c].index = dsttile;
	}
}
