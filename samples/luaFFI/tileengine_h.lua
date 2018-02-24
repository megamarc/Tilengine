-- TileEngine 1.19 from Github
local ffi = require'ffi'

ffi.cdef[[


/* version */
/*
#define TILENGINE_VER_MAJ	1
#define TILENGINE_VER_MIN	19
#define TILENGINE_VER_REV	0
#define TILENGINE_HEADER_VERSION ((TILENGINE_VER_MAJ<<16) | (TILENGINE_VER_MIN<<8) | TILENGINE_VER_REV)
*/
//define BITVAL(n) (1<<(n))

/*! tile/sprite flags. Can be none or a combination of the following: */
typedef enum
{
	FLAG_NONE		= 0,			/*!< no flags */
	FLAG_FLIPX		= 1<<15,	/*!< horizontal flip */
	FLAG_FLIPY		= 1<<14,	/*!< vertical flip */
	FLAG_ROTATE		= 1<<13,	/*!< row/column flip (unsupported, Tiled compatibility) */
	FLAG_PRIORITY	= 1<<12,	/*!< tile goes in front of sprite layer */
}
TLN_TileFlags;

/* fixed point helper */
typedef int fix_t;
/*
#define FIXED_BITS	16
#define float2fix(f)	(fix_t)(f*(1 << FIXED_BITS))
#define int2fix(i)		((int)(i) << FIXED_BITS)
#define fix2int(f)		((int)(f) >> FIXED_BITS)
#define fix2float(f)	(float)(f)/(1 << FIXED_BITS)
*/
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
	BLEND_CUSTOM,	/*!< user provided blend function with TLN_SetCustomBlendFunction() */
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

/*! Tile item for Tilemap access methods */
typedef struct Tile
{
	uint16_t index;		/*!< tile index */
	uint16_t flags;		/*!< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY) */
}
Tile;

/*! frame animation definition */
typedef struct
{
	int index;		/*!< tile/sprite index */
	int delay;		/*!< time delay between frames */
}
TLN_SequenceFrame;

/*! color strip definition */
typedef struct
{
	int delay;		/*!< time delay between frames */
	uint8_t first;	/*!< index of first color to cycle */
	uint8_t count;	/*!< number of colors in the cycle */
	uint8_t dir;	/*!< direction: 0=descending, 1=ascending */
}
TLN_ColorStrip;

/*! sequence info returned by TLN_GetSequenceInfo */
typedef struct
{
	char name[32];	/*!< sequence name */
	int num_frames;	/*!< number of frames */
}
TLN_SequenceInfo;

/*! Sprite creation info for TLN_CreateSpriteset() */
typedef struct
{
	char name[64];	/*!< entry name */
	int x;			/*!< horizontal position */
	int y;			/*!< vertical position */
	int w;			/*!< width */
	int h;			/*!< height */
}
TLN_SpriteData;

/*! Sprite information */
typedef struct
{
	int w;			/*!< width of sprite */
	int h;			/*!< height of sprite */
}
TLN_SpriteInfo;

/*! Tile information returned by TLN_GetLayerTile() */
typedef struct
{
	uint16_t index;	/*!< tile index */
	uint16_t flags;	/*!< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY) */
	int row;		/*!< row number in the tilemap */
	int col;		/*!< col number in the tilemap */
	int xoffset;	/*!< horizontal position inside the title */
	int yoffset;	/*!< vertical position inside the title */
	uint8_t color;	/*!< color index at collision point */
	uint8_t type;	/*!< tile type */
	bool empty;		/*!< cell is empty*/
}
TLN_TileInfo;

/*! Tileset attributes for TLN_CreateTileset() */
typedef struct
{
	uint8_t	type;		/*!< tile type */
	bool	priority;	/*!< priority flag set */
}
TLN_TileAttributes;

/*! overlays for CRT effect */
typedef enum
{
	TLN_OVERLAY_NONE,		/*!< no overlay */
	TLN_OVERLAY_SHADOWMASK,	/*!< Shadow mask pattern */
	TLN_OVERLAY_APERTURE,	/*!< Aperture grille pattern */
	TLN_OVERLAY_SCANLINES,	/*!< Scanlines pattern */
	TLN_OVERLAY_CUSTOM,		/*!< User-provided when calling TLN_CreateWindow() */
	TLN_MAX_OVERLAY
}
TLN_Overlay;

/*! pixel mapping for TLN_SetLayerPixelMapping() */
typedef struct
{
	int16_t dx;		/*! horizontal pixel displacement */
	int16_t dy;		/*! vertical pixel displacement */
}
TLN_PixelMap;

typedef struct Tile*		 TLN_Tile;				/*!< Tile reference */
typedef struct Tileset*		 TLN_Tileset;			/*!< Opaque tileset reference */
typedef struct Tilemap*		 TLN_Tilemap;			/*!< Opaque tilemap reference */
typedef struct Palette*		 TLN_Palette;			/*!< Opaque palette reference */
typedef struct Spriteset*	 TLN_Spriteset;			/*!< Opaque sspriteset reference */
typedef struct Sequence*	 TLN_Sequence;			/*!< Opaque sequence reference */
typedef struct SequencePack* TLN_SequencePack;		/*!< Opaque sequence pack reference */
typedef struct Bitmap*		 TLN_Bitmap;			/*!< Opaque bitmap reference */

/*! Player index for input assignment functions */
typedef enum
{
	PLAYER1,	/*!< Player 1 */
	PLAYER2,	/*!< Player 2 */
	PLAYER3,	/*!< Player 3 */
	PLAYER4,	/*!< Player 4 */
}
TLN_Player;

/*! Standard inputs query for TLN_GetInput() */
typedef enum
{
	INPUT_NONE,		/*!< no input */
	INPUT_UP,		/*!< up direction */
	INPUT_DOWN,		/*!< down direction */
	INPUT_LEFT,		/*!< left direction */
	INPUT_RIGHT,	/*!< right direction */
	INPUT_BUTTON1,	/*!< 1st action button */
	INPUT_BUTTON2,	/*!< 2nd action button */
	INPUT_BUTTON3,	/*!< 3th action button */
	INPUT_BUTTON4,	/*!< 4th action button */
	INPUT_BUTTON5,	/*!< 5th action button */
	INPUT_BUTTON6,	/*!< 6th action button */
	INPUT_START,	/*!< Start button */

	INPUT_P1 = (PLAYER1 << 4), 	/*!< request player 1 input (default) */
	INPUT_P2 = (PLAYER2 << 4),	/*!< request player 2 input */
	INPUT_P3 = (PLAYER3 << 4),	/*!< request player 3 input */
	INPUT_P4 = (PLAYER4 << 4),	/*!< request player 4 input */
	
	/* compatibility symbols for pre-1.18 input model */ 
	INPUT_A = INPUT_BUTTON1,
	INPUT_B = INPUT_BUTTON2,
	INPUT_C = INPUT_BUTTON3,
	INPUT_D = INPUT_BUTTON4,
	INPUT_E = INPUT_BUTTON5,
	INPUT_F = INPUT_BUTTON6,
	
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
	CWF_NEAREST		=   (1<<6),	/*<! unfiltered upscaling */
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
	TLN_ERR_REF_SEQUENCE,	/*!< Invalid TLN_Sequence reference */
	TLN_ERR_REF_SEQPACK,	/*!< Invalid TLN_SequencePack reference */
	TLN_ERR_REF_BITMAP,		/*!< Invalid TLN_Bitmap reference */
	TLN_ERR_NULL_POINTER,	/*!< Null pointer as argument */ 
	TLN_ERR_FILE_NOT_FOUND,	/*!< Resource file not found */
	TLN_ERR_WRONG_FORMAT,	/*!< Resource file has invalid format */
	TLN_ERR_WRONG_SIZE,		/*!< A width or height parameter is invalid */
	TLN_ERR_UNSUPPORTED,	/*!< Unsupported function */
	TLN_MAX_ERR,
}
TLN_Error;

/** 
 * \anchor group_setup
 * \name Setup
 * Basic setup and management */
/**@{*/
 bool TLN_Init (int hres, int vres, int numlayers, int numsprites, int numanimations);
 bool TLN_InitBPP (int hres, int vres, int bpp, int numlayers, int numsprites, int numanimations);
 void TLN_Deinit (void);
 int TLN_GetWidth (void);
 int TLN_GetHeight (void);
 int TLN_GetBPP (void);
 uint32_t TLN_GetNumObjects (void);
 uint32_t TLN_GetUsedMemory (void);
 uint32_t TLN_GetVersion (void);
 int TLN_GetNumLayers (void);
 int TLN_GetNumSprites (void);
 void TLN_SetBGColor (uint8_t r, uint8_t g, uint8_t b);
 bool TLN_SetBGColorFromTilemap (TLN_Tilemap tilemap);
 void TLN_DisableBGColor (void);
 bool TLN_SetBGBitmap (TLN_Bitmap bitmap);
 bool TLN_SetBGPalette (TLN_Palette palette);
 void TLN_SetRasterCallback (void (*callback)(int));
 void TLN_SetFrameCallback (void (*callback)(int));
 void TLN_SetRenderTarget (uint8_t* data, int pitch);
 void TLN_UpdateFrame (int time);
 void TLN_BeginFrame (int time);
 bool TLN_DrawNextScanline (void);
 void TLN_SetLoadPath (const char* path);
 void TLN_SetCustomBlendFunction (uint8_t (*blend_function)(uint8_t src, uint8_t dst));

/**@}*/

/** 
 * \anchor group_errors
 * \name Errors
 * Error handling */
/**@{*/
 void TLN_SetLastError (TLN_Error error);
 TLN_Error TLN_GetLastError (void);
 const char *TLN_GetErrorString (TLN_Error error);
/**@}*/

/** 
 * \anchor group_windowing
 * \name Windowing
 * Built-in window and input management */
/**@{*/
 bool TLN_CreateWindow (const char* overlay, TLN_WindowFlags flags);
 bool TLN_CreateWindowThread (const char* overlay, TLN_WindowFlags flags);
 void TLN_SetWindowTitle (const char* title);
 bool TLN_ProcessWindow (void);
 bool TLN_IsWindowActive (void);
 bool TLN_GetInput (TLN_Input id);
 void TLN_EnableInput (TLN_Player player, bool enable);
 void TLN_AssignInputJoystick (TLN_Player player, int index);
 void TLN_DefineInputKey (TLN_Player player, TLN_Input input, uint32_t keycode);
 void TLN_DefineInputButton (TLN_Player player, TLN_Input input, uint8_t joybutton);
 void TLN_DrawFrame (int time);
 void TLN_WaitRedraw (void);
 void TLN_DeleteWindow (void);
 void TLN_EnableBlur (bool mode);
 void TLN_EnableCRTEffect (TLN_Overlay overlay, uint8_t overlay_factor, uint8_t threshold, uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, bool blur, uint8_t glow_factor);
 void TLN_DisableCRTEffect (void);
 void TLN_Delay (uint32_t msecs);
 uint32_t TLN_GetTicks (void);
 void TLN_BeginWindowFrame (int time);
 void TLN_EndWindowFrame (void);

/**@}*/

/** 
 * \anchor group_spriteset
 * \name Spritesets
 * Spriteset resources management for sprites */
/**@{*/
 TLN_Spriteset TLN_CreateSpriteset (TLN_Bitmap bitmap, TLN_SpriteData* data, int num_entries);
 TLN_Spriteset TLN_LoadSpriteset (const char* name);
 TLN_Spriteset TLN_CloneSpriteset (TLN_Spriteset src);
 bool TLN_GetSpriteInfo (TLN_Spriteset spriteset, int entry, TLN_SpriteInfo* info);
 TLN_Palette TLN_GetSpritesetPalette (TLN_Spriteset spriteset);
 int TLN_FindSpritesetSprite (TLN_Spriteset spriteset, char* name);
 bool TLN_SetSpritesetData (TLN_Spriteset spriteset, int entry, TLN_SpriteData* data, void* pixels, int pitch);
 bool TLN_DeleteSpriteset (TLN_Spriteset Spriteset);
/**@}*/

/** 
 * \anchor group_tileset
 * \name Tilesets
 * Tileset resources management for background layers */
/**@{*/
 TLN_Tileset TLN_CreateTileset (int numtiles, int width, int height, TLN_Palette palette, TLN_SequencePack sp, TLN_TileAttributes* attributes);
 TLN_Tileset TLN_LoadTileset (const char* filename);
 TLN_Tileset TLN_CloneTileset (TLN_Tileset src);
 bool TLN_SetTilesetPixels (TLN_Tileset tileset, int entry, uint8_t* srcdata, int srcpitch);
 bool TLN_CopyTile (TLN_Tileset tileset, int src, int dst);
 int TLN_GetTileWidth (TLN_Tileset tileset);
 int TLN_GetTileHeight (TLN_Tileset tileset);
 TLN_Palette TLN_GetTilesetPalette (TLN_Tileset tileset);
 TLN_SequencePack TLN_GetTilesetSequencePack (TLN_Tileset tileset);
 bool TLN_DeleteTileset (TLN_Tileset tileset);
/**@}*/

/** 
 * \anchor group_tilemap
 * \name Tilemaps 
 * Tilemap resources management for background layers */
/**@{*/
 TLN_Tilemap TLN_CreateTilemap (int rows, int cols, TLN_Tile tiles, uint32_t bgcolor, TLN_Tileset tileset);
 TLN_Tilemap TLN_LoadTilemap (const char* filename, const char* layername);
 TLN_Tilemap TLN_CloneTilemap (TLN_Tilemap src);
 int TLN_GetTilemapRows (TLN_Tilemap tilemap);
 int TLN_GetTilemapCols (TLN_Tilemap tilemap);
 TLN_Tileset TLN_GetTilemapTileset (TLN_Tilemap tilemap);
 bool TLN_GetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);
 bool TLN_SetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);
 bool TLN_CopyTiles (TLN_Tilemap src, int srcrow, int srccol, int rows, int cols, TLN_Tilemap dst, int dstrow, int dstcol);
 bool TLN_DeleteTilemap (TLN_Tilemap tilemap);
