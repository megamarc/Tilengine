/******************************************************************************
*
* Tilengine header
* 2015 Marc Palacios
* http://www.tilengine.org
*
* Main header for Tilengine 2D scanline-based graphics engine
*
******************************************************************************/

#ifndef _TILENGINE_H
#define _TILENGINE_H

#if defined (_MSC_VER)
	#ifdef LIB_EXPORTS
		#define LIBAPI __declspec(dllexport)
	#else
		#define LIBAPI __declspec(dllimport)
	#endif
#else
	#ifdef LIB_EXPORTS
		#define LIBAPI __attribute__((visibility("default")))
	#else
		#define LIBAPI
	#endif
#endif	

#ifndef _WINDEF_
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned int	DWORD;
#endif
#ifndef NULL
#define NULL			0
#endif

/* bool C++ */
#ifndef __cplusplus
typedef unsigned char bool;
#define false	0
#define true	1
#endif

/* version */
#define TILENGINE_VER_MAJ	1
#define TILENGINE_VER_MIN	4
#define TILENGINE_VER_REV	0
#define TILENGINE_HEADER_VERSION ((TILENGINE_VER_MAJ<<16) | (TILENGINE_VER_MIN<<8) | TILENGINE_VER_REV)

#define BITVAL(n) (1<<(n))

/* tile/sprite flags */
#define FLAG_FLIPX		BITVAL(15)	/* horizontal flip */
#define FLAG_FLIPY		BITVAL(14)	/* vertical flip */
#define FLAG_ROTATE		BITVAL(13)	/* row/column flip (unsupported, Tiled compatibility) */
#define FLAG_PRIORITY	BITVAL(12)	/* tile goes in front of sprite layer */

/* fixed point helper */
typedef int fix_t;
#define FIXED_BITS	16
#define float2fix(f)	(fix_t)(f*(1 << FIXED_BITS))
#define int2fix(i)		((int)(i) << FIXED_BITS)
#define fix2int(f)		((int)(f) >> FIXED_BITS)
#define fix2float(f)	(float)(f)/(1 << FIXED_BITS)

/* leyer blend modes */
typedef enum
{
	BLEND_NONE,
	BLEND_MIX,
	BLEND_ADD,
	BLEND_SUB,
	MAX_BLEND
}
blend_t;

typedef struct
{
	float angle;	/* rotation */
	float dx,dy;	/* translation */
	float sx,sy;	/* scale */
}
affine_t;

/* celda */
typedef struct Tile
{
	WORD index;		/* tile index */
	WORD flags;		/* attributes */
}
Tile;

/* Rectangle */
typedef struct
{
	int x,y,w,h;
}
TLN_Rect;

typedef struct
{
	int offset;
	int w,h;
}
TLN_SpriteInfo;

typedef struct
{
	WORD index;
	WORD flags;
	int xoffset;
	int yoffset;
}
TLN_TileInfo;

typedef struct Tile*		 TLN_Tile;
typedef struct Tileset*		 TLN_Tileset;
typedef struct Tilemap*		 TLN_Tilemap;
typedef struct Palette*		 TLN_Palette;
typedef struct Spriteset*	 TLN_Spriteset;
typedef struct Sequence*	 TLN_Sequence;
typedef struct SequencePack* TLN_SequencePack;
typedef struct Bitmap*		 TLN_Bitmap;

/* inputs */
enum
{
	INPUT_NONE,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_A,
	INPUT_B,
	INPUT_C,
	INPUT_D,
};

/* CreateWindow flags */
#define CWF_FULLSCREEN	(1<<0)
#define CWF_VSYNC		(1<<1)
#define CWF_S1			(1<<2)
#define CWF_S2			(2<<2)
#define CWF_S3			(3<<2)
#define CWF_S4			(4<<2)
#define CWF_S5			(5<<2)

