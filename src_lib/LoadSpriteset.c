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
#include <malloc.h>
#include <string.h>
#include "Tilengine.h"

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
TLN_Spriteset TLN_LoadSpriteset (char *name)
{
	FILE *pf;
	char filename[64];
	char line[64];
	int entries = 0;
	TLN_Bitmap bitmap;
	TLN_Spriteset spriteset;
	TLN_Rect *rects, *rect;
	int c;

	/* load png file */
	sprintf (filename, "%s.png", name);
	bitmap = TLN_LoadBitmap (filename);
	if (!bitmap)
		return NULL;

	/* load txt file */
	sprintf (filename, "%s.txt", name);
	pf = fopen (filename, "rt");
	if (!pf)
	{
		TLN_DeleteBitmap (bitmap);
		TLN_SetLastError (TLN_ERR_FILE_NOT_FOUND);
		return NULL;
	}

	/* count lines */
	while (fgets (line, 64, pf))
		entries++;

	rects = malloc (sizeof(TLN_Rect)*entries);
	if (!rects)
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
		char* src;

		/* lee linea */
		fgets (line, 64, pf);
		src = line;
		while (*src!='=')
			src++;
		src++;
		rect = &rects[c];
		sscanf (src, "%d %d %d %d", &rect->x, &rect->y, &rect->w, &rect->h);
	}
	fclose (pf);

	/* create */
	spriteset = TLN_CreateSpriteset (
		entries, 
		rects, 
		TLN_GetBitmapPtr (bitmap, 0,0), 
		TLN_GetBitmapWidth (bitmap),
		TLN_GetBitmapHeight (bitmap), 
		TLN_GetBitmapPitch (bitmap), 
		TLN_ClonePalette(TLN_GetBitmapPalette(bitmap))
	);
	
	/* free resources */
	free (rects);
	TLN_DeleteBitmap (bitmap);
	
	if (spriteset)
		TLN_SetLastError (TLN_ERR_OK);
	else
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
	
	return spriteset;
}
