/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef _TILENGINE_H
#define _TILENGINE_H

/**
 * \defgroup types
 * \brief Common data types
 * @{ */

/* Tilengine shared */
#if defined _MSC_VER
	#ifdef LIB_EXPORTS
		#define TLNAPI __declspec(dllexport)
	#else
		#define TLNAPI __declspec(dllimport)
	#endif
	
	#pragma warning(disable : 4200)

	#if _MSC_VER >= 1600	/* Visual C++ 2010? */
		#include <stdint.h>
	#else
		typedef char			int8_t;		/*!< signed 8-bit wide data */
		typedef short			int16_t;	/*!< signed 16-bit wide data */
		typedef int				int32_t;	/*!< signed 32-bit wide data */
		typedef unsigned char	uint8_t;	/*!< unsigned 8-bit wide data */
		typedef unsigned short	uint16_t;	/*!< unsigned 16-bit wide data */
		typedef unsigned int	uint32_t;	/*!< unsigned 32-bit wide data */
	#endif

	#if _MSC_VER >= 1800	/* Visual C++ 2013? */
		#include <stdbool.h>
	#else
		typedef unsigned char bool;		/*!< C++ bool type for C language */
		#define false	0
		#define true	1
	#endif

#else
	#ifdef LIB_EXPORTS
		#define TLNAPI __attribute__((visibility("default")))
	#else
		#define TLNAPI
	#endif
	#include <stdint.h>
	#include <stdbool.h>
#endif

#include <stdio.h>

/* version */
#define TILENGINE_VER_MAJ	2
#define TILENGINE_VER_MIN	15
#define TILENGINE_VER_REV	5
#define TILENGINE_HEADER_VERSION ((TILENGINE_VER_MAJ << 16) | (TILENGINE_VER_MIN << 8) | TILENGINE_VER_REV)

#define BITVAL(n) (1<<(n))

/*! tile/sprite flags. Can be none or a combination of the following: */
typedef enum
{
	FLAG_NONE		= 0,			/*!< no flags */
	FLAG_FLIPX		= BITVAL(15),	/*!< horizontal flip */
	FLAG_FLIPY		= BITVAL(14),	/*!< vertical flip */
	FLAG_ROTATE		= BITVAL(13),	/*!< row/column flip (unsupported, Tiled compatibility) */
	FLAG_PRIORITY	= BITVAL(12),	/*!< tile goes in front of sprite layer */
	FLAG_MASKED		= BITVAL(11),	/*!< sprite won't be drawn inside masked region */
	FLAG_TILESET	= (15 << 7),	/*!< tileset index (0 - 15) */
	FLAG_PALETTE	= (7 << 4),		/*!< palette index (0 - 7) */
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

/*!
 * layer type retrieved by \ref TLN_GetLayerType
 */
typedef enum
{
	LAYER_NONE,		/*!< undefined */
	LAYER_TILE,		/*!< tilemap-based layer */
	LAYER_OBJECT,	/*!< objects layer */
	LAYER_BITMAP,	/*!< bitmapped layer */
}
TLN_LayerType;

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
		union
		{
			uint16_t flags;	/*!< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY) */
			struct
			{
				uint8_t unused : 4;
				uint8_t palette : 3;
				uint8_t tileset : 4;
				
				// COMPILER ERROR: although whole struct fits in 32 bits, compiler expands to 64, causing addressing errors
				/*
				bool masked : 1;
				bool priority : 1;
				bool rotated : 1;
				bool flipy : 1;
				bool flipx : 1;
				*/
			};
		};
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

/*! Object item info returned by TLN_GetObjectInfo() */
typedef struct
{
	uint16_t id;	/*!< unique ID */
	uint16_t gid;	/*!< graphic ID (tile index) */
	uint16_t flags;	/*!< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY) */
	int x;			/*!< horizontal position */
	int y;			/*!< vertical position */
	int width;		/*!< horizontal size */
	int height;		/*!< vertical size */
	uint8_t type;	/*!< type property */
	bool visible;	/*!< visible property */
	char name[64];	/*!< name property */
}
TLN_ObjectInfo;

/*! Tileset attributes for TLN_CreateTileset() */
typedef struct
{
	uint8_t	type;		/*!< tile type */
	bool	priority;	/*!< priority flag set */
}
TLN_TileAttributes;

/* kept for backwards compatibility with pre-2.10 release */
#define TLN_OVERLAY_NONE		0
#define TLN_OVERLAY_SHADOWMASK	0
#define TLN_OVERLAY_APERTURE	0
#define TLN_OVERLAY_SCANLINES	0
#define TLN_OVERLAY_CUSTOM		0

/*! types of built-in CRT effect for \ref TLN_ConfigCRTEffect */
typedef enum
{
	TLN_CRT_NONE,		/*!< no overlay */
	TLN_CRT_SLOT,		/*!< slot mask */
	TLN_CRT_APERTURE,	/*!< aperture grille */
	TLN_CRT_SHADOW,		/*!< shadow mask */
	TLN_CRT_HORIZONTAL,	/*!< horizontal subpixels */
}
TLN_CRT;

/*! pixel mapping for TLN_SetLayerPixelMapping() */
typedef struct
{
	int16_t dx;		/*!< horizontal pixel displacement */
	int16_t dy;		/*!< vertical pixel displacement */
}
TLN_PixelMap;

typedef struct Engine*		 TLN_Engine;			/*!< Engine context */
typedef union  Tile*		 TLN_Tile;				/*!< Tile reference */
typedef struct Tileset*		 TLN_Tileset;			/*!< Opaque tileset reference */
typedef struct Tilemap*		 TLN_Tilemap;			/*!< Opaque tilemap reference */
typedef struct Palette*		 TLN_Palette;			/*!< Opaque palette reference */
typedef struct Spriteset*	 TLN_Spriteset;			/*!< Opaque sspriteset reference */
typedef struct Sequence*	 TLN_Sequence;			/*!< Opaque sequence reference */
typedef struct SequencePack* TLN_SequencePack;		/*!< Opaque sequence pack reference */
typedef struct Bitmap*		 TLN_Bitmap;			/*!< Opaque bitmap reference */
typedef struct ObjectList*	 TLN_ObjectList;		/*!< Opaque object list reference */

/*! Image Tile items for TLN_CreateImageTileset() */
typedef struct
{
	TLN_Bitmap bitmap;
	uint16_t id;
	uint8_t	type;
}
TLN_TileImage;

/*! Sprite state */
typedef struct
{
	int x;						/*!< Screen position x */
	int y;						/*!< Screen position y */
	int w;						/*!< Actual width in screen (after scaling) */
	int h;						/*!< Actual height in screen (after scaling) */
	uint32_t flags;				/*!< flags */
	TLN_Palette palette;		/*!< assigned palette */	 
	TLN_Spriteset spriteset;	/*!< assigned spriteset */	
	int index;					/*!< graphic index inside spriteset */
	bool enabled;				/*!< enabled or not */
	bool collision;				/*!< per-pixel collision detection enabled or not */
}
TLN_SpriteState;

/* callbacks */
typedef union SDL_Event SDL_Event;
typedef void(*TLN_VideoCallback)(int scanline);
typedef uint8_t(*TLN_BlendFunction)(uint8_t src, uint8_t dst);
typedef void(*TLN_SDLCallback)(SDL_Event*);

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

	/* ... up to 32 unique inputs */
	INPUT_QUIT = 30,/*!< Window close (only Player 1 keyboard) */
	INPUT_CRT,		/*!< CRT toggle (only Player 1 keyboard) */

	INPUT_P1 = (PLAYER1 << 5), 	/*!< request player 1 input (default) */
	INPUT_P2 = (PLAYER2 << 5),	/*!< request player 2 input */
	INPUT_P3 = (PLAYER3 << 5),	/*!< request player 3 input */
	INPUT_P4 = (PLAYER4 << 5),	/*!< request player 4 input */

	/* X-Input button names */
	INPUT_A = INPUT_BUTTON1,
	INPUT_B,
	INPUT_X,
	INPUT_Y,
	INPUT_LB,
	INPUT_RB,
	INPUT_LT,
	INPUT_RT,
	INPUT_SELECT,
	INPUT_START,
	INPUT_L3,
	INPUT_R3,
}
TLN_Input;

