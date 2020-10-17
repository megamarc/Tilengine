/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _MATH_2D_H
#define _MATH_2D_H

typedef float math2d_t;
//typedef int math2d_t;

typedef struct
{
	math2d_t m11, m12, m13;
	math2d_t m21, m22, m23;
	math2d_t m31, m32, m33;
}
Matrix3;

typedef struct
{
	math2d_t x,y;
}
Point2D;

/* fixed point helper */
typedef int fix_t;
#define FIXED_BITS	16
#define float2fix(f)	(fix_t)(f*(1 << FIXED_BITS))
#define int2fix(i)		((int)(i) << FIXED_BITS)
#define fix2int(f)		((int)(f) >> FIXED_BITS)
#define fix2float(f)	(float)(f)/(1 << FIXED_BITS)

#ifdef __cplusplus
extern "C" {
#endif

	void Matrix3SetIdentity(Matrix3*);
	void Matrix3Set(Matrix3*, math2d_t*);
	void Matrix3Add(Matrix3*, Matrix3*);
	void Matrix3Multiply(Matrix3*, Matrix3*);
	void Matrix3SetRotation(Matrix3 *matrix, math2d_t angle);
	void Matrix3SetTranslation(Matrix3 *matrix, math2d_t x, math2d_t y);
	void Matrix3SetScale(Matrix3 *matrix, math2d_t sx, math2d_t sy);

	void Point2DSet(Point2D*, math2d_t, math2d_t);
	void Point2DAdd(Point2D*, Point2D*);
	void Point2DMultiply(Point2D*, Matrix3*);

#if defined _MSC_VER && _MSC_VER < 1900
	int roundf(float fvalue);
#endif

#ifdef __cplusplus
}
#endif

#endif