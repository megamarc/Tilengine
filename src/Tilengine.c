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
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include "Tilengine.h"
#include "Tilemap.h"
#include "Tileset.h"
#include "Bitmap.h"
#include "Palette.h"
#include "Spriteset.h"
#include "Math2D.h"
#include "Engine.h"
#include "Layer.h"
#include "Sprite.h"
#include "Tables.h"
#include "LoadTMX.h"

/* magic number to recognize context object */
#define ID_CONTEXT	0x7E5D0AB1

TLN_Engine engine;	/* current context */

static TLN_Engine create_context(int hres, int vres, int numlayers, int numsprites, int numanimations);

TLN_Engine TLN_Init (int hres, int vres, int numlayers, int numsprites, int numanimations)
{
	printf("Tilengine v%d.%d.%d %d-bit %s built %s %s\n", 
		TILENGINE_VER_MAJ, 
		TILENGINE_VER_MIN, 
		TILENGINE_VER_REV, 
		(int)(sizeof(UINTPTR_MAX) << 3),
#if defined _DEBUG
		"debug",
#else
		"release",
#endif
		__DATE__, 
		__TIME__);
	return create_context (hres, vres, numlayers, numsprites, numanimations);
}

/* creates new engine context */
static TLN_Engine create_context(int hres, int vres, int numlayers, int numsprites, int numanimations)
{
	int c;
	TLN_Engine context;

	TLN_SetLastError (TLN_ERR_OK);

	/* create framebuffer */
	context = (TLN_Engine)calloc(sizeof(Engine), 1);
	context->header = ID_CONTEXT;
	context->framebuffer.width = hres;
	context->framebuffer.height = vres;
	context->framebuffer.pitch = (((hres * 32)>>3) + 3) & ~0x03;
	context->target_fps = INTERNAL_FPS;

	/* create static layers */
	if (numlayers > 0)
	{
		context->numlayers = numlayers;
		context->layers = (Layer*)calloc(numlayers, sizeof(Layer));
		if (!context->layers)
		{
			TLN_DeleteContext(context);
			TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);
			return NULL;
		}
		for (c = 0; c < context->numlayers; c++)
			context->layers[c].mosaic.buffer = (uint32_t*)calloc(hres, sizeof(uint32_t));

		/* buffer for intermediate scanline output */
		context->linebuffer = (uint32_t*)calloc(hres, sizeof(uint32_t));
		context->priority = (uint32_t*)malloc(context->framebuffer.pitch);
	}

	/* create static sprites */
	if (numsprites > 0)
	{
		context->numsprites = numsprites;
		context->sprites = (Sprite*)calloc(numsprites, sizeof(Sprite));
		if (!context->sprites)
		{
			TLN_DeleteContext(context);
			TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);
			return NULL;
		}
		for (c = 0; c < context->numsprites; c++)
		{
			Sprite* sprite = &context->sprites[c];
			sprite->draw = GetSpriteDraw(MODE_NORMAL);
			sprite->blitter = SelectBlitter(true, false, false);
			sprite->sx = sprite->sy = 1.0f;
		}
		ListInit(&context->list_sprites, &context->sprites[0].list_node, sizeof(Sprite), context->numsprites);

		/* sprite collision buffer */
		context->collision = (uint16_t*)calloc(hres, sizeof(uint16_t));
	}

	/* create static animations */
	if (numanimations > 0)
	{
		context->numanimations = numanimations;
		context->animations = (Animation*)calloc(numanimations, sizeof(Animation));
		if (!context->animations)
		{
			TLN_DeleteContext(context);
			TLN_SetLastError(TLN_ERR_OUT_OF_MEMORY);
			return NULL;
		}
		ListInit(&context->list_animations, &context->animations[0].list_node, sizeof(Animation), context->numanimations);
	}

	context->bgcolor = PackRGB32(0,0,0);
	context->blit_fast = SelectBlitter (false, false, false);
	if (!CreateBlendTables ())
	{
		TLN_DeleteContext(context);
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}
	context->blend_table = SelectBlendTable (BLEND_MOD);

	/* set as default context if it's the first one */
	if (engine == NULL)
		engine = context;

	for (c = 0; c<context->numlayers; c++)
		TLN_DisableLayerClip(c);

#ifdef _DEBUG
	TLN_SetLogLevel(TLN_LOG_ERRORS);
#endif

	return context;
}

static bool check_context(TLN_Engine context)
{
	if (context != NULL)
	{
		if (context->header == ID_CONTEXT)
			return true;
	}
	return false;
}

bool TLN_SetContext(TLN_Engine context)
{
	if (check_context(context))
	{
		engine = context;
		TLN_SetLastError(TLN_ERR_OK);
		return true;
	}
	else
	{
		TLN_SetLastError(TLN_ERR_NULL_POINTER);
		return false;
	}
}

TLN_Engine TLN_GetContext(void)
{
	return engine;
}

