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
#include "Tilengine.h"

#ifndef _LOAD_FILE_H
#define _LOAD_FILE_H

/* win32 replacement for unix strcasecmp() */
#if defined (_MSC_VER)
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

uint8_t* LoadFile (const char* filename, size_t* out_size);
FILE* FileOpen (const char* filename);
bool CheckFile (const char* filename);

#endif