/*! CreateWindow flags. Can be none or a combination of the following: */
enum
{
	CWF_FULLSCREEN	= (1 << 0),	/*!< create a fullscreen window */
	CWF_VSYNC		= (1 << 1),	/*!< sync frame updates with vertical retrace */
	CWF_S1			= (1 << 2),	/*!< create a window the same size as the framebuffer */
	CWF_S2			= (2 << 2),	/*!< create a window 2x the size the framebuffer */
	CWF_S3			= (3 << 2),	/*!< create a window 3x the size the framebuffer */
	CWF_S4			= (4 << 2),	/*!< create a window 4x the size the framebuffer */
	CWF_S5			= (5 << 2),	/*!< create a window 5x the size the framebuffer */
	CWF_NEAREST		= (1 << 6),	/*<! unfiltered upscaling */
	CWF_NOVSYNC		= (1 << 7)  /*<! disable default vsync */
};

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
	TLN_ERR_REF_LIST,		/*!< Invalid TLN_ObjectList reference */
	TLN_ERR_IDX_PALETTE,	/*!< Palette index out of range */
	TLN_MAX_ERR,
}
TLN_Error;

/*! Debug level */
typedef enum
{
	TLN_LOG_NONE,		/*!< Don't print anything (default) */
	TLN_LOG_ERRORS,		/*!< Print only runtime errors */
	TLN_LOG_VERBOSE,	/*!< Print everything */
}
TLN_LogLevel;

