/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Tilengine.h"
#include "simplexml.h"
#include "LoadFile.h"

#define MAX_COLOR_STRIP	32

/* load manager */
struct
{
	TLN_SequencePack sp;
	char name[16];
	int target;
	int count;
	int delay;
	TLN_SequenceFrame frames[100];
	TLN_ColorStrip strips[MAX_COLOR_STRIP];
}
static loader;

static bool ishex (char dat);

/* XML parser callback */
static void* handler (SimpleXmlParser parser, SimpleXmlEvent evt, 
	const char* szName, const char* szAttribute, const char* szValue)
{
	TLN_Sequence sequence = NULL;

	switch (evt)
	{
	case ADD_SUBTAG:
		if (!strcasecmp(szName, "cycle"))
		{
			loader.count = 0;
			memset (loader.strips, 0, sizeof(loader.strips));
		}
		break;

	case ADD_ATTRIBUTE:
		if (!strcasecmp(szName, "sequence"))
		{
			if (!strcasecmp(szAttribute, "name"))
				strncpy (loader.name, szValue, sizeof(loader.name));
			else if (!strcasecmp(szAttribute, "delay"))
				loader.delay = atoi(szValue);
			else if (!strcasecmp(szAttribute, "first") || !strcasecmp(szAttribute, "target"))
				loader.target = atoi(szValue);
			else if (!strcasecmp(szAttribute, "count"))
				loader.count = atoi(szValue);
		}
		else if (!strcasecmp(szName, "cycle"))
		{
			if (!strcasecmp(szAttribute, "name"))
				strncpy (loader.name, szValue, sizeof(loader.name));
		}
		else if (!strcasecmp(szName, "strip"))
		{
			if (!strcasecmp(szAttribute, "delay"))
				loader.strips[loader.count].delay = atoi(szValue);
			else if (!strcasecmp(szAttribute, "first"))
				loader.strips[loader.count].first = (uint8_t)atoi(szValue);
			else if (!strcasecmp(szAttribute, "count"))
				loader.strips[loader.count].count = (uint8_t)atoi(szValue);
			else if (!strcasecmp(szAttribute, "dir"))
				loader.strips[loader.count].dir = (uint8_t)atoi(szValue);
		}

		loader.name[sizeof(loader.name) - 1] = '\0';

		break;

	case FINISH_ATTRIBUTES:
		if (!strcasecmp(szName, "strip"))
		{
			if (loader.strips[loader.count].delay != 0)
				loader.count++;
		}
		break;

	case ADD_CONTENT:
		if (!strcasecmp(szName, "sequence"))
		{
			char* ptr = (char*)szValue;

			loader.count = 0;
			while (*ptr)
			{
				int value;

				/* find number */
				while (*ptr && !ishex(*ptr) && *ptr!='#')
					ptr++;
				if (!*ptr)
					continue;

				/* copy and find end of number */
				if (*ptr=='#')
				{
					ptr++;
					sscanf (ptr, "%x", &value);
				}
				else
					sscanf (ptr, "%u", &value);

				loader.frames[loader.count].index = value;
				loader.frames[loader.count].delay = loader.delay;
				loader.count++;
				while (*ptr && ishex(*ptr))
					ptr++;
			}
		}
		break;

	case FINISH_TAG:
		if (!strcasecmp(szName, "sequence"))
			sequence = TLN_CreateSequence (loader.name, loader.target, loader.count, loader.frames);
		else if (!strcasecmp (szName, "cycle"))
			sequence = (TLN_Sequence)TLN_CreateCycle (loader.name, loader.count, loader.strips);
		if (sequence)
			TLN_AddSequenceToPack (loader.sp, sequence);
		break;
	}
	return handler;
}

/*!
 * \brief
 * Loads a sqx file containing one or more sequences
 * 
 * \param filename
 * SQX filename with the sequences to load
 * 
 * \returns
 * Reference to the newly created TLN_SequencePack() or NULL if error
 *
 * \remarks
 * A SQX file can contain many sequences. This function loads all of them
 * inside a single TLN_SequencePack(). Individual sequences can be later
 * queried with TLN_FindSequence() 
 *
 * \see
 * TLN_FindSequence()
 */
TLN_SequencePack TLN_LoadSequencePack (const char* filename)
{
	SimpleXmlParser parser;
	ssize_t size;
	uint8_t *data;

	/* load file */
	data = (uint8_t*)LoadFile (filename, &size);
	if (!data)
	{
		if (size == 0)
			TLN_SetLastError (TLN_ERR_FILE_NOT_FOUND);
		else if (size == -1)
			TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}
	
	/* create empty */
	loader.sp = TLN_CreateSequencePack ();
	if (!loader.sp)
	{
		free (data);
		return NULL;
	}

	/* parse */
	parser = simpleXmlCreateParser ((char*)data, (long)size);
	if (parser != NULL)
	{
		if (simpleXmlParse(parser, handler) != 0)
		{
			printf("parse error on line %li:\n%s\n", 
				simpleXmlGetLineNumber(parser), simpleXmlGetErrorDescription(parser));
			simpleXmlDestroyParser(parser);
			free (data);
			TLN_SetLastError (TLN_ERR_WRONG_FORMAT);
			return NULL;
		}
		else
			TLN_SetLastError (TLN_ERR_OK);
	}
	else
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);

	simpleXmlDestroyParser(parser); 
	free (data);
	return loader.sp;
}

static bool ishex (char dat)
{
	if (dat>='0' && dat<='9')
		return true;

	if (dat>='A' && dat<='F')
		return true;

	if (dat>='a' && dat<='f')
		return true;

	return false;
}
