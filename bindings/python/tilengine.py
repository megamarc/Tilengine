# Python wrapper for Tilengine
from ctypes import *

# constants -----------------------------------------------------------------------

# window creation flags
CWF_FULLSCREEN	= (1<<0)
CWF_VSYNC		= (1<<1)
CWF_S1			= (1<<2)
CWF_S2			= (2<<2)
CWF_S3			= (3<<2)
CWF_S4			= (4<<2)
CWF_S5			= (5<<2)

# tile/sprite flags
FLAG_FLIPX		= (1<<15)	# horizontal flip
FLAG_FLIPY		= (1<<14)	# vertical flip
FLAG_ROTATE		= (1<<13)	# row/column flip (unsupported, Tiled compatibility)
FLAG_PRIORITY	= (1<<12)	# tile goes in front of sprite layer

# Error codes
ERR_OK 				= 0	 # No error 
ERR_OUT_OF_MEMORY 	= 1	 # Not enough memory 
ERR_IDX_LAYER 		= 2	 # Layer index out of range 
ERR_IDX_SPRITE 		= 3	 # Sprite index out of range 
ERR_IDX_ANIMATION 	= 4	 # Animation index out of range 
ERR_IDX_PICTURE 	= 5	 # Picture or tile index out of range 
ERR_REF_TILESET 	= 6	 # Invalid Tileset reference 
ERR_REF_TILEMAP 	= 7	 # Invalid Tilemap reference 
ERR_REF_SPRITESET 	= 8	 # Invalid Spriteset reference 
ERR_REF_PALETTE 	= 9	 # Invalid Palette reference 
ERR_REF_SEQUENCE 	= 10 # Invalid SequencePack reference 
ERR_REF_SEQPACK 	= 11 # Invalid Sequence reference 
ERR_REF_BITMAP 		= 12 # Invalid Bitmap reference 
ERR_NULL_POINTER 	= 13 # Null pointer as argument  
ERR_FILE_NOT_FOUND 	= 14 # Resource file not found 
ERR_WRONG_FORMAT 	= 15 # Resource file has invalid format 
ERR_WRONG_SIZE 		= 16 # A width or height parameter is invalid 
ERR_UNSUPPORTED		= 17 # Unsupported function

# blend modes
BLEND_NONE		= 0
BLEND_MIX		= 1
BLEND_ADD		= 2
BLEND_SUB		= 3
BLEND_MOD		= 4
MAX_BLEND		= 5

# inputs
INPUT_NONE 		= 0
INPUT_UP 		= 1
INPUT_DOWN 		= 2
INPUT_LEFT 		= 3
INPUT_RIGHT		= 4
INPUT_A 		= 5
INPUT_B 		= 6
INPUT_C 		= 7
INPUT_D 		= 8

# structures ------------------------------------------------------------------
class Affine(Structure):
	_fields_ = [
		("angle", c_float),
		("dx", c_float),
		("dy", c_float),
		("sx", c_float),
		("sy", c_float)
	]
	
class Tile(Structure):
	_fields_ = [
		("index", c_ushort),
		("flags", c_ushort)
	]

class ColorStrip(Structure):
	_fields_ = [
		("delay", c_int),
		("first", c_ubyte),
		("count", c_ubyte),
		("dir", c_ubyte)
	]

class SpriteInfo(Structure):
	_fields_ = [
		("offset", c_int),
		("w", c_int),
		("h", c_int)
	]

class TileInfo(Structure):
	_fields_ = [
		("index", c_ushort),
		("flags", c_ushort),
		("row", c_int),
		("col", c_int),
		("xoffset", c_int),
		("yoffset", c_int),
		("color", c_ubyte)
	]
	
class Rect(Structure):
	_fields_ = [
		("x", c_int),
		("y", c_int),
		("w", c_int),
		("h", c_int)
	]

from sys import platform as _platform
if _platform == "linux" or _platform == "linux2":
	_tln = cdll.LoadLibrary("libTilengine.so")