/**@}*/

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \defgroup setup
 * \brief Basic setup and management
 * @{ */

 /*!
	\brief Initializes the graphic engine
	\param hres horizontal resolution in pixels
	\param vres vertical resolution in pixels
	\param numlayers number of layers
	\param numsprites number of sprites
	\param numanimations number of palette animation slots
		
	Performs initialisation of the main engine, creates the viewport with the specified dimensions
	and allocates the number of layers, sprites and animation slots
*/
TLNAPI TLN_Engine TLN_Init (int hres, int vres, int numlayers, int numsprites, int numanimations);

/*!	\brief Deinitialises current engine context and frees used resources */
TLNAPI void TLN_Deinit (void);

/*!
	\brief Deletes explicit context
	\param context context reference to delete
 */
TLNAPI bool TLN_DeleteContext (TLN_Engine context);

/*!
	\brief Sets current engine context
	\param context TLN_Engine object to set as current context, returned by TLN_Init()
	\returns true if success or false if wrong context is supplied
*/
TLNAPI bool TLN_SetContext(TLN_Engine context);

/*! \brief Returns the current engine context */
TLNAPI TLN_Engine TLN_GetContext(void);

/*!
	\brief Set Target fps (default 60)
	\param fps Target fps
	\remarks The engine internally runs at 60 fps. Use this function to keep constant animation pacing at other frequencies
	\see TLN_GetTargetFps
*/
TLNAPI void TLN_SetTargetFps(int fps);

/*!
	\brief Returns target fps
	\remarks By default the engine runs at 60 fps. This value is automatically changed to actual monitor Hz with TLN_CreateWindow, or manually with TLN_SetTargetFps
	\see TLN_GetTargetFps, TLN_CreateWindow
*/
TLNAPI int TLN_GetTargetFps(void);

/*!
	\brief Returns the width in pixels of the framebuffer
	\see TLN_Init(), TLN_GetHeight()
*/
TLNAPI int TLN_GetWidth (void);

/*!
	 \brief Returns the height in pixels of the framebuffer
	 \see TLN_Init(), TLN_GetWidth()
*/
TLNAPI int TLN_GetHeight (void);

/*!
	\brief Returns the number of objets used by the engine so far
	\remarks The objects is the total amount of tilesets, tilemaps, spritesets, palettes or sequences combined
	\see TLN_GetUsedMemory()
*/
TLNAPI uint32_t TLN_GetNumObjects (void);

/*!
	\brief Returns the total amount of memory used by the objects
	\see TLN_GetNumObjects()
*/
TLNAPI uint32_t TLN_GetUsedMemory (void);

/*!
	\brief Retrieves Tilengine dll version
	\returns
	Returns a 32-bit integer containing three packed numbers:
	bits 23:16 -> major version
	bits 15: 8 -> minor version
	bits  7: 0 -> bugfix revision
	\remarks Compare this number with the TILENGINE_HEADER_VERSION macro to check that both versions match!
*/
TLNAPI uint32_t TLN_GetVersion (void);

/*!
	\brief Returns the number of layers specified during initialisation
	\see TLN_Init()
*/
TLNAPI int TLN_GetNumLayers (void);

/*!
	\brief Returns the number of sprites specified during initialisation
	\see TLN_Init()
*/
TLNAPI int TLN_GetNumSprites (void);

/*!
	\brief Sets the background color
	\param r red component (0-255)
	\param g green component (0-255)
	\param b blue component (0-255)

	The background color is the color of the pixel when there isn't any layer or sprite at
	that position.

	\remarks This funcion can be called during a raster callback to create gradient backgrounds
*/
TLNAPI void TLN_SetBGColor (uint8_t r, uint8_t g, uint8_t b);

/*!
	\brief Sets the background color from a Tilemap defined color
	\param tilemap Reference to the tilemap with the background color to set
*/
TLNAPI bool TLN_SetBGColorFromTilemap (TLN_Tilemap tilemap);

/*!
	\brief Disales background color rendering. If you know that the last background layer will always cover the entire screen, you can disable it to gain some performance
	\see TLN_SetBGColor()
*/
TLNAPI void TLN_DisableBGColor (void);

/*!
	\brief Sets a static bitmap as background
	\param bitmap Reference to bitmap for the background. Set NULL to disable

	Sets an optional bitmap instead of a solid color where there is no layer or sprite.
	Unlike tilemaps or sprites, this bitmap cannot be moved and has no transparency

	\see TLN_SetBGPalette()
*/
TLNAPI bool TLN_SetBGBitmap (TLN_Bitmap bitmap);

/*!
	\brief Changes the palette for the background bitmap
	\param palette Reference to palette
	\see TLN_SetBGBitmap()
*/
TLNAPI bool TLN_SetBGPalette (TLN_Palette palette);

/* \brief Sets one of the eight global palettes used by tiled layers
	\param index Palette index [0 - 7]
	\param palette Reference of palette to set, or NULL to disable it
	\returns true if success, or false if error
	\see TLN_GetGlobalPalette()
*/
TLNAPI bool TLN_SetGlobalPalette(int index, TLN_Palette palette);

/*
	\brief Returns one of the eight global palettes
	\param index Index of global palette to query [0 - 7]
	\returns TLN_Palette reference or NULL if not set
	\see TLN_SetGlobalPalette
*/
TLNAPI TLN_Palette TLN_GetGlobalPalette(int index);

/*!
	\brief Specifies the address of the funcion to call for each drawn scanline
	\param callback	Address of the function to call

	Tilengine renders its output line by line, just as the 2D graphics chips did. The
	raster callback is a way to simulate the "horizontal blanking interrupt" of those systems,
	where many parameters of the rendering can be modified per line.

	\remarks Setting a raster callback is optional, but much of the fun of using Tilengine comes from the use of raster effects
*/
TLNAPI void TLN_SetRasterCallback (TLN_VideoCallback);

/*!
	\brief Specifies the address of the funcion to call for each drawn frame
	\param callback Address of the function to call
*/
TLNAPI void TLN_SetFrameCallback (TLN_VideoCallback);

/*!
	\brief Sets the output surface for rendering
	\param data	Pointer to the start of the target framebuffer
	\param pitch Number of bytes per each scanline of the framebuffer

	Sets the output surface for rendering. Tilengine doesn't provide a windowing or hardware
	video access. The application is responsible of allocating and maintaining the surface where
	tilengine does the rendering. It can be a SDL surface, a locked DirectX surface, an OpenGL texture,
	or whatever the application has access to.

	\remarks The render target pixel format must be 32 bits RGBA
	\see TLN_UpdateFrame()
*/
TLNAPI void TLN_SetRenderTarget (uint8_t* data, int pitch);

/*!
	\brief Draws the frame to the previously specified render target
	\param frame Optional frame number. Set to 0 to autoincrement from previous value
	\see TLN_SetRenderTarget()
*/
TLNAPI void TLN_UpdateFrame (int frame);

/*!
	\brief Sets base path for TLN_LoadXXX functions.	
	\param path	Base path. Files will load at path/filename. Can be NULL
*/
TLNAPI void TLN_SetLoadPath (const char* path);

/*!
	\brief Sets custom blend function to use when BLEND_CUSTOM mode is selected
	\param blend_function pointer to a user-provided function that takes two parameters: source component intensity, destination component intensity, and returns the desired intensity. This function is called for each RGB component when blending is enabled
	\remarks This function is not called in realtime, but its result is precomputed into a look-up table when TLN_SetCustomBlendFunction() is called, so the performance impact is minimal, just as low as the other built-in blending modes
	\see TLN_SetSpriteBlendMode()|TLN_SetLayerBlendMode()
*/
TLNAPI void TLN_SetCustomBlendFunction (TLN_BlendFunction);

/*!
	\brief Sets logging level for current instance
	\param log_level value to set, member of the TLN_LogLevel enumeration
*/
TLNAPI void TLN_SetLogLevel(TLN_LogLevel log_level);

/*!
	\brief Open the resource package with optional aes-128 key and binds it
	\param filename file with the resource package (.dat extension)
	\param key optional null-terminated ASCII string with aes decryption key
	\return true if package opened and made current, or false if error
	\remarks
	When the package is opened, it's globally bind to all TLN_LoadXXX functions.
	The assets inside the package are indexed with their original path/file as when
	they were plain files. As long as the structure used to build the package
	matches the original structure of the assets, the TLN_SetLoadPath() and the TLN_LoadXXX
	functions will work transparently, easing the migration with minimal changes.
	\sa TLN_CloseResourcePack
*/
TLNAPI bool TLN_OpenResourcePack(const char* filename, const char* key);

/*!
	\brief Closes current resource package and unbinds it
	\sa TLN_OpenResourcePack
*/
TLNAPI void TLN_CloseResourcePack(void);

/**@}*/

/**
 * \defgroup errors
 * \brief Basic setup and management
* @{ */

/*!
	\brief Sets the global error code of tilengine. Useful for custom loaders that need to set the error state.
	\param error Error code to set
	\see TLN_GetLastError()
*/
TLNAPI void TLN_SetLastError (TLN_Error error);

/*!
	\brief Returns the last error after an invalid operation
	\see TLN_Error
*/
TLNAPI TLN_Error TLN_GetLastError (void);

/*!
	\brief Returns the string description of the specified error code
	\param error Error code to get description
	\see TLN_GetLastError()
*/
TLNAPI const char *TLN_GetErrorString (TLN_Error error);
/**@}*/

/**
 * \defgroup windowing
 * \brief Built-in window and input management
* @{ */

/*!
	\brief Creates a window for rendering
	\param overlay Deprecated parameter in 2.10, kept for compatibility. Set to NULL
	\param flags Mask of the possible creation flags: CWF_FULLSCREEN, CWF_VSYNC, CWF_S1 - CWF_S5 (scaling factor, none = auto max)
	\returns True if window was created or false if error

	Creates a host window with basic user input for tilengine. If fullscreen, it uses the desktop
	resolution and stretches the output resolution with aspect correction, letterboxing or pillarboxing
	as needed. If windowed, it creates a centered window that is the maximum possible integer multiply of
	the resolution configured at TLN_Init()

	\remarks
	Using this feature is optional, Tilengine is designed to output its rendering to a user-provided surface
	so it can be used as a backend renderer of an already existing framework. But it is provided for convenience,
	so it isn't needed to provide external components to run the examples or do engine tests.

	\see TLN_DeleteWindow(), TLN_ProcessWindow(), TLN_GetInput(), TLN_DrawFrame()
*/
TLNAPI bool TLN_CreateWindow (const char* overlay, int flags);

/*!
	\brief Creates a multithreaded window for rendering
	\param overlay Deprecated parameter in 2.10, kept for compatibility. Set to NULL
	\param flags Mask of the possible creation flags: CWF_FULLSCREEN, CWF_VSYNC, CWF_S1 - CWF_S5 (scaling factor, none = auto max)
	\returns True if window was created or false if error

	Creates a host window with basic user input for tilengine. If fullscreen, it uses the desktop
	resolution and stretches the output resolution with aspect correction, letterboxing or pillarboxing
	as needed. If windowed, it creates a centered window that is the maximum possible integer multiply of
	the resolution configured at TLN_Init()

	\remarks Unlike TLN_CreateWindow, This window runs in its own thread
	\see TLN_DeleteWindow(), TLN_IsWindowActive(), TLN_GetInput(), TLN_UpdateFrame()
*/
TLNAPI bool TLN_CreateWindowThread (const char* overlay, int flags);

/*!
	\brief Sets window title
	\param title Text with the title to set
*/
TLNAPI void TLN_SetWindowTitle (const char* title);

/*!
	\brief Does basic window housekeeping in signgle-threaded window
	\returns True if window is active or false if the user has requested to end the application (by pressing Esc key or clicking the close button)

	If a window has been created with TLN_CreateWindow, this function must be called periodically (call it inside
	the main loop so it gets called regularly). If the window was created with TLN_CreateWindowThread, do not use it

	\see TLN_CreateWindow()
*/
TLNAPI bool TLN_ProcessWindow (void);

/*!
	\brief Checks window state
	\returns True if window is active or false if the user has requested to end the application (by pressing Esc key or clicking the close button)
	\see TLN_CreateWindow(), TLN_CreateWindowThread()
*/
TLNAPI bool TLN_IsWindowActive (void);

/*!
	\brief Returns the state of a given input

	\param input Input to check state. It can be one of the following values:
	 * INPUT_UP
	 * INPUT_DOWN
	 * INPUT_LEFT
	 * INPUT_RIGHT
	 * INPUT_BUTTON1 - INPUT_BUTTON6,
	 * INPUT_START
	 * Optionally combine with INPUT_P1 to INPUT_P4 to request input for specific player

	\returns True if that input is pressed or false if not

	If a window has been created with TLN_CreateWindow, it provides basic user input.
	It simulates a classic arcade setup, with 4 directional buttons (INPUT_UP to INPUT_RIGHT),
	6 action buttons (INPUT_BUTTON1 to INPUT_BUTTON6) and a start button (INPUT_START).
	By default directional buttons are mapped to keyboard cursors and joystick 1 D-PAD,
	and the first four action buttons are the keys Z,X,C,V and joystick buttons 1 to 4.

	\see TLN_CreateWindow(), TLN_DefineInputKey(), TLN_DefineInputButton()
*/
TLNAPI bool TLN_GetInput (TLN_Input id);

/*!
	\brief Enables or disables input for specified player
	\param player Player number to enable (PLAYER1 - PLAYER4)
	\param enable Set true to enable, false to disable
*/
TLNAPI void TLN_EnableInput (TLN_Player player, bool enable);

/*!
	\brief Assigns a joystick index to the specified player
	\param player Player number to configure (PLAYER1 - PLAYER4)
	\param index Joystick index to assign, 0-based index. -1 = disable
 */
TLNAPI void TLN_AssignInputJoystick (TLN_Player player, int index);

/*!
	\brief Assigns a keyboard input to a player
	\param player Player number to configure (PLAYER1 - PLAYER4)
	\param input Input to associate to the given key
	\param keycode ASCII key value or scancode as defined in SDL.h
*/
TLNAPI void TLN_DefineInputKey (TLN_Player player, TLN_Input input, uint32_t keycode);

/*!
	\brief Assigns a button joystick input to a player
	\param player Player number to configure (PLAYER1 - PLAYER4)
	\param input Input to associate to the given button
	\param joybutton Button index
*/
TLNAPI void TLN_DefineInputButton (TLN_Player player, TLN_Input input, uint8_t joybutton);

/*!
	\brief Draws a frame to the window
	\param frame Optional frame number. Set to 0 to autoincrement from previous value
	\remarks
	If a window has been created with TLN_CreateWindow(), it renders the frame to it. This function is a wrapper to
	TLN_UpdateFrame which also automatically sets the render target for the window, so when calling this function it is
	not needed to call TLN_UpdateFrame() too.

	\see TLN_CreateWindow(), TLN_UpdateFrame()
 */
TLNAPI void TLN_DrawFrame (int frame);

/*!
	\brief Thread synchronization for multithreaded window. Waits until the current frame has ended rendering
	\see TLN_CreateWindowThread()
*/
TLNAPI void TLN_WaitRedraw (void);

/*!
	\brief Deletes the window previoulsy created with TLN_CreateWindow() or TLN_CreateWindowThread()
	\see TLN_CreateWindow()
*/
TLNAPI void TLN_DeleteWindow (void);

/*!
	\brief Configures CRT simulation post-processing effect to give true retro appeareance
	\param type One possible value of \ref TLN_CRT enumeration
	\param blur simulate RF (horizontal) blur
	\param scanlines simulate horizontal scanlines
*/
TLNAPI void TLN_ConfigCRTEffect(TLN_CRT type, bool blur, bool scanlines);

/*!
	\brief Disables the CRT post-processing effect
	\see TLN_ConfigCRTEffect
*/
TLNAPI void TLN_DisableCRTEffect (void);

/*!
	\brief Registers a user-defined callback to capture internal SDL2 events
	\param callback pointer to user funcion with signature void (SDL_Event*)
*/
TLNAPI void TLN_SetSDLCallback(TLN_SDLCallback);

/*!
	\brief Suspends execition for a fixed time
	\param time Number of milliseconds to wait
*/
TLNAPI void TLN_Delay (uint32_t msecs);

/*!	\brief Returns the number of milliseconds since application start */
TLNAPI uint32_t TLN_GetTicks (void);

/*! \brief Returns averaged fps being rendered on the built-in window, updated each 500 ms */
TLNAPI uint32_t TLN_GetAverageFps(void);

/*! \brief Returns horizontal dimension of window after scaling */
TLNAPI int TLN_GetWindowWidth(void);

/*! \brief Returns vertical dimension of window after scaling */
TLNAPI int TLN_GetWindowHeight(void);

/*!
	\brief Returns current window scaling factor.
	\remarks This value can be set during call to TLN_CreateWindow() (flags CWF_S1 to CWF_S5), calling TLN_SetWindowScaleFactor(), or pressing ALT-1 to ALT-5 at runtime
*/
TLNAPI int TLN_GetWindowScaleFactor(void);

/*! \brief Sets current window scaling factor */
TLNAPI void TLN_SetWindowScaleFactor(int);

/**@}*/

/**
 * \defgroup spriteset
 * \brief Spriteset resources management for sprites
* @{ */

/*!
	\brief Creates a new spriteset
	\param bitmap Bitmap containing the sprite graphics
	\param data	Array of TLN_SpriteData structures with sprite descriptions
	\param num_entries Number of entries in data[] array
	\returns Reference to the created spriteset, or NULL if error
	\see TLN_DeleteSpriteset()
*/
TLNAPI TLN_Spriteset TLN_CreateSpriteset (TLN_Bitmap bitmap, TLN_SpriteData* data, int num_entries);

/*!
	\brief Loads a spriteset from an image png and its associated atlas descriptor
	\param name Base name of the files containing the spriteset, with or without .png extension
	\returns Reference to the newly loaded spriteset or NULL if error

	\remarks
	The spriteset comes in a pair of files: an image file (bmp or png) and a standarized atlas descriptor (json, csv or txt)
	The supported json format is the array.
*/
TLNAPI TLN_Spriteset TLN_LoadSpriteset (const char* name);

/*!
	\brief Creates a duplicate of the specified spriteset and its associated palette
	\param src Spriteset to clone
	\returns A reference to the newly cloned spriteset, or NULL if error
	\see TLN_LoadSpriteset()
*/
TLNAPI TLN_Spriteset TLN_CloneSpriteset (TLN_Spriteset src);

/*!
	\brief Query the details about the specified sprite inside a spriteset
	\param spriteset Reference to the spriteset to get info about
	\param entry The entry index inside the spriteset [0, num_sprites - 1]
	\param info	Pointer to application-allocated TLN_SpriteInfo structure that will receive the data
	\returns true if success or false if error
 */
TLNAPI bool TLN_GetSpriteInfo (TLN_Spriteset spriteset, int entry, TLN_SpriteInfo* info);

/*!
	\brief Returns a reference to the palette associated to the specified spriteset
	\param spriteset Spriteset to obtain the palette
	\remarks
	The palette of a spriteset is created at load time and cannot be modified. When TLN_ConfigSprite
	function is used to setup a sprite, the palette associated with the specified spriteset is automatically
	assigned to that sprite, but it can be later replaced with TLN_SetSpritePalette

	\see TLN_SetSpritePalette()
*/
TLNAPI TLN_Palette TLN_GetSpritesetPalette (TLN_Spriteset spriteset);

/*!
	\brief Returns a reference to the palette associated to the specified spriteset
	\param spriteset Spriteset where to find the sprite
	\param name Name of the sprite to findo
	\returns sprite index (0 -> num_sprites - 1) if found, or -1 if not found
*/
TLNAPI int TLN_FindSpritesetSprite (TLN_Spriteset spriteset, const char* name);

/*!
	\brief Sets attributes and pixels of a given sprite inside a spriteset
	\param spriteset Spriteset to set the data
	\param entry The entry index inside the spriteset to modify [0, num_sprites - 1]
	\param data Pointer to a user-provided TLN_SpriteData structure with sprite description
	\param pixels Pointer to source pixel data
	\param pitch Number of bytes per scanline of the source pixel data
	\see TLN_CreateSpriteset()
*/
TLNAPI bool TLN_SetSpritesetData (TLN_Spriteset spriteset, int entry, TLN_SpriteData* data, void* pixels, int pitch);

/*!
	\brief Deletes the specified spriteset and frees memory
	\param spriteset Spriteset to delete
	\remarks Don't delete a spriteset currently attached to a sprite!
	\see TLN_LoadSpriteset(), TLN_CloneSpriteset()
*/
TLNAPI bool TLN_DeleteSpriteset (TLN_Spriteset Spriteset);
/**@}*/

/**
 * \defgroup tileset
 * \brief Tileset resources management for background layers 
* @{ */
TLNAPI TLN_Tileset TLN_CreateTileset (int numtiles, int width, int height, TLN_Palette palette, TLN_SequencePack sp, TLN_TileAttributes* attributes);
TLNAPI TLN_Tileset TLN_CreateImageTileset(int numtiles, TLN_TileImage* images);
TLNAPI TLN_Tileset TLN_LoadTileset (const char* filename);
TLNAPI TLN_Tileset TLN_CloneTileset (TLN_Tileset src);
TLNAPI bool TLN_SetTilesetPixels (TLN_Tileset tileset, int entry, uint8_t* srcdata, int srcpitch);
TLNAPI int TLN_GetTileWidth (TLN_Tileset tileset);
TLNAPI int TLN_GetTileHeight (TLN_Tileset tileset);
TLNAPI int TLN_GetTilesetNumTiles(TLN_Tileset tileset);
TLNAPI TLN_Palette TLN_GetTilesetPalette (TLN_Tileset tileset);
TLNAPI TLN_SequencePack TLN_GetTilesetSequencePack (TLN_Tileset tileset);
TLNAPI int TLN_GetTilesetNumAnimations(TLN_Tileset tileset);
TLNAPI bool TLN_PauseTilesetAnimation(TLN_Tileset tileset, int index);
TLNAPI bool TLN_ResumeTilesetAnimation(TLN_Tileset tileset, int index);
TLNAPI bool TLN_DeleteTileset (TLN_Tileset tileset);
/**@}*/

/**
 * \defgroup tilemap
 * \brief Tilemap resources management for background layers 
* @{ */
TLNAPI TLN_Tilemap TLN_CreateTilemap (int rows, int cols, TLN_Tile tiles, uint32_t bgcolor, TLN_Tileset tileset);
TLNAPI TLN_Tilemap TLN_LoadTilemap (const char* filename, const char* layername);
TLNAPI TLN_Tilemap TLN_CloneTilemap (TLN_Tilemap src);
TLNAPI int TLN_GetTilemapRows (TLN_Tilemap tilemap);
TLNAPI int TLN_GetTilemapCols (TLN_Tilemap tilemap);
TLNAPI bool TLN_SetTilemapTileset(TLN_Tilemap tilemap, TLN_Tileset tileset);
TLNAPI TLN_Tileset TLN_GetTilemapTileset (TLN_Tilemap tilemap);
TLNAPI bool TLN_SetTilemapTileset2(TLN_Tilemap tilemap, TLN_Tileset tileset, int index);
TLNAPI TLN_Tileset TLN_GetTilemapTileset2(TLN_Tilemap tilemap, int index);
TLNAPI bool TLN_GetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);
TLNAPI bool TLN_SetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);
TLNAPI bool TLN_CopyTiles (TLN_Tilemap src, int srcrow, int srccol, int rows, int cols, TLN_Tilemap dst, int dstrow, int dstcol);
TLNAPI TLN_Tile TLN_GetTilemapTiles(TLN_Tilemap tilemap, int row, int col);
TLNAPI bool TLN_DeleteTilemap (TLN_Tilemap tilemap);
/**@}*/

