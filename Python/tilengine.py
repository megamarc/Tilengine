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
TLN_ERR_OK 				= 0	 # No error 
TLN_ERR_OUT_OF_MEMORY 	= 1	 # Not enough memory 
TLN_ERR_IDX_LAYER 		= 2	 # Layer index out of range 
TLN_ERR_IDX_SPRITE 		= 3	 # Sprite index out of range 
TLN_ERR_IDX_ANIMATION 	= 4	 # Animation index out of range 
TLN_ERR_IDX_PICTURE 	= 5	 # Picture or tile index out of range 
TLN_ERR_REF_TILESET 	= 6	 # Invalid TLN_Tileset reference 
TLN_ERR_REF_TILEMAP 	= 7	 # Invalid TLN_Tilemap reference 
TLN_ERR_REF_SPRITESET 	= 8	 # Invalid TLN_Spriteset reference 
TLN_ERR_REF_PALETTE 	= 9	 # Invalid TLN_Palette reference 
TLN_ERR_REF_SEQUENCE 	= 10 # Invalid TLN_SequencePack reference 
TLN_ERR_REF_SEQPACK 	= 11 # Invalid TLN_Sequence reference 
TLN_ERR_REF_BITMAP 		= 12 # Invalid TLN_Bitmap reference 
TLN_ERR_NULL_POINTER 	= 13 # Null pointer as argument  
TLN_ERR_FILE_NOT_FOUND 	= 14 # Resource file not found 
TLN_ERR_WRONG_FORMAT 	= 15 # Resource file has invalid format 
TLN_ERR_WRONG_SIZE 		= 16 # A width or height parameter is invalid 
TLN_ERR_UNSUPPORTED		= 17 # Unsupported function

# blend modes
BLEND_NONE		= 0
BLEND_MIX		= 1
BLEND_ADD		= 2
BLEND_SUB		= 3
MAX_BLEND		= 4

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
	_fields = [
		("angle", c_float),
		("dx", c_float),
		("dy", c_float),
		("sx", c_float),
		("sy", c_float)
	]
	
class Tile(Structure):
	_fields = [
		("index", c_ushort),
		("flags", c_ushort)
	]

class SpriteInfo(Structure):
	_fields = [
		("offset", c_int),
		("w", c_int),
		("h", c_int)
	]

class TileInfo(Structure):
	_fields = [
		("index", c_ushort),
		("flags", c_ushort),
		("xoffset", c_int),
		("yoffset", c_int)
	]
	
class Rect(Structure):
	_fields = [
		("x", c_int),
		("y", c_int),
		("w", c_int),
		("h", c_int)
	]

from sys import platform as _platform
if _platform == "linux" or _platform == "linux2":
	tln = cdll.LoadLibrary("libTilengine.so")
elif _platform == "win32":
	tln = cdll.LoadLibrary("Tilengine.dll")

# basic management ------------------------------------------------------------
Init = tln.TLN_Init
Init.argtypes = [c_int, c_int, c_int, c_int, c_int]
Init.restype = c_bool
		
Deinit = tln.TLN_Deinit
Deinit.restype = None
	
GetNumObjects = tln.TLN_GetNumObjects
GetNumObjects.restype = c_int
	
GetVersion = tln.TLN_GetVersion
GetVersion.restype = c_int

GetUsedMemory = tln.TLN_GetUsedMemory
GetUsedMemory.restype = c_int
	
GetNumLayers = tln.TLN_GetNumLayers
GetNumLayers.restype = c_int

GetNumSprites = tln.TLN_GetNumSprites
GetNumSprites.restype = c_int

SetBGColor = tln.TLN_SetBGColor
SetBGColor.argtypes = [c_int, c_int, c_int]
SetBGColor.restype = None

SetBGBitmap = tln.TLN_SetBGBitmap
SetBGBitmap.argtypes = [c_void_p]
SetBGBitmap.restype = c_bool

SetBGPalette = tln.TLN_SetBGPalette
SetBGPalette.argtypes = [c_void_p]
SetBGPalette.restype = c_bool