elif _platform == "win32":
	_tln = cdll.LoadLibrary("Tilengine.dll")
	
RasterCallbackFunc = CFUNCTYPE(None, c_int)

# basic management ------------------------------------------------------------
Init = _tln.TLN_Init
Init.argtypes = [c_int, c_int, c_int, c_int, c_int]
Init.restype = c_bool
		
InitBPP = _tln.TLN_InitBPP
InitBPP.argtypes = [c_int, c_int, c_int, c_int, c_int, c_int]
InitBPP.restype = c_bool

Deinit = _tln.TLN_Deinit
Deinit.restype = None
	
GetNumObjects = _tln.TLN_GetNumObjects
GetNumObjects.restype = c_int
	
GetVersion = _tln.TLN_GetVersion
GetVersion.restype = c_int

GetUsedMemory = _tln.TLN_GetUsedMemory
GetUsedMemory.restype = c_int
	
GetNumLayers = _tln.TLN_GetNumLayers
GetNumLayers.restype = c_int

GetNumSprites = _tln.TLN_GetNumSprites
GetNumSprites.restype = c_int

SetBGColor = _tln.TLN_SetBGColor
SetBGColor.argtypes = [c_ubyte, c_ubyte, c_ubyte]
SetBGColor.restype = None

SetBGBitmap = _tln.TLN_SetBGBitmap
SetBGBitmap.argtypes = [c_void_p]
SetBGBitmap.restype = c_bool

SetBGPalette = _tln.TLN_SetBGPalette
SetBGPalette.argtypes = [c_void_p]
SetBGPalette.restype = c_bool

SetRasterCallback = _tln.TLN_SetRasterCallback
SetRasterCallback.restype = None

SetRenderTarget = _tln.TLN_SetRenderTarget
SetRenderTarget.argtypes = [c_void_p, c_int]
SetRenderTarget.restype = None

UpdateFrame = _tln.TLN_UpdateFrame
UpdateFrame.argtypes = [c_int]
UpdateFrame.restype = None

BeginFrame = _tln.TLN_BeginFrame
BeginFrame.argtypes = [c_int]
BeginFrame.restype = None

DrawNextScanline = _tln.TLN_DrawNextScanline
DrawNextScanline.restype = c_bool

SetLoadPath = _tln.TLN_SetLoadPath
SetLoadPath.argtypes = [c_char_p]
SetLoadPath.restype = None

# error handling --------------------------------------------------------------
GetLastError = _tln.TLN_GetLastError
GetLastError.restype = c_int

GetErrorString = _tln.TLN_GetErrorString
GetErrorString.argtypes = [c_int]
GetErrorString.restype = c_char_p

# window management -----------------------------------------------------------
CreateWindow = _tln.TLN_CreateWindow
CreateWindow.argtypes = [c_char_p, c_int]
CreateWindow.restype = c_bool

CreateWindowThread = _tln.TLN_CreateWindowThread
CreateWindowThread.argtypes = [c_char_p, c_int]
CreateWindowThread.restype = c_bool

ProcessWindow = _tln.TLN_ProcessWindow
ProcessWindow.restype = c_bool

IsWindowActive = _tln.TLN_IsWindowActive
IsWindowActive.restype = c_bool

GetInput = _tln.TLN_GetInput
GetInput.argtypes = [c_int]
GetInput.restype = c_bool

DrawFrame = _tln.TLN_DrawFrame 
DrawFrame.argtypes = [c_int]
DrawFrame.restype = None

WaitRedraw = _tln.TLN_WaitRedraw
WaitRedraw.restype = None

DeleteWindow = _tln.TLN_DeleteWindow
DeleteWindow.restype = None

EnableBlur = _tln.TLN_EnableBlur
EnableBlur.argtypes = [c_bool]
EnableBlur.restype = None

GetTicks = _tln.TLN_GetTicks
GetTicks.restype = c_int

Delay = _tln.TLN_Delay
Delay.argtypes = [c_int]
Delay.restype = None

