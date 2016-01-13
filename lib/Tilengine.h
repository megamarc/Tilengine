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

/* Tilengine_core */
#if defined _LIB
	#define TLNAPI

/* Tilengine shared */
#elif defined (_MSC_VER)
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

#ifndef _WINDEF_
typedef unsigned char	BYTE;	/*!< 8-bit wide data */
typedef unsigned short	WORD;	/*!< 16-bit wide data */
typedef unsigned int	DWORD;	/*!< 32-bit wide data */
#endif
#ifndef NULL
#define NULL			0
#endif

/* bool C++ */
#ifndef __cplusplus
typedef unsigned char bool;		/*!< C++ bool type for C language */
#define false	0
#define true	1
#endif

/* version */
#define TILENGINE_VER_MAJ	1
#define TILENGINE_VER_MIN	6
#define TILENGINE_VER_REV	0
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
	BLEND_NONE,	/*!< blending disabled */
	BLEND_MIX,	/*!< color averaging */
	BLEND_ADD,	/*!< color is always brighter (simulate light effects) */
	BLEND_SUB,	/*!< color is always darker (simulate shadow effects) */
	MAX_BLEND
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
	WORD index;		/*!< tile index */
	WORD flags;		/*!< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY) */
}
Tile;

/*! color strip definition */
typedef struct
{
	int delay;		/*!< time delay between frames */
	BYTE first;		/*!< index of first color to cycle */
	BYTE count;		/*!< number of colors in the cycle */
	BYTE dir;		/*!< direction: 0=descending, 1=ascending */
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
	WORD index;		/*!< tile index */
	WORD flags;		/*!< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY) */
	int xoffset;	/*!< horizontal position inside the title */
	int yoffset;	/*!< vertical position inside the title */
}
TLN_TileInfo;

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
TLNAPI void TLN_Deinit (void);
TLNAPI int TLN_GetWidth (void);
TLNAPI int TLN_GetHeight (void);
TLNAPI DWORD TLN_GetNumObjects (void);
TLNAPI DWORD TLN_GetUsedMemory (void);
TLNAPI DWORD TLN_GetVersion (void);
TLNAPI int TLN_GetNumLayers (void);
TLNAPI int TLN_GetNumSprites (void);
TLNAPI void TLN_SetBGColor (int r, int g, int b);
TLNAPI bool TLN_SetBGBitmap (TLN_Bitmap bitmap);
TLNAPI bool TLN_SetBGPalette (TLN_Palette palette);
TLNAPI void TLN_SetRasterCallback (void (*callback)(int));
TLNAPI void TLN_SetRenderTarget (BYTE* data, int pitch);
TLNAPI void TLN_UpdateFrame (int time);
/**@}*/

/** 
 * \anchor group_errors
 * \name Errors
 * Error handling */
/**@{*/
TLNAPI void TLN_SetLastError (TLN_Error error);
TLNAPI TLN_Error TLN_GetLastError (void);
/**@}*/

/** 
 * \anchor group_windowing
 * \name Windowing
 * Built-in window and input management */
/**@{*/
TLNAPI bool TLN_CreateWindow (char* overlay, TLN_WindowFlags flags);
TLNAPI bool TLN_CreateWindowThread (char* overlay, TLN_WindowFlags flags);
TLNAPI void TLN_SetWindowTitle (char* title);
TLNAPI bool TLN_ProcessWindow (void);
TLNAPI bool TLN_IsWindowActive (void);
TLNAPI bool TLN_GetInput (TLN_Input id);
TLNAPI int  TLN_GetLastInput (void);
TLNAPI void TLN_DrawFrame (int time);
TLNAPI void TLN_WaitRedraw (void);
TLNAPI void TLN_DeleteWindow (void);
TLNAPI void TLN_EnableBlur (bool mode);
TLNAPI DWORD TLN_GetTicks (void);
/**@}*/

/** 
 * \anchor group_spriteset
 * \name Spritesets
 * Spriteset resources management for sprites */
/**@{*/
TLNAPI TLN_Spriteset TLN_CreateSpriteset (int entries, TLN_Rect* rects, BYTE* data, int width, int height, int pitch, TLN_Palette palette);
TLNAPI TLN_Spriteset TLN_LoadSpriteset (char *name);
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
TLNAPI TLN_Tileset TLN_LoadTileset (char *filename);
TLNAPI TLN_Tileset TLN_CloneTileset (TLN_Tileset src);
TLNAPI bool TLN_SetTilesetPixels (TLN_Tileset tileset, int entry, BYTE* srcdata, int srcpitch);
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
TLNAPI TLN_Tilemap TLN_LoadTilemap (char *filename, char *layername);
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
TLNAPI TLN_Palette TLN_LoadPalette (char *filename);
TLNAPI TLN_Palette TLN_ClonePalette (TLN_Palette src);
TLNAPI bool TLN_SetPaletteColor (TLN_Palette palette, int color, BYTE r, BYTE g, BYTE b);
TLNAPI bool TLN_MixPalettes (TLN_Palette src1, TLN_Palette src2, TLN_Palette dst, BYTE factor);
TLNAPI DWORD* TLN_GetPaletteData (TLN_Palette palette, int index);
TLNAPI bool TLN_DeletePalette (TLN_Palette palette);
/**@}*/

/** 
 * \anchor group_bitmap
 * \name Bitmaps 
 * Bitmap management */
/**@{*/
TLNAPI TLN_Bitmap TLN_CreateBitmap (int width, int height, int bpp);
TLNAPI TLN_Bitmap TLN_LoadBitmap (char *filename);
TLNAPI TLN_Bitmap TLN_CloneBitmap (TLN_Bitmap src);
TLNAPI BYTE* TLN_GetBitmapPtr (TLN_Bitmap bitmap, int x, int y);
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
TLNAPI bool TLN_SetLayerBlendMode (int nlayer, TLN_Blend mode, BYTE factor);
TLNAPI bool TLN_SetLayerColumnOffset (int nlayer, int* offset);
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
TLNAPI bool TLN_SetSpriteBlendMode (int nsprite, TLN_Blend mode, BYTE factor);
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
TLNAPI TLN_Sequence TLN_CreateSequence (char* name, int delay, int first, int num_frames, int* data);
TLNAPI TLN_Cycle TLN_CreateCycle (char* name, int num_strips, TLN_ColorStrip* strips);
TLNAPI TLN_Sequence TLN_CloneSequence (TLN_Sequence src);
TLNAPI bool TLN_DeleteSequence (TLN_Sequence sequence);
/**@}*/

/** 
 * \anchor group_sequencepack
 * \name Sequence packs
 * Sequence pack manager for grouping and finding sequences */
/**@{*/
TLNAPI TLN_SequencePack TLN_CreateSequencePack (void);
TLNAPI TLN_SequencePack TLN_LoadSequencePack (char* filename);
TLNAPI TLN_Sequence TLN_FindSequence (TLN_SequencePack sp, char* name);
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
