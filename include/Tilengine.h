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
 * \defgroup types Common data types
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
#define TILENGINE_VER_MIN	16
#define TILENGINE_VER_REV	0
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
typedef void(*TLN_TaskCallback)(uint32_t frame);

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
	CWF_NOVSYNC		= (1 << 7), /*<! disable default vsync */
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
 * \defgroup setup Basic setup and management
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
TLNAPI void TLN_SetRasterCallback (TLN_VideoCallback callback);

/*!
	\brief Specifies the address of the funcion to call for each drawn frame
	\param callback Address of the function to call
*/
TLNAPI void TLN_SetFrameCallback (TLN_VideoCallback callback);

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
TLNAPI void TLN_SetCustomBlendFunction (TLN_BlendFunction blend_function);
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

/*! Player index for input checking */
typedef enum
{
	PLAYER1,
	PLAYER2,
	PLAYER3,
	PLAYER4,
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

TLNAPI void SetFrameCallbackName(const char* name);
TLNAPI void SetRasterCallbackName(const char* name);
TLNAPI bool CheckRetroInput(TLN_Player port, TLN_Input input);

/**@}*/

/**
 * \defgroup errors Basic setup and management
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
 * \defgroup windowing Built-in window and input management
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
TLNAPI bool TLN_GetInput (TLN_Input input);

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
TLNAPI void TLN_SetSDLCallback(TLN_SDLCallback callback);

/*!
	\brief Suspends execition for a fixed time
	\param time Number of milliseconds to wait
*/
TLNAPI void TLN_Delay (uint32_t time);

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

/*!
	\brief Set task to execute for each frame. Required when targeting HTML5
	\param pointer to user-provided function to execute on each frame
	This function blocks until window is closed
*/
TLNAPI void TLN_SetMainTask(TLN_TaskCallback);

/**@}*/

/**
 * \defgroup spriteset Spriteset resources management for sprites

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
TLNAPI bool TLN_DeleteSpriteset (TLN_Spriteset spriteset);
/**@}*/

/**
 * \defgroup tileset Tileset resources management for background layers 
* @{ */

/*!
	\brief Creates a tile-based tileset
	\param numtiles	Number of tiles that the tileset will hold
	\param width Width of each tile (must be multiple of 8)
	\param height Height of each tile (must be multiple of 8)
	\param palette Reference to the palette to assign
	\param sp Optional reference to the optional sequence pack with associated tileset animations, can be NULL
	\param attributes Optional array of attributes, one for each tile. Can be NULL
	\returns Reference to the created tileset, or NULL if error
	\see TLN_SetTilesetPixels()
*/
TLNAPI TLN_Tileset TLN_CreateTileset (int numtiles, int width, int height, TLN_Palette palette, TLN_SequencePack sp, TLN_TileAttributes* attributes);

/*!
	\brief Creates a multiple image-based tileset
	\param numtiles	Number of tiles that the tileset will hold
	\param images Array of image structures, one for each tile. Can be NULL
	\returns Reference to the created tileset, or NULL if error
*/
TLNAPI TLN_Tileset TLN_CreateImageTileset(int numtiles, TLN_TileImage* images);

/*!
	\brief Loads a tileset from a Tiled .tsx file
	\param filename TSX file to load
	\returns Reference to the newly loaded tileset or NULL if error
	\remarks An associated palette is also created, it can be obtained calling TLN_GetTilesetPalette()
*/
TLNAPI TLN_Tileset TLN_LoadTileset (const char* filename);

/*!
	\brief Creates a duplicate of the specified tileset and its associated palette
	\param src Tileset to clone
	\returns A reference to the newly cloned tileset, or NULL if error
	\see TLN_LoadTileset()
 */
TLNAPI TLN_Tileset TLN_CloneTileset (TLN_Tileset src);

/*!
	\brief Sets pixel data for a tile in a tile-based tileset
	\param tileset Reference to the tileset
	\param entry Number of tile to set [0, num_tiles - 1]
	\param srcdata Pointer to pixel data to set
	\param srcpitch Bytes per line of source data
	\returns true if success, or false if error
	\remarks Care must be taken in providing pixel data and pitch as it can crash the aplication
	\see TLN_CreateTileset()
*/
TLNAPI bool TLN_SetTilesetPixels (TLN_Tileset tileset, int entry, uint8_t* srcdata, int srcpitch);

/*!
	\brief Returns the width in pixels of each individual tile in the tileset
	\param tileset Reference to the tileset to get info from
	\see TLN_GetTileHeight()
*/
TLNAPI int TLN_GetTileWidth (TLN_Tileset tileset);

/*!
	\brief Returns the height in pixels of each individual tile in the tileset
	\param tileset Reference to the tileset to get info from
	\see TLN_GetTileWidth()
*/
TLNAPI int TLN_GetTileHeight (TLN_Tileset tileset);

/*!
	\brief Returns the number of different tiles in tileset
	\param tileset Reference to the tileset to get info from
*/
TLNAPI int TLN_GetTilesetNumTiles(TLN_Tileset tileset);

/*!
	\brief Returns a reference to the palette associated to the specified tileset
	\param tileset Reference to the tileset to get the palette
	\remarks The palette of a tileset is created at load time and cannot be modified. When TLN_SetLayer
	function is used to attach a tileset to a layer, the palette associated with the specified tileset is automatically
	assigned to that layer, but it can be later replaced with TLN_SetLayerPalette

	\see TLN_LoadTileset(), TLN_SetLayerPalette()
 */
TLNAPI TLN_Palette TLN_GetTilesetPalette (TLN_Tileset tileset);

/*!
	\brief Returns a reference to the optional sequence pack associated to the specified tileset
	\param tileset Reference to the tileset to get the palette
	\see TLN_LoadTileset(), TLN_CreateTileset()
*/
TLNAPI TLN_SequencePack TLN_GetTilesetSequencePack (TLN_Tileset tileset);

/*!
	\brief Returns number of animations in given tileset
	\param tileset Reference to the tileset to get the number of animations
	\returns Number of animations in the tileset, or 0 if none
*/
TLNAPI int TLN_GetTilesetNumAnimations(TLN_Tileset tileset);

/*!
	\brief Pauses animation for the given tileset
	\param tileset Reference of the tileset to pause animation
	\param index Id of the animation to pause (0 <= id < num_animations)
	\see Animations TLN_ResumeTilesetAnimation, TLN_GetTilesetNumAnimations
*/
TLNAPI bool TLN_PauseTilesetAnimation(TLN_Tileset tileset, int index);

/*!
	\brief Restores animation for the given tileset
	\param tileset Reference of the tileset to resume animation
	\param index Id of the animation to resume (0 <= id < num_animations)
	\see Animations TLN_PauseTilesetAnimation, TLN_GetTilesetNumAnimations
*/
TLNAPI bool TLN_ResumeTilesetAnimation(TLN_Tileset tileset, int index);

/*!
	\brief Deletes the specified tileset and frees memory
	\param tileset Tileset to delete
	\remarks Don't delete a tileset currently attached to a layer!
	\see TLN_LoadTileset(), TLN_CloneTileset()
*/
TLNAPI bool TLN_DeleteTileset (TLN_Tileset tileset);
/**@}*/

/**
 * \defgroup tilemap Tilemap resources management for background layers 
* @{ */

/*!
	\brief Creates a new tilemap
	\param rows	Number of rows (vertical dimension)
	\param cols	Number of cols (horizontal dimension)
	\param tiles Array of tiles with data (see struct Tile)
	\param bgcolor Background color value (RGB32 packed)
	\param tileset Optional reference to associated tileset, can be NULL
	\returns Reference to the created tilemap, or NULL if error
	\remarks Make sure that the tiles[] array is has at least rows*cols items or application may crash
	\see TLN_DeleteTilemap(), struct Tile
*/
TLNAPI TLN_Tilemap TLN_CreateTilemap (int rows, int cols, TLN_Tile tiles, uint32_t bgcolor, TLN_Tileset tileset);

/*!
	\brief Loads a tilemap layer from a Tiled .tmx file
	\param filename	TMX file with the tilemap
	\param layername Optional name of the layer inside the tmx file to load. NULL to load the first layer
	\returns Reference to the newly loaded tilemap or NULL if error
	\remarks
	A tmx map file from Tiled can contain one or more layers, each with its own name. TLN_LoadTilemap()
	doesn't load a full tmx file, only the specified layer. The associated *external* tileset (TSX file) is
	also loaded and associated to the tilemap
*/
TLNAPI TLN_Tilemap TLN_LoadTilemap (const char* filename, const char* layername);

/*!
	\brief Creates a duplicate of the specified tilemap
	\param src Reference to the tilemap to clone
	\returns A reference to the newly cloned tilemap, or NULL if error
	\see TLN_LoadTilemap()
*/
TLNAPI TLN_Tilemap TLN_CloneTilemap (TLN_Tilemap src);

/*!
	\brief Returns the number of vertical tiles in the tilemap
	\param tilemap Reference of the tilemap to get info
	\see TLN_GetTilemapCols()
*/
TLNAPI int TLN_GetTilemapRows (TLN_Tilemap tilemap);

/*!
	\brief Returns the number of horizontal tiles in the tilemap
	\param tilemap Reference of the tilemap to get info
	\see TLN_GetTilemapRows()
*/
TLNAPI int TLN_GetTilemapCols (TLN_Tilemap tilemap);

/*!
	\brief Sets default tileset to specified tilemap
	\param tilemap Reference to the tilemap to modify
	\param tileset Reference to the tileset being assigned
	\see TLN_GetTilemapTileset()
*/
TLNAPI bool TLN_SetTilemapTileset(TLN_Tilemap tilemap, TLN_Tileset tileset);

/*!
	\brief Returns the optional associated tileset to the specified tilemap
	\param tilemap Reference of the tilemap to get info
	\see TLN_CreateTilemap(), TLN_LoadTilemap()
*/
TLNAPI TLN_Tileset TLN_GetTilemapTileset (TLN_Tilemap tilemap);

/*!
	\brief Sets default tileset to specified tilemap
	\param tilemap Reference to the tilemap to modify
	\param tileset Reference to the tileset being assigned
	\param index Index of tileset to set (0 - 7)
	\see TLN_GetTilemapTileset()
*/
TLNAPI bool TLN_SetTilemapTileset2(TLN_Tilemap tilemap, TLN_Tileset tileset, int index);

/*!
	\brief Returns the nth tileset associated tileset to the specified tilemap
	\param tilemap Reference of the tilemap to get info
	\param index Tileset index (0 - 7)
	\see TLN_CreateTilemap(), TLN_LoadTilemap()
*/
TLNAPI TLN_Tileset TLN_GetTilemapTileset2(TLN_Tilemap tilemap, int index);

/*!
	\brief Gets data of a single tile inside a tilemap
	\param tilemap Reference of the tilemap to get the tile
	\param row Vertical location of the tile (0 <= row < rows)
	\param col Horizontal location of the tile (0 <= col < cols)
	\param tile	Reference to an application-allocated struct Tile that will get the data
*/
TLNAPI bool TLN_GetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);