/**
 * \defgroup palette
 * \brief Color palette resources management for sprites and background layers
* @{ */
TLNAPI TLN_Palette TLN_CreatePalette (int entries);
TLNAPI TLN_Palette TLN_LoadPalette (const char* filename);
TLNAPI TLN_Palette TLN_ClonePalette (TLN_Palette src);
TLNAPI bool TLN_SetPaletteColor (TLN_Palette palette, int color, uint8_t r, uint8_t g, uint8_t b);
TLNAPI bool TLN_MixPalettes (TLN_Palette src1, TLN_Palette src2, TLN_Palette dst, uint8_t factor);
TLNAPI bool TLN_AddPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);
TLNAPI bool TLN_SubPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);
TLNAPI bool TLN_ModPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);
TLNAPI uint8_t* TLN_GetPaletteData (TLN_Palette palette, int index);
TLNAPI int TLN_GetPaletteNumColors(TLN_Palette palette);
TLNAPI bool TLN_DeletePalette (TLN_Palette palette);
/**@}*/

/**
 * \defgroup bitmap
 * \brief Bitmap management
* @{ */
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
 * \defgroup objects
 * \brief ObjectList resources management
 * @{ */
TLNAPI TLN_ObjectList TLN_CreateObjectList(void);
TLNAPI bool TLN_AddTileObjectToList(TLN_ObjectList list, uint16_t id, uint16_t gid, uint16_t flags, int x, int y);
TLNAPI TLN_ObjectList TLN_LoadObjectList(const char* filename, const char* layername);
TLNAPI TLN_ObjectList TLN_CloneObjectList(TLN_ObjectList src);
TLNAPI int TLN_GetListNumObjects(TLN_ObjectList list);
TLNAPI bool TLN_GetListObject(TLN_ObjectList list, TLN_ObjectInfo* info);
TLNAPI bool TLN_DeleteObjectList(TLN_ObjectList list);
/**@}*/

/**
 * \defgroup layer
 * \brief Background layers management
* @{ */
TLNAPI bool TLN_SetLayer (int nlayer, TLN_Tileset tileset, TLN_Tilemap tilemap);
TLNAPI bool TLN_SetLayerTilemap(int nlayer, TLN_Tilemap tilemap);
TLNAPI bool TLN_SetLayerBitmap(int nlayer, TLN_Bitmap bitmap);
TLNAPI bool TLN_SetLayerPalette (int nlayer, TLN_Palette palette);
TLNAPI bool TLN_SetLayerPosition (int nlayer, int hstart, int vstart);
TLNAPI bool TLN_SetLayerScaling (int nlayer, float xfactor, float yfactor);
TLNAPI bool TLN_SetLayerAffineTransform (int nlayer, TLN_Affine *affine);
TLNAPI bool TLN_SetLayerTransform (int layer, float angle, float dx, float dy, float sx, float sy);
TLNAPI bool TLN_SetLayerPixelMapping (int nlayer, TLN_PixelMap* table);
TLNAPI bool TLN_SetLayerBlendMode (int nlayer, TLN_Blend mode, uint8_t factor);
TLNAPI bool TLN_SetLayerColumnOffset (int nlayer, int* offset);
TLNAPI bool TLN_SetLayerClip (int nlayer, int x1, int y1, int x2, int y2);
TLNAPI bool TLN_DisableLayerClip (int nlayer);
TLNAPI bool TLN_SetLayerWindow(int nlayer, int x1, int y1, int x2, int y2, bool invert);
TLNAPI bool TLN_SetLayerWindowColor(int nlayer, uint8_t r, uint8_t g, uint8_t b, TLN_Blend blend);
TLNAPI bool TLN_DisableLayerWindow(int nlayer);
TLNAPI bool TLN_DisableLayerWindowColor(int nlayer);
TLNAPI bool TLN_SetLayerMosaic (int nlayer, int width, int height);
TLNAPI bool TLN_DisableLayerMosaic (int nlayer);
TLNAPI bool TLN_ResetLayerMode (int nlayer);
TLNAPI bool TLN_SetLayerObjects(int nlayer, TLN_ObjectList objects, TLN_Tileset tileset);
TLNAPI bool TLN_SetLayerPriority(int nlayer, bool enable);
TLNAPI bool TLN_SetLayerParent(int nlayer, int parent);
TLNAPI bool TLN_DisableLayerParent(int nlayer);
TLNAPI bool TLN_DisableLayer (int nlayer);
TLNAPI bool TLN_EnableLayer(int nlayer);
TLNAPI TLN_LayerType TLN_GetLayerType(int nlayer);
TLNAPI TLN_Palette TLN_GetLayerPalette (int nlayer);
TLNAPI TLN_Tileset TLN_GetLayerTileset(int nlayer);
TLNAPI TLN_Tilemap TLN_GetLayerTilemap(int nlayer);
TLNAPI TLN_Bitmap TLN_GetLayerBitmap(int nlayer);
TLNAPI TLN_ObjectList TLN_GetLayerObjects(int nlayer);
TLNAPI bool TLN_GetLayerTile (int nlayer, int x, int y, TLN_TileInfo* info);
TLNAPI int TLN_GetLayerWidth (int nlayer);
TLNAPI int TLN_GetLayerHeight (int nlayer);
TLNAPI int TLN_GetLayerX(int nlayer);
TLNAPI int TLN_GetLayerY(int nlayer);

/**@}*/

/**
 * \defgroup sprite
 * \brief Sprites management
* @{ */
TLNAPI bool TLN_ConfigSprite (int nsprite, TLN_Spriteset spriteset, uint32_t flags);
TLNAPI bool TLN_SetSpriteSet (int nsprite, TLN_Spriteset spriteset);
TLNAPI bool TLN_SetSpriteFlags (int nsprite, uint32_t flags);
TLNAPI bool TLN_EnableSpriteFlag(int nsprite, uint32_t flag, bool enable);
TLNAPI bool TLN_SetSpritePivot(int nsprite, float px, float py);
TLNAPI bool TLN_SetSpritePosition (int nsprite, int x, int y);
TLNAPI bool TLN_SetSpritePicture (int nsprite, int entry);
TLNAPI bool TLN_SetSpritePalette (int nsprite, TLN_Palette palette);
TLNAPI bool TLN_SetSpriteBlendMode (int nsprite, TLN_Blend mode, uint8_t factor);
TLNAPI bool TLN_SetSpriteScaling (int nsprite, float sx, float sy);
TLNAPI bool TLN_ResetSpriteScaling (int nsprite);
//TLNAPI bool TLN_SetSpriteRotation (int nsprite, float angle);
//TLNAPI bool TLN_ResetSpriteRotation (int nsprite);
TLNAPI int  TLN_GetSpritePicture (int nsprite);
TLNAPI int TLN_GetSpriteX(int nsprite);
TLNAPI int TLN_GetSpriteY(int nsprite);
TLNAPI int  TLN_GetAvailableSprite (void);
TLNAPI bool TLN_EnableSpriteCollision (int nsprite, bool enable);
TLNAPI bool TLN_GetSpriteCollision (int nsprite);
TLNAPI bool TLN_GetSpriteState(int nsprite, TLN_SpriteState* state);
TLNAPI bool TLN_SetFirstSprite(int nsprite);
TLNAPI bool TLN_SetNextSprite(int nsprite, int next);
TLNAPI bool TLN_EnableSpriteMasking(int nsprite, bool enable);
TLNAPI void TLN_SetSpritesMaskRegion(int top_line, int bottom_line);
TLNAPI bool TLN_SetSpriteAnimation (int nsprite, TLN_Sequence sequence, int loop);
TLNAPI bool TLN_DisableSpriteAnimation(int nsprite);
TLNAPI bool TLN_PauseSpriteAnimation(int index);
TLNAPI bool TLN_ResumeSpriteAnimation(int index);
TLNAPI bool TLN_DisableAnimation(int index);
TLNAPI bool TLN_DisableSprite (int nsprite);
TLNAPI TLN_Palette TLN_GetSpritePalette (int nsprite);
/**@}*/

/**
 * \defgroup sequence
 * \brief Sequence resources management for layer, sprite and palette animations
* @{ */
TLNAPI TLN_Sequence TLN_CreateSequence (const char* name, int target, int num_frames, TLN_SequenceFrame* frames);
TLNAPI TLN_Sequence TLN_CreateCycle (const char* name, int num_strips, TLN_ColorStrip* strips);
TLNAPI TLN_Sequence TLN_CreateSpriteSequence(const char* name, TLN_Spriteset spriteset, const char* basename, int delay);
TLNAPI TLN_Sequence TLN_CloneSequence (TLN_Sequence src);
TLNAPI bool TLN_GetSequenceInfo (TLN_Sequence sequence, TLN_SequenceInfo* info);
TLNAPI bool TLN_DeleteSequence (TLN_Sequence sequence);
/**@}*/

/**
 * \defgroup sequencepack
 * \brief Sequence pack manager for grouping and finding sequences
* @{ */
TLNAPI TLN_SequencePack TLN_CreateSequencePack (void);
TLNAPI TLN_SequencePack TLN_LoadSequencePack (const char* filename);
TLNAPI TLN_Sequence TLN_GetSequence (TLN_SequencePack sp, int index);
TLNAPI TLN_Sequence TLN_FindSequence (TLN_SequencePack sp, const char* name);
TLNAPI int TLN_GetSequencePackCount (TLN_SequencePack sp);
TLNAPI bool TLN_AddSequenceToPack (TLN_SequencePack sp, TLN_Sequence sequence);
TLNAPI bool TLN_DeleteSequencePack (TLN_SequencePack sp);
/**@}*/

/**
 * \defgroup animation
 * \brief Color cycle animation
* @{ */
TLNAPI bool TLN_SetPaletteAnimation (int index, TLN_Palette palette, TLN_Sequence sequence, bool blend);
TLNAPI bool TLN_SetPaletteAnimationSource (int index, TLN_Palette);
TLNAPI bool TLN_GetAnimationState (int index);
TLNAPI bool TLN_SetAnimationDelay (int index, int frame, int delay);
TLNAPI int  TLN_GetAvailableAnimation (void);
TLNAPI bool TLN_DisablePaletteAnimation(int index);
/**@}*/

/**
 * \defgroup world
 * \brief World management
* @{ */
TLNAPI bool TLN_LoadWorld(const char* tmxfile, int first_layer);
TLNAPI void TLN_SetWorldPosition(int x, int y);
TLNAPI bool TLN_SetLayerParallaxFactor(int nlayer, float x, float y);
TLNAPI bool TLN_SetSpriteWorldPosition(int nsprite, int x, int y);
TLNAPI void TLN_ReleaseWorld(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif
