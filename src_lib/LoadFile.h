/*
*******************************************************************************
	
	Public Tilengine source code - Megamarc 20 sep 2015
	http://www.tilengine.org
	
	File loading support

*******************************************************************************
*/

#include <stdio.h>
#include "Tilengine.h"

/* win32 replacement for unix strcasecmp() */
#if defined (_MSC_VER)
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

BYTE* LoadFile (const char* filename, size_t* out_size);
FILE* FileOpen (const char* filename);
bool CheckFile (const char* filename);
