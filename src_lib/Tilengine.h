/*
Tilengine - 2D Graphics library with raster effects
Copyright (c) 2015-2017 Marc Palacios Domenech (megamarc@hotmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 ****************************************************************************
 * \file
 * Tilengine header
 * \author Marc Palacios (Megamarc)
 * \date Jun 2015
 * http://www.tilengine.org
 *
 * Main header for Tilengine 2D scanline-based graphics engine
 *
 *****************************************************************************
 */

#ifndef _TILENGINE_H
#define _TILENGINE_H

/* Tilengine shared */
#if defined _MSC_VER
	#ifdef LIB_EXPORTS
		#define TLNAPI __declspec(dllexport)
	#else
		#define TLNAPI __declspec(dllimport)
	#endif
#else
	#ifdef LIB_EXPORTS
		#define TLNAPI __attribute__((visibility("default")))
	#else
		#define TLNAPI
	#endif
#endif	

#include <stdio.h>
#if defined _MSC_VER
typedef unsigned char	uint8_t;	/*!< 8-bit wide data */
typedef unsigned short	uint16_t;	/*!< 16-bit wide data */
typedef unsigned int	uint32_t;	/*!< 32-bit wide data */
#else
#include <stdint.h>
#endif

/* bool C++ */
#ifndef __cplusplus
typedef unsigned char bool;		/*!< C++ bool type for C language */
#define false	0
#define true	1
#endif

/* version */
#define TILENGINE_VER_MAJ	1
#define TILENGINE_VER_MIN	12
#define TILENGINE_VER_REV	1
#define TILENGINE_HEADER_VERSION ((TILENGINE_VER_MAJ<<16) | (TILENGINE_VER_MIN<<8) | TILENGINE_VER_REV)

#define BITVAL(n) (1<<(n))

/*! tile/sprite flags. Can be none or a combination of the following: */
typedef enum
{
	FLAG_NONE		= 0,			/*!< no flags */
	FLAG_FLIPX		= BITVAL(15),	/*!< horizontal flip */
	FLAG_FLIPY		= BITVAL(14),	/*!< vertical flip */
	FLAG_ROTATE		= BITVAL(13),	/*!< row/column flip (unsupported, Tiled compatibility) */
	FLAG_PRIORITY	= BITVAL(12),	/*!< tile goes in front of sprite layer */
}
TLN_TileFlags;

/* fixed point helper */
typedef int fix_t;
#define FIXED_BITS	16
#define float2fix(f)	(fix_t)(f*(1 << FIXED_BITS))
#define int2fix(i)		((int)(i) << FIXED_BITS)
#define fix2int(f)		((int)(f) >> FIXED_BITS)
#define fix2float(f)	(float)(f)/(1 << FIXED_BITS)

/*! 
 * layer blend modes. Must be one of these and are mutually exclusive:
 */
typedef enum
{
	BLEND_NONE,		/*!< blending disabled */
	BLEND_MIX25,	/*!< color averaging 1 */
	BLEND_MIX50,	/*!< color averaging 2 */
	BLEND_MIX75,	/*!< color averaging 3 */
	BLEND_ADD,		/*!< color is always brighter (simulate light effects) */
	BLEND_SUB,		/*!< color is always darker (simulate shadow effects) */
	BLEND_MOD,		/*!< color is always darker (simulate shadow effects) */
	BLEND_CUSTOM,	/*!< user provided blend function */
	MAX_BLEND,
	BLEND_MIX = BLEND_MIX50
}
TLN_Blend;

/*! Affine transformation parameters */ 
typedef struct
{
	float angle;	/*!< rotation in degrees */
	float dx;		/*!< horizontal translation */
	float dy;		/*!< vertical translation */
	float sx;		/*!< horizontal scaling */
	float sy;		/*!< vertical scaling */
}
TLN_Affine;

/*! Tile description */
typedef struct Tile
{
	uint16_t index;		/*!< tile index */
	uint16_t flags;		/*!< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY) */
}
Tile;

/*! color strip definition */
typedef struct
{
	int delay;		/*!< time delay between frames */
	uint8_t first;		/*!< index of first color to cycle */
	uint8_t count;		/*!< number of colors in the cycle */
	uint8_t dir;		/*!< direction: 0=descending, 1=ascending */
}
TLN_ColorStrip;

/*! Basic rectangle */
typedef struct
{
	int x;			/*!< horizontal position */
	int y;			/*!< vertical position */
	int w;			/*!< width */
	int h;			/*!< height */
}
TLN_Rect;

/*! Sprite information */
typedef struct
{
	int offset;		/*!< internal use */
	int w;			/*!< width of sprite */
	int h;			/*!< height of sprite */
}
TLN_SpriteInfo;

