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

	localpath[MAX_PATH - 1] = '\0';

	/* cut trailing separator */
	trailing = strlen (localpath) - 1;
	if (trailing > 0 && (localpath[trailing] == SLASH || localpath[trailing] == BACKSLASH))
		localpath[trailing] = 0;
}

FILE* FileOpen (const char* filename)
{
	FILE* pf;
	char path[MAX_PATH + 1];
	char oldchar, newchar;
	char* p;
	
#if (_MSC_VER) && (_MSC_VER < 1900)
	sprintf (path, "%s/%s", localpath, filename);
#else
	snprintf (path, sizeof(path), "%s/%s", localpath, filename);
#endif

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
uint8_t* LoadFile (const char* filename, ssize_t* out_size)
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
