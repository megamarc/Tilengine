/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _TILEMAP_H
#define _TILEMAP_H

#include "Object.h"
#include "Tileset.h"

/* mapa */
struct Tilemap
{
	DEFINE_OBJECT;
	int		rows;		/* filas */
	int		cols;		/* columnas */
	int		maxindex;	/* nº de tile más alto */
	int		bgcolor;	/* color de fondo */
	struct Tileset* tileset; /* tileset asociado (si hay) */
	Tile	tiles[];
};

#endif