/*!
	\brief Sets a tile of a tilemap
	\param tilemap Reference to the tilemap
	\param row Row (vertical position) of the tile [0 - num_rows - 1]
	\param col Column (horizontal position) of the tile [0 - num_cols - 1]
	\param tile	Reference to the tile to set, or NULL to set an empty tile
	\returns true (success) or false (error)
*/
TLNAPI bool TLN_SetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile);

/*!
	\brief Copies blocks of tiles between two tilemaps
	\param src Reference to the source tilemap
	\param srcrow Starting row (vertical position) inside the source tilemap
	\param srccol Starting column (horizontal position) inside the source tilemap
	\param rows	Number of rows to copy
	\param cols	Number of columns to copy
	\param dst Reference to the target tilemap
	\param dstrow Starting row (vertical position) inside the target tilemap
	\param dstcol Starting column (horizontal position) inside the target tilemap
	\remarks Use this function to implement tile streaming
*/
TLNAPI bool TLN_CopyTiles (TLN_Tilemap src, int srcrow, int srccol, int rows, int cols, TLN_Tilemap dst, int dstrow, int dstcol);

/*!
	\brief Returns pointer to internal tilemap data data
	\param tilemap Tilemap being queried
	\param row Row index
	\param col Column index
	\returns pointer to corresponding TLN_Tile object or NULL if error
	\remarks Having direct access to internal memory is convenient for performance reasons when lots of tiles
	must be updated at runtime, but wrong manipulation can lead to memory corruption or crashes. Use with caution!
*/
TLNAPI TLN_Tile TLN_GetTilemapTiles(TLN_Tilemap tilemap, int row, int col);

/*!
	\brief Deletes the specified tilemap and frees memory
	\param tilemap Reference to the tilemap to delete
	\remarks Don't delete a tilemap currently attached to a layer!
	\see TLN_LoadTilemap(), TLN_CloneTilemap()
*/
TLNAPI bool TLN_DeleteTilemap (TLN_Tilemap tilemap);

/**@}*/

/**
 * \defgroup palette Color palette resources management for sprites and background layers
* @{ */

/*!
	\brief Creates a new color table
	\param entries Number of color entries (typically 256)
	\returns Reference to the created palette or NULL if error
*/
TLNAPI TLN_Palette TLN_CreatePalette (int entries);

/*!
	\brief Loads a palette from a standard .act file
	\param filename ACT file containing the palette to load
	\returns A reference to the newly loaded palette, or NULL if error
	\remarks
	Palettes are also automatically created when loading tilesets and spritesets.
	Use the functions TLN_GetTilesetPalette() and TLN_GetSpritesetPalette() to retrieve them.
	
	\see TLN_GetTilesetPalette(), TLN_GetSpritesetPalette()
*/
TLNAPI TLN_Palette TLN_LoadPalette (const char* filename);