/*! Tile information in screen coordinates */
typedef struct
{
	uint16_t index;	/*!< tile index */
	uint16_t flags;	/*!< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY) */
	int row;		/*!< row number in the tilemap */
	int col;		/*!< col number in the tilemap */
	int xoffset;	/*!< horizontal position inside the title */
	int yoffset;	/*!< vertical position inside the title */
	uint8_t color;	/*!< color index at collision point */
}
TLN_TileInfo;

/*! overlays for CRT effect */
typedef enum
{
	TLN_OVERLAY_NONE,
	TLN_OVERLAY_SHADOWMASK,
	TLN_OVERLAY_APERTURE,
	TLN_OVERLAY_CUSTOM,
	TLN_MAX_OVERLAY
}
TLN_Overlay;

typedef struct Tile*		 TLN_Tile;				/*!< Tile reference */
typedef struct Tileset*		 TLN_Tileset;			/*!< Opaque tileset reference */
typedef struct Tilemap*		 TLN_Tilemap;			/*!< Opaque tilemap reference */
typedef struct Palette*		 TLN_Palette;			/*!< Opaque palette reference */
typedef struct Spriteset*	 TLN_Spriteset;			/*!< Opaque sspriteset reference */
typedef struct Sequence*	 TLN_Sequence;			/*!< Opaque sequence reference */
typedef struct SequencePack* TLN_SequencePack;		/*!< Opaque sequence pack reference */
typedef struct Bitmap*		 TLN_Bitmap;			/*!< Opaque bitmap reference */
typedef struct Cycle*		 TLN_Cycle;				/*!< Opaque color cycle reference */

/*! Standard inputs. Must be one of these and are mutually exclusive: */
typedef enum
{
	INPUT_NONE,		/*!< no input */
	INPUT_UP,		/*!< up direction */
	INPUT_DOWN,		/*!< down direction */
	INPUT_LEFT,		/*!< left direction */
	INPUT_RIGHT,	/*!< right direction */
	INPUT_A,		/*!< first action button */
	INPUT_B,		/*!< second action button */
	INPUT_C,		/*!< third action button */
	INPUT_D,		/*!< fourth action button */
}
TLN_Input;

/*! CreateWindow flags. Can be none or a combination of the following: */
typedef enum
{
	CWF_FULLSCREEN	=	(1<<0),	/*!< create a fullscreen window */
	CWF_VSYNC		=	(1<<1),	/*!< sync frame updates with vertical retrace */
	CWF_S1			=	(1<<2),	/*!< create a window the same size as the framebuffer */
	CWF_S2			=	(2<<2),	/*!< create a window 2x the size the framebuffer */
	CWF_S3			=	(3<<2),	/*!< create a window 3x the size the framebuffer */
	CWF_S4			=	(4<<2),	/*!< create a window 4x the size the framebuffer */
	CWF_S5			=	(5<<2),	/*!< create a window 5x the size the framebuffer */
}
TLN_WindowFlags;

/*! Error codes */
typedef enum
{
	TLN_ERR_OK,				/*!< No error */
	TLN_ERR_OUT_OF_MEMORY,	/*!< Not enough memory */
	TLN_ERR_IDX_LAYER,		/*!< Layer index out of range */
	TLN_ERR_IDX_SPRITE,		/*!< Sprite index out of range */
	TLN_ERR_IDX_ANIMATION,	/*!< Animation index out of range */
	TLN_ERR_IDX_PICTURE,	/*!< Picture or tile index out of range */
	TLN_ERR_REF_TILESET,	/*!< Invalid TLN_Tileset reference */
	TLN_ERR_REF_TILEMAP,	/*!< Invalid TLN_Tilemap reference */
	TLN_ERR_REF_SPRITESET,	/*!< Invalid TLN_Spriteset reference */
	TLN_ERR_REF_PALETTE,	/*!< Invalid TLN_Palette reference */
	TLN_ERR_REF_SEQUENCE,	/*!< Invalid TLN_SequencePack reference */
	TLN_ERR_REF_SEQPACK,	/*!< Invalid TLN_Sequence reference */
	TLN_ERR_REF_BITMAP,		/*!< Invalid TLN_Bitmap reference */
	TLN_ERR_NULL_POINTER,	/*!< Null pointer as argument */ 
	TLN_ERR_FILE_NOT_FOUND,	/*!< Resource file not found */
	TLN_ERR_WRONG_FORMAT,	/*!< Resource file has invalid format */
	TLN_ERR_WRONG_SIZE,		/*!< A width or height parameter is invalid */
	TLN_ERR_UNSUPPORTED,	/*!< Unsupported function */
	TLN_MAX_ERR,
}
TLN_Error;

