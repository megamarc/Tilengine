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

static void hblur (uint8_t* src, uint8_t* dst, int width, int height, int pitch, int radius)
{
	int c, y;
	const int half_radius = radius/2;
	const int right = width - half_radius;
	RGBQuad* pixel_sub;	/* left pixel (origen, resta) */
	RGBQuad* pixel_add;	/* right pixel (origen, suma) */
	RGBQuad* pixel_mid;	/* center pixel (destino) */
	radius++;

	for (y=0; y<height; y++)
	{
		int sum[3] = {0};
		RGBQuad* src_pixel = (RGBQuad*)(src + y*pitch);
		RGBQuad* dst_pixel = (RGBQuad*)(dst + y*pitch);

		pixel_add = pixel_sub = src_pixel;
		pixel_mid = dst_pixel;

		/* primera semi-suma */
		for (c=0; c<=half_radius; c++)
		{
			sum[0] += pixel_add->r;
			sum[1] += pixel_add->g;
			sum[2] += pixel_add->b;
			pixel_add++;
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
			pixel_mid++;
			pixel_add++;
		}

		/* parte central: radio completo */
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

		/* medio radio en la derecha: sólo resta */
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
