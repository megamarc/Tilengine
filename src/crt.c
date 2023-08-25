#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "crt.h"

#define ZERO	0x00
#define SCAN	0x20
#define RED		0xFF,ZERO,ZERO,0xFF
#define GREEN	ZERO,0xFF,ZERO,0xFF
#define BLUE	ZERO,ZERO,0xFF,0xFF
#define BLACK	0x00,0x00,0x00,0xFF
#define WHITE	0xFF,0xFF,0xFF,0xFF

static const uint8_t pattern_slot[] =
{
	RED,   GREEN, BLUE,  RED,   GREEN, BLUE,
	RED,   GREEN, BLUE,  BLACK, BLACK, BLACK,
	RED,   GREEN, BLUE,  RED,   GREEN, BLUE,
	BLACK, BLACK, BLACK, RED,   GREEN, BLUE,
}; 

static const uint8_t pattern_aperture[] =
{
	RED, GREEN, BLUE
};

static const uint8_t pattern_shadow[] =
{
	RED, GREEN, BLUE,
	GREEN, BLUE, RED,
	BLUE, RED, GREEN,
};

static const uint8_t pattern_scanline[] =
{
	WHITE,
	SCAN, SCAN, SCAN, 0xFF
};

typedef struct
{
	const uint8_t* mask;
	int width;
	int height;
	int glow;
}
Pattern;

static Pattern patterns[] = 
{
	{pattern_slot, 6, 4, 192},
	{pattern_aperture, 3, 1, 204},
	{pattern_shadow, 3, 3, 204},
};

typedef struct
{
	int width;
	int height;
}
Size2D;

struct _CRTHandler
{
	SDL_Renderer* renderer;
	SDL_Texture* framebuffer;
	SDL_Texture* overlay;
	Size2D size_fb;
	uint8_t glow;
	bool blur;
};

/* private prototypes */
static void hblur(uint8_t* scan, int width, int height, int pitch);
static SDL_Texture* create_tiled_texture(SDL_Renderer* renderer, int width, int height, int tile_width, int tile_height, const uint8_t* tile_data);

/* create CRT effect */
CRTHandler CRTCreate(SDL_Renderer* renderer, SDL_Texture* framebuffer, CRTType type, int wnd_width, int wnd_height, bool blur)
{
	CRTHandler crt = (CRTHandler)calloc(1, sizeof(struct _CRTHandler));
	if (crt == NULL)
		return NULL;

	crt->renderer = renderer;
	crt->framebuffer = framebuffer;
	crt->blur = blur;
	
	/* get framebuffer size */
	Uint32 format = 0;
	int access = 0;	
	SDL_QueryTexture(framebuffer, &format, &access, &crt->size_fb.width, &crt->size_fb.height);

	/* build composed overlay with RGB mask + scanlines */
	Pattern* pattern = &patterns[type];
	crt->glow = pattern->glow;
	SDL_Texture* tex_mask = create_tiled_texture(renderer, wnd_width, wnd_height, pattern->width, pattern->height, pattern->mask);
	SDL_Texture* tex_scan = create_tiled_texture(renderer, crt->size_fb.width, crt->size_fb.height*2, 1, 2, pattern_scanline);
	SDL_SetTextureBlendMode(tex_scan, SDL_BLENDMODE_MOD);

	crt->overlay = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, wnd_width, wnd_height);
	SDL_SetRenderTarget(renderer, crt->overlay);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, tex_mask, NULL, NULL);
	if (type != CRT_SLOT)
		SDL_RenderCopy(renderer, tex_scan, NULL, NULL);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetTextureBlendMode(crt->overlay, SDL_BLENDMODE_MOD);
	SDL_DestroyTexture(tex_scan);
	SDL_DestroyTexture(tex_mask);

	return crt;
}

/* draws effect, gets locked texture data */
void CRTDraw(CRTHandler crt, void* pixels, int pitch, SDL_Rect* dstrect)
{
	/* RF blur */
	if (crt->blur)
		hblur((uint8_t*)pixels, crt->size_fb.width, crt->size_fb.height, pitch);
	SDL_UnlockTexture(crt->framebuffer);

	/* base image */
	SDL_SetTextureBlendMode(crt->framebuffer, SDL_BLENDMODE_NONE);
	SDL_RenderCopy(crt->renderer, crt->framebuffer, NULL, dstrect);

	/* rgb + scanline overlay */
	SDL_RenderCopy(crt->renderer, crt->overlay, NULL, dstrect);

	/* glow overlay */
	if (crt->glow != 0)
	{
		SDL_SetTextureBlendMode(crt->framebuffer, SDL_BLENDMODE_ADD);
		SDL_SetTextureColorMod(crt->framebuffer, crt->glow, crt->glow, crt->glow);
		SDL_RenderCopy(crt->renderer, crt->framebuffer, NULL, dstrect);
	}
}

void CRTSetRenderTarget(CRTHandler crt, SDL_Texture* framebuffer)
{
	if (crt != NULL && framebuffer != NULL)
		crt->framebuffer = framebuffer;
}

void CRTIncreaseGlow(CRTHandler crt)
{
	if (crt != NULL && crt->glow < 255)
		crt->glow += 1;
}

void CRTDecreaseGlow(CRTHandler crt)
{
	if (crt != NULL && crt->glow > 0)
		crt->glow -= 1;
}

void CRTSetBlur(CRTHandler crt, bool blur)
{
	if (crt != NULL)
		crt->blur = blur;
}

void CRTDelete(CRTHandler crt)
{
	if (crt != NULL)
		SDL_DestroyTexture(crt->overlay);

	free(crt);
}

/* basic horizontal blur emulating RF blurring */
static void hblur(uint8_t* scan, int width, int height, int pitch)
{
	int x, y;
	uint8_t *pixel;

	width -= 1;
	for (y = 0; y < height; y++)
	{
		pixel = scan;
		for (x = 0; x < width; x++)
		{
			pixel[0] = (pixel[0] + pixel[4]) >> 1;
			pixel[1] = (pixel[1] + pixel[5]) >> 1;
			pixel[2] = (pixel[2] + pixel[6]) >> 1;
			pixel += 4;
		}
		scan += pitch;
	}
}

static void blit(const uint8_t* srcptr, uint8_t* dstptr, int srcpitch, int lines, int dstpitch)
{
	int y;
	for (y = 0; y < lines; y += 1)
	{
		memcpy(dstptr, srcptr, srcpitch);
		srcptr += srcpitch;
		dstptr += dstpitch;
	}
}

static SDL_Texture* create_tiled_texture(SDL_Renderer* renderer, int width, int height, int tile_width, int tile_height, const uint8_t* tile_data)
{
	SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	
	const int tile_pitch = tile_width * 4;
	SDL_Rect dstrect = { 0, 0, tile_width, tile_height };
	for (dstrect.y = 0; dstrect.y <= height - tile_height; dstrect.y += tile_height)
	{
		uint8_t* dstptr = (uint8_t*)surface->pixels + dstrect.y * surface->pitch;
		for (dstrect.x = 0; dstrect.x <= width - tile_width; dstrect.x += tile_width)
		{
			blit(tile_data, dstptr, tile_pitch, tile_height, surface->pitch);
			dstptr += tile_pitch;
		}
		if (dstrect.x < width)
			blit(tile_data, dstptr, (width - dstrect.x) * 4, tile_height, surface->pitch);
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}