SetRasterCallback = tln.TLN_SetRasterCallback
SetRasterCallback.argtypes = [c_void_p]
SetRasterCallback.restype = None

SetRenderTarget = tln.TLN_SetRenderTarget
SetRenderTarget.argtypes = [c_void_p, c_int]
SetRenderTarget.restype = None

UpdateFrame = tln.TLN_UpdateFrame
UpdateFrame.argtypes = [c_int]
UpdateFrame.restype = None

# window management -----------------------------------------------------------
CreateWindow = tln.TLN_CreateWindow
CreateWindow.argtypes = [c_char_p, c_int]
CreateWindow.restype = c_bool

CreateWindowThread = tln.TLN_CreateWindowThread
CreateWindowThread.argtypes = [c_char_p, c_int]
CreateWindowThread.restype = c_bool

ProcessWindow = tln.TLN_ProcessWindow
ProcessWindow.restype = c_bool

IsWindowActive = tln.TLN_IsWindowActive
IsWindowActive.restype = c_bool

GetInput = tln.TLN_GetInput
GetInput.argtypes = [c_int]
GetInput.restype = c_bool

DrawFrame = tln.TLN_DrawFrame 
DrawFrame.argtypes = [c_int]
DrawFrame.restype = None

WaitRedraw = tln.TLN_WaitRedraw
WaitRedraw.restype = None

DeleteWindow = tln.TLN_DeleteWindow
DeleteWindow.restype = None

EnableBlur = tln.TLN_EnableBlur
EnableBlur.argtypes = [c_bool]
EnableBlur.restype = None

GetTicks = tln.TLN_GetTicks
GetTicks.restype = c_int

# spritesets management
CreateSpriteset = tln.TLN_CreateSpriteset
CreateSpriteset.argtypes = [c_int, POINTER(Rect), c_void_p, c_int, c_int, c_int, c_void_p]
CreateSpriteset.restype = c_void_p

LoadSpriteset = tln.TLN_LoadSpriteset
LoadSpriteset.argtypes = [c_char_p]
LoadSpriteset.restype = c_void_p

CloneSpriteset = tln.TLN_CloneSpriteset
CloneSpriteset.argtypes = [c_void_p]
CloneSpriteset.restype = c_void_p

GetSpriteInfo = tln.TLN_GetSpriteInfo
GetSpriteInfo.argtypes = [c_void_p, c_int, c_void_p]
GetSpriteInfo.restype = c_bool
	
GetSpritesetPalette = tln.TLN_GetSpritesetPalette
GetSpritesetPalette.argtypes = [c_void_p]
GetSpritesetPalette.restype = c_void_p
	
DeleteSpriteset = tln.TLN_DeleteSpriteset
DeleteSpriteset.argtypes = [c_void_p]
DeleteSpriteset.restype = c_bool

# tilesets management ---------------------------------------------------------
CreateTileset = tln.TLN_CreateTileset
CreateTileset.argtypes = [c_int, c_int, c_int, c_void_p]
CreateTileset.restype = c_void_p

LoadTileset = tln.TLN_LoadTileset
LoadTileset.argtypes = [c_char_p]
LoadTileset.restype = c_void_p

CloneTileset = tln.TLN_CloneTileset
CloneTileset.argtypes = [c_void_p]
CloneTileset.restype = c_void_p

SetTilesetPixels = tln.TLN_SetTilesetPixels
SetTilesetPixels.argtypes = [c_void_p, c_int, c_void_p, c_int]
SetTilesetPixels.restype = c_bool

GetTileWidth = tln.TLN_GetTileWidth
GetTileWidth.argtypes = [c_void_p]
GetTileWidth.restype = c_int

GetTileHeight = tln.TLN_GetTileWidth
GetTileHeight.argtypes = [c_void_p]
GetTileHeight.restype = c_int

GetTilesetPalette = tln.TLN_GetTilesetPalette
GetTilesetPalette.argtypes = [c_void_p]
GetTilesetPalette.restype = c_void_p