/*!
	\brief Creates a duplicate of the specified palette
	\param src Reference to the palette to clone
	\returns A reference to the newly cloned palette, or NULL if error
	\see TLN_CreatePalette()
*/
TLNAPI TLN_Palette TLN_ClonePalette (TLN_Palette src);

/*!
	\brief Sets the RGB color value of a palette entry
	\param palette Reference to the palette to modify
	\param index Index of the palette entry to modify (0-255)
	\param r Red component of the color (0-255)
	\param g Green component of the color (0-255)
	\param b Blue component of the color (0-255)
*/
TLNAPI bool TLN_SetPaletteColor (TLN_Palette palette, int index, uint8_t r, uint8_t g, uint8_t b);

/*!
	\brief Mixes two palettes to create a third one
	\param src1 Reference to the first source palette
	\param src2	Reference to the second source palette
	\param dst Reference to the target palette
	\param factor Integer with mixing factor. 0=100% src1, 255=100% src2, 128=50%/50%
*/
TLNAPI bool TLN_MixPalettes (TLN_Palette src1, TLN_Palette src2, TLN_Palette dst, uint8_t factor);

/*!
	\brief Modifies a range of colors by adding the provided color value to the selected range. The result is always a brighter color.
	\param palette Reference to the palette to modify
	\param r Red component of the color (0-255)
	\param g Green component of the color (0-255)
	\param b Blue component of the color (0-255)
	\param start index of the first color entry to modify
	\param num number of colors from start to modify
*/
TLNAPI bool TLN_AddPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);

/*!
	\brief Modifies a range of colors by subtracting the provided color value to the selected range. The result is always a darker color.
	\param palette Reference to the palette to modify
	\param r Red component of the color (0-255)
	\param g Green component of the color (0-255)
	\param b Blue component of the color (0-255)
	\param start index of the first color entry to modify
	\param num number of colors from start to modify
*/
TLNAPI bool TLN_SubPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);

/*!
	\brief Modifies a range of colors by modulating (normalized product) the provided color value to the selected range. The result is always a darker color.
	\param palette Reference to the palette to modify
	\param r Red component of the color (0-255)
	\param g Green component of the color (0-255)
	\param b Blue component of the color (0-255)
	\param start index of the first color entry to modify
	\param num number of colors from start to modify
*/
TLNAPI bool TLN_ModPaletteColor (TLN_Palette palette, uint8_t r, uint8_t g, uint8_t b, uint8_t start, uint8_t num);

/*!
	\brief Returns the color value of a palette entry
	\param palette Reference to the palette to get the color
	\param index Index of the palette entry to obtain (0-255)
	\returns 32-bit integer with the packed color in internal pixel format RGBA
*/
TLNAPI uint8_t* TLN_GetPaletteData (TLN_Palette palette, int index);

/*!
	\brief Returns the number of color entries in the given palette
	\param palette Reference to the palette to query
	\returns number of color entries
*/
TLNAPI int TLN_GetPaletteNumColors(TLN_Palette palette);

/*!
	\brief Deletes the specified palette and frees memory
	\param palette Reference to the palette to delete
	\remarks Don't delete a palette currently attached to a layer or sprite!
*/
TLNAPI bool TLN_DeletePalette (TLN_Palette palette);
/**@}*/

/**
 * \defgroup bitmap Bitmap management
* @{ */

/*!
	\brief Creates a memory bitmap
	\param width Width in pixels
	\param height Height in pixels
	\param bpp Bits per pixel
	\returns Reference to the created bitmap, or NULL if error
	\see TLN_SetBGBitmap()
*/
TLNAPI TLN_Bitmap TLN_CreateBitmap (int width, int height, int bpp);

/*!
	\brief Load image file (8-bit BMP or PNG)
	\param filename File name with the image
	\returns Handler to the loaded image or NULL if error
	\see TLN_DeleteBitmap()
*/
TLNAPI TLN_Bitmap TLN_LoadBitmap (const char* filename);

/*!
	\brief Creates a copy of a bitmap
	\param src Reference to the original bitmap
	\returns Reference to the created bitmap, or NULL if error
	\see TLN_SetBGBitmap()
 */
TLNAPI TLN_Bitmap TLN_CloneBitmap (TLN_Bitmap src);

/*!
	\brief Gets memory access for direct pixel manipulation
	\param bitmap Reference to bitmap
	\param x Starting x position [0, width - 1]
	\param y Starting y position [0, height - 1]
	\returns Pointer to pixel data starting at x,y
	\remarks Care must be taken in manipulating memory directly as it can crash the application
*/
TLNAPI uint8_t* TLN_GetBitmapPtr (TLN_Bitmap bitmap, int x, int y);

/*!
	\brief Returns the width in pixels
	\param bitmap Reference to the bitmap
*/
TLNAPI int TLN_GetBitmapWidth (TLN_Bitmap bitmap);

/*!
	\brief Returns the height in pixels
	\param bitmap Reference to the bitmap
*/
TLNAPI int TLN_GetBitmapHeight (TLN_Bitmap bitmap);

/*!
	\brief Returns the number of bits per pixel
	\param bitmap Reference to the bitmap
*/
TLNAPI int TLN_GetBitmapDepth (TLN_Bitmap bitmap);

/*!
	\brief Returns the number of bytes of each scanline (row) of the bitmap, also known as stride
	\param bitmap Reference to the bitmap
*/
TLNAPI int TLN_GetBitmapPitch (TLN_Bitmap bitmap);

/*!
	\brief Gets the associated palete of a bitmap
	\param bitmap Reference to bitmap
	\returns Reference to the bitmap palette
	\see TLN_SetBitmapPalette()
*/
TLNAPI TLN_Palette TLN_GetBitmapPalette (TLN_Bitmap bitmap);

/*!
	\brief Assigns a new palette to the bitmap
	\param bitmap Reference to the bitmap
	\param palette Reference to the palette to assign
	\see TLN_GetBitmapPalette()
*/
TLNAPI bool TLN_SetBitmapPalette (TLN_Bitmap bitmap, TLN_Palette palette);

/*!
	\brief Deletes bitmap and frees resources
	\param bitmap Reference to bitmap to delete
	\see TLN_CreateBitmap89, TLN_CloneBitmap()
*/
TLNAPI bool TLN_DeleteBitmap (TLN_Bitmap bitmap);
/**@}*/

/**
 * \defgroup objects ObjectList resources management
 * @{ */

 /*!
	 \brief Creates a TLN_ObjectList
	 The list is created empty, it must be populated with TLN_AddSpriteToList()
	 and assigned to a layer with TLN_SetLayerObjects()
	 \return Reference to new object or NULL if error
 */
TLNAPI TLN_ObjectList TLN_CreateObjectList(void);