void TLN_Deinit(void)
{
	if (engine != NULL)
	{
		TLN_DeleteContext(engine);
		engine = NULL;
	}
}

bool TLN_DeleteContext(TLN_Engine context)
{
	int c;

	if (!check_context(context))
	{
		TLN_SetLastError(TLN_ERR_NULL_POINTER);
		return false;
	}

	DeleteBlendTables();

	for (c = 0; c < context->numlayers; c++)
		free(context->layers[c].mosaic.buffer);

	if (context->sprites)
		free(context->sprites);

	if (context->layers)
		free(context->layers);

	if (context->priority)
		free(context->priority);

	if (context->animations)
		free(context->animations);

	if (context->collision)
		free(context->collision);

	if (context->linebuffer)
		free(context->linebuffer);

	free(context);
	return true;
}

void TLN_SetLogLevel(TLN_LogLevel log_level)
{
	if (engine != NULL)
		engine->log_level = log_level;
}

uint32_t TLN_GetVersion (void)
{
	TLN_SetLastError (TLN_ERR_OK);
	return TILENGINE_HEADER_VERSION;
}

void TLN_SetTargetFps(int fps)
{
	engine->target_fps = fps;
}

int TLN_GetTargetFps(void)
{
	return engine->target_fps;
}

int TLN_GetWidth (void)
{
	TLN_SetLastError (TLN_ERR_OK);
	return engine->framebuffer.width;
}

int TLN_GetHeight (void)
{
	TLN_SetLastError (TLN_ERR_OK);
	return engine->framebuffer.height;
}

void TLN_SetRenderTarget (uint8_t* data, int pitch)
{
	engine->framebuffer.data = data;
	engine->framebuffer.pitch = pitch;
	TLN_SetLastError (TLN_ERR_OK);
}

/*!
 * \brief
 * Gets the location of the currently set render target
 * 
 * \see
 * TLN_SetRenderTarget()
 */
uint8_t* TLN_GetRenderTarget (void)
{
	TLN_SetLastError (TLN_ERR_OK);
	return engine->framebuffer.data;
}

/*!
 * \brief
 * Gets the pitch (bytes per scanline) of the currently set render target
 * 
 * \see
 * TLN_SetRenderTarget()
 */
int TLN_GetRenderTargetPitch (void)
{
	TLN_SetLastError (TLN_ERR_OK);
	return engine->framebuffer.pitch;
}

/* basic reference list without duplicates */
typedef struct
{
	int index;
	void* refs[TMX_MAX_TILESET];
}
RefList;

/* finds reference in list */
bool ref_find(RefList* refs, void* item)
{
	int c;
	for (c = 0; c < refs->index; c += 1)
	{
		if (refs->refs[c] == item)
			return true;
	}
	return false;
}

/* adds reference to list */
bool ref_add(RefList* refs, void* item)
{
	if (refs->index < TMX_MAX_TILESET - 1 && !ref_find(refs, item))
	{
		refs->refs[refs->index++] = item;
		return true;
	}
	return false;
}

/* Starts active rendering of the current frame */
static void BeginFrame (int frame)
{
	/* update active animations */
	List* list;
	int index;

	/* adjust to target fps */
	frame = (engine->frame*INTERNAL_FPS) / engine->target_fps;
	engine->frame += 1;

	/* color cycle animations */
	if (engine->numanimations > 0)
	{
		list = &engine->list_animations;
		index = list->first;
		while (index != -1)
		{
			Animation* animation = &engine->animations[index];
			UpdateAnimation(animation, frame);
			index = animation->list_node.next;
		}
	}

	/* sprite animations */
	if (engine->numsprites > 0)
	{
		list = &engine->list_sprites;
		index = list->first;
		while (index != -1)
		{
			Sprite* sprite = &engine->sprites[index];
			sprite->collision = false;
			Animation* animation = &sprite->animation;
			if (animation->enabled && !animation->paused)
				UpdateAnimation(animation, frame);
			index = sprite->list_node.next;
		}
	}

	/* tileset animations. calls just once per globally used tileset, avoids duplicate calls */
	RefList tilesets = { 0 };
	for (index = 0; index < engine->numlayers; index += 1)
	{
		Layer* layer = &engine->layers[index];
		if (layer->tilemap != NULL)
		{
			int ts;
			for (ts = 0; ts < MAX_TILESETS; ts += 1)
			{
				TLN_Tileset tileset = layer->tilemap->tilesets[ts];
				if (tileset == NULL)
					break;

				if (tileset->sp != NULL && ref_add(&tilesets, tileset))
				{
					int c;
					for (c = 0; c < tileset->sp->num_sequences; c += 1)
					{
						Animation* animation = &tileset->animations[c];
						if (animation->enabled && !animation->paused)
							UpdateAnimation(animation, frame);
					}
				}
			}
		}
	}

	/* frame callback */
	engine->line = 0;
	if (engine->cb_frame)
		engine->cb_frame (engine->frame);
}