#ifdef __cplusplus
extern "C"{
#endif

/* basic management */
void LIBAPI TLN_Init (int hres, int vres, int numlayers, int numsprites, int numanimations);
void LIBAPI TLN_Deinit (void);
DWORD LIBAPI TLN_GetNumObjects (void);
DWORD LIBAPI TLN_GetUsedMemory (void);
int LIBAPI TLN_GetNumLayers (void);
int LIBAPI TLN_GetNumSprites (void);
DWORD LIBAPI TLN_GetVersion (void);

/* window management */
bool LIBAPI TLN_CreateWindow (char* overlay, int flags);
bool LIBAPI TLN_CreateWindowThread (char* overlay, int flags);
bool LIBAPI TLN_ProcessWindow (void);
bool LIBAPI TLN_IsWindowActive (void);
bool LIBAPI TLN_GetInput (int id);
void LIBAPI TLN_DrawFrame (int time);
void LIBAPI TLN_WaitRedraw (void);
void LIBAPI TLN_DeleteWindow (void);
void LIBAPI TLN_EnableBlur (bool mode);
DWORD LIBAPI TLN_GetTicks (void);

/* image generation */
void LIBAPI TLN_SetBGColor (int r, int g, int b);
void LIBAPI TLN_SetBGBitmap (TLN_Bitmap bitmap);
void LIBAPI TLN_SetBGPalette (TLN_Palette palette);
void LIBAPI TLN_SetRasterCallback (void (*callback)(int));
void LIBAPI TLN_SetRenderTarget (BYTE* data, int pitch);
void LIBAPI TLN_UpdateFrame (int time);

/* spritesets management */
TLN_Spriteset LIBAPI TLN_CreateSpriteset (int entries, TLN_Rect* rects, BYTE* data, int width, int height, int pitch, TLN_Palette palette);
TLN_Spriteset LIBAPI TLN_LoadSpriteset (char *name);
TLN_Spriteset LIBAPI TLN_CloneSpriteset (TLN_Spriteset src);
bool LIBAPI TLN_GetSpriteInfo (TLN_Spriteset spriteset, int entry, TLN_SpriteInfo* info);
TLN_Palette LIBAPI TLN_GetSpritesetPalette (TLN_Spriteset spriteset);
void LIBAPI TLN_DeleteSpriteset (TLN_Spriteset Spriteset);

/* tilesets management */
TLN_Tileset LIBAPI TLN_CreateTileset (int numtiles, int width, int height, TLN_Palette palette);
TLN_Tileset LIBAPI TLN_LoadTileset (char *filename);
TLN_Tileset LIBAPI TLN_CloneTileset (TLN_Tileset src);
bool LIBAPI TLN_SetTilesetPixels (TLN_Tileset tileset, int entry, BYTE* srcdata, int srcpitch);
int LIBAPI TLN_GetTileWidth (TLN_Tileset tileset);
int LIBAPI TLN_GetTileHeight (TLN_Tileset tileset);
TLN_Palette LIBAPI TLN_GetTilesetPalette (TLN_Tileset tileset);
void LIBAPI TLN_DeleteTileset (TLN_Tileset tileset);

/* tilemaps management */
TLN_Tilemap LIBAPI TLN_CreateTilemap (int rows, int cols, TLN_Tile tiles);
TLN_Tilemap LIBAPI TLN_LoadTilemap (char *filename, char *layername);
TLN_Tilemap LIBAPI TLN_CloneTilemap (TLN_Tilemap src);
int LIBAPI TLN_GetTilemapRows (TLN_Tilemap tilemap);
int LIBAPI TLN_GetTilemapCols (TLN_Tilemap tilemap);
bool LIBAPI TLN_GetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);
bool LIBAPI TLN_SetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);
void LIBAPI TLN_CopyTiles (TLN_Tilemap src, int srcrow, int srccol, int rows, int cols, TLN_Tilemap dst, int dstrow, int dstcol);
void LIBAPI TLN_DeleteTilemap (TLN_Tilemap tilemap);

/* color tables management */
TLN_Palette LIBAPI TLN_CreatePalette (int entries);
TLN_Palette LIBAPI TLN_LoadPalette (char *filename);
TLN_Palette LIBAPI TLN_ClonePalette (TLN_Palette src);
void LIBAPI TLN_DeletePalette (TLN_Palette palette);
void LIBAPI TLN_SetPaletteColor (TLN_Palette palette, int color, BYTE r, BYTE g, BYTE b);
void LIBAPI TLN_MixPalettes (TLN_Palette src1, TLN_Palette src2, TLN_Palette dst, BYTE factor);
DWORD LIBAPI *TLN_GetPaletteData (TLN_Palette palette, int index);

