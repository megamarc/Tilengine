/*
*******************************************************************************
	
	Public Tilengine source code - Megamarc 20 sep 2015
	http://www.tilengine.org
	
	XML sequences file loader (.sqx)

*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Tilengine.h"
#include "simplexml.h"
#include "Loadfile.h"

#define MAX_COLOR_STRIP	32

/* load manager */
struct
{
	TLN_SequencePack sp;
	char name[16];
	int first;
	int count;
	int delay;
	int frames[100];
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
		if (!strcmp(szName, "cycle"))
		{
			loader.count = 0;
			memset (loader.strips, 0, sizeof(loader.strips));
		}
		break;

	case ADD_ATTRIBUTE:
		if (!strcmp(szName, "sequence"))
		{
			if (!strcmp(szAttribute, "name"))
				strcpy (loader.name, szValue);
			else if (!strcmp(szAttribute, "delay"))
				loader.delay = atoi(szValue);
			else if (!strcmp(szAttribute, "first"))
				loader.first = atoi(szValue);
			else if (!strcmp(szAttribute, "count"))
				loader.count = atoi(szValue);
		}
		else if (!strcmp(szName, "cycle"))
		{
			if (!strcmp(szAttribute, "name"))
				strcpy (loader.name, szValue);
		}
		else if (!strcmp(szName, "strip"))
		{
			if (!strcmp(szAttribute, "delay"))
				loader.strips[loader.count].delay = atoi(szValue);
			else if (!strcmp(szAttribute, "first"))
				loader.strips[loader.count].first = (BYTE)atoi(szValue);
			else if (!strcmp(szAttribute, "count"))
				loader.strips[loader.count].count = (BYTE)atoi(szValue);
			else if (!strcmp(szAttribute, "dir"))
				loader.strips[loader.count].dir = (BYTE)atoi(szValue);
		}
		break;

	case FINISH_ATTRIBUTES:
		if (!strcmp(szName, "strip"))
		{
			if (loader.strips[loader.count].delay != 0)
				loader.count++;
		}
		break;

	case ADD_CONTENT:
		if (!strcmp(szName, "sequence"))
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

				loader.frames[loader.count++] = value;
				while (*ptr && ishex(*ptr))
					ptr++;
			}
		}
		break;

	case FINISH_TAG:
		if (!strcmp(szName, "sequence"))
			sequence = TLN_CreateSequence (loader.name, loader.delay, loader.first, loader.count, loader.frames);
		else if (!strcmp (szName, "cycle"))
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
TLN_SequencePack TLN_LoadSequencePack (char* filename)
{
	SimpleXmlParser parser;
	size_t size;
	BYTE *data;

	/* load file */
	data = LoadFile (filename, &size);
	if (!data)
		return NULL;
	
	/* parse */
	loader.sp = TLN_CreateSequencePack ();
	parser = simpleXmlCreateParser (data, (long)size);
	if (parser != NULL)
	{
		if (simpleXmlParse(parser, handler) != 0)
		{
			printf("parse error on line %li:\n%s\n", 
				simpleXmlGetLineNumber(parser), simpleXmlGetErrorDescription(parser));
			free (data);
			return NULL;
		}
	}

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