/*!
	\brief Adds an image-based tileset item to given TLN_ObjectList
	\param list Reference to TLN_ObjectList
	\param id Unique ID of the tileset object
	\param gid Graphic Id (tile index) of the tileset object
	\param flags Combination of FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY
	\param x Layer-space horizontal coordinate of the top-left corner
	\param y Layer-space bertical coordinate of the top-left corner
	\return true if success or false if error
*/
TLNAPI bool TLN_AddTileObjectToList(TLN_ObjectList list, uint16_t id, uint16_t gid, uint16_t flags, int x, int y);

/*!
	\brief Loads an object list from a Tiled object layer
	\param filename Name of the .tmx file containing the list
	\param layername Name of the layer to load
	\return Reference to the loaded object or NULL if error
*/
TLNAPI TLN_ObjectList TLN_LoadObjectList(const char* filename, const char* layername);

/*!
	\brief Creates a duplicate of a given TLN_ObjectList object
	\param src Reference to the source object to clone
	\return A reference to the newly cloned object list, or NULL if error
*/
TLNAPI TLN_ObjectList TLN_CloneObjectList(TLN_ObjectList src);

/*!
	\brief Returns number of items in TLN_ObjectList
	\param list Pointer to TLN_ObjectList to query
	\return number of items
*/
TLNAPI int TLN_GetListNumObjects(TLN_ObjectList list);

/*!
	\brief Iterates over elements in a TLN_ObjectList
	\param list Reference to TLN_ObjectList to get items
	\param info Pointer to user-allocated TLN_ObjectInfo struct
	\return true if item returned, false if no more items left
	\remarks The info pointer acts as a switch to select first/next element:
	- If not NULL, starts the iterator and returns the first item
	- If NULL, return the next item
*/
TLNAPI bool TLN_GetListObject(TLN_ObjectList list, TLN_ObjectInfo* info);

/*!
	\brief Deletes object list
	\param list Reference to list to delete
	\return true if success or false if error
*/
TLNAPI bool TLN_DeleteObjectList(TLN_ObjectList list);
/**@}*/

/**
 * \defgroup layer Background layers management
* @{ */

/*!
	\deprecated Use \ref TLN_SetLayerTilemap instead
	\brief Configures a background layer with the specified tileset and tilemap
	\param nlayer Layer index [0, num_layers - 1]
	\param tileset Optional reference to the tileset to assign. If the tilemap has a reference to its own tileset, passing NULL will assign the default tileset.
	\param tilemap Reference to the tilemap to assign
	\remarks This function doesn't modify the current position nor the blend mode, but assigns the palette of the specified tileset
	\see TLN_DisableLayer()
*/
TLNAPI bool TLN_SetLayer (int nlayer, TLN_Tileset tileset, TLN_Tilemap tilemap);

/*!
	\brief Configures a tiled background layer with the specified tilemap
	\param nlayer Layer index [0, num_layers - 1]
	\param tilemap Reference to the tilemap to assign
	\returns true if success or false if error
	\see TLN_LoadTilemap()
*/
TLNAPI bool TLN_SetLayerTilemap(int nlayer, TLN_Tilemap tilemap);

/*!
	\brief Configures a background layer with the specified full bitmap
	\param nlayer Layer index [0, num_layers - 1]
	\param bitmap Reference to the bitmap to assign
	\remarks This function doesn't modify the current position nor the blend mode, but assigns the palette of the specified bitmap
	\see TLN_LoadBitmap() TLN_DisableLayer()
*/
TLNAPI bool TLN_SetLayerBitmap(int nlayer, TLN_Bitmap bitmap);

/*!
	\brief Sets the color palette to the layer
	\param nlayer Layer index [0, num_layers - 1]
	\param palette Reference to the  palette to assign to the layer

	Overrides the palette of the current tileset or bitmap

	\remarks
	Call this function inside a raster callback to change the palette in the middle
	of the frame to get raster effect colors, like and "underwater" palette below the
	water line in a partially submerged background, or a gradient palette in an area at
	the top of the screen to simulate a "depth fog effect" in a pseudo 3d background
*/
TLNAPI bool TLN_SetLayerPalette (int nlayer, TLN_Palette palette);

/*!
	\brief Sets the position of the tileset that corresponds to the upper left corner
	\param nlayer Layer index [0, num_layers - 1]
	\param hstart Horizontal offset in the tileset on the left side
	\param vstart Vertical offset in the tileset on the top side

	The tileset usually spans an area much bigger than the viewport. Use this
	function to move the viewport insde the tileset. Change this value progressively
	for each frame to get a scrolling effect

	\remarks
	Call this function inside a raster callback to get a raster scrolling effect.
	Use this to create horizontal strips of the same
	layer that move at different speeds to simulate depth. The extreme case of this effect, where
	the position is changed in each scanline, is called "line scroll" and was the technique used by
	games such as Street Fighter II to simualte a pseudo 3d floor, or many racing games to simulate
	a 3D road.
*/
TLNAPI bool TLN_SetLayerPosition (int nlayer, int hstart, int vstart);

/*!
	\brief Sets simple scaling
	\param nlayer Layer index [0, num_layers - 1]
	\param xfactor Horizontal scale factor
	\param yfactor Vertical scale factor

	By default the scaling factor of a given layer is 1.0f, 1.0f, which means
	no scaling. Use values below 1.0 to downscale (shrink) and above 1.0 to upscale (enlarge).
	Call TLN_ResetLayerMode() to disable scaling

	\see TLN_ResetLayerMode()
*/
TLNAPI bool TLN_SetLayerScaling (int nlayer, float xfactor, float yfactor);

/*!
	\brief Sets affine transform matrix to enable rotating and scaling of this layer
	\param nlayer Layer index [0, num_layers - 1]
	\param affine Pointer to an TLN_Affine matrix, or NULL to disable it

	Enable the transformation matrix to give the layer the capabilities of the famous
	Super Nintendo / Famicom Mode 7. Beware that the rendering of a transformed layer
	uses more CPU than a regular layer. Unlike the original Mode 7, that could only transform
	the single layer available, Tilengine can transform all the layers at the same time. The only
	limitation is the available CPU power.

	\remarks
	Call this function inside a raster callback to set the transformation matrix in the middle of
	the frame. Setting it for each scanline is the trick used by many Super Nintendo games to fake
	a 3D perspective projection.

	\see TLN_SetLayerTransform()
 */
TLNAPI bool TLN_SetLayerAffineTransform (int nlayer, TLN_Affine *affine);

/*!
	\brief Sets affine transform matrix to enable rotating and scaling of this layer
	\param layer Layer index [0, num_layers - 1]
	\param angle Rotation angle in degrees
	\param dx Horizontal displacement
	\param dy Vertical displacement
	\param sx Horizontal scaling
	\param sy Vertical scaling
	\remarks This function is a simple wrapper to TLN_SetLayerAffineTransform() without using the TLN_Affine struct
	\see TLN_SetLayerAffineTransform()
*/
TLNAPI bool TLN_SetLayerTransform (int layer, float angle, float dx, float dy, float sx, float sy);