/* bitmaps */
TLN_Bitmap LIBAPI TLN_CreateBitmap (int width, int height, int bpp);
TLN_Bitmap LIBAPI TLN_LoadBitmap (char *filename);
TLN_Bitmap LIBAPI TLN_CloneBitmap (TLN_Bitmap src);
BYTE LIBAPI *TLN_GetBitmapPtr (TLN_Bitmap bitmap, int x, int y);
TLN_Palette LIBAPI TLN_GetBitmapPalette (TLN_Bitmap bitmap);
void LIBAPI TLN_DeleteBitmap (TLN_Bitmap bitmap);

/* layer management */
void LIBAPI TLN_SetLayer (int nlayer, TLN_Tileset tileset, TLN_Tilemap tilemap);
void LIBAPI TLN_SetLayerPalette (int nlayer, TLN_Palette palette);
void LIBAPI TLN_SetLayerPosition (int nlayer, int hstart, int vstart);
void LIBAPI TLN_SetLayerScaling (int nlayer, float xfactor, float yfactor);
void LIBAPI TLN_SetLayerAffineTransform (int nlayer, affine_t *affine);
void LIBAPI TLN_SetLayerTransform (int layer, float angle, float dx, float dy, float sx, float sy);
void LIBAPI TLN_SetLayerBlendMode (int nlayer, blend_t mode, BYTE factor);
void LIBAPI TLN_SetLayerColumnOffset (int nlayer, int* offset);
void LIBAPI TLN_ResetLayerMode (int nlayer);
void LIBAPI TLN_DisableLayer (int nlayer);
TLN_Palette LIBAPI TLN_GetLayerPalette (int nlayer);
bool LIBAPI TLN_GetLayerTile (int nlayer, int x, int y, TLN_TileInfo* info);

/* sprites management */
void LIBAPI TLN_ConfigSprite (int nsprite, TLN_Spriteset spriteset, WORD flags);
void LIBAPI TLN_SetSpriteSet (int nsprite, TLN_Spriteset spriteset);
void LIBAPI TLN_SetSpriteFlags (int nsprite, WORD flags);
void LIBAPI TLN_SetSpritePosition (int nsprite, int x, int y);
void LIBAPI TLN_SetSpritePicture (int nsprite, int entry);
void LIBAPI TLN_SetSpritePalette (int nsprite, TLN_Palette palette);
void LIBAPI TLN_SetSpriteBlendMode (int nsprite, blend_t mode, BYTE factor);
void LIBAPI TLN_SetSpriteScaling (int nsprite, float sx, float sy);
void LIBAPI TLN_ResetSpriteScaling (int nsprite);
int  LIBAPI TLN_GetSpritePicture (int nsprite);
int  LIBAPI TLN_GetAvailableSprite (void);
void LIBAPI TLN_DisableSprite (int nsprite);
TLN_Palette LIBAPI TLN_GetSpritePalette (int nsprite);

/* sequences management */
TLN_Sequence LIBAPI TLN_CreateSequence (char* name, int delay, int first, int num_frames, int* data);
TLN_Sequence LIBAPI TLN_CloneSequence (TLN_Sequence src);
void LIBAPI TLN_DeleteSequence (TLN_Sequence sequence);
TLN_SequencePack LIBAPI TLN_LoadSequencePack (char* filename);
TLN_Sequence LIBAPI TLN_FindSequence (TLN_SequencePack sp, char* name);
void LIBAPI TLN_DeleteSequencePack (TLN_SequencePack sp);

/* animation engine */
void LIBAPI TLN_SetPaletteAnimation (int index, TLN_Palette palette, TLN_Sequence sequence, bool blend);
void LIBAPI TLN_SetPaletteAnimationSource (int index, TLN_Palette);
void LIBAPI TLN_SetTilemapAnimation (int index, int nlayer, TLN_Sequence);
void LIBAPI TLN_SetSpriteAnimation (int index, int nsprite, TLN_Sequence sequence, int loop);
bool LIBAPI TLN_GetAnimationState (int index);
void LIBAPI TLN_SetAnimationDelay (int index, int delay);
int  LIBAPI TLN_GetAvailableAnimation (void);
void LIBAPI TLN_DisableAnimation (int index);

#ifdef __cplusplus
}
#endif

#endif
