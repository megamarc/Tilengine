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

#define RESPACK_ID			"ResPack"
#define RESPACK_KEYSIZE		128
#define RESPACK_ENCRYPTED	0x80

/* asset descriptor register */
typedef struct
{
	uint32_t	id;			/* hash identifier derived from original file path */
	uint32_t	crc;		/* crc of asset content to verify integrity */
	uint32_t	data_size;	/* actual size of asset */
	uint32_t	pack_size;	/* size padded to 16-byte boundary, required by AES */
	uint32_t	offset;		/* start of asset content */
}
ResEntry;

/* ResPack file header*/
typedef struct
{
	char id[8];				/* file header, must be "ResPack" null-terminated */
	uint8_t version;		/* file version number */
	uint8_t seed;			/* seed used for perfect hashing */
	uint8_t reserved[2];	/* reserved, for future usage */
	uint32_t num_regs;		/* number of assets */
}
ResHeader;

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

#ifdef __cplusplus
}
#endif

#endif