/**@}*/

/** 
 * \anchor group_palette
 * \name Palettes
 * Color palette resources management for sprites and background layers */
/**@{*/
 TLN_Palette TLN_CreatePalette (int entries);
 TLN_Palette TLN_LoadPalette (const char* filename);
 TLN_Palette TLN_ClonePalette (TLN_Palette src);
 bool TLN_SetPaletteColor (TLN_Palette palette, int color, uint8_t r, uint8_t g, uint8_t b);
 bool TLN_MixPalettes (TLN_Palette src1, TLN_Palette src2, TLN_Palette dst, uint8_t factor);
 bool TLN_AddPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);
 bool TLN_SubPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);
 bool TLN_ModPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);
 uint8_t* TLN_GetPaletteData (TLN_Palette palette, int index);
 bool TLN_DeletePalette (TLN_Palette palette);
/**@}*/

/** 
 * \anchor group_bitmap
 * \name Bitmaps 
 * Bitmap management */
/**@{*/
 TLN_Bitmap TLN_CreateBitmap (int width, int height, int bpp);
 TLN_Bitmap TLN_LoadBitmap (const char* filename);
 TLN_Bitmap TLN_CloneBitmap (TLN_Bitmap src);
 uint8_t* TLN_GetBitmapPtr (TLN_Bitmap bitmap, int x, int y);
 int TLN_GetBitmapWidth (TLN_Bitmap bitmap);
 int TLN_GetBitmapHeight (TLN_Bitmap bitmap);
 int TLN_GetBitmapDepth (TLN_Bitmap bitmap);
 int TLN_GetBitmapPitch (TLN_Bitmap bitmap);
 TLN_Palette TLN_GetBitmapPalette (TLN_Bitmap bitmap);
 bool TLN_SetBitmapPalette (TLN_Bitmap bitmap, TLN_Palette palette);
 bool TLN_DeleteBitmap (TLN_Bitmap bitmap);
