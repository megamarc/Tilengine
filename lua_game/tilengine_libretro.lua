-- TileEngine 2.4.1
local ffi = require'ffi'

ffi.cdef[[

/*! tile/sprite flags. Can be none or a combination of the following: */
typedef enum
{
	FLAG_NONE		= 0,		/*!< no flags */
	FLAG_FLIPX		= 1 << 15,	/*!< horizontal flip */
	FLAG_FLIPY		= 1 << 14,	/*!< vertical flip */
	FLAG_ROTATE		= 1 << 13,	/*!< row/column flip (unsupported, Tiled compatibility) */
	FLAG_PRIORITY	= 1 << 12,	/*!< tile goes in front of sprite layer */
}
TLN_TileFlags;

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
typedef union Tile
{
	uint32_t value;
	struct
	{
		uint16_t index;		/*!< tile index */
		uint16_t flags;		/*!< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY) */
	};
}
Tile;

/*! frame animation definition */
typedef struct
{
	int index;		/*!< tile/sprite index */
	int delay;		/*!< time delay for next frame */
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

/*! ObjectList item for TLN_CreateObjectList() */
typedef struct
{
	int id;		/*!< object unique identifier */
	int gid;	/*!< graphic identifier (tile index in Tiled) */
	int x;
	int y;
	int width;
	int height;
}
TLN_Object;

/*! pixel mapping for TLN_SetLayerPixelMapping() */
typedef struct
{
	int16_t dx;		/*! horizontal pixel displacement */
	int16_t dy;		/*! vertical pixel displacement */
}
TLN_PixelMap;

typedef union  Tile*		 TLN_Tile;				/*!< Tile reference */
typedef struct Tileset*		 TLN_Tileset;			/*!< Opaque tileset reference */
typedef struct Tilemap*		 TLN_Tilemap;			/*!< Opaque tilemap reference */
typedef struct Palette*		 TLN_Palette;			/*!< Opaque palette reference */
typedef struct Spriteset*	 TLN_Spriteset;			/*!< Opaque sspriteset reference */
typedef struct Sequence*	 TLN_Sequence;			/*!< Opaque sequence reference */
typedef struct SequencePack* TLN_SequencePack;		/*!< Opaque sequence pack reference */
typedef struct Bitmap*		 TLN_Bitmap;			/*!< Opaque bitmap reference */
typedef struct ObjectList*	 TLN_ObjectList;		/*!< Opaque object list reference */

/*! Sprite state */
typedef struct
{
	int x;						/*!< Screen position x */
	int y;						/*!< Screen position y */
	int w;						/*!< Actual width in screen (after scaling) */
	int h;						/*!< Actual height in screen (after scaling) */
	TLN_TileFlags flags;		/*!< flags */
	TLN_Palette palette;		/*!< assigned palette */
	TLN_Spriteset spriteset;	/*!< assigned spriteset */
	int index;					/*!< graphic index inside spriteset */
	bool enabled;				/*!< enabled or not */
	bool collision;				/*!< per-pixel collision detection enabled or not */
}
TLN_SpriteState;

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

/*! Debug level */
typedef enum
{
	TLN_LOG_NONE,		/*!< Do not print anything (default) */
	TLN_LOG_ERRORS,		/*!< Print only runtime errors */
	TLN_LOG_VERBOSE,	/*!< Print everything */
}
TLN_LogLevel;

/**@}*/

/**
 * \defgroup setup
 * \brief Basic setup and management
 * @{ */
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
void TLN_SetLoadPath (const char* path);
void TLN_SetLogLevel(TLN_LogLevel log_level);
bool TLN_OpenResourcePack(const char* filename, const char* key);
void TLN_CloseResourcePack(void);

/*! Player index for input checking */
typedef enum
{
	PLAYER1,	/*!< Player 1 */
	PLAYER2,	/*!< Player 2 */
	PLAYER3,	/*!< Player 3 */
	PLAYER4,	/*!< Player 4 */
}
TLN_Player;

/* input flags for input_mask,
   values aligned with RETRO_DEVICE_ID_JOYPAD_n */
typedef enum
{
	INPUT_BUTTON1	= 0x0001,
	INPUT_BUTTON2	= 0x0002,
	INPUT_SELECT	= 0x0004,
	INPUT_START		= 0x0008,
	INPUT_UP		= 0x0010,
	INPUT_DOWN		= 0x0020,
	INPUT_LEFT		= 0x0040,
	INPUT_RIGHT		= 0x0080,
	INPUT_BUTTON3	= 0x0100,
	INPUT_BUTTON4	= 0x0200,
}
TLN_Input;

void SetFrameCallbackName(const char* name);
void SetRasterCallbackName(const char* name);
bool CheckRetroInput(TLN_Player player, TLN_Input input);

/**@}*/

/**
 * \defgroup errors
 * \brief Basic setup and management
* @{ */
void TLN_SetLastError (TLN_Error error);
TLN_Error TLN_GetLastError (void);
const char *TLN_GetErrorString (TLN_Error error);
/**@}*/

/**
 * \defgroup spriteset
 * \brief Spriteset resources management for sprites
* @{ */
TLN_Spriteset TLN_CreateSpriteset (TLN_Bitmap bitmap, TLN_SpriteData* data, int num_entries);
TLN_Spriteset TLN_LoadSpriteset (const char* name);
TLN_Spriteset TLN_CloneSpriteset (TLN_Spriteset src);
bool TLN_GetSpriteInfo (TLN_Spriteset spriteset, int entry, TLN_SpriteInfo* info);
TLN_Palette TLN_GetSpritesetPalette (TLN_Spriteset spriteset);
int TLN_FindSpritesetSprite (TLN_Spriteset spriteset, const char* name);
bool TLN_SetSpritesetData (TLN_Spriteset spriteset, int entry, TLN_SpriteData* data, void* pixels, int pitch);
bool TLN_DeleteSpriteset (TLN_Spriteset Spriteset);
/**@}*/

/**
 * \defgroup tileset
 * \brief Tileset resources management for background layers
* @{ */
TLN_Tileset TLN_CreateTileset (int numtiles, int width, int height, TLN_Palette palette, TLN_SequencePack sp, TLN_TileAttributes* attributes);
TLN_Tileset TLN_LoadTileset (const char* filename);
TLN_Tileset TLN_CloneTileset (TLN_Tileset src);
bool TLN_SetTilesetPixels (TLN_Tileset tileset, int entry, uint8_t* srcdata, int srcpitch);
bool TLN_CopyTile (TLN_Tileset tileset, int src, int dst);
int TLN_GetTileWidth (TLN_Tileset tileset);
int TLN_GetTileHeight (TLN_Tileset tileset);
int TLN_GetTilesetNumTiles(TLN_Tileset tileset);
TLN_Palette TLN_GetTilesetPalette (TLN_Tileset tileset);
TLN_SequencePack TLN_GetTilesetSequencePack (TLN_Tileset tileset);
bool TLN_DeleteTileset (TLN_Tileset tileset);
/**@}*/

/**
 * \defgroup tilemap
 * \brief Tilemap resources management for background layers
* @{ */
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
 * \defgroup palette
 * \brief Color palette resources management for sprites and background layers
* @{ */
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
 * \defgroup bitmap
 * \brief Bitmap management
* @{ */
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
 * \defgroup objects
 * \brief ObjectList resources management
 * @{ */
TLN_ObjectList TLN_CreateObjectList(void);
bool TLN_AddObjectToList(TLN_ObjectList list, TLN_Object* data);
bool TLN_AddSpriteToList(TLN_ObjectList list, TLN_Spriteset spriteset, const char* name, int id, int x, int y);
TLN_ObjectList TLN_LoadObjectList(const char* filename, const char* layername, int firstgid);
TLN_ObjectList TLN_CloneObjectList(TLN_ObjectList src);
int TLN_GetObjectsInReigion(TLN_ObjectList list, int x, int y, int width, int height, int array_size, TLN_Object* objects[]);
bool TLN_DeleteObjectList(TLN_ObjectList list);
/**@}*/

/**
 * \defgroup layer
 * \brief Background layers management
* @{ */
bool TLN_SetLayer (int nlayer, TLN_Tileset tileset, TLN_Tilemap tilemap);
bool TLN_SetLayerBitmap(int nlayer, TLN_Bitmap bitmap);
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
bool TLN_SetLayerObjects(int nlayer, TLN_ObjectList objects, TLN_Spriteset spriteset, int width, int height);
bool TLN_SetLayerPriority(int nlayer, bool enable);
bool TLN_SetLayerParent(int nlayer, int parent);
bool TLN_DisableLayerParent(int nlayer);
bool TLN_DisableLayer (int nlayer);
TLN_Palette TLN_GetLayerPalette (int nlayer);
bool TLN_GetLayerTile (int nlayer, int x, int y, TLN_TileInfo* info);
int  TLN_GetLayerWidth (int nlayer);
int  TLN_GetLayerHeight (int nlayer);

/**@}*/

/**
 * \defgroup sprite
 * \brief Sprites management
* @{ */
bool TLN_ConfigSprite (int nsprite, TLN_Spriteset spriteset, TLN_TileFlags flags);
bool TLN_SetSpriteSet (int nsprite, TLN_Spriteset spriteset);
bool TLN_SetSpriteFlags (int nsprite, TLN_TileFlags flags);
bool TLN_SetSpritePosition (int nsprite, int x, int y);
bool TLN_SetSpritePicture (int nsprite, int entry);
bool TLN_SetSpritePalette (int nsprite, TLN_Palette palette);
bool TLN_SetSpriteBlendMode (int nsprite, TLN_Blend mode, uint8_t factor);
bool TLN_SetSpriteScaling (int nsprite, float sx, float sy);
bool TLN_ResetSpriteScaling (int nsprite);
//bool TLN_SetSpriteRotation (int nsprite, float angle);
//bool TLN_ResetSpriteRotation (int nsprite);
int  TLN_GetSpritePicture (int nsprite);
int  TLN_GetAvailableSprite (void);
bool TLN_EnableSpriteCollision (int nsprite, bool enable);
bool TLN_GetSpriteCollision (int nsprite);
bool TLN_GetSpriteState(int nsprite, TLN_SpriteState* state);
bool TLN_DisableSprite (int nsprite);
TLN_Palette TLN_GetSpritePalette (int nsprite);
/**@}*/

/**
 * \defgroup sequence
 * \brief Sequence resources management for layer, sprite and palette animations
* @{ */
TLN_Sequence TLN_CreateSequence (const char* name, int target, int num_frames, TLN_SequenceFrame* frames);
TLN_Sequence TLN_CreateCycle (const char* name, int num_strips, TLN_ColorStrip* strips);
TLN_Sequence TLN_CreateSpriteSequence(const char* name, TLN_Spriteset spriteset, char* basename, int count, int delay);
TLN_Sequence TLN_CloneSequence (TLN_Sequence src);
bool TLN_GetSequenceInfo (TLN_Sequence sequence, TLN_SequenceInfo* info);
bool TLN_DeleteSequence (TLN_Sequence sequence);
/**@}*/

/**
 * \defgroup sequencepack
 * \brief Sequence pack manager for grouping and finding sequences
* @{ */
TLN_SequencePack TLN_CreateSequencePack (void);
TLN_SequencePack TLN_LoadSequencePack (const char* filename);
TLN_Sequence TLN_GetSequence (TLN_SequencePack sp, int index);
TLN_Sequence TLN_FindSequence (TLN_SequencePack sp, const char* name);
int TLN_GetSequencePackCount (TLN_SequencePack sp);
bool TLN_AddSequenceToPack (TLN_SequencePack sp, TLN_Sequence sequence);
bool TLN_DeleteSequencePack (TLN_SequencePack sp);
/**@}*/

/**
 * \defgroup animation
 * \brief Animation engine manager
* @{ */
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