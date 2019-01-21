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

#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Sequence.h"
#include "Tilengine.h"
#include "Object.h"
#include "Hash.h"

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
	sequence = CreateBaseObject (OT_SEQUENCE, sizeof(struct Sequence) + size);
	if (!sequence)
		return NULL;

	if (name)
	{
		sequence->hash = hash(0, name, strlen(name));
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
	sequence = CreateBaseObject (OT_SEQUENCE, sizeof(struct Sequence) + size);
	if (!sequence)
		return NULL;

	if (name)
	{
		sequence->hash = hash(0, name, strlen(name));
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

	sequence = CloneBaseObject (src);
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
