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
 * \brief Adobe Color Table (.act) loader
 * \author Megamarc
 * \date 20 sep 2015
 *
 * Public Tilengine source code
 * http://www.tilengine.org
 *
 ******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include "Tilengine.h"
#include "LoadFile.h"

#define SWAP(w) ((w)&0xFF)<<8 | ((w)>>8)

#define ACT_ENTRIES		256
#define ACT_SIZE		(ACT_ENTRIES*3 + sizeof(trailing))

/* optional trailing bytes in an ACT file */
struct
{
	short entries;		/* number of entries 1-255 */
	short transparent;	/* index of transparent color */
}
trailing;

/*!
 * \brief
 * Loads a palette from a standard .act file
 * 
 * \param filename
 * ACT file containing the palette to load
 * 
 * \returns
 * A reference to the newly loaded palette, or NULL if error
 * 
 * \remarks
 * Palettes are also automatically created when loading tilesets and spritesets.
 * Use the functions TLN_GetTilesetPalette() and TLN_GetSpritesetPalette() to retrieve them.
 * 
 * \see
 * TLN_GetTilesetPalette(), TLN_GetSpritesetPalette()
 */
TLN_Palette TLN_LoadPalette (const char* filename)
{
	FILE *pf;
	TLN_Palette palette = NULL;
	int size;
	int c;

	/* open file */
	pf = FileOpen (filename);
	if (!pf)
	{
		TLN_SetLastError (TLN_ERR_FILE_NOT_FOUND);
		return NULL;
	}

	/* check size */
	fseek (pf, 0, SEEK_END);
	size = ftell (pf);

	/* load trailing and get number of entries */
	if (size == ACT_SIZE)
	{
		fseek (pf, - (int)sizeof(trailing), SEEK_END);
		fread (&trailing, sizeof(trailing), 1, pf);
		trailing.entries = SWAP(trailing.entries);
		trailing.transparent = SWAP(trailing.transparent);
	}
	else
		trailing.entries = size/3;

	/* create palette and load from file */
	palette = TLN_CreatePalette (trailing.entries);
	fseek (pf, 0, SEEK_SET);
	for (c=0; c<trailing.entries; c++)
	{
		uint8_t src[3];
		fread (src, sizeof(src), 1, pf);
		TLN_SetPaletteColor (palette, c, src[0], src[1], src[2]);
	}

	fclose (pf);
	TLN_SetLastError (TLN_ERR_OK);
	return palette;
}
