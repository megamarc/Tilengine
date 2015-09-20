/*
*******************************************************************************
	
	Public Tilengine source code - Megamarc 20 sep 2015
	http://www.tilengine.org
	
	File loading support

*******************************************************************************
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
		return NULL;

	/* carga */
	fseek (pf, 0, SEEK_END);
	size = ftell (pf);
	fseek (pf, 0, SEEK_SET);
	data = malloc (size);
	fread (data, size, 1, pf);
	fclose (pf);

	*out_size = size;
	return data;
}