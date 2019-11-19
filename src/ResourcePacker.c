/*
* ResPacker - General purpose asset packer with optional AES-128 encryption
* Copyright (C) 2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#pragma warning(disable : 4200)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "aes.h"
#include "Hash.h"
#include "ResPack.h"

#define KEY_SIZE	128
#define FILE_ID		"ResPack"

static uint8_t iv[AES_BLOCK_SIZE] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };
static uint32_t key[60] = { 0 };

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
	uint32_t reserved;		/* reserved, for future usage */
	uint32_t num_regs;		/* number of assets */
}
ResHeader;

/* private ResPack memory handler */
struct _ResPack
{
	FILE* pf;				/* file handler */
	uint32_t key[60];		/* scheduled AES key*/
	uint32_t num_entries;	/* number of assets */
	bool encrypted;			/* true if pack is encrypted */
	ResEntry entries[0];	/* array of ResEntry fields */
};

/* private opened asset memory handler */
struct _ResAsset
{
	FILE* pf;				/* file handler */
	uint32_t size;			/* actual size of asset */
	char filename[32];		/* temp filename */
};

/* lowercases path and uses forward slash */
static void normalize_path(char* path)
{
	while (*path != 0)
	{
		*path = tolower(*path);
		if (*path == '\\')
			*path = '/';
		path += 1;
	}
}

/* calculates hash of file path */
static uint32_t path2hash(const char* filename)
{
	char path[200];
	strncpy(path, filename, sizeof(path));
	path[sizeof(path) - 1] = 0;
	normalize_path(path);
	return hash(0, path, strlen(path));
}

/* finds given entry inside a resource pack */
static ResEntry* find_entry(ResPack rp, const char* filename)
{
	uint32_t id;
	uint32_t c;
	ResEntry* entry = NULL;

	/* validate params */
	if (rp == NULL || filename == NULL)
		return NULL;

	/* find entry */
	id = path2hash(filename);
	for (c = 0; c < rp->num_entries; c++)
	{
		if (rp->entries[c].id == id)
			return &rp->entries[c];
	}
	return NULL;
}

/* loads given asset to memory buffer */
static void* load_asset(ResPack rp, ResEntry* entry)
{
	uint32_t crc;
	void* buffer = malloc(entry->data_size);
	if (buffer == NULL)
		return NULL;

	fseek(rp->pf, entry->offset, SEEK_SET);
	if (rp->encrypted == true)
	{
		uint8_t* cypher = (uint8_t*)malloc(entry->pack_size);
		uint8_t* content = (uint8_t*)malloc(entry->pack_size);
		fread(cypher, entry->pack_size, 1, rp->pf);
		aes_decrypt_cbc(cypher, entry->pack_size, content, rp->key, KEY_SIZE, iv);
		memcpy(buffer, content, entry->data_size);
		free(content);
		free(cypher);
	}
	else
		fread(buffer, entry->data_size, 1, rp->pf);

	/* validate integrity */
	crc = hash(0, buffer, entry->data_size);
	if (crc != entry->crc)
	{
		free(buffer);
		buffer = NULL;
	}

	return buffer;
}

/* opens a resource pack */
ResPack ResPack_Open(const char* filename, const char* key)
{
	ResPack rp = NULL;
	ResHeader res_header;
	FILE* pf;
	uint32_t size;

	/* open file */
	pf = fopen(filename, "rb");
	if (pf == NULL)
		return NULL;

	/* check header */
	fread(&res_header, sizeof(res_header), 1, pf);
	if (strcmp(res_header.id, FILE_ID))
	{
		fclose(pf);
		return NULL;
	}

	/* create object */
	size = sizeof(struct _ResPack) + sizeof(ResEntry)*res_header.num_regs;
	rp = (ResPack)calloc(size, 1);
	rp->num_entries = res_header.num_regs;
	rp->pf = pf;
	
	/* prepare AES-128 key*/
	if (key != NULL)
	{
		char padded_key[16] = { 0 };
		strncpy(padded_key, key, sizeof(padded_key));
		aes_key_setup((uint8_t*)padded_key, rp->key, KEY_SIZE);
		rp->encrypted = true;
	}

	/* load index */
	fread(rp->entries, sizeof(ResEntry), rp->num_entries, pf);
	return rp;
}

/* closes an opened resource pack */
void ResPack_Close(ResPack rp)
{
	if (rp != NULL)
	{
		if (rp->pf != NULL)
			fclose(rp->pf);
		free(rp);
	}
}

/* loads contents of asset to memory, returns actual size*/
void* ResPack_LoadAsset(ResPack rp, const char* filename, uint32_t* size)
{
	void* asset;
	ResEntry* entry = NULL;

	/* validate & find */
	entry = find_entry(rp, filename);
	if (entry == NULL)
		return NULL;

	/* load */
	asset = load_asset(rp, entry);
	if (size != NULL)
		*size = entry->data_size;
	
	return asset;
}

/* creates a temporal file and opens it */
ResAsset ResPack_OpenAsset(ResPack rp, const char* filename)
{
	ResEntry* entry = NULL;
	ResAsset asset;
	void* content;

	/* validate & find */
	entry = find_entry(rp, filename);
	if (entry == NULL)
		return NULL;

	content = load_asset(rp, entry);
	if (content == NULL)
		return NULL;

	asset = (ResAsset)malloc(sizeof(struct _ResAsset));
	sprintf(asset->filename, "_tmp%d", entry->id);
	asset->pf = fopen(asset->filename, "wb");
	fwrite(content, entry->data_size, 1, asset->pf);
	fclose(asset->pf);
	asset->pf = fopen(asset->filename, "rb");
	asset->size = entry->data_size;
	return asset;
}

/* returns file handler of an opened asset */
FILE* ResPack_GetAssetFile(ResAsset asset)
{
	if (asset != NULL)
		return asset->pf;
	else
		return NULL;
}

/* returns actual size of an opened asset */
uint32_t ResPack_GetAssetSize(ResAsset asset)
{
	if (asset != NULL)
		return asset->size;
	else
		return 0;
}

/* closes asset, deletes temporal filename */
void ResPack_CloseAsset(ResAsset asset)
{
	if (asset != NULL)
	{
		if (asset->pf != NULL)
			fclose(asset->pf);
		remove(asset->filename);
		free(asset);
	}
}

/* loads file to memory, padded to 16 byte boundary for AES */
static void* load_file(const char* filename, uint32_t padding, uint32_t* data_size, uint32_t* pack_size)
{
	FILE* pf;
	uint32_t size;
	uint32_t pad_size;
	void* buffer;

	pf = fopen(filename, "rb");
	if (pf == NULL)
		return NULL;

	fseek(pf, 0, SEEK_END);
	size = ftell(pf);
	if (padding > 1)
		pad_size = ((size + padding - 1) / padding) * padding;
	else
		pad_size = size;
	fseek(pf, 0, SEEK_SET);

	buffer = malloc(pad_size);
	memset(buffer, 0, pad_size);
	fread(buffer, size, 1, pf);
	fclose(pf);

	*data_size = size;
	*pack_size = pad_size;
	return buffer;
}
