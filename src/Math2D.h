/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2018 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

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

#ifdef __cplusplus
extern "C"{
#endif

void Matrix3SetIdentity (Matrix3*);
void Matrix3Set (Matrix3*, math2d_t*);
void Matrix3Add (Matrix3*, Matrix3*);
void Matrix3Multiply (Matrix3*, Matrix3*);
void Matrix3SetRotation (Matrix3 *matrix, math2d_t angle);
void Matrix3SetTranslation (Matrix3 *matrix, math2d_t x, math2d_t y);
void Matrix3SetScale (Matrix3 *matrix, math2d_t sx, math2d_t sy);

void Point2DSet (Point2D*, math2d_t,math2d_t);
void Point2DAdd (Point2D*, Point2D*);
void Point2DMultiply (Point2D*, Matrix3*);

#if _MSC_VER < 1900
int roundf(float fvalue);
#endif

#ifdef __cplusplus
}
#endif

#endif