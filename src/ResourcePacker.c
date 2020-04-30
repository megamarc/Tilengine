/*
* ResPacker - General purpose asset packer with optional AES-128 encryption
* Copyright (C) 2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "aes.h"
#include "Hash.h"
#include "ResPack.h"

static uint8_t iv[AES_BLOCK_SIZE] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };
static uint32_t key[60] = { 0 };

/* private ResPack memory handler */
struct _ResPack
{
	FILE* pf;				/* file handler */
	uint32_t key[60];		/* scheduled AES key*/
	uint32_t num_entries;	/* number of assets */
	bool encrypted;			/* true if pack is encrypted */
	bool sorted;			/* can perform binary search */
	uint8_t seed;			/* seed used for perfect hash */
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
static void normalize_path(char* path, uint8_t seed)
{
	while (*path != 0)
	{
		if (*path == '\\')
			*path = '/';
		*path ^= seed;
		path += 1;
	}
}

/* calculates hash of file path and random seed */
uint32_t path2hash(const char* filename, uint8_t seed)
{
	char path[200];
	strncpy(path, filename, sizeof(path));
	path[sizeof(path) - 1] = 0;
	normalize_path(path, seed);
	return hash(0, path, strlen(path));
}

/* builds aes key schedule from string */
void build_aes_key(const char* string, uint32_t* output)
{
	uint8_t padded_key[16] = { 0 };
	int c = 0;
	while (*string)
	{
		padded_key[c] ^= *string;
		c = (c + 1) & 0x0F;
		string += 1;
	}
	aes_key_setup(padded_key, output, RESPACK_KEYSIZE);
}

int rpcompare(void const* a, void const* b)
{
	ResEntry* e1 = (ResEntry*)a;
	ResEntry* e2 = (ResEntry*)b;

	if (e1->id < e2->id)
		return -1;
	if (e1->id > e2->id)
		return 1;
	else
		return 0;
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
	id = path2hash(filename, 0);

	/* sorted: binary search */
	if (rp->sorted)
	{
		entry = bsearch(&id, rp->entries, rp->num_entries, sizeof(ResEntry), rpcompare);
		return entry;
	}

	/* unsorted: linear search */
	else
	{
		for (c = 0; c < rp->num_entries; c++)
		{
			if (rp->entries[c].id == id)
				return &rp->entries[c];
		}
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
		void* cypher = malloc(entry->pack_size);
		void* content = malloc(entry->pack_size);
		fread(cypher, entry->pack_size, 1, rp->pf);
		aes_decrypt_cbc(cypher, entry->pack_size, content, rp->key, RESPACK_KEYSIZE, iv);
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
	uint8_t version;
	uint8_t flags;

	/* open file */
	pf = fopen(filename, "rb");
	if (pf == NULL)
		return NULL;

	/* check header */
	fread(&res_header, sizeof(res_header), 1, pf);
	if (strcmp(res_header.id, RESPACK_ID))
	{
		fclose(pf);
		return NULL;
	}

	/* create object */
	size = sizeof(struct _ResPack) + sizeof(ResEntry)*res_header.num_regs;
	rp = calloc(size, 1);
	rp->num_entries = res_header.num_regs;
	rp->seed = res_header.seed;
	version = res_header.version & 0x0F;
	flags = res_header.version & 0xF0;
	if (version >= 1)
		rp->sorted = true;
	if (flags & RESPACK_ENCRYPTED)
		rp->encrypted = true;
	rp->pf = pf;
	
	if (rp->encrypted == true)
	{
		/* prepare AES-128 key*/
		if (key != NULL)
			build_aes_key(key, rp->key);
		else
		{
			free(rp);
			fclose(pf);
			printf("ResPack: must provide AES key for %s\n", filename);
			return NULL;
		}
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

	asset = malloc(sizeof(struct _ResAsset));
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
