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
 * \brief Spriteset loader (.png / .txt pair) created with Spritesheet packer
 * http://spritesheetpacker.codeplex.com/
 *
 * \author Megamarc
 * \date 20 sep 2015
 *
 * Public Tilengine source code
 * http://www.tilengine.org
 *
 ******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tilengine.h"
#include "LoadFile.h"

/*!
 * \brief
 * Loads a spriteset from a png/txt file pair
 * 
 * \param name
 * Base name of the files containing the spriteset
 * 
 * \returns
 * Reference to the newly loaded spriteset or NULL if error
 * 
 * \remarks
 * The spriteset comes in a pair of files called name.png and name.txt. The png file contains
 * the spriteset, whereas the txt contains the coordinates of the rectangles that define individual sprites.
 * These files can be created wit the spritesheet packer tool (http://spritesheetpacker.codeplex.com/)
 * \remarks
 * An associated palette is also created, it can be obtained calling TLN_GetSpritesetPalette()
 */
TLN_Spriteset TLN_LoadSpriteset (const char* name)
{
	FILE *pf;
	char filename[64];
	char line[64];
	int entries = 0;
	TLN_Bitmap bitmap;
	TLN_Spriteset spriteset;
	TLN_SpriteData *sprite_data;
	int c;

	/* load png file */
	sprintf (filename, "%s.png", name);
	bitmap = TLN_LoadBitmap (filename);
	if (!bitmap)
		return NULL;

	/* load txt file */
	sprintf (filename, "%s.txt", name);
	pf = FileOpen (filename);
	if (!pf)
	{
		TLN_DeleteBitmap (bitmap);
		TLN_SetLastError (TLN_ERR_FILE_NOT_FOUND);
		return NULL;
	}

	/* count lines */
	while (fgets (line, 64, pf))
		entries++;

	sprite_data = malloc (sizeof(TLN_SpriteData)*entries);
	if (!sprite_data)
	{
		TLN_DeleteBitmap (bitmap);
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		fclose (pf);
		return NULL;
	}

	/* read entries */
	fseek (pf, 0, SEEK_SET);
	for (c=0; c<entries; c++)
	{
		char* equals;
		TLN_SpriteData* entry;

		/* lee linea */
		fgets (line, 64, pf);
		entry = &sprite_data[c];

		/* format SpriteSheetPacker: name = x y w h */
		equals = strchr (line, '=');
		if (equals != NULL)
		{
			sscanf (line, "%s = %d %d %d %d", entry->name, &entry->x, &entry->y, &entry->w, &entry->h);
			continue;
		}

		/* format Leshy SpriteSheet Tool csv: name,x,y,w,h */
		equals = strchr (line, ',');
		if (equals != NULL)
		{
			char* del = line;
			while (*del)
			{
				if (*del == ',')
					*del = ' ';
				del++;
			}
			sscanf (line, "%s %d %d %d %d", entry->name, &entry->x, &entry->y, &entry->w, &entry->h);
			continue;
		}
	}
	fclose (pf);

	/* create */
	spriteset = TLN_CreateSpriteset (bitmap, sprite_data, entries);
	
	/* free resources */
	free (sprite_data);
	
	if (spriteset)
		TLN_SetLastError (TLN_ERR_OK);
	else
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
	
	return spriteset;
}