void TLN_UpdateFrame(int frame)
{
	BeginFrame(frame);
	while (DrawScanline()) {}
	TLN_SetLastError(TLN_ERR_OK);
}

int TLN_GetNumLayers (void)
{
	TLN_SetLastError (TLN_ERR_OK);
	return engine->numlayers;
}

int TLN_GetNumSprites (void)
{
	TLN_SetLastError (TLN_ERR_OK);
	return engine->numsprites;
}

void TLN_SetRasterCallback (TLN_VideoCallback callback)
{
	TLN_SetLastError (TLN_ERR_OK);
	engine->cb_raster = callback;
}

void TLN_SetFrameCallback (TLN_VideoCallback callback)
{
	TLN_SetLastError (TLN_ERR_OK);
	engine->cb_frame = callback;
}

void TLN_SetBGColor (uint8_t r, uint8_t g, uint8_t b)
{
	engine->bgcolor = PackRGB32 (r,g,b);
}

bool TLN_SetBGColorFromTilemap (TLN_Tilemap tilemap)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP))
	{
		engine->bgcolor = tilemap->bgcolor | 0xFF000000;
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
		return false;
}

void TLN_DisableBGColor (void)
{
	engine->bgcolor = 0;
}

bool TLN_SetBGBitmap (TLN_Bitmap bitmap)
{
	if (bitmap != NULL)
	{
		if (!CheckBaseObject(bitmap, OT_BITMAP))
			return false;
		engine->bgpalette = bitmap->palette;
	}
	engine->bgbitmap = bitmap;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_SetBGPalette (TLN_Palette palette)
{
	if (!CheckBaseObject(palette, OT_PALETTE))
		return false;

	engine->bgpalette = palette;
	TLN_SetLastError (TLN_ERR_OK);
	return true;
}

bool TLN_SetGlobalPalette(int index, TLN_Palette palette)
{
	if (index < 0 || index > NUM_PALETTES - 1)
	{
		TLN_SetLastError(TLN_ERR_IDX_PALETTE);
		return false;
	}

	if (palette != NULL && !CheckBaseObject(palette, OT_PALETTE))
		return false;

	engine->palettes[index] = palette;
	TLN_SetLastError(TLN_ERR_OK);
	return true;
}

TLN_Palette TLN_GetGlobalPalette(int index)
{
	if (index < 0 || index > NUM_PALETTES - 1)
	{
		TLN_SetLastError(TLN_ERR_IDX_PALETTE);
		return false;
	}

	TLN_SetLastError(TLN_ERR_OK);
	return engine->palettes[index];
}

void TLN_SetCustomBlendFunction (TLN_BlendFunction blend_function)
{
	uint8_t* table = SelectBlendTable (BLEND_CUSTOM);
	int a,b;

	if (blend_function == NULL)
		return;

	/* rellena tabla */
	for (a=0; a<256; a++)
	{
		for (b=0; b<256; b++)
			table[(a<<8) + b] = blend_function (a, b);
	}
}

uint32_t TLN_GetNumObjects (void)
{
	TLN_SetLastError (TLN_ERR_OK);
	return GetNumObjects ();
}

uint32_t TLN_GetUsedMemory (void)
{
	TLN_SetLastError (TLN_ERR_OK);
	return GetNumBytes ();
}

const char* const errornames[] =
{
	"No error",
	"Not enough memory",
	"Layer index out of range",
	"Sprite index out of range",
	"Animation index out of range",
	"Picture or tile index out of range",
	"Invalid Tileset reference",
	"Invalid Tilemap reference",
	"Invalid Spriteset reference",
	"Invalid Palette reference",
	"Invalid SequencePack reference",
	"Invalid Sequence reference",
	"Invalid Bitmap reference",
	"Null pointer as required argument",
	"Resource file not found",
	"Resource file has invalid format",
	"A width or height parameter is invalid",
	"Unsupported function",
	"Invalid ObjectList reference",
	"Palette index out of range"
};

void TLN_SetLastError (TLN_Error error)
{
	if (check_context(engine))
	{
		engine->error = error;
		if (error != TLN_ERR_OK)
			tln_trace(TLN_LOG_ERRORS, errornames[error]);
	}
}

TLN_Error TLN_GetLastError (void)
{
	if (check_context(engine))
		return engine->error;
	else
		return TLN_ERR_NULL_POINTER;
}

const char *TLN_GetErrorString (TLN_Error error)
{
	if (error < TLN_MAX_ERR)
		return errornames[error];
	else
		return "Invalid error code";
}

/* outputs trace message */
void tln_trace(TLN_LogLevel log_level, const char* format, ...)
{
	if (engine != NULL && engine->log_level >= log_level)
	{
		char line[255];
		va_list ap;

		va_start(ap, format);
		vsprintf(line, format, ap);
		va_end(ap);

		printf("Tilengine: %s\n", line);
	}
}
