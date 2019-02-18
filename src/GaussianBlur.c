/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#include <string.h>
#include "Tilengine.h"

typedef struct
{
	uint8_t r,g,b,a;
}
RGBQuad;

static void hblur (uint8_t* src, uint8_t* dst, int width, int height, int pitch, int radius);
static void vblur (uint8_t* src, uint8_t* dst, int width, int height, int pitch, int radius);

void GaussianBlur (uint8_t* src, uint8_t* dst, int width, int height, int pitch, int radius)
{
	int c;

	memset (dst, 0, pitch * height);
	for (c=0; c<2; c++)
	{
		hblur (src, dst, width, height, pitch, radius);
		vblur (dst, src, width, height, pitch, radius);
	}
}

/* horizontal blur */
static void hblur (uint8_t* src, uint8_t* dst, int width, int height, int pitch, int radius)
{
	int c, y;
	const int half_radius = radius/2;
	const int right = width - half_radius;
	RGBQuad* pixel_sub;	/* left pixel (source, substract) */
	RGBQuad* pixel_add;	/* right pixel (source, add) */
	RGBQuad* pixel_mid;	/* center pixel (destination) */
	radius++;

	for (y=0; y<height; y++)
	{
		int sum[3] = {0};
		RGBQuad* src_pixel = (RGBQuad*)(src + y*pitch);
		RGBQuad* dst_pixel = (RGBQuad*)(dst + y*pitch);

		pixel_add = pixel_sub = src_pixel;
		pixel_mid = dst_pixel;

		/* firs half-add */
		for (c=0; c<=half_radius; c++)
		{
			sum[0] += pixel_add->r;
			sum[1] += pixel_add->g;
			sum[2] += pixel_add->b;
			pixel_add++;
		}

		/* left half-radius: only adds */
		for (c=0; c<half_radius; c++)
		{
			pixel_mid->r = sum[0] / radius;
			pixel_mid->g = sum[1] / radius;
			pixel_mid->b = sum[2] / radius;
			sum[0] += pixel_add->r;
			sum[1] += pixel_add->g;
			sum[2] += pixel_add->b;
			pixel_mid++;
			pixel_add++;
		}

		/* central part: full radius */
		for (; c<right; c++)
		{
			pixel_mid->r = sum[0] / radius;
			pixel_mid->g = sum[1] / radius;
			pixel_mid->b = sum[2] / radius;
			sum[0] = sum[0] + pixel_add->r - pixel_sub->r;
			sum[1] = sum[1] + pixel_add->g - pixel_sub->g;
			sum[2] = sum[2] + pixel_add->b - pixel_sub->b;
			pixel_mid++;
			pixel_add++;
			pixel_sub++;
		}

		/* right half-radius: only substracts */
		for (; c<width; c++)
		{
			pixel_mid->r = sum[0] / radius;
			pixel_mid->g = sum[1] / radius;
			pixel_mid->b = sum[2] / radius;
			sum[0] -= pixel_sub->r;
			sum[1] -= pixel_sub->g;
			sum[2] -= pixel_sub->b;
			pixel_mid++;
			pixel_sub++;
		}
	}
}

/* vertical blur */
static void vblur (uint8_t* src, uint8_t* dst, int width, int height, int pitch, int radius)
{
	int c, x;
	const int half_radius = radius/2;
	const int bottom = height - half_radius;
	RGBQuad* pixel_sub;	/* top pixel (origen, resta) */
	RGBQuad* pixel_add;	/* bottom pixel (origen, suma) */
	RGBQuad* pixel_mid;	/* center pixel (destino) */
	radius++;

	for (x=0; x<width; x++)
	{
		int sum[3] = {0};
		RGBQuad* src_pixel = (RGBQuad*)(src + x*4);
		RGBQuad* dst_pixel = (RGBQuad*)(dst + x*4);

		pixel_add = pixel_sub = src_pixel;
		pixel_mid = dst_pixel;

		/* primera semi-suma */
		for (c=0; c<=half_radius; c++)
		{
			sum[0] += pixel_add->r;
			sum[1] += pixel_add->g;
			sum[2] += pixel_add->b;
			pixel_add += width;
		}

		/* medio radio en la izquerda: sólo suma */
		for (c=0; c<half_radius; c++)
		{
			pixel_mid->r = sum[0] / radius;
			pixel_mid->g = sum[1] / radius;
			pixel_mid->b = sum[2] / radius;
			sum[0] += pixel_add->r;
			sum[1] += pixel_add->g;
			sum[2] += pixel_add->b;
			pixel_mid += width;
			pixel_add += width;
		}

		/* parte central: radio completo */
		for (; c<bottom; c++)
		{
			pixel_mid->r = sum[0] / radius;
			pixel_mid->g = sum[1] / radius;
			pixel_mid->b = sum[2] / radius;
			sum[0] = sum[0] + pixel_add->r - pixel_sub->r;
			sum[1] = sum[1] + pixel_add->g - pixel_sub->g;
			sum[2] = sum[2] + pixel_add->b - pixel_sub->b;
			pixel_mid += width;
			pixel_add += width;
			pixel_sub += width;
		}

		/* medio radio en la derecha: sólo resta */
		for (; c<height; c++)
		{
			pixel_mid->r = sum[0] / radius;
			pixel_mid->g = sum[1] / radius;
			pixel_mid->b = sum[2] / radius;
			sum[0] -= pixel_sub->r;
			sum[1] -= pixel_sub->g;
			sum[2] -= pixel_sub->b;
			pixel_mid += width;
			pixel_sub += width;
		}
	}
}
