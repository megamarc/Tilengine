/*
*******************************************************************************
	
	Public Tilengine source code - Megamarc 20 sep 2015
	http://www.tilengine.org
	
	File loading support

*******************************************************************************
*/

#include "Tilengine.h"

/* win32 replacement for unix strcasecmp() */
#if defined (_MSC_VER)
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

BYTE* LoadFile (char* filename, size_t* out_size);
bool CheckFile (char* filename);