/*!
	\brief Sets the table for pixel mapping render mode
	\param nlayer Layer index [0, num_layers - 1]
	\param table User-provided array of hres*vres sized TLN_PixelMap items
	\see TLN_SetLayerScaling(), TLN_SetLayerAffineTransform()
*/
TLNAPI bool TLN_SetLayerPixelMapping (int nlayer, TLN_PixelMap* table);

/*!
	\brief Sets the blending mode (transparency effect)
	\param nlayer Layer index [0, num_layers - 1]
	\param mode Member of the TLN_Blend enumeration
	\param factor Deprecated as of 1.12, left for backwards compatibility but doesn't have effect.
	\see Blending
*/
TLNAPI bool TLN_SetLayerBlendMode (int nlayer, TLN_Blend mode, uint8_t factor);

/*!
	\brief Enables column offset mode for this layer
	\param nlayer Layer index [0, num_layers - 1]
	\param offset Array of offsets to set. Set NULL to disable column offset mode

	Column offset is a value that is added or substracted (depending on the
	sign) to the vertical position for that layer (see TLN_SetLayerPosition) for
	each column in the tilemap assigned to that layer.

	\remarks
	This feature is typically used to simulate vertical strips moving at different
	speeds, or combined with a line scroll effect, to fake rotations where the angle
	is small. The Sega Genesis games Puggsy and Chuck Rock II used this trick to simulate
	partially rotating backgrounds
*/
TLNAPI bool TLN_SetLayerColumnOffset (int nlayer, int* offset);

/*!
	\deprecated Use \ref TLN_SetLayerWindow instead
	\brief Enables clipping rectangle on selected layer
	\param nlayer Layer index [0, num_layers - 1]
	\param x1 left coordinate
	\param y1 top coordinate
	\param x2 right coordinate
	\param y2 bottom coordinate
*/
TLNAPI bool TLN_SetLayerClip (int nlayer, int x1, int y1, int x2, int y2);

/*!
	\deprecated Use \ref TLN_DisableLayerWindow instead
	\brief Disables clipping rectangle on selected layer
	\param nlayer Layer index [0, num_layers - 1]
*/
TLNAPI bool TLN_DisableLayerClip (int nlayer);

/*!
	\brief Enables clipping window on selected layer
	\param nlayer Layer index [0, num_layers - 1]
	\param x1 left coordinate
	\param y1 top coordinate
	\param x2 right coordinate
	\param y2 bottom coordinate
	\param invert false=clip outer region, true=clip inner region
	\see TLN_SetLayerWindowColor(), TLN_DisableLayerWindow()
 */
TLNAPI bool TLN_SetLayerWindow(int nlayer, int x1, int y1, int x2, int y2, bool invert);

/*!
	\brief Enables solid color processing on clipped region in window layer
	\param nlayer Layer index [0, num_layers - 1]
	\param r Red component (0-255)
	\param g Green component (0-255)
	\param b Blue component (0-255)
	\param blend one of possible TLN_Blend modes
	When color is enabled on window, the area outside the clipped region gets filled with this color.
	If one of blending modes is selected, color math is performed with underlying layer
	\see TLN_SetLayerWindow(), TLN_DisableLayerWindowColor()
*/
TLNAPI bool TLN_SetLayerWindowColor(int nlayer, uint8_t r, uint8_t g, uint8_t b, TLN_Blend blend);

/*!
	\brief Disables layer window clipping
	\param nlayer Layer index [0, num_layers - 1]
	\see TLN_SetLayerWindow()
*/
TLNAPI bool TLN_DisableLayerWindow(int nlayer);

/*!
	\brief Disables color processing for window on selected layer
	\param nlayer Layer index [0, num_layers - 1]
	\see TLN_SetLayerWindowColor()
*/
TLNAPI bool TLN_DisableLayerWindowColor(int nlayer);

/*!
	\brief Enables mosaic effect (pixelation) for selected layer
	\param nlayer Layer index [0, num_layers - 1]
	\param width horizontal pixel size
	\param height vertical pixel size
	\see TLN_DisableLayerMosaic()
*/
TLNAPI bool TLN_SetLayerMosaic (int nlayer, int width, int height);

/*!
	\brief Disables mosaic effect for selected layer
	\param nlayer Layer index [0, num_layers - 1]
	\see TLN_SetLayerMosaic()
*/
TLNAPI bool TLN_DisableLayerMosaic (int nlayer);

/*!
	\brief Disables scaling or affine transform for the layer
	\param nlayer Layer index [0, num_layers - 1]
	\see TLN_SetLayerScaling(), TLN_SetLayerAffineTransform()
*/
TLNAPI bool TLN_ResetLayerMode (int nlayer);

/*!
	\brief Configures a background layer with a object list and an image-based tileset
	\param nlayer Layer index [0, num_layers - 1]
	\param objects Reference to the TLN_ObjectList to attach
	\param tileset optional reference to the image-based tileset object. If NULL, object list must have an attached tileset
	\see TLN_LoadObjectList()
*/
TLNAPI bool TLN_SetLayerObjects(int nlayer, TLN_ObjectList objects, TLN_Tileset tileset);

/*!
	\brief Sets full layer priority, appearing in front of sprites
	\param nlayer Layer index [0, num_layers - 1]
	\param enable Enable (true) or dsiable (false) full priority
*/
TLNAPI bool TLN_SetLayerPriority(int nlayer, bool enable);

/*! \deprecated removed, keep for ABI compatibility with old versions*/
TLNAPI bool TLN_SetLayerParent(int nlayer, int parent);

/*! \deprecated removed, keep for ABI compatibility with old versions*/
TLNAPI bool TLN_DisableLayerParent(int nlayer);

/*!
	\brief Returns the layer width in pixels
	\param nlayer Layer index [0, num_layers - 1]
	\see TLN_SetLayer(), TLN_GetLayerHeight()
*/
TLNAPI bool TLN_DisableLayer (int nlayer);

/*!
	\brief Enables a layer previously disabled with \ref TLN_DisableLayer
	\param nlayer Layer index [0, num_layers - 1]
	\remarks The layer must have been previously configured. A layer without a prior configuration can't be enabled
*/
TLNAPI bool TLN_EnableLayer(int nlayer);

/*!
	\brief Returns the type of the layer
	\param nlayer Layer index [0, num_layers - 1]
	\returns \ref TLN_LayerType enumeration
	\see TLN_SetLayerTilemap(), TLN_SetLayerObjects(), TLN_SetLayerBitmap()
*/
TLNAPI TLN_LayerType TLN_GetLayerType(int nlayer);