#ifdef __cplusplus
extern "C"{
#endif

/** 
 * \anchor group_setup
 * \name Setup
 * Basic setup and management */
/**@{*/
TLNAPI bool TLN_Init (int hres, int vres, int numlayers, int numsprites, int numanimations);
TLNAPI bool TLN_InitBPP (int hres, int vres, int bpp, int numlayers, int numsprites, int numanimations);
TLNAPI void TLN_Deinit (void);
TLNAPI int TLN_GetWidth (void);
TLNAPI int TLN_GetHeight (void);
TLNAPI int TLN_GetBPP (void);
TLNAPI uint32_t TLN_GetNumObjects (void);
TLNAPI uint32_t TLN_GetUsedMemory (void);
TLNAPI uint32_t TLN_GetVersion (void);
TLNAPI int TLN_GetNumLayers (void);
TLNAPI int TLN_GetNumSprites (void);
TLNAPI void TLN_SetBGColor (uint8_t r, uint8_t g, uint8_t b);
TLNAPI bool TLN_SetBGBitmap (TLN_Bitmap bitmap);
TLNAPI bool TLN_SetBGPalette (TLN_Palette palette);
TLNAPI void TLN_SetRasterCallback (void (*callback)(int));
TLNAPI void TLN_SetRenderTarget (uint8_t* data, int pitch);
TLNAPI void TLN_UpdateFrame (int time);
TLNAPI void TLN_BeginFrame (int time);
TLNAPI bool TLN_DrawNextScanline (void);
TLNAPI void TLN_SetLoadPath (const char* path);
TLNAPI void TLN_SetCustomBlendFunction (uint8_t (*blend_function)(uint8_t src, uint8_t dst));

/**@}*/

/** 
 * \anchor group_errors
 * \name Errors
 * Error handling */
/**@{*/
TLNAPI void TLN_SetLastError (TLN_Error error);
TLNAPI TLN_Error TLN_GetLastError (void);
TLNAPI const char *TLN_GetErrorString (TLN_Error error);
/**@}*/

/** 
 * \anchor group_windowing
 * \name Windowing
 * Built-in window and input management */
/**@{*/
TLNAPI bool TLN_CreateWindow (const char* overlay, TLN_WindowFlags flags);
TLNAPI bool TLN_CreateWindowThread (const char* overlay, TLN_WindowFlags flags);
TLNAPI void TLN_SetWindowTitle (const char* title);
TLNAPI bool TLN_ProcessWindow (void);
TLNAPI bool TLN_IsWindowActive (void);
TLNAPI bool TLN_GetInput (TLN_Input id);
TLNAPI int  TLN_GetLastInput (void);
TLNAPI void TLN_DrawFrame (int time);
TLNAPI void TLN_WaitRedraw (void);
TLNAPI void TLN_DeleteWindow (void);
TLNAPI void TLN_EnableBlur (bool mode);
TLNAPI void TLN_EnableCRTEffect (TLN_Overlay overlay, uint8_t overlay_factor, uint8_t threshold, uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, bool blur, uint8_t glow_factor);
TLNAPI void TLN_DisableCRTEffect (void);
TLNAPI void TLN_Delay (uint32_t msecs);
TLNAPI uint32_t TLN_GetTicks (void);
TLNAPI void TLN_BeginWindowFrame (int time);
TLNAPI void TLN_EndWindowFrame (void);

/**@}*/

/** 
 * \anchor group_spriteset
 * \name Spritesets
 * Spriteset resources management for sprites */
/**@{*/
TLNAPI TLN_Spriteset TLN_CreateSpriteset (int entries, TLN_Rect* rects, uint8_t* data, int width, int height, int pitch, TLN_Palette palette);
TLNAPI TLN_Spriteset TLN_LoadSpriteset (const char* name);
TLNAPI TLN_Spriteset TLN_CloneSpriteset (TLN_Spriteset src);
TLNAPI bool TLN_GetSpriteInfo (TLN_Spriteset spriteset, int entry, TLN_SpriteInfo* info);
TLNAPI TLN_Palette TLN_GetSpritesetPalette (TLN_Spriteset spriteset);
TLNAPI bool TLN_DeleteSpriteset (TLN_Spriteset Spriteset);
/**@}*/

/** 
 * \anchor group_tileset
 * \name Tilesets
 * Tileset resources management for background layers */
/**@{*/
TLNAPI TLN_Tileset TLN_CreateTileset (int numtiles, int width, int height, TLN_Palette palette);
TLNAPI TLN_Tileset TLN_LoadTileset (const char* filename);
TLNAPI TLN_Tileset TLN_CloneTileset (TLN_Tileset src);
TLNAPI bool TLN_SetTilesetPixels (TLN_Tileset tileset, int entry, uint8_t* srcdata, int srcpitch);
TLNAPI bool TLN_CopyTile (TLN_Tileset tileset, int src, int dst);
TLNAPI int TLN_GetTileWidth (TLN_Tileset tileset);
TLNAPI int TLN_GetTileHeight (TLN_Tileset tileset);
TLNAPI TLN_Palette TLN_GetTilesetPalette (TLN_Tileset tileset);
TLNAPI bool TLN_DeleteTileset (TLN_Tileset tileset);
/**@}*/

/** 
 * \anchor group_tilemap
 * \name Tilemaps 
 * Tilemap resources management for background layers */
/**@{*/
TLNAPI TLN_Tilemap TLN_CreateTilemap (int rows, int cols, TLN_Tile tiles);
TLNAPI TLN_Tilemap TLN_LoadTilemap (const char* filename, const char* layername);
TLNAPI TLN_Tilemap TLN_CloneTilemap (TLN_Tilemap src);
TLNAPI int TLN_GetTilemapRows (TLN_Tilemap tilemap);
TLNAPI int TLN_GetTilemapCols (TLN_Tilemap tilemap);
TLNAPI bool TLN_GetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);
TLNAPI bool TLN_SetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);
TLNAPI bool TLN_CopyTiles (TLN_Tilemap src, int srcrow, int srccol, int rows, int cols, TLN_Tilemap dst, int dstrow, int dstcol);
TLNAPI bool TLN_DeleteTilemap (TLN_Tilemap tilemap);
/**@}*/

