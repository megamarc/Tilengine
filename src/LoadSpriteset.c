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
#include <stdlib.h>
#include <string.h>
#include "Tilengine.h"
#include "LoadFile.h"
#include "cJSON.h"

/* loads txt format: name = x y w h */
/* loads csv format: name,x,y,w,h */
static TLN_SpriteData* load_txt_csv(const char* filename, int* num_entries)
{
	TLN_SpriteData* data = NULL;
	TLN_SpriteData* entry;
	char line[200];
	FILE* pf = FileOpen(filename);
	if (!pf)
		return NULL;

	/* count lines */
	*num_entries = 0;
	while (fgets(line, sizeof(line), pf))
		*num_entries += 1;
	fseek(pf, 0, SEEK_SET);

	data = (TLN_SpriteData*)calloc(*num_entries, sizeof(TLN_SpriteData));
	entry = data;
	while (fgets(line, sizeof(line), pf))
	{
		if (strchr(line, '='))
			sscanf(line, "%s = %d %d %d %d", entry->name, &entry->x, &entry->y, &entry->w, &entry->h);
		else if (strchr(line, ','))
			sscanf(line, "%64[^,],%d,%d,%d,%d", entry->name, &entry->x, &entry->y, &entry->w, &entry->h);
		entry += 1;
	}
	FileClose(pf);
	return data;
}

static void parse_json_frame(cJSON* item, TLN_SpriteData* entry)
{
	cJSON* field;
	cJSON* frame;

	field = cJSON_GetObjectItem(item, "filename");
	if (field)
		strncpy(entry->name, cJSON_GetStringValue(field), sizeof(entry->name));

	frame = cJSON_GetObjectItem(item, "frame");
	if (frame)
	{
		field = cJSON_GetObjectItem(frame, "x");
		if (field)
			entry->x = field->valueint;
		field = cJSON_GetObjectItem(frame, "y");
		if (field)
			entry->y = field->valueint;
		field = cJSON_GetObjectItem(frame, "w");
		if (field)
			entry->w = field->valueint;
		field = cJSON_GetObjectItem(frame, "h");
		if (field)
			entry->h = field->valueint;
	}
}

/* loads json array format: frames[{filename, frame: {x,y,w,h}}] */
static TLN_SpriteData* load_json(const char* filename, int* num_entries)
{
	TLN_SpriteData* data = NULL;
	TLN_SpriteData* entry;
	ssize_t file_size = 0;
	cJSON* root;
	cJSON* frames;
	cJSON* item;
	char* buffer = (char*)LoadFile(filename, &file_size);
	if (!buffer)
		return NULL;

	root = cJSON_Parse(buffer);
	free(buffer);
	if (!root)
		return NULL;

	frames = cJSON_GetObjectItem(root, "frames");
	if (frames)
	{
		int array_size = cJSON_GetArraySize(frames);
		data = (TLN_SpriteData*)calloc(array_size, sizeof(TLN_SpriteData));

		entry = data;
		cJSON_ArrayForEach(item, frames)
		{
			parse_json_frame(item, entry);
			entry += 1;
		}
		*num_entries = (int)array_size;
	}
	cJSON_free(root);
	return data;
}

/*!
 * \brief Loads a spriteset from an image png and its associated atlas descriptor
 * \param name Base name of the files containing the spriteset, with or without .png extension
 * \returns Reference to the newly loaded spriteset or NULL if error
 * 
 * \remarks
 * The spriteset comes in a pair of files: an image file (bmp or png) and a standarized atlas descriptor (json, csv or txt)
 * The supported json format is the array.
 */
TLN_Spriteset TLN_LoadSpriteset (const char* name)
{
	FileInfo fileinfo = { 0 };
	char filename[200] = { 0 };
	char line[64] = { 0 };
	int entries = 0;
	TLN_Bitmap bitmap = NULL;
	TLN_Spriteset spriteset = NULL;
	TLN_SpriteData *sprite_data = NULL;

	SplitFilename(name, &fileinfo);
	
	/* load image file: assume png if no extension */
	if (fileinfo.ext[0] != 0)
		sprintf(filename, "%s", name);
	else
		BuildFilePath(filename, sizeof(filename), fileinfo.path, fileinfo.name, "png");
	bitmap = TLN_LoadBitmap (filename);
	if (!bitmap)
		return NULL;

	/* try different atlas formats: json, csv, txt */
	BuildFilePath(filename, sizeof(filename), fileinfo.path, fileinfo.name, "json");
	sprite_data = load_json(filename, &entries);
	if (sprite_data == NULL)
	{
		BuildFilePath(filename, sizeof(filename), fileinfo.path, fileinfo.name, "csv");
		sprite_data = load_txt_csv(filename, &entries);
	}
	if (sprite_data == NULL)
	{
		BuildFilePath(filename, sizeof(filename), fileinfo.path, fileinfo.name, "txt");
		sprite_data = load_txt_csv(filename, &entries);
	}

	/* no compatible atlas found */
	if (sprite_data == NULL)
	{
		TLN_DeleteBitmap(bitmap);
		TLN_SetLastError(TLN_ERR_FILE_NOT_FOUND);
		return NULL;
	}

	/* create */
	spriteset = TLN_CreateSpriteset (bitmap, sprite_data, entries);
	
	if (spriteset)
		TLN_SetLastError (TLN_ERR_OK);
	else
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
	
	free(sprite_data);
	return spriteset;
}
