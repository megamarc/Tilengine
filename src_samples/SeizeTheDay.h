#ifndef _SEIZE_THE_DAY_H
#define _SEIZE_THE_DAY_H

#include "Tilengine.h"

typedef struct
{
	int seconds;
	const char* palette;
}
Timeline;

typedef struct
{
	const char* bitmap;
	const char* sequence;
	Timeline* timeline;
}
Scene;

#endif