DeleteTileset = tln.TLN_DeleteTileset
DeleteTileset.argtypes = [c_void_p]
DeleteTileset.restype = c_bool

# tilemaps management ---------------------------------------------------------
CreateTilemap = tln.TLN_CreateTilemap
CreateTilemap.argtypes = [c_int, c_int, POINTER(Tile)]
CreateTilemap.restype = c_void_p

LoadTilemap = tln.TLN_LoadTilemap
LoadTilemap.argtypes = [c_char_p]
LoadTilemap.restype = c_void_p

CloneTilemap = tln.TLN_CloneTilemap
CloneTilemap.argtypes = [c_void_p]
CloneTilemap.restype = c_void_p

GetTilemapRows = tln.TLN_GetTilemapRows
GetTilemapRows.argtypes = [c_void_p]
GetTilemapRows.restype = c_int

GetTilemapCols = tln.TLN_GetTilemapRows
GetTilemapCols.argtypes = [c_void_p]
GetTilemapCols.restype = c_int

GetTilemapTile = tln.TLN_GetTilemapTile
GetTilemapTile.argtypes = [c_void_p, c_int, c_int, POINTER(Tile)]
GetTilemapTile.restype = c_bool

SetTilemapTile = tln.TLN_SetTilemapTile
SetTilemapTile.argtypes = [c_void_p, c_int, c_int, POINTER(Tile)]
SetTilemapTile.restype = c_bool

CopyTiles = tln.TLN_CopyTiles
CopyTiles.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_void_p, c_int, c_int]
CopyTiles.restype = c_bool

DeleteTilemap = tln.TLN_DeleteTilemap
DeleteTilemap.argtypes = [c_void_p]
DeleteTilemap.restype = c_bool

# color tables management -----------------------------------------------------
CreatePalette = tln.TLN_CreatePalette
CreatePalette.argtypes = [c_int]
CreatePalette.restype = c_void_p

LoadPalette = tln.TLN_LoadPalette
LoadPalette.argtypes = [c_char_p]
LoadPalette.restype = c_void_p

ClonePalette = tln.TLN_ClonePalette
ClonePalette.argtypes = [c_void_p]
ClonePalette.restype = c_void_p

DeletePalette = tln.TLN_DeletePalette
DeletePalette.argtypes = [c_void_p]
DeletePalette.restype = c_bool

SetPaletteColor = tln.TLN_SetPaletteColor
SetPaletteColor.argtypes = [c_void_p, c_int, c_ubyte, c_ubyte, c_ubyte]
SetPaletteColor.restype = c_bool

MixPalettes = tln.TLN_MixPalettes
MixPalettes.argtypes = [c_void_p, c_void_p, c_void_p, c_ubyte]
MixPalettes.restype = c_bool

GetPaletteData = tln.TLN_GetPaletteData
GetPaletteData.argtypes = [c_void_p, c_int]
GetPaletteData.restype = POINTER(c_int)

# bitmaps ---------------------------------------------------------------------
CreateBitmap = tln.TLN_CreateBitmap
CreateBitmap.argtypes = [c_int, c_int, c_int]
CreateBitmap.restype = c_void_p

LoadBitmap = tln.TLN_LoadBitmap
LoadBitmap.argtypes = [c_char_p]
LoadBitmap.restype = c_void_p

CloneBitmap = tln.TLN_CloneBitmap
CloneBitmap.argtypes = [c_void_p]
CloneBitmap.restype = c_void_p

GetBitmapPtr = tln.TLN_GetBitmapPtr
GetBitmapPtr.argtypes = [c_void_p, c_int, c_int]
GetBitmapPtr.restype = POINTER(c_ubyte)

GetBitmapWidth = tln.TLN_GetBitmapWidth
GetBitmapWidth.argtypes = [c_void_p]
GetBitmapWidth.restype = c_int

GetBitmapHeight = tln.TLN_GetBitmapHeight
GetBitmapHeight.argtypes = [c_void_p]
GetBitmapHeight.restype = c_int

