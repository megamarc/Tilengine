/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _TABLES_H
#define _TABLES_H

#ifdef __cplusplus
extern "C" {
#endif

	bool CreateBlendTables(void);
	void DeleteBlendTables(void);
	uint8_t* SelectBlendTable(TLN_Blend mode);

#ifdef __cplusplus
}
#endif

#define blendfunc(t,a,b) *(t  + ((a)<<8) + (b))

#endif