/** 
 * \anchor group_palette
 * \name Palettes
 * Color palette resources management for sprites and background layers */
/**@{*/
TLNAPI TLN_Palette TLN_CreatePalette (int entries);
TLNAPI TLN_Palette TLN_LoadPalette (const char* filename);
TLNAPI TLN_Palette TLN_ClonePalette (TLN_Palette src);
TLNAPI bool TLN_SetPaletteColor (TLN_Palette palette, int color, uint8_t r, uint8_t g, uint8_t b);
TLNAPI bool TLN_MixPalettes (TLN_Palette src1, TLN_Palette src2, TLN_Palette dst, uint8_t factor);
TLNAPI bool TLN_AddPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);
TLNAPI bool TLN_SubPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);
TLNAPI bool TLN_ModPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);
TLNAPI uint8_t* TLN_GetPaletteData (TLN_Palette palette, int index);
TLNAPI bool TLN_DeletePalette (TLN_Palette palette);
/**@}*/

/** 
 * \anchor group_bitmap
 * \name Bitmaps 
 * Bitmap management */
/**@{*/
TLNAPI TLN_Bitmap TLN_CreateBitmap (int width, int height, int bpp);
TLNAPI TLN_Bitmap TLN_LoadBitmap (const char* filename);
TLNAPI TLN_Bitmap TLN_CloneBitmap (TLN_Bitmap src);
TLNAPI uint8_t* TLN_GetBitmapPtr (TLN_Bitmap bitmap, int x, int y);
TLNAPI int TLN_GetBitmapWidth (TLN_Bitmap bitmap);
TLNAPI int TLN_GetBitmapHeight (TLN_Bitmap bitmap);
TLNAPI int TLN_GetBitmapDepth (TLN_Bitmap bitmap);
TLNAPI int TLN_GetBitmapPitch (TLN_Bitmap bitmap);
TLNAPI TLN_Palette TLN_GetBitmapPalette (TLN_Bitmap bitmap);
TLNAPI bool TLN_SetBitmapPalette (TLN_Bitmap bitmap, TLN_Palette palette);
TLNAPI bool TLN_DeleteBitmap (TLN_Bitmap bitmap);
/**@}*/

/** 
 * \anchor group_layer
 * \name Layers
 * Background layers management */