GetBitmapDepth = tln.TLN_GetBitmapDepth
GetBitmapDepth.argtypes = [c_void_p]
GetBitmapDepth.restype = c_int

GetBitmapPitch = tln.TLN_GetBitmapPitch
GetBitmapPitch.argtypes = [c_void_p]
GetBitmapPitch.restype = c_int

GetBitmapPalette = tln.TLN_GetBitmapPalette
GetBitmapPalette.argtypes = [c_void_p]
GetBitmapPalette.restype = c_void_p

DeleteBitmap = tln.TLN_DeleteBitmap
DeleteBitmap.argtypes = [c_void_p]
DeleteBitmap.restype = c_bool

# layer management ------------------------------------------------------------
SetLayer = tln.TLN_SetLayer
SetLayer.argtypes = [c_int, c_void_p, c_void_p]
SetLayer.restype = c_bool
	
SetLayerPalette = tln.TLN_SetLayerPalette
SetLayerPalette.argtypes = [c_int, c_void_p]
SetLayerPalette.restype = c_bool

SetLayerPosition = tln.TLN_SetLayerPosition
SetLayerPosition.argtypes = [c_int, c_int, c_int]
SetLayerPosition.restype = c_bool

SetLayerScaling = tln.TLN_SetLayerScaling
SetLayerScaling.argtypes = [c_int, c_float, c_float]
SetLayerScaling.restype = c_bool

SetLayerAffineTransform = tln.TLN_SetLayerAffineTransform
SetLayerAffineTransform.argtypes = [c_int, POINTER(Affine)]
SetLayerAffineTransform.restype = c_bool

SetLayerTransform = tln.TLN_SetLayerTransform
SetLayerTransform.argtypes = [c_int, c_float, c_float, c_float, c_float, c_float]
SetLayerTransform.restype = c_bool

SetLayerBlendMode = tln.TLN_SetLayerBlendMode
SetLayerBlendMode.argtypes = [c_int, c_int, c_ubyte]
SetLayerBlendMode.restype = c_bool

SetLayerColumnOffset = tln.TLN_SetLayerColumnOffset
SetLayerColumnOffset.argtypes = [c_int, POINTER(c_int)]
SetLayerColumnOffset.restype = c_bool
	
ResetLayerMode = tln.TLN_ResetLayerMode
ResetLayerMode.argtypes = [c_int]
ResetLayerMode.restype = c_bool

DisableLayer = tln.TLN_DisableLayer
DisableLayer.argtypes = [c_int]
DisableLayer.restype = c_bool

GetLayerPalette = tln.TLN_GetLayerPalette
GetLayerPalette.argtypes = [c_int]
restype = c_void_p

GetLayerTile = tln.TLN_GetLayerTile
GetLayerTile.argtypes = [c_int, c_int, c_int, POINTER(TileInfo)]
GetLayerTile.restype = c_bool
	
# sprite management -----------------------------------------------------------
ConfigSprite = tln.TLN_ConfigSprite
ConfigSprite.argtypes = [c_int, c_int, c_ushort]
ConfigSprite.restype = c_bool

SetSpriteSet = tln.TLN_SetSpriteSet
SetSpriteSet.argtypes = [c_int, c_void_p]
SetSpriteSet.restype = c_bool

SetSpriteFlags = tln.TLN_SetSpriteFlags
SetSpriteFlags.argtypes = [c_int, c_ushort]
SetSpriteFlags.restype = c_bool
	
SetSpritePosition = tln.TLN_SetSpritePosition
SetSpritePosition.argtypes = [c_int, c_int, c_int]
SetSpritePosition.restype = c_bool
	
SetSpritePicture = tln.TLN_SetSpritePicture
SetSpritePicture.argtypes = [c_int, c_int]
SetSpritePicture.restype = c_bool

SetSpritePalette = tln.TLN_SetSpritePalette
SetSpritePalette.argtypes = [c_int, c_void_p]
SetSpritePalette.restype = c_bool

SetSpriteBlendMode = tln.TLN_SetSpriteBlendMode
SetSpriteBlendMode.argtypes = [c_int, c_int, c_ubyte]
SetSpriteBlendMode.restype = c_bool

