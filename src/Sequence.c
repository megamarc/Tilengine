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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Sequence.h"
#include "Tilengine.h"
#include "Object.h"
#include "crc32.h"

/*!
 * \brief
 * Creates a new sequence for the animation engine
 * 
 * \param name
 * String with an unique name to query later
 * 
 * \param target
 * For tileset animations, the tile index to animate
 * 
 * \param count
 * Number of frames
 * 
 * \param frames
 * Array of TLN_Frame items with indexes and delays
 * 
 * \returns
 * Reference to the new sequence or NULL if error
 * 
 * \remarks
 * Use this function to create tileset or sprite animations
 * 
 * \see
 * TLN_SetTilemapAnimation(), TLN_SetSpriteAnimation()
 */
TLN_Sequence TLN_CreateSequence (const char* name, int target, int count, TLN_SequenceFrame* frames)
{
	int size;
	TLN_Sequence sequence;
	TLN_SequenceFrame* frame;
	
	size = count*sizeof(TLN_SequenceFrame);
	sequence = (TLN_Sequence)CreateBaseObject (OT_SEQUENCE, sizeof(struct Sequence) + size);
	if (!sequence)
		return NULL;

	if (name)
	{
		sequence->hash = _crc32(0, name, strlen(name));
		strncpy (sequence->name, name, sizeof(sequence->name));
		sequence->name[sizeof(sequence->name) - 1] = '\0';
	}
	sequence->target = target;
	sequence->count = count;

	frame = (TLN_SequenceFrame*)&sequence->data;
	memcpy (frame, frames, sizeof(TLN_SequenceFrame)*count);

	TLN_SetLastError (TLN_ERR_OK);
	return sequence;
}

/*!
 * \brief
 * Creates a color cycle sequence for palette animation
 * 
 * \param name
 * String with an unique name to query later
 * 
 * \param count
 * Number of color strips
 * 
 * \param strips
 * Array of color strips to assign
 * 
 * \returns
 * Reference to the created cycle or NULL if error
 * 
 * \remarks
 * Use this function to create advanced palette animation effects
 * 
 * \see
 * TLN_ColorStrip(), TLN_SetPaletteAnimation()
 */
TLN_Sequence TLN_CreateCycle (const char* name, int count, TLN_ColorStrip* strips)
{
	int size, c;
	TLN_Sequence sequence;
	TLN_ColorStrip* srcstrip;
	struct Strip* dststrip;
	
	size = count*sizeof(struct Strip);
	sequence = (TLN_Sequence)CreateBaseObject (OT_SEQUENCE, sizeof(struct Sequence) + size);
	if (!sequence)
		return NULL;

	if (name)
	{
		sequence->hash = _crc32(0, name, strlen(name));
		strncpy (sequence->name, name, sizeof(sequence->name));
		sequence->name[sizeof(sequence->name) - 1] = '\0';
	}
	
	sequence->count = count;
	srcstrip = strips;
	dststrip = (struct Strip*)&sequence->data;
	for (c=0; c<count; c++)
	{
		dststrip->delay = srcstrip->delay;
		dststrip->first = srcstrip->first;
		dststrip->count = srcstrip->count;
		dststrip->dir   = srcstrip->dir;
		srcstrip++;
		dststrip++;
	}

	TLN_SetLastError (TLN_ERR_OK);
	return sequence;
}

/*!
 * \see Creates a name based sprite sequence
 * 
 * \param name Optional name used to retrieve it when adding to a TLN_SequencePack, can be NULL
 * \param spriteset Reference to the spriteset with frames to animate
 * \param basename Base of the sprite name for the numbered sequence
 * \param delay Number of ticks to delay between frame
 * \return Reference to the created TLN_Sequence object or NULL if error
 * \remarks Trailing numbers in sprite names must start with 1 and be correlative (eg basename1... basename14)
  */
TLN_Sequence TLN_CreateSpriteSequence(const char* name, TLN_Spriteset spriteset, const char* basename, int delay)
{
	int size;
	TLN_Sequence sequence;
	TLN_SequenceFrame* frame;
	int c;
	int count = 0;
	int index;
	char framename[64];

	if (!CheckBaseObject(spriteset, OT_SPRITESET))
	{
		TLN_SetLastError(TLN_ERR_REF_SPRITESET);
		return NULL;
	}

	/* find number of frames */
	do
	{
		snprintf(framename, sizeof(framename), "%s%d", basename, count + 1);
		index = TLN_FindSpritesetSprite(spriteset, framename);
		if (index != -1)
			count += 1;
	} while (index != -1);

	/* noi matching frames found: exit */
	if (count == 0)
	{
		TLN_SetLastError(TLN_ERR_REF_SPRITESET);
		return NULL;
	}

	size = count * sizeof(TLN_SequenceFrame);
	sequence = (TLN_Sequence)CreateBaseObject(OT_SEQUENCE, sizeof(struct Sequence) + size);
	if (!sequence)
		return NULL;

	if (name)
	{
		sequence->hash = _crc32(0, name, strlen(name));
		strncpy(sequence->name, name, sizeof(sequence->name));
		sequence->name[sizeof(sequence->name) - 1] = '\0';
	}
	sequence->count = count;

	/* build frames from sprite name */
	frame = (TLN_SequenceFrame*)&sequence->data;
	for (c = 0; c < count; c++)
	{
		snprintf(framename, sizeof(framename), "%s%d", basename, c + 1);
		frame->index = TLN_FindSpritesetSprite(spriteset, framename);
		frame->delay = delay;
		frame += 1;
	}

	TLN_SetLastError(TLN_ERR_OK);
	return sequence;
}

/*!
 * \brief
 * Creates a duplicate of the specified sequence
 * 
 * \param src
 * Sequence to clone
 * 
 * \returns
 * A reference to the newly cloned sequence, or NULL if error
 *
 * \see
 * TLN_FindSequence()
 */
TLN_Sequence TLN_CloneSequence (TLN_Sequence src)
{
	TLN_Sequence sequence;

	if (!CheckBaseObject (src, OT_SEQUENCE))
		return NULL;

	sequence = (TLN_Sequence)CloneBaseObject (src);
	if (sequence)
	{
		TLN_SetLastError (TLN_ERR_OK);
		return sequence;
	}
	else
		return NULL;
}

/*!
 * \brief
 * Returns runtime info about a given sequence
 * 
 * \param sequence
 * Sequence to query
 * 
 * \param info
 * Pointer to a user-provided TLN_SequenceInfo structure to hold the returned data
 *
 * \see
 * TLN_FindSequence()
 */
bool TLN_GetSequenceInfo (TLN_Sequence sequence, TLN_SequenceInfo* info)
{
	if (CheckBaseObject (sequence, OT_SEQUENCE) && info != NULL)
	{
		strncpy (info->name, sequence->name, sizeof(info->name));
		info->name[sizeof(info->name) - 1] = '\0';
		info->num_frames = sequence->count;
		return true;
	}
	else
		return false;
}

/*!
 * \brief
 * Deletes the sequence and frees resources
 * 
 * \param sequence
 * Reference to the sequence to be deleted
 * 
 * \remarks
 * Don't delete an active sequence!
 */
bool TLN_DeleteSequence (TLN_Sequence sequence)
{
	if (CheckBaseObject (sequence, OT_SEQUENCE))
	{
		DeleteBaseObject (sequence);
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return false;
}