/**@}*/

/** 
 * \anchor group_layer
 * \name Layers
 * Background layers management */
/**@{*/
 bool TLN_SetLayer (int nlayer, TLN_Tileset tileset, TLN_Tilemap tilemap);
 bool TLN_SetLayerPalette (int nlayer, TLN_Palette palette);
 bool TLN_SetLayerPosition (int nlayer, int hstart, int vstart);
 bool TLN_SetLayerScaling (int nlayer, float xfactor, float yfactor);
 bool TLN_SetLayerAffineTransform (int nlayer, TLN_Affine *affine);
 bool TLN_SetLayerTransform (int layer, float angle, float dx, float dy, float sx, float sy);
 bool TLN_SetLayerPixelMapping (int nlayer, TLN_PixelMap* table);
 bool TLN_SetLayerBlendMode (int nlayer, TLN_Blend mode, uint8_t factor);
 bool TLN_SetLayerColumnOffset (int nlayer, int* offset);
 bool TLN_SetLayerClip (int nlayer, int x1, int y1, int x2, int y2);
 bool TLN_DisableLayerClip (int nlayer);
 bool TLN_SetLayerMosaic (int nlayer, int width, int height);
 bool TLN_DisableLayerMosaic (int nlayer);
 bool TLN_ResetLayerMode (int nlayer);
 bool TLN_DisableLayer (int nlayer);
 TLN_Palette TLN_GetLayerPalette (int nlayer);
 bool TLN_GetLayerTile (int nlayer, int x, int y, TLN_TileInfo* info);
 int  TLN_GetLayerWidth (int nlayer);
 int  TLN_GetLayerHeight (int nlayer);

