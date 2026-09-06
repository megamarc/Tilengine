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
#include "Tilengine.h"

#ifndef _LOAD_FILE_H
#define _LOAD_FILE_H

/* win32 replacement for unix strcasecmp() */
#if defined (_MSC_VER)
#include <BaseTsd.h>
#define strcasecmp _stricmp
typedef SSIZE_T ssize_t;
#else
#include <strings.h>
#ifdef __linux__
#include <sys/types.h> // ssize_t
#endif
#endif

typedef struct
{
	char path[200];
	char name[200];
	char ext[16];
}
FileInfo;

#ifdef __cplusplus
extern "C" {
#endif

	void* LoadFile(const char* filename, ssize_t* out_size);
	void FileClose(FILE* pf);
	FILE* FileOpen(const char* filename);
	bool CheckFile(const char* filename);
	void SplitFilename(const char* filename, FileInfo* fileinfo);
	void BuildFilePath(char* full_path, int len, const char* path, const char* name, const char* ext);

#ifdef __cplusplus
}
#endif

#endif