BeginWindowFrame = _tln.TLN_BeginWindowFrame
BeginWindowFrame.argtypes = [c_int]
BeginWindowFrame.restype = None

EndWindowFrame = _tln.TLN_EndWindowFrame
EndWindowFrame.restype = None

# spritesets management
CreateSpriteset = _tln.TLN_CreateSpriteset
CreateSpriteset.argtypes = [c_int, POINTER(Rect), c_void_p, c_int, c_int, c_int, c_void_p]
CreateSpriteset.restype = c_void_p

LoadSpriteset = _tln.TLN_LoadSpriteset
LoadSpriteset.argtypes = [c_char_p]
LoadSpriteset.restype = c_void_p

CloneSpriteset = _tln.TLN_CloneSpriteset
CloneSpriteset.argtypes = [c_void_p]
CloneSpriteset.restype = c_void_p

GetSpriteInfo = _tln.TLN_GetSpriteInfo
GetSpriteInfo.argtypes = [c_void_p, c_int, c_void_p]
GetSpriteInfo.restype = c_bool
	
GetSpritesetPalette = _tln.TLN_GetSpritesetPalette
GetSpritesetPalette.argtypes = [c_void_p]
GetSpritesetPalette.restype = c_void_p
	
DeleteSpriteset = _tln.TLN_DeleteSpriteset
DeleteSpriteset.argtypes = [c_void_p]
DeleteSpriteset.restype = c_bool

# tilesets management ---------------------------------------------------------
CreateTileset = _tln.TLN_CreateTileset
CreateTileset.argtypes = [c_int, c_int, c_int, c_void_p]
CreateTileset.restype = c_void_p

LoadTileset = _tln.TLN_LoadTileset
LoadTileset.argtypes = [c_char_p]
LoadTileset.restype = c_void_p

CloneTileset = _tln.TLN_CloneTileset
CloneTileset.argtypes = [c_void_p]
CloneTileset.restype = c_void_p

SetTilesetPixels = _tln.TLN_SetTilesetPixels
SetTilesetPixels.argtypes = [c_void_p, c_int, c_void_p, c_int]
SetTilesetPixels.restype = c_bool

CopyTile = _tln.TLN_CopyTile
CopyTile.argtypes = [c_void_p, c_int, c_int]
CopyTile.restype = c_bool

GetTileWidth = _tln.TLN_GetTileWidth
GetTileWidth.argtypes = [c_void_p]
GetTileWidth.restype = c_int

GetTileHeight = _tln.TLN_GetTileWidth
GetTileHeight.argtypes = [c_void_p]
GetTileHeight.restype = c_int

GetTilesetPalette = _tln.TLN_GetTilesetPalette
GetTilesetPalette.argtypes = [c_void_p]
GetTilesetPalette.restype = c_void_p

DeleteTileset = _tln.TLN_DeleteTileset
DeleteTileset.argtypes = [c_void_p]
DeleteTileset.restype = c_bool

# tilemaps management ---------------------------------------------------------
CreateTilemap = _tln.TLN_CreateTilemap
CreateTilemap.argtypes = [c_int, c_int, POINTER(Tile)]
CreateTilemap.restype = c_void_p

LoadTilemap = _tln.TLN_LoadTilemap
LoadTilemap.argtypes = [c_char_p]
LoadTilemap.restype = c_void_p

CloneTilemap = _tln.TLN_CloneTilemap
CloneTilemap.argtypes = [c_void_p]
CloneTilemap.restype = c_void_p

GetTilemapRows = _tln.TLN_GetTilemapRows
GetTilemapRows.argtypes = [c_void_p]
GetTilemapRows.restype = c_int

GetTilemapCols = _tln.TLN_GetTilemapRows
GetTilemapCols.argtypes = [c_void_p]
GetTilemapCols.restype = c_int