/*!
	\brief Returns the active palette of a layer if set with \ref TLN_SetLayerPalette(), or the palette of the first tileset, or palette of bitmap
	\param nlayer Layer index [0, num_layers - 1]
	\returns Reference of the palette assigned to the layer
	\see TLN_SetLayerPalette()
*/
TLNAPI TLN_Palette TLN_GetLayerPalette (int nlayer);

/*! \deprecated Returns the first tilesetof the attached layer's tilemap */
TLNAPI TLN_Tileset TLN_GetLayerTileset(int nlayer);

/*!
	\brief Returns the active tilemap on a \ref LAYER_TILE layer type
	\param nlayer Layer index [0, num_layers - 1]
	\returns Reference to the active tilemap
	\see TLN_SetLayerTilemap()
*/
TLNAPI TLN_Tilemap TLN_GetLayerTilemap(int nlayer);

/*!
	\brief Returns the active bitmap on a \ref LAYER_BITMAP layer type
	\param nlayer Layer index [0, num_layers - 1]
	\returns Reference to the active bitmap
	\see TLN_SetLayerBitmap()
*/
TLNAPI TLN_Bitmap TLN_GetLayerBitmap(int nlayer);

/*!
	\brief Returns the active object list on a \ref LAYER_OBJECT layer type
	\param nlayer Layer index [0, num_layers - 1]
	\returns Reference to the active objects list
	\see TLN_SetLayerObjects(), TLN_GetListObject()
*/
TLNAPI TLN_ObjectList TLN_GetLayerObjects(int nlayer);

/*!
	\brief Gets info about the tile located in tilemap space
	\param nlayer Id of the layer to query [0, num_layers - 1]
	\param x horizontal position
	\param y vertical position
	\param info	Pointer to an application-allocated TLN_TileInfo struct that will get the data
	\returns true if success or false if error
	\remarks Use this function to implement collision detection between sprites and the main background layer.
	\see TLN_TileInfo
*/
TLNAPI bool TLN_GetLayerTile (int nlayer, int x, int y, TLN_TileInfo* info);

/*!
	\brief Returns the layer width in pixels
	\param nlayer Layer index [0, num_layers - 1]
	\see TLN_SetLayer(), TLN_GetLayerHeight()
*/
TLNAPI int TLN_GetLayerWidth (int nlayer);

/*!
	\brief Returns the layer height in pixels
	\param nlayer Layer index [0, num_layers - 1]
	\see TLN_SetLayer(), TLN_GetLayerWidth()
*/
TLNAPI int TLN_GetLayerHeight (int nlayer);

/*
	\brief returns layer's horizontal position
	\param nlayer Layer index to query
	\returns x position
	\see TLN_SetLayerPosition()
*/
TLNAPI int TLN_GetLayerX(int nlayer);

/*
	\brief returns layer's vertical position
	\param nlayer Layer index to query
	\returns y position
	\see TLN_SetLayerPosition()
*/
TLNAPI int TLN_GetLayerY(int nlayer);

/**@}*/

/**
 * \defgroup sprite Sprites management
* @{ */

/*!
	\deprecated use \ref TLN_SetSpriteSet and \ref TLN_EnableSpriteFlag
	\brief Configures a sprite, setting spriteset and flags at once
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param spriteset Reference of the spriteset containing the graphics to set
	\param flags Can be 0 or a combination of FLAG_FLIPX and FLAG_FLIPY
	\remarks This function also assigns the palette of the spriteset
	\see TLN_SetSpritePicture()
*/
TLNAPI bool TLN_ConfigSprite (int nsprite, TLN_Spriteset spriteset, uint32_t flags);

/*!
	\brief Assigns the spriteset and its palette to a given sprite
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param spriteset Reference of the spriteset containing the graphics to set
	\remarks This function also assigns the palette of the spriteset and resets pivot to top left corner (default)
	\see TLN_SetSpritePicture()
*/
TLNAPI bool TLN_SetSpriteSet (int nsprite, TLN_Spriteset spriteset);

/*!
	\deprecated Use \ref TLN_EnableSpriteFlag to enable or disable individual flags
	\brief Sets flags for a given sprite
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param flags Can be 0 or a combination of TLN_TileFlags
*/
TLNAPI bool TLN_SetSpriteFlags (int nsprite, uint32_t flags);

/*!
	\brief Enables or disables specified flag for a sprite
	\param nsprite of the sprite [0, num_sprites - 1]
	\param flag flag (or combination of flags) to modfy
	\param enable true for enable, false for disable
*/
TLNAPI bool TLN_EnableSpriteFlag(int nsprite, uint32_t flag, bool enable);

/*!
	\brief Sets sprite pivot point. By default is at (0,0) = top left corner
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param px horizontal normalized value (0.0 = full left, 1.0 = full right)
	\param py vertical normalized value (0.0 = full top, 1.0 = full bottom)
	\remarks Sprite pivot is reset automatically to default position after changing the spriteset
*/
TLNAPI bool TLN_SetSpritePivot(int nsprite, float px, float py);

/*!
	\brief Sets the sprite position in screen space
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param x Horizontal position of pivot (0 = left margin)
	\param y Vertical position of pivot (0 = top margin)

	\remarks
	Call this function inside a raster callback to so some vertical distortion effects
	(fake scaling) or sprite multiplexing (reusing a single sprite at different screen heights).
	This technique was used by some 8 bit games, with very few hardware sprites, to draw much more
	sprites in the screen, as long as they don't overlap vertically

	\sa TLN_SetSpritePivot
*/
TLNAPI bool TLN_SetSpritePosition (int nsprite, int x, int y);

/*!
	\brief Sets the actual graphic to the sprite
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param entry Index of the actual picture inside the srpteset to assign (0 <= entry < num_spriteset_graphics)
	\see TLN_SetSpriteSet()
*/
TLNAPI bool TLN_SetSpritePicture (int nsprite, int entry);

/*!
	\brief Assigns a palette to a sprite
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param palette Reference of the palete to assign
*/
TLNAPI bool TLN_SetSpritePalette (int nsprite, TLN_Palette palette);

/*!
	\brief Sets the blending mode (transparency effect)
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param mode Member of the TLN_Blend enumeration
	\param factor Deprecated as of 1.12, left for backwards compatibility but doesn't have effect.
	\see Blending
*/
TLNAPI bool TLN_SetSpriteBlendMode (int nsprite, TLN_Blend mode, uint8_t factor);

/*!
	\brief Sets the scaling factor of the sprite
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param sx Horizontal scale factor
	\param sy Vertical scale factor

	By default the scaling factor of a given sprite is 1.0f, 1.0f, which means
	no scaling. Use values below 1.0 to downscale (shrink) and above 1.0 to upscale (enlarge).
	Call TLN_ResetScaling() to disable scaling

	\remarks The rendering of a sprite with scaling enabled requires somewhat more CPU power than a regular sprite.
	\see TLN_ResetSpriteScaling()
*/
TLNAPI bool TLN_SetSpriteScaling (int nsprite, float sx, float sy);

