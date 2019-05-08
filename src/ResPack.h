/*
* ResPacker - General purpose asset packer with AES-128 encryption
* Copyright (C) 2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _RESPACK_H
#define _RESPACK_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct _ResPack* ResPack;
typedef struct _ResAsset* ResAsset;

#ifdef __cplusplus
extern"C"{
#endif

ResPack		ResPack_Open(const char* filename, const char* key);
void		ResPack_Close(ResPack rp);
void*		ResPack_LoadAsset(ResPack rp, const char* filename, uint32_t* size);
ResAsset	ResPack_OpenAsset(ResPack rp, const char* filename);
FILE*		ResPack_GetAssetFile(ResAsset asset);
uint32_t	ResPack_GetAssetSize(ResAsset asset);
void		ResPack_CloseAsset(ResAsset asset);
int			ResPack_Build(const char* filelist, const char* aes_key);

#ifdef __cplusplus
}
#endif

#endif