/**@}*/

/** 
 * \anchor group_sprite
 * \name Sprites 
 * Sprites management */
/**@{*/
 bool TLN_ConfigSprite (int nsprite, TLN_Spriteset spriteset, TLN_TileFlags flags);
 bool TLN_SetSpriteSet (int nsprite, TLN_Spriteset spriteset);
 bool TLN_SetSpriteFlags (int nsprite, TLN_TileFlags flags);
 bool TLN_SetSpritePosition (int nsprite, int x, int y);
 bool TLN_SetSpritePicture (int nsprite, int entry);
 bool TLN_SetSpritePalette (int nsprite, TLN_Palette palette);
 bool TLN_SetSpriteBlendMode (int nsprite, TLN_Blend mode, uint8_t factor);
 bool TLN_SetSpriteScaling (int nsprite, float sx, float sy);
 bool TLN_ResetSpriteScaling (int nsprite);
 int  TLN_GetSpritePicture (int nsprite);
 int  TLN_GetAvailableSprite (void);
 bool TLN_EnableSpriteCollision (int nsprite, bool enable);
 bool TLN_GetSpriteCollision (int nsprite);
 bool TLN_DisableSprite (int nsprite);
 TLN_Palette TLN_GetSpritePalette (int nsprite);
/**@}*/