GetTilemapTile = _tln.TLN_GetTilemapTile
GetTilemapTile.argtypes = [c_void_p, c_int, c_int, POINTER(Tile)]
GetTilemapTile.restype = c_bool

SetTilemapTile = _tln.TLN_SetTilemapTile
SetTilemapTile.argtypes = [c_void_p, c_int, c_int, POINTER(Tile)]
SetTilemapTile.restype = c_bool

CopyTiles = _tln.TLN_CopyTiles
CopyTiles.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_void_p, c_int, c_int]
CopyTiles.restype = c_bool

DeleteTilemap = _tln.TLN_DeleteTilemap
DeleteTilemap.argtypes = [c_void_p]
DeleteTilemap.restype = c_bool

# color tables management -----------------------------------------------------
CreatePalette = _tln.TLN_CreatePalette
CreatePalette.argtypes = [c_int]
CreatePalette.restype = c_void_p

LoadPalette = _tln.TLN_LoadPalette
LoadPalette.argtypes = [c_char_p]
LoadPalette.restype = c_void_p

ClonePalette = _tln.TLN_ClonePalette
ClonePalette.argtypes = [c_void_p]
ClonePalette.restype = c_void_p

DeletePalette = _tln.TLN_DeletePalette
DeletePalette.argtypes = [c_void_p]
DeletePalette.restype = c_bool

SetPaletteColor = _tln.TLN_SetPaletteColor
SetPaletteColor.argtypes = [c_void_p, c_int, c_ubyte, c_ubyte, c_ubyte]
SetPaletteColor.restype = c_bool

MixPalettes = _tln.TLN_MixPalettes
MixPalettes.argtypes = [c_void_p, c_void_p, c_void_p, c_ubyte]
MixPalettes.restype = c_bool

GetPaletteData = _tln.TLN_GetPaletteData
GetPaletteData.argtypes = [c_void_p, c_int]
GetPaletteData.restype = POINTER(c_ubyte)

# bitmaps ---------------------------------------------------------------------
CreateBitmap = _tln.TLN_CreateBitmap
CreateBitmap.argtypes = [c_int, c_int, c_int]
CreateBitmap.restype = c_void_p

LoadBitmap = _tln.TLN_LoadBitmap
LoadBitmap.argtypes = [c_char_p]
LoadBitmap.restype = c_void_p

CloneBitmap = _tln.TLN_CloneBitmap
CloneBitmap.argtypes = [c_void_p]
CloneBitmap.restype = c_void_p

GetBitmapPtr = _tln.TLN_GetBitmapPtr
GetBitmapPtr.argtypes = [c_void_p, c_int, c_int]
GetBitmapPtr.restype = POINTER(c_ubyte)

GetBitmapWidth = _tln.TLN_GetBitmapWidth
GetBitmapWidth.argtypes = [c_void_p]
GetBitmapWidth.restype = c_int

GetBitmapHeight = _tln.TLN_GetBitmapHeight
GetBitmapHeight.argtypes = [c_void_p]
GetBitmapHeight.restype = c_int

GetBitmapDepth = _tln.TLN_GetBitmapDepth
GetBitmapDepth.argtypes = [c_void_p]
GetBitmapDepth.restype = c_int

GetBitmapPitch = _tln.TLN_GetBitmapPitch
GetBitmapPitch.argtypes = [c_void_p]
GetBitmapPitch.restype = c_int

GetBitmapPalette = _tln.TLN_GetBitmapPalette
GetBitmapPalette.argtypes = [c_void_p]
GetBitmapPalette.restype = c_void_p

DeleteBitmap = _tln.TLN_DeleteBitmap
DeleteBitmap.argtypes = [c_void_p]
DeleteBitmap.restype = c_bool

# layer management ------------------------------------------------------------
SetLayer = _tln.TLN_SetLayer
SetLayer.argtypes = [c_int, c_void_p, c_void_p]
SetLayer.restype = c_bool
	
SetLayerPalette = _tln.TLN_SetLayerPalette
SetLayerPalette.argtypes = [c_int, c_void_p]
SetLayerPalette.restype = c_bool

