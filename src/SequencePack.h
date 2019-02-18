/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _SEQUENCE_PACK_H
#define _SEQUENCE_PACK_H

#include "Object.h"
#include "Sequence.h"


struct SequencePack
{
	DEFINE_OBJECT;
	int num_sequences;
	TLN_Sequence sequences;
	TLN_Sequence last;
};

#endif
