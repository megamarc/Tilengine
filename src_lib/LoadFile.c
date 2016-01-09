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
#include "LoadFile.h"

/* generic load file into RAM buffer */
BYTE* LoadFile (char* filename, size_t* out_size)
{
	size_t size;
	FILE* pf;
	BYTE* data;

	/* abre */
	pf = fopen (filename, "rb");
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
bool CheckFile (char* filename)
{
	FILE* pf;

	pf = fopen (filename, "rb");
	if (!pf)
		return false;

	fclose (pf);
	return true;
}