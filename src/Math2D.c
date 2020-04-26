/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <math.h>
#include "Math2D.h"

// matriz identidad
static math2d_t identity3[] =
{
	1,0,0,
	0,1,0,
	0,0,1
};

// inicializa matriz con valores indicados
void Matrix3Set (Matrix3 *matrix, math2d_t *data)
{
	matrix->m11 = *data++;
	matrix->m12 = *data++;
	matrix->m13 = *data++;
	matrix->m21 = *data++;
	matrix->m22 = *data++;
	matrix->m23 = *data++;
	matrix->m31 = *data++;
	matrix->m32 = *data++;
	matrix->m33 = *data++;
}

// carga matriz identidad
void Matrix3SetIdentity (Matrix3 *matrix)
{
	Matrix3Set (matrix, identity3);
}

// carga matriz de rotación
void Matrix3SetRotation (Matrix3 *matrix, math2d_t ang)
{
	float angle = ang*3.1415926f/180;
	math2d_t data[] = 
	{
		(math2d_t)cos(angle), (math2d_t)sin(angle), 0,
		(math2d_t)-sin(angle), (math2d_t)cos(angle), 0,
		0, 0, 1
	};
	Matrix3Set (matrix, data);
}

// carga matriz de traslación
void Matrix3SetTranslation (Matrix3 *matrix, math2d_t x, math2d_t y)
{
	math2d_t data[] = 
	{
		1,0,x,
		0,1,y,
		0,0,1
	};
	Matrix3Set (matrix, data);
}

// carga matriz de escala
void Matrix3SetScale (Matrix3 *matrix, math2d_t sx, math2d_t sy)
{
	math2d_t data[] = 
	{
		sx,0,0,
		0,sy,0,
		0,0,1
	};
	Matrix3Set (matrix, data);
}

// suma con otra matriz
void Matrix3Add (Matrix3 *matrix, Matrix3 *add)
{
	matrix->m11 += add->m11;
	matrix->m12 += add->m12;
	matrix->m13 += add->m13;
	matrix->m21 += add->m21;
	matrix->m22 += add->m22;
	matrix->m23 += add->m23;
	matrix->m31 += add->m31;
	matrix->m32 += add->m32;
	matrix->m33 += add->m33;
}

// multiplica por otra matriz
void Matrix3Multiply (Matrix3 *matrix, Matrix3 *mul)
{	
	Matrix3 tmp;

	tmp.m11 = mul->m11*matrix->m11 + mul->m12*matrix->m21 + mul->m13*matrix->m31;
	tmp.m12 = mul->m11*matrix->m12 + mul->m12*matrix->m22 + mul->m13*matrix->m32;
	tmp.m13 = mul->m11*matrix->m13 + mul->m12*matrix->m23 + mul->m13*matrix->m33;

	tmp.m21 = mul->m21*matrix->m11 + mul->m22*matrix->m21 + mul->m23*matrix->m31;
	tmp.m22 = mul->m21*matrix->m12 + mul->m22*matrix->m22 + mul->m23*matrix->m32;
	tmp.m23 = mul->m21*matrix->m13 + mul->m22*matrix->m23 + mul->m23*matrix->m33;

	tmp.m31 = mul->m31*matrix->m11 + mul->m32*matrix->m21 + mul->m33*matrix->m31;
	tmp.m32 = mul->m31*matrix->m12 + mul->m32*matrix->m22 + mul->m33*matrix->m32;
	tmp.m33 = mul->m31*matrix->m13 + mul->m32*matrix->m23 + mul->m33*matrix->m33;

	matrix->m11 = tmp.m11;
	matrix->m12 = tmp.m12;
	matrix->m13 = tmp.m13;
	matrix->m21 = tmp.m21;
	matrix->m22 = tmp.m22;
	matrix->m23 = tmp.m23;
	matrix->m31 = tmp.m31;
	matrix->m32 = tmp.m32;
	matrix->m33 = tmp.m33;
}

// inicializa punto
void Point2DSet (Point2D *point, math2d_t x, math2d_t y)
{
	point->x = x;
	point->y = y;
}

// suma otro punto
void Point2DAdd (Point2D *point, Point2D *add)
{
	point->x += add->x;
	point->y += add->y;
}

// multiplica por una matriz
void Point2DMultiply (Point2D *point, Matrix3 *matrix)
{
	Point2D tmp;
	Point2DSet (&tmp, 
		matrix->m11*point->x + matrix->m12*point->y + matrix->m13,
		matrix->m21*point->x + matrix->m22*point->y + matrix->m23
		);

	point->x = tmp.x;
	point->y = tmp.y;
}

// roundf()
#if defined _MSC_VER && _MSC_VER < 1900
int roundf(float fvalue)
{
	int value = (int)fvalue;
	fvalue -= value;
	if (fvalue > 0.5f)
		value += 1;
	else if (fvalue < -0.5f)
		value -= 1;
	return value;
}
#endif