SetLayerPosition = _tln.TLN_SetLayerPosition
SetLayerPosition.argtypes = [c_int, c_int, c_int]
SetLayerPosition.restype = c_bool

SetLayerScaling = _tln.TLN_SetLayerScaling
SetLayerScaling.argtypes = [c_int, c_float, c_float]
SetLayerScaling.restype = c_bool

SetLayerAffineTransform = _tln.TLN_SetLayerAffineTransform
SetLayerAffineTransform.argtypes = [c_int, POINTER(Affine)]
SetLayerAffineTransform.restype = c_bool

SetLayerTransform = _tln.TLN_SetLayerTransform
SetLayerTransform.argtypes = [c_int, c_float, c_float, c_float, c_float, c_float]
SetLayerTransform.restype = c_bool

SetLayerBlendMode = _tln.TLN_SetLayerBlendMode
SetLayerBlendMode.argtypes = [c_int, c_int, c_ubyte]
SetLayerBlendMode.restype = c_bool

SetLayerColumnOffset = _tln.TLN_SetLayerColumnOffset
SetLayerColumnOffset.argtypes = [c_int, POINTER(c_int)]
SetLayerColumnOffset.restype = c_bool
	
ResetLayerMode = _tln.TLN_ResetLayerMode
ResetLayerMode.argtypes = [c_int]
ResetLayerMode.restype = c_bool

DisableLayer = _tln.TLN_DisableLayer
DisableLayer.argtypes = [c_int]
DisableLayer.restype = c_bool

GetLayerPalette = _tln.TLN_GetLayerPalette
GetLayerPalette.argtypes = [c_int]
restype = c_void_p

GetLayerTile = _tln.TLN_GetLayerTile
GetLayerTile.argtypes = [c_int, c_int, c_int, POINTER(TileInfo)]
GetLayerTile.restype = c_bool

def CreateTileInfoPtr(info):
	return POINTER(TileInfo)(info)
	
# sprite management -----------------------------------------------------------
ConfigSprite = _tln.TLN_ConfigSprite
ConfigSprite.argtypes = [c_int, c_int, c_ushort]
ConfigSprite.restype = c_bool

SetSpriteSet = _tln.TLN_SetSpriteSet
SetSpriteSet.argtypes = [c_int, c_void_p]
SetSpriteSet.restype = c_bool

SetSpriteFlags = _tln.TLN_SetSpriteFlags
SetSpriteFlags.argtypes = [c_int, c_ushort]
SetSpriteFlags.restype = c_bool
	
SetSpritePosition = _tln.TLN_SetSpritePosition
SetSpritePosition.argtypes = [c_int, c_int, c_int]
SetSpritePosition.restype = c_bool
	
SetSpritePicture = _tln.TLN_SetSpritePicture
SetSpritePicture.argtypes = [c_int, c_int]
SetSpritePicture.restype = c_bool

SetSpritePalette = _tln.TLN_SetSpritePalette
SetSpritePalette.argtypes = [c_int, c_void_p]
SetSpritePalette.restype = c_bool

SetSpriteBlendMode = _tln.TLN_SetSpriteBlendMode
SetSpriteBlendMode.argtypes = [c_int, c_int, c_ubyte]
SetSpriteBlendMode.restype = c_bool

SetSpriteScaling = _tln.TLN_SetSpriteScaling
SetSpriteScaling.argtypes = [c_int, c_float, c_float]
SetSpriteScaling.restype = c_bool

ResetSpriteScaling = _tln.TLN_ResetSpriteScaling
ResetSpriteScaling.argtypes = [c_int]
ResetSpriteScaling.restype = c_bool

GetSpritePicture = _tln.TLN_GetSpritePicture
GetSpritePicture.argtypes = [c_int]
GetSpritePicture.restype = c_int

GetAvailableSprite = _tln.TLN_GetAvailableSprite
GetAvailableSprite.restype = c_int

