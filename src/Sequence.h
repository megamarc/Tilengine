/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _SEQUENCE_H
#define _SEQUENCE_H

#include "Object.h"
#include "crc32.h"

/* ciclo de color */
struct Strip
{
	int delay;
	int timer;
	int t0;
	uint8_t first;
	uint8_t count;
	uint8_t dir;
	uint8_t pos;
};

/* secuencia de sprites y tiles */
struct Sequence
{
	DEFINE_OBJECT;
	uint32_t hash;
	int count;
	int target;
	char name[32];
	struct Sequence* next;
	uint8_t data[0];	/* array de Frame o Strip */
};

#endif