/*!
	\brief Disables scaling for a given sprite
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\see TLN_SetSpriteScaling()
*/
TLNAPI bool TLN_ResetSpriteScaling (int nsprite);

/*!
	\brief Returns the index of the assigned picture from the spriteset
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\see TLN_SetSpritePicture()
*/
TLNAPI int  TLN_GetSpritePicture (int nsprite);

/*
	\brief returns sprite's horizontal position
	\param nsprite Sprite index to query
	\returns x position
	\see TLN_SetSpritePosition()
*/
TLNAPI int TLN_GetSpriteX(int nsprite);

/*
	\brief returns sprite's vertical position
	\param nsprite Sprite index to query
	\returns y position
	\see TLN_SetSpritePosition()
*/
TLNAPI int TLN_GetSpriteY(int nsprite);

/*!
	\brief Finds an available (unused) sprite
	\returns Index of the first unused sprite (starting from 0) or -1 if none found
*/
TLNAPI int  TLN_GetAvailableSprite (void);

/*!
	\brief Enable sprite collision checking at pixel level
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param enable Set true to enable o false to disable (default value)

	\remarks
	Only sprites that have collision enabled are checked between them,
	so to detect a collision between two sprites, both of them must
	have collision detection enabled. Processing collision detection
	sprites take more a bit more CPU time compared to non-colliding sprites, so
	by default it is disabled on all sprites.

	\see TLN_GetSpriteCollision()
*/
TLNAPI bool TLN_EnableSpriteCollision (int nsprite, bool enable);

/*!
	\brief Gets the collision status of a given sprite
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\returns true if this sprite is involved in a collision with another sprite
	\remarks Collision detection must be enabled for the sprite to get checked
	\see TLN_EnableSpriteCollision()
 */
TLNAPI bool TLN_GetSpriteCollision (int nsprite);

/*!
	\brief Returns runtime info about a given sprite
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param state Pointer to a user-allocated TLN_SpriteState structure to fill with requested data
*/
TLNAPI bool TLN_GetSpriteState(int nsprite, TLN_SpriteState* state);

/*!
	\brief Sets the first sprite drawn (beginning of list)
	\param nsprite Id of the sprite [0, num_sprites - 1]. Must be enabled (visible)
 */
TLNAPI bool TLN_SetFirstSprite(int nsprite);

/*!
	\brief Sets the next sprite to draw after a given sprite, builds list
	\param nsprite Id of the sprite [0, num_sprites - 1]. Must be enabled (visible)
	\param next Id of the sprite to draw after Id [0, num_sprites - 1]. Must be enabled (visible)
*/
TLNAPI bool TLN_SetNextSprite(int nsprite, int next);

/*!
	\deprecated, use \ref TLN_EnableSpriteFlag (nsprite, FLAG_MASKED, enable)
	\brief Enables or disables masking for this sprite, if enabled it won't be drawn inside the region set up with TLN_SetSpritesMaskRegion()
	\param nsprite Id of the sprite to mask [0, num_sprites - 1].
	\param enable Enables (true) or disables (false) masking
*/
TLNAPI bool TLN_EnableSpriteMasking(int nsprite, bool enable);

/*!
	\brief Defines a sprite masking region between the two scanlines. Sprites masked with TLN_EnableSpriteMasking() won't be drawn inside this region.
	\param top_line Top scaline where masking starts
	\param bottom_line Bottom scaline where masking ends
*/
TLNAPI void TLN_SetSpritesMaskRegion(int top_line, int bottom_line);

/*!
	\brief Starts a sprite animation
	\param nsprite Id of the sprite to animate (0 <= id < num_sprites)
	\param sequence	Reference of the sequence to assign
	\param loop	amount of times to loop, 0=infinite
	\see Animations
*/
TLNAPI bool TLN_SetSpriteAnimation (int nsprite, TLN_Sequence sequence, int loop);

/*!
	\brief Disables animation for the given sprite
	\param index Id of the spriteto set (0 <= id < num_sprites)
	\see Animations
*/
TLNAPI bool TLN_DisableSpriteAnimation(int index);

/*!
	\brief Pauses animation for the given sprite
	\param index Id of the sprite to pause animation (0 <= id < num_sprites)
	\see Animations TLN_ResumeSpriteAnimation
*/
TLNAPI bool TLN_PauseSpriteAnimation(int index);

/*!
	\brief Restores animation for the given sprite
	\param index Id of the sprite to resume animation (0 <= id < num_sprites)
	\see Animations TLN_PauseSpriteAnimation
*/
TLNAPI bool TLN_ResumeSpriteAnimation(int index);

/* \deprecated, use TLN_DisableSpriteAnimation() instead */
TLNAPI bool TLN_DisableAnimation(int index);

/*!
	\brief Disables the sprite so it is not drawn
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\remarks
	A sprite is also automatically disabled when assigned with an invalid spriteste or palette. Disabled
	sprites are returned by the function TLN_GetAvailableSprite as available
*/
TLNAPI bool TLN_DisableSprite (int nsprite);

/*!
	\brief Gets the palette assigned to a given sprite
	\param nsprite Id of the sprite to query (0 <= id < max_sprites)
	\returns Reference to the sprite palette
	\see TLN_SetSpritePalette(), TLN_SetSpriteSet()
*/
TLNAPI TLN_Palette TLN_GetSpritePalette (int nsprite);
/**@}*/

/**
 * \defgroup sequence Sequence resources management for layer, sprite and palette animations
* @{ */

/*!
	\brief Creates a new sequence for the animation engine
	\param name String with an unique name to query later
	\param target For tileset animations, the tile index to animate
	\param num_frames Number of frames
	\param frames Array of TLN_Frame items with indexes and delays
	\returns Reference to the new sequence or NULL if error
	\remarks Use this function to create tileset or sprite animations
	\see TLN_SetTilemapAnimation(), TLN_SetSpriteAnimation()
*/
TLNAPI TLN_Sequence TLN_CreateSequence (const char* name, int target, int num_frames, TLN_SequenceFrame* frames);

/*!
	\brief Creates a color cycle sequence for palette animation
	\param name String with an unique name to query later
	\param num_strips Number of color strips
	\param strips Array of color strips to assign
	\returns Reference to the created cycle or NULL if error
	\remarks Use this function to create advanced palette animation effects
	\see TLN_ColorStrip(), TLN_SetPaletteAnimation()
*/
TLNAPI TLN_Sequence TLN_CreateCycle (const char* name, int num_strips, TLN_ColorStrip* strips);

/*!
	\brief Creates a name based sprite sequence
	\param name Optional name used to retrieve it when adding to a TLN_SequencePack, can be NULL
	\param spriteset Reference to the spriteset with frames to animate
	\param basename Base of the sprite name for the numbered sequence
	\param delay Number of ticks to delay between frame
	\return Reference to the created TLN_Sequence object or NULL if error
	\remarks Trailing numbers in sprite names must start with 1 and be correlative (eg basename1... basename14)
*/
TLNAPI TLN_Sequence TLN_CreateSpriteSequence(const char* name, TLN_Spriteset spriteset, const char* basename, int delay);

