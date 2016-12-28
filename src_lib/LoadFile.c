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
#include <malloc.h>
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