SetSpriteScaling = tln.TLN_SetSpriteScaling
SetSpriteScaling.argtypes = [c_int, c_float, c_float]
SetSpriteScaling.restype = c_bool

ResetSpriteScaling = tln.TLN_ResetSpriteScaling
ResetSpriteScaling.argtypes = [c_int]
ResetSpriteScaling.restype = c_bool

GetSpritePicture = tln.TLN_GetSpritePicture
GetSpritePicture.argtypes = [c_int]
GetSpritePicture.restype = c_int

GetAvailableSprite = tln.TLN_GetAvailableSprite
GetAvailableSprite.restype = c_int
	
DisableSprite = tln.TLN_DisableSprite
DisableSprite.argtypes = [c_int]
DisableSprite.restype = c_bool

GetSpritePalette = tln.TLN_GetSpritePalette
GetSpritePalette.argtypes = [c_int]
GetSpritePalette.restype = c_void_p

# sequences management --------------------------------------------------------
CreateSequence = tln.TLN_CreateSequence
CreateSequence.argtypes = [c_char_p, c_int, c_int, c_int, POINTER(c_int)]
CreateSequence.restype = c_void_p

CreateCycle = tln.TLN_CreateCycle
CreateCycle.argtypes = [c_char_p, c_int, POINTER(TileInfo)]
CreateCycle.restype = c_void_p

CloneSequence = tln.TLN_CloneSequence
CloneSequence.argtypes = [c_void_p]
CloneSequence.restype = c_void_p

DeleteSequence = tln.TLN_DeleteSequence
DeleteSequence.argtypes = [c_void_p]
DeleteSequence.restype = c_bool

# sequence pack management --------------------------------------------------------
CreateSequencePack = tln.TLN_CreateSequencePack
CreateSequencePack.argtypes = []
CreateSequencePack.restype = c_void_p

LoadSequencePack = tln.TLN_LoadSequencePack
LoadSequencePack.argtypes = [c_char_p]
LoadSequencePack.restype = c_void_p

FindSequence = tln.TLN_FindSequence
FindSequence.argtypes = [c_void_p, c_char_p]
FindSequence.restype = c_void_p

AddSequenceToPack = tln.TLN_AddSequenceToPack
AddSequenceToPack.argtypes = [c_void_p, c_void_p]
AddSequenceToPack.restype = c_bool

DeleteSequencePack = tln.TLN_DeleteSequencePack
DeleteSequencePack.argtypes = [c_void_p]
DeleteSequencePack.restype = c_bool

# animation engine ------------------------------------------------------------
SetPaletteAnimation = tln.TLN_SetPaletteAnimation
SetPaletteAnimation.argtypes = [c_int, c_void_p, c_void_p, c_bool]
SetPaletteAnimation.restype = c_bool

SetPaletteAnimationSource = tln.TLN_SetPaletteAnimationSource
SetPaletteAnimationSource.argtypes = [c_int, c_void_p]
SetPaletteAnimationSource = c_bool

SetTilemapAnimation = tln.TLN_SetTilemapAnimation
SetTilemapAnimation.argtypes = [c_int, c_int, c_void_p]
SetTilemapAnimation.restype = c_bool

SetSpriteAnimation = tln.TLN_SetSpriteAnimation
SetSpriteAnimation.argtypes = [c_int, c_int, c_void_p, c_int]
SetSpriteAnimation.restype = c_bool

GetAnimationState = tln.TLN_GetAnimationState
GetAnimationState.argtypes = [c_int]
GetAnimationState.restype = c_bool

SetAnimationDelay = tln.TLN_SetAnimationDelay
SetAnimationDelay.argtypes = [c_int, c_int]
SetAnimationDelay.restype = c_bool

GetAvailableAnimation = tln.TLN_GetAvailableAnimation
GetAvailableAnimation.restype = c_int

DisableAnimation = tln.TLN_DisableAnimation
DisableAnimation.argtypes = [c_int]
DisableAnimation.restype = c_bool