/*!
	\brief Creates a duplicate of the specified sequence
	\param src Sequence to clone
	\returns A reference to the newly cloned sequence, or NULL if error
	\see TLN_FindSequence()
*/
TLNAPI TLN_Sequence TLN_CloneSequence (TLN_Sequence src);

/*!
	\brief Returns runtime info about a given sequence
	\param sequence Sequence to query
	\param info	Pointer to a user-provided TLN_SequenceInfo structure to hold the returned data
	\see TLN_FindSequence()
*/
TLNAPI bool TLN_GetSequenceInfo (TLN_Sequence sequence, TLN_SequenceInfo* info);

/*!
	\brief Deletes the sequence and frees resources
	\param sequence	Reference to the sequence to be deleted
	\remarks Don't delete an active sequence!
 */
TLNAPI bool TLN_DeleteSequence (TLN_Sequence sequence);
/**@}*/

/**
 * \defgroup sequencepack Sequence pack manager for grouping and finding sequences
* @{ */

/*!
	\brief Creates a new collection of sequences
	\returns Reference to the created pack or NULL if error
	\see TLN_AddSequenceToPack(), TLN_CreateSequence()
*/
TLNAPI TLN_SequencePack TLN_CreateSequencePack (void);

/*!
	\brief Loads a sqx file containing one or more sequences
	\param filename SQX filename with the sequences to load
	\returns Reference to the newly created TLN_SequencePack() or NULL if error
	\remarks A SQX file can contain many sequences. This function loads all of them	inside a single TLN_SequencePack(). Individual sequences can be later queried with TLN_FindSequence()
	\see TLN_FindSequence()
*/
TLNAPI TLN_SequencePack TLN_LoadSequencePack (const char* filename);

/*!
	\brief Returns the nth sequence inside a sequence pack
	\param sp Reference to the sequence pack containing the sequence to find
	\param index Sequence number to return [0, num_sequences - 1]
	\see TLN_LoadSequencePack(), TLN_GetSequencePackCount()
*/
TLNAPI TLN_Sequence TLN_GetSequence (TLN_SequencePack sp, int index);

/*!
	\brief Finds a sequence inside a sequence pack
	\param sp Reference to the sequence pack containing the sequence to find
	\param name Name of the sequence to find
	\returns Reference to the sequence with the specified name, or NULL if not found
	\see TLN_LoadSequencePack()
*/
TLNAPI TLN_Sequence TLN_FindSequence (TLN_SequencePack sp, const char* name);

/*!
	\brief Returns the number of sequences inside a sequence pack
	\param sp Reference to the sequence pack to query
 */
TLNAPI int TLN_GetSequencePackCount (TLN_SequencePack sp);

/*!
	\brief Adds a sequence to a sequence pack
	\param sp Reference to the sequence pack
	\param sequence	Reference to the sequence to attach
	\remarks Write remarks for TLN_AddSequenceToPack here.
	\see TLN_CreateSequencePack(), TLN_CreateSequence()
*/
TLNAPI bool TLN_AddSequenceToPack (TLN_SequencePack sp, TLN_Sequence sequence);

/*!
	\brief Deletes the specified sequence pack and frees memory
	\param sp Reference to the sequence pack to delete
	\remarks Don't delete a sequence pack that has sequences currently attached to animations!
	\remarks The attached sequences are also deleted, so they haven't to be deleted externally.
	\see TLN_LoadSequencePack()
*/
TLNAPI bool TLN_DeleteSequencePack (TLN_SequencePack sp);
/**@}*/

/**
 * \defgroup animation Color cycle animation
* @{ */

/*!
	\brief Starts a palette animation
	\param index Id of the animation to set (0 <= id < num_animations)
	\param palette Reference of the palette to be animated
	\param sequence	Reference of the sequence to assign
	\param blend true for smooth frame interpolation, false for classic, discrete mode
*/
TLNAPI bool TLN_SetPaletteAnimation (int index, TLN_Palette palette, TLN_Sequence sequence, bool blend);

/*!
	\brief Sets the source palette of a color cycle animation
	\param index Id of the animation to set (0 <= id < num_animations)
	\param palette Reference of the palette to assign
	\remarks Use this function to change the palette assigned to a color cycle animation running. This is useful to combine color cycling and palette interpolation at the same time
*/
TLNAPI bool TLN_SetPaletteAnimationSource (int index, TLN_Palette palette);

/*!
	\brief Checks the state of the animation for given sprite
	\param index Id of the sprite to check (0 <= id < num_sprites)
	\returns true if animation is running, false if it's finished or inactive
*/
TLNAPI bool TLN_GetAnimationState (int index);

/*!
	\brief Sets animation delay for single frame of given sprite animation
	\param index Id of the sprite with animation (0 <= id < num_sprites)
	\param frame Id of animation frame to change delay in (0 <= id < sequence->count)
	\param delay New animation frame delay to set
	\see Animations
*/
TLNAPI bool TLN_SetAnimationDelay (int index, int frame, int delay);

/*!
	\brief Finds an available (unused) animation
	\returns Index of the first unused animation (starting from 0) or -1 if none found
*/
TLNAPI int  TLN_GetAvailableAnimation (void);

/*!
	\brief Disables the color cycle animation so it stops playing
	\param index Id of the animation to set (0 <= id < num_animations)
	\see Animations
*/
TLNAPI bool TLN_DisablePaletteAnimation(int index);
/**@}*/

/**
 * \defgroup world World management
* @{ */

/*!
	\brief Loads and assigns complete TMX file
	\param tmxfile Filename of TMX file to load
	\param first_layer Starting layer number where place the loaded tmx
*/
TLNAPI bool TLN_LoadWorld(const char* tmxfile, int first_layer);

/*!
	\brief Sets global world position, moving all layers in sync according to their parallax factor
	\param x horizontal position in world space
	\param y vertical position in world space
*/
TLNAPI void TLN_SetWorldPosition(int x, int y);

/*!
	\brief Sets layer parallax factor to use in conjunction with \ref TLN_SetWorldPosition
	\param nlayer Layer index [0, num_layers - 1]
	\param x Horizontal parallax factor
	\param y Vertical parallax factor
*/
TLNAPI bool TLN_SetLayerParallaxFactor(int nlayer, float x, float y);

/*!
	\brief Sets the sprite position in world space coordinates
	\param nsprite Id of the sprite [0, num_sprites - 1]
	\param x Horizontal world position of pivot (0 = left margin)
	\param y Vertical world position of pivot (0 = top margin)
	\sa TLN_SetSpritePivot
*/
TLNAPI bool TLN_SetSpriteWorldPosition(int nsprite, int x, int y);

/*! \brief Releases world resources loaded with TLN_LoadWorld  */
TLNAPI void TLN_ReleaseWorld(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif
