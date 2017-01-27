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
 * \brief File loading support
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
#include "LoadFile.h"

#if defined (_MSC_VER)
	#define DASH "\\"
#else
	#define DASH "//"
#endif

static char localpath[255] = ".";

/*!
 * \brief
 * Sets base path for TLN_LoadXXX functions.
 * 
 * \param path
 * Base path. Files will load at path/filename. Can be NULL
 */
void TLN_SetLoadPath (const char* path)
{
	if (path)
		strcpy (localpath, path);
	else
		strcpy (localpath, ".");
}

FILE* FileOpen (const char* filename)
{
	FILE* pf;
	char path[255];
	
	sprintf (path, "%s" DASH "%s", localpath, filename);
	pf = fopen (path, "rb");
	return pf;
}

/* generic load file into RAM buffer */
BYTE* LoadFile (const char* filename, size_t* out_size)
{
	size_t size;
	FILE* pf;
	BYTE* data;

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
	data = malloc (size);
	if (data)
	{
		fread (data, size, 1, pf);
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