/** 
 * \anchor group_sequence
 * \name Sequences
 * Sequence resources management for layer, sprite and palette animations */
/**@{*/
 TLN_Sequence TLN_CreateSequence (const char* name, int target, int num_frames, TLN_SequenceFrame* frames);
 TLN_Sequence TLN_CreateCycle (const char* name, int num_strips, TLN_ColorStrip* strips);
 TLN_Sequence TLN_CloneSequence (TLN_Sequence src);
 bool TLN_GetSequenceInfo (TLN_Sequence sequence, TLN_SequenceInfo* info);
 bool TLN_DeleteSequence (TLN_Sequence sequence);
/**@}*/

/** 
 * \anchor group_sequencepack
 * \name Sequence packs
 * Sequence pack manager for grouping and finding sequences */
/**@{*/
 TLN_SequencePack TLN_CreateSequencePack (void);
 TLN_SequencePack TLN_LoadSequencePack (const char* filename);
 TLN_SequencePack TLN_CloneSequencePack (TLN_SequencePack src);
 TLN_Sequence TLN_GetSequence (TLN_SequencePack sp, int index);
 TLN_Sequence TLN_FindSequence (TLN_SequencePack sp, const char* name);
 int TLN_GetSequencePackCount (TLN_SequencePack sp);
 bool TLN_AddSequenceToPack (TLN_SequencePack sp, TLN_Sequence sequence);
 bool TLN_DeleteSequencePack (TLN_SequencePack sp);
/**@}*/

/** 
 * \anchor group_animation
 * \name Animations 
 * Animation engine manager */
/**@{*/
 bool TLN_SetPaletteAnimation (int index, TLN_Palette palette, TLN_Sequence sequence, bool blend);
 bool TLN_SetPaletteAnimationSource (int index, TLN_Palette);
 bool TLN_SetTilesetAnimation (int index, int nlayer, TLN_Sequence);
 bool TLN_SetTilemapAnimation (int index, int nlayer, TLN_Sequence);
 bool TLN_SetSpriteAnimation (int index, int nsprite, TLN_Sequence sequence, int loop);
 bool TLN_GetAnimationState (int index);
 bool TLN_SetAnimationDelay (int index, int delay);
 int  TLN_GetAvailableAnimation (void);
 bool TLN_DisableAnimation (int index);
/**@}*/

]]