/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <string.h>
#include <stdlib.h>
#include "SequencePack.h"
#include "Object.h"
#include "crc32.h"

/*!
 * \brief
 * Creates a new collection of sequences
 * 
 * \returns
 * Reference to the created pack or NULL if error
 * 
 * \see
 * TLN_AddSequenceToPack(), TLN_CreateSequence()
 */
TLN_SequencePack TLN_CreateSequencePack (void)
{
	TLN_SequencePack sp;
	int size = sizeof(struct SequencePack);

	sp = (TLN_SequencePack)CreateBaseObject (OT_SEQPACK, size);
	if (sp != NULL)
		TLN_SetLastError (TLN_ERR_OK);
	return sp;
}

/*!
 * \brief
 * Adds a sequence to a sequence pack
 * 
 * \param sp
 * Reference to the sequence pack
 * 
 * \param sequence
 * Reference to the sequence to attach
 * 
 * \remarks
 * Write remarks for TLN_AddSequenceToPack here.
 * 
 * \see
 * TLN_CreateSequencePack(), TLN_CreateSequence()
 */
bool TLN_AddSequenceToPack (TLN_SequencePack sp, TLN_Sequence sequence)
{
	if (!CheckBaseObject (sp, OT_SEQPACK) || !CheckBaseObject (sequence, OT_SEQUENCE))
		return false;

	/* añade a lista enlazada */
	if (sp->sequences == NULL)
		sp->sequences = sequence;
	else
		sp->last->next = sequence;
	sp->last = sequence;
	sp->last->next = NULL;
	
	sp->num_sequences++;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

/*!
 * \brief
 * Returns the number of sequences inside a sequence pack
 * 
 * \param sp
 * Reference to the sequence pack to query
 */
int TLN_GetSequencePackCount (TLN_SequencePack sp)
{
	if (!CheckBaseObject (sp, OT_SEQPACK))
		return 0;

	TLN_SetLastError (TLN_ERR_OK);
	return sp->num_sequences;
}

/*!
 * \brief
 * Returns the nth sequence inside a sequence pack
 * 
 * \param sp
 * Reference to the sequence pack containing the sequence to find
 * 
 * \param index
 * Sequence number to return [0, num_sequences - 1]
 * 
 * \see
 * TLN_LoadSequencePack(), TLN_GetSequencePackCount()
 */
TLN_Sequence TLN_GetSequence (TLN_SequencePack sp, int index)
{
	TLN_Sequence sequence;
	int c;

	if (!CheckBaseObject (sp, OT_SEQPACK))
		return NULL;

	if (index >= sp->num_sequences)
	{
		TLN_SetLastError (TLN_ERR_IDX_ANIMATION);
		return NULL;
	}

	sequence = sp->sequences;
	for (c=0; c<index; c++)
		sequence = sequence->next;

	TLN_SetLastError (TLN_ERR_OK);
	return sequence;
}

/*!
 * \brief
 * Finds a sequence inside a sequence pack
 * 
 * \param sp
 * Reference to the sequence pack containing the sequence to find
 * 
 * \param name
 * Name of the sequence to find
 * 
 * \returns
 * Reference to the sequence with the specified name, or NULL if not found
 * 
 * \see
 * TLN_LoadSequencePack()
 */
TLN_Sequence TLN_FindSequence (TLN_SequencePack sp, const char* name)
{
	TLN_Sequence sequence;
	uint32_t find;

	if (!CheckBaseObject (sp, OT_SEQPACK))
		return NULL;

	if (!name)
	{
		TLN_SetLastError (TLN_ERR_NULL_POINTER);
		return NULL;
	}

	/* recorre lista */
	find = _crc32(0, name, strlen(name));
	sequence = sp->sequences;
	while (sequence != NULL)
	{
		if (sequence->hash == find)
		{
			TLN_SetLastError (TLN_ERR_OK);
			return sequence;
		}
		else
			sequence = sequence->next;
	}

	TLN_SetLastError (TLN_ERR_FILE_NOT_FOUND);
	return NULL;
}

/*!
 * \brief
 * Deletes the specified sequence pack and frees memory
 * 
 * \param sp
 * Reference to the sequence pack to delete
 * 
 * \remarks
 * Don't delete a sequence pack that has sequences currently attached to animations!
 * The attached sequences are also deleted, so they haven't to be deleted externally.
 *
 * \see
 * TLN_LoadSequencePack()
 */
bool TLN_DeleteSequencePack (TLN_SequencePack sp)
{
	if (!CheckBaseObject (sp, OT_SEQPACK))
		return false;

	if (ObjectOwner (sp))
	{
		TLN_Sequence sequence = sp->sequences;
		while (sequence != NULL)
		{
			TLN_Sequence next = sequence->next;
			TLN_DeleteSequence (sequence);
			sequence = next;
		}
	}

	DeleteBaseObject (sp);
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}
