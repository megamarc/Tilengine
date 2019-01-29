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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LoadFile.h"

#define SLASH	  '/'
#define BACKSLASH '\\'
#define MAX_PATH	300

static char localpath[MAX_PATH] = ".";

/*!
 * \brief
 * Sets base path for TLN_LoadXXX functions.
 * 
 * \param path
 * Base path. Files will load at path/filename. Can be NULL
 */
void TLN_SetLoadPath (const char* path)
{
	size_t trailing;

	if (path)
		strncpy (localpath, path, MAX_PATH);
	else
		strncpy (localpath, ".", MAX_PATH);

	/* cut trailing separator */
	trailing = strlen (localpath) - 1;
	if (trailing > 0 && (localpath[trailing] == SLASH || localpath[trailing] == BACKSLASH))
		localpath[trailing] = 0;
}

FILE* FileOpen (const char* filename)
{
	FILE* pf;
	char path[255];
	char oldchar, newchar;
	char* p;
	
	sprintf (path, "%s/%s", localpath, filename);

	/* replace correct path separator */
	p = path;
#if defined (_MSC_VER)
	oldchar = SLASH;
	newchar = BACKSLASH;
#else
	oldchar = BACKSLASH;
	newchar = SLASH;
#endif
	while (*p != 0)
	{
		if (*p == oldchar)
			*p = newchar;
		p++;
	}

	pf = fopen (path, "rb");
	return pf;
}

/* generic load file into RAM buffer */
uint8_t* LoadFile (const char* filename, size_t* out_size)
{
	size_t size;
	FILE* pf;
	uint8_t* data;

	/* abre */
	pf = FileOpen (filename);
	if (!pf)
	{
		*out_size = 0;
		return NULL;
	}

	/* carga */
	fseek (pf, 0, SEEK_END);
	size = ftell (pf);
	fseek (pf, 0, SEEK_SET);
	data = malloc (size + 1);
	if (data)
	{
		fread (data, size, 1, pf);
		data[size] = 0;
		*out_size = size;
	}
	else
		*out_size = -1;

	fclose (pf);
	return data;
}

/* check if file exists */
bool CheckFile (const char* filename)
{
	FILE* pf;

	pf = FileOpen (filename);
	if (!pf)
		return false;

	fclose (pf);
	return true;
}