EnableSpriteCollision = _tln.TLN_EnableSpriteCollision
EnableSpriteCollision.argtypes = [c_int, c_bool]
EnableSpriteCollision.restype = c_bool

GetSpriteCollision = _tln.TLN_GetSpriteCollision
GetSpriteCollision.argtypes = [c_int]
GetSpriteCollision.restype = c_bool
	
DisableSprite = _tln.TLN_DisableSprite
DisableSprite.argtypes = [c_int]
DisableSprite.restype = c_bool

GetSpritePalette = _tln.TLN_GetSpritePalette
GetSpritePalette.argtypes = [c_int]
GetSpritePalette.restype = c_void_p

# sequences management --------------------------------------------------------
CreateSequence = _tln.TLN_CreateSequence
CreateSequence.argtypes = [c_char_p, c_int, c_int, c_int, POINTER(c_int)]
CreateSequence.restype = c_void_p

CreateCycle = _tln.TLN_CreateCycle
CreateCycle.argtypes = [c_char_p, c_int, POINTER(ColorStrip)]
CreateCycle.restype = c_void_p

CloneSequence = _tln.TLN_CloneSequence
CloneSequence.argtypes = [c_void_p]
CloneSequence.restype = c_void_p

DeleteSequence = _tln.TLN_DeleteSequence
DeleteSequence.argtypes = [c_void_p]
DeleteSequence.restype = c_bool

# sequence pack management --------------------------------------------------------
CreateSequencePack = _tln.TLN_CreateSequencePack
CreateSequencePack.argtypes = []
CreateSequencePack.restype = c_void_p

LoadSequencePack = _tln.TLN_LoadSequencePack
LoadSequencePack.argtypes = [c_char_p]
LoadSequencePack.restype = c_void_p

FindSequence = _tln.TLN_FindSequence
FindSequence.argtypes = [c_void_p, c_char_p]
FindSequence.restype = c_void_p

AddSequenceToPack = _tln.TLN_AddSequenceToPack
AddSequenceToPack.argtypes = [c_void_p, c_void_p]
AddSequenceToPack.restype = c_bool

DeleteSequencePack = _tln.TLN_DeleteSequencePack
DeleteSequencePack.argtypes = [c_void_p]
DeleteSequencePack.restype = c_bool

# animation engine ------------------------------------------------------------
SetPaletteAnimation = _tln.TLN_SetPaletteAnimation
SetPaletteAnimation.argtypes = [c_int, c_void_p, c_void_p, c_bool]
SetPaletteAnimation.restype = c_bool

SetPaletteAnimationSource = _tln.TLN_SetPaletteAnimationSource
SetPaletteAnimationSource.argtypes = [c_int, c_void_p]
SetPaletteAnimationSource = c_bool

SetTilesetAnimation = _tln.TLN_SetTilesetAnimation
SetTilesetAnimation.argtypes = [c_int, c_int, c_void_p]
SetTilesetAnimation.restype = c_bool

SetTilemapAnimation = _tln.TLN_SetTilemapAnimation
SetTilemapAnimation.argtypes = [c_int, c_int, c_void_p]
SetTilemapAnimation.restype = c_bool

SetSpriteAnimation = _tln.TLN_SetSpriteAnimation
SetSpriteAnimation.argtypes = [c_int, c_int, c_void_p, c_int]
SetSpriteAnimation.restype = c_bool

GetAnimationState = _tln.TLN_GetAnimationState
GetAnimationState.argtypes = [c_int]
GetAnimationState.restype = c_bool

SetAnimationDelay = _tln.TLN_SetAnimationDelay
SetAnimationDelay.argtypes = [c_int, c_int]
SetAnimationDelay.restype = c_bool

GetAvailableAnimation = _tln.TLN_GetAvailableAnimation
GetAvailableAnimation.restype = c_int

DisableAnimation = _tln.TLN_DisableAnimation
DisableAnimation.argtypes = [c_int]
DisableAnimation.restype = c_bool