/**@{*/
TLNAPI bool TLN_SetLayer (int nlayer, TLN_Tileset tileset, TLN_Tilemap tilemap);
TLNAPI bool TLN_SetLayerPalette (int nlayer, TLN_Palette palette);
TLNAPI bool TLN_SetLayerPosition (int nlayer, int hstart, int vstart);
TLNAPI bool TLN_SetLayerScaling (int nlayer, float xfactor, float yfactor);
TLNAPI bool TLN_SetLayerAffineTransform (int nlayer, TLN_Affine *affine);
TLNAPI bool TLN_SetLayerTransform (int layer, float angle, float dx, float dy, float sx, float sy);
TLNAPI bool TLN_SetLayerBlendMode (int nlayer, TLN_Blend mode, uint8_t factor);
TLNAPI bool TLN_SetLayerColumnOffset (int nlayer, int* offset);
TLNAPI bool TLN_SetLayerClip (int nlayer, int x1, int y1, int x2, int y2);
TLNAPI bool TLN_DisableLayerClip (int nlayer);
TLNAPI bool TLN_SetLayerMosaic (int nlayer, int width, int height);
TLNAPI bool TLN_DisableLayerMosaic (int nlayer);
TLNAPI bool TLN_ResetLayerMode (int nlayer);
TLNAPI bool TLN_DisableLayer (int nlayer);
TLNAPI TLN_Palette TLN_GetLayerPalette (int nlayer);
TLNAPI bool TLN_GetLayerTile (int nlayer, int x, int y, TLN_TileInfo* info);
/**@}*/

/** 
 * \anchor group_sprite
 * \name Sprites 
 * Sprites management */
/**@{*/
TLNAPI bool TLN_ConfigSprite (int nsprite, TLN_Spriteset spriteset, TLN_TileFlags flags);
TLNAPI bool TLN_SetSpriteSet (int nsprite, TLN_Spriteset spriteset);
TLNAPI bool TLN_SetSpriteFlags (int nsprite, TLN_TileFlags flags);
TLNAPI bool TLN_SetSpritePosition (int nsprite, int x, int y);
TLNAPI bool TLN_SetSpritePicture (int nsprite, int entry);
TLNAPI bool TLN_SetSpritePalette (int nsprite, TLN_Palette palette);
TLNAPI bool TLN_SetSpriteBlendMode (int nsprite, TLN_Blend mode, uint8_t factor);
TLNAPI bool TLN_SetSpriteScaling (int nsprite, float sx, float sy);
TLNAPI bool TLN_ResetSpriteScaling (int nsprite);
TLNAPI int  TLN_GetSpritePicture (int nsprite);
TLNAPI int  TLN_GetAvailableSprite (void);
TLNAPI bool TLN_EnableSpriteCollision (int nsprite, bool enable);
TLNAPI bool TLN_GetSpriteCollision (int nsprite);
TLNAPI bool TLN_DisableSprite (int nsprite);
TLNAPI TLN_Palette TLN_GetSpritePalette (int nsprite);
/**@}*/

/** 
 * \anchor group_sequence
 * \name Sequences
 * Sequence resources management for layer, sprite and palette animations */
/**@{*/
TLNAPI TLN_Sequence TLN_CreateSequence (const char* name, int delay, int first, int num_frames, int* data);
TLNAPI TLN_Cycle TLN_CreateCycle (const char* name, int num_strips, TLN_ColorStrip* strips);
TLNAPI TLN_Sequence TLN_CloneSequence (TLN_Sequence src);
TLNAPI bool TLN_DeleteSequence (TLN_Sequence sequence);
/**@}*/

/** 
 * \anchor group_sequencepack
 * \name Sequence packs
 * Sequence pack manager for grouping and finding sequences */
/**@{*/
TLNAPI TLN_SequencePack TLN_CreateSequencePack (void);
TLNAPI TLN_SequencePack TLN_LoadSequencePack (const char* filename);
TLNAPI TLN_Sequence TLN_FindSequence (TLN_SequencePack sp, const char* name);
TLNAPI bool TLN_AddSequenceToPack (TLN_SequencePack sp, TLN_Sequence sequence);
TLNAPI bool TLN_DeleteSequencePack (TLN_SequencePack sp);
/**@}*/

/** 
 * \anchor group_animation
 * \name Animations 
 * Animation engine manager */
/**@{*/
TLNAPI bool TLN_SetPaletteAnimation (int index, TLN_Palette palette, TLN_Sequence sequence, bool blend);
TLNAPI bool TLN_SetPaletteAnimationSource (int index, TLN_Palette);
TLNAPI bool TLN_SetTilesetAnimation (int index, int nlayer, TLN_Sequence);
TLNAPI bool TLN_SetTilemapAnimation (int index, int nlayer, TLN_Sequence);
TLNAPI bool TLN_SetSpriteAnimation (int index, int nsprite, TLN_Sequence sequence, int loop);
TLNAPI bool TLN_GetAnimationState (int index);
TLNAPI bool TLN_SetAnimationDelay (int index, int delay);
TLNAPI int  TLN_GetAvailableAnimation (void);
TLNAPI bool TLN_DisableAnimation (int index);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif
