'''
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
'''

"""
Python wrapper for Tilengine retro graphics engine
http://www.tilengine.org
"""
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
ERR_OK 				= 0	 	# No error 
ERR_OUT_OF_MEMORY 	= 1	 	# Not enough memory 
ERR_IDX_LAYER 		= 2	 	# Layer index out of range 
ERR_IDX_SPRITE 		= 3	 	# Sprite index out of range 
ERR_IDX_ANIMATION 	= 4	 	# Animation index out of range 
ERR_IDX_PICTURE 	= 5	 	# Picture or tile index out of range 
ERR_REF_TILESET 	= 6	 	# Invalid Tileset reference 
ERR_REF_TILEMAP 	= 7	 	# Invalid Tilemap reference 
ERR_REF_SPRITESET 	= 8	 	# Invalid Spriteset reference 
ERR_REF_PALETTE 	= 9	 	# Invalid Palette reference 
ERR_REF_SEQUENCE 	= 10 	# Invalid SequencePack reference 
ERR_REF_SEQPACK 	= 11 	# Invalid Sequence reference 
ERR_REF_BITMAP 		= 12 	# Invalid Bitmap reference 
ERR_NULL_POINTER 	= 13 	# Null pointer as argument  
ERR_FILE_NOT_FOUND 	= 14 	# Resource file not found 
ERR_WRONG_FORMAT 	= 15 	# Resource file has invalid format 
ERR_WRONG_SIZE 		= 16 	# A width or height parameter is invalid 
ERR_UNSUPPORTED		= 17 	# Unsupported function

# blend modes
BLEND_NONE		= 0
BLEND_MIX25		= 1
BLEND_MIX50		= 2
BLEND_MIX75		= 3
BLEND_ADD		= 4
BLEND_SUB		= 5
BLEND_MOD		= 6
BLEND_CUSTOM	= 7
MAX_BLEND		= 8
BLEND_MIX		= BLEND_MIX50

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

# CRT overlay patterns
OVERLAY_NONE		= 0
OVERLAY_SHADOWMASK	= 1
OVERLAY_APERTURE	= 2
OVERLAY_CUSTOM		= 3

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
	
class PixelMap(Structure):
	_fields_ = [
		("dx", c_short),
		("dy", c_short)
	]
	
# load native library
from sys import platform as _platform
if _platform == "linux" or _platform == "linux2":
	_tln = cdll.LoadLibrary("libTilengine.so")
elif _platform == "win32":
	_tln = cdll.LoadLibrary("Tilengine.dll")
elif _platform == "darwin":
	_tln = cdll.LoadLibrary("Tilengine.dylib")
	
# callback types for user functions
RasterCallbackFunc = CFUNCTYPE(None, c_int)
CustomBlendFunc = CFUNCTYPE(c_ubyte, c_ubyte, c_ubyte)

# return c_char_p compatible string
def _encode_string(string):
	if string != None:
		return string.encode()
	else:
		return None
		
# global arrays holding references to indexed entities
layers 		= None
sprites 	= None
animations  = None

# basic management ------------------------------------------------------------
_tln.TLN_Init.argtypes = [c_int, c_int, c_int, c_int, c_int]
_tln.TLN_Init.restype = c_bool
_tln.TLN_GetNumObjects.restype = c_int
_tln.TLN_GetVersion.restype = c_int
_tln.TLN_GetUsedMemory.restype = c_int
_tln.TLN_GetNumLayers.restype = c_int
_tln.TLN_GetNumSprites.restype = c_int
_tln.TLN_SetBGColor.argtypes = [c_ubyte, c_ubyte, c_ubyte]
_tln.TLN_SetBGBitmap.argtypes = [c_void_p]
_tln.TLN_SetBGBitmap.restype = c_bool
_tln.TLN_SetBGPalette.argtypes = [c_void_p]
_tln.TLN_SetBGPalette.restype = c_bool
_tln.TLN_SetRenderTarget.argtypes = [c_void_p, c_int]
_tln.TLN_UpdateFrame.argtypes = [c_int]
_tln.TLN_BeginFrame.argtypes = [c_int]
_tln.TLN_DrawNextScanline.restype = c_bool
_tln.TLN_SetLoadPath.argtypes = [c_char_p]

def Init(width, height, num_layers, num_sprites, num_animations):
	ok = _tln.TLN_Init(width, height, num_layers, num_sprites, num_animations)
	if ok == True:
		global layers
		layers = [Layer(n) for n in range(num_layers)]
		global sprites
		sprites = [Sprite(n) for n in range(num_sprites)]
		global animations
		animations = [Animation(n) for n in range(num_animations)]
	return ok
		
def Deinit():
	_tln.TLN_Deinit()
	
def GetNumObjects():
	return _tln.TLN_GetNumObjects()
	
def GetVersion():
	return _tln.TLN_GetVersion()

def GetUsedMemory():
	return _tln.TLN_GetUsedMemory()
	
def GetNumLayers():
	return _tln.TLN_GetNumLayers()

def GetNumSprites():
	return _tln.TLN_GetNumSprites()

def SetBGColor(r, g, b):
	_tln.TLN_SetBGColor(r, g, b)

def SetBGBitmap():
	return _tln.TLN_SetBGBitmap(bitmap)

def SetBGPalette(palette):
	return _tln.TLN_SetBGPalette(palette)

def SetRasterCallback(raster_callback):
	_tln.TLN_SetRasterCallback(raster_callback)

def SetRenderTarget(pixels, pitch):
	_tln.TLN_SetRenderTarget(pixels, pitch)

def UpdateFrame(num_frame=0):
	_tln.TLN_UpdateFrame(num_frame)

def BeginFrame(num_frame=0):
	_tln.TLN_BeginFrame(num_frame)

def DrawNextScanline():
	return _tln.TLN_DrawNextScanline()

def SetLoadPath(path):
	_tln.TLN_SetLoadPath(_encode_string(path))

def SetCustomBlendFunction(blend_function):
	_tln.TLN_SetCustomBlendFunction(blend_function)
	
# error handling --------------------------------------------------------------
_tln.TLN_GetLastError.restype = c_int
_tln.TLN_GetErrorString.argtypes = [c_int]
_tln.TLN_GetErrorString.restype = c_char_p

def GetLastError():
	return _tln.TLN_GetLastError()

def GetErrorString(num_error):
	return _tln.TLN_GetErrorString(num_error).decode()

# window management -----------------------------------------------------------
_tln.TLN_CreateWindow.argtypes = [c_char_p, c_int]
_tln.TLN_CreateWindow.restype = c_bool
_tln.TLN_CreateWindowThread.argtypes = [c_char_p, c_int]
_tln.TLN_CreateWindowThread.restype = c_bool
_tln.TLN_ProcessWindow.restype = c_bool
_tln.TLN_IsWindowActive.restype = c_bool
_tln.TLN_GetInput.argtypes = [c_int]
_tln.TLN_GetInput.restype = c_bool
_tln.TLN_DrawFrame.argtypes = [c_int]
_tln.TLN_EnableCRTEffect.argtypes = [c_int, c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_bool, c_ubyte]
_tln.TLN_GetTicks.restype = c_int
_tln.TLN_Delay.argtypes = [c_int]
_tln.TLN_BeginWindowFrame.argtypes = [c_int]

def CreateWindow(overlay, flags):
	ok = _tln.TLN_CreateWindow(_encode_string(overlay), flags)
	if ok == True:
		return Window()
	else:
		return None

def CreateWindowThread(overlay, flags):
	ok = _tln.TLN_CreateWindowThread(_encode_string(overlay), flags)
	if ok == True:
		return Window()
	else:
		return None

class Window:
	def Process(self):
		return _tln.TLN_ProcessWindow()

	def IsActive(self):
		return _tln.TLN_IsWindowActive()
		
	def GetInput(self, input_id):
		return _tln.TLN_GetInput(input_id)

	def DrawFrame(self, num_frame=0):
		_tln.TLN_DrawFrame (num_frame)

	def WaitRedraw(self):
		_tln.TLN_WaitRedraw()

	def EnableCRTEffect(self, overlay_id, overlay_blend, threshold, v0, v1, v2, v3, blur, glow_factor):
		_tln.TLN_EnableCRTEffect(overlay_id, overlay_blend, threshold, v0, v1, v2, v3, blur, glow_factor)

	def DisableCRTEffect(self):
		_tln.TLN_DisableCRTEffect()

	def GetTicks(self):
		return _tln.TLN_GetTicks()

	def Delay(self, msecs):
		_tln.TLN_Delay(msecs)

	def BeginFrame(self, num_frame):
		_tln.TLN_BeginWindowFrame(num_frame)

	def EndFrame(self):
		_tln.TLN_EndWindowFrame()
	
# spritesets management -----------------------------------------------------------
_tln.TLN_CreateSpriteset.argtypes = [c_int, POINTER(Rect), c_void_p, c_int, c_int, c_int, c_void_p]
_tln.TLN_CreateSpriteset.restype = c_void_p
_tln.TLN_LoadSpriteset.argtypes = [c_char_p]
_tln.TLN_LoadSpriteset.restype = c_void_p
_tln.TLN_CloneSpriteset.argtypes = [c_void_p]
_tln.TLN_CloneSpriteset.restype = c_void_p
_tln.TLN_GetSpriteInfo.argtypes = [c_void_p, c_int, c_void_p]
_tln.TLN_GetSpriteInfo.restype = c_bool
_tln.TLN_GetSpritesetPalette.argtypes = [c_void_p]
_tln.TLN_GetSpritesetPalette.restype = c_void_p
_tln.TLN_DeleteSpriteset.argtypes = [c_void_p]
_tln.TLN_DeleteSpriteset.restype = c_bool

def CreateSpriteset(entries, rects, data, width, height, pitch, palette):
	handle = _tln.TLN_CreateSpriteset(entries, rects, data, width, height, pitch, palette)
	if handle != None:
		return Spriteset(handle)
	else:
		return None

def LoadSpriteset(filename):
	handle = _tln.TLN_LoadSpriteset(_encode_string(filename))
	if handle != None:
		return Spriteset(handle)
	else:
		return None
		
class Spriteset:
	def __init__(self, handle):
		self._as_parameter_ = handle
		self.palette = Palette(_tln.TLN_GetSpritesetPalette(handle))
	
	def Clone(self):
		handle = _tln.TLN_CloneSpriteset(self)
		if handle != None:
			return Spriteset(handle)
		else:
			return None

	def GetSpriteInfo(self, entry, info):
		return _tln.TLN_GetSpriteInfo(self, entry, info) 
		
	def Delete(self):
		return _tln.TLN_DeleteSpriteset(self)

# tilesets management ---------------------------------------------------------
_tln.TLN_CreateTileset.argtypes = [c_int, c_int, c_int, c_void_p]
_tln.TLN_CreateTileset.restype = c_void_p
_tln.TLN_LoadTileset.argtypes = [c_char_p]
_tln.TLN_LoadTileset.restype = c_void_p
_tln.TLN_CloneTileset.argtypes = [c_void_p]
_tln.TLN_CloneTileset.restype = c_void_p
_tln.TLN_SetTilesetPixels.argtypes = [c_void_p, c_int, c_void_p, c_int]
_tln.TLN_SetTilesetPixels.restype = c_bool
_tln.TLN_CopyTile.argtypes = [c_void_p, c_int, c_int]
_tln.TLN_CopyTile.restype = c_bool
_tln.TLN_GetTileWidth.argtypes = [c_void_p]
_tln.TLN_GetTileWidth.restype = c_int
_tln.TLN_GetTileHeight.argtypes = [c_void_p]
_tln.TLN_GetTileHeight.restype = c_int
_tln.TLN_GetTilesetPalette.argtypes = [c_void_p]
_tln.TLN_GetTilesetPalette.restype = c_void_p
_tln.TLN_DeleteTileset.argtypes = [c_void_p]
_tln.TLN_DeleteTileset.restype = c_bool

def CreateTileset(num_tiles, width, height, palette):
	handle = _tln.TLN_CreateTileset(num_tiles, width, height, palette)
	if handle != None:
		return Tileset(handle)
	else:
		return None

def LoadTileset(filename):
	handle = _tln.TLN_LoadTileset(_encode_string(filename))
	if handle != None:
		return Tileset(handle)
	else:
		return None
		
class Tileset:
	def __init__(self, handle):
		self._as_parameter_ = handle
		self.palette = Palette(_tln.TLN_GetTilesetPalette(handle))
		self.tile_width = _tln.TLN_GetTileWidth(handle)
		self.tile_height = _tln.TLN_GetTileHeight(handle)	

	def Clone(self):
		handle = _tln.TLN_CloneTileset(self)
		if handle != None:
			return Tileset(handle)
		else:
			return None

	def SetPixels(self, entry, data, pitch):
		return _tln.TLN_SetTilesetPixels(self, entry, data, pitch)

	def CopyTile(self, source, destination):
		return _tln.TLN_CopyTile(self, source, destination)

	def Delete(self):
		return _tln.TLN_DeleteTileset(self)

# tilemaps management ---------------------------------------------------------
_tln.TLN_CreateTilemap.argtypes = [c_int, c_int, POINTER(Tile)]
_tln.TLN_CreateTilemap.restype = c_void_p
_tln.TLN_LoadTilemap.argtypes = [c_char_p]
_tln.TLN_LoadTilemap.restype = c_void_p
_tln.TLN_CloneTilemap.argtypes = [c_void_p]
_tln.TLN_CloneTilemap.restype = c_void_p
_tln.TLN_GetTilemapRows.argtypes = [c_void_p]
_tln.TLN_GetTilemapRows.restype = c_int
_tln.TLN_GetTilemapRows.argtypes = [c_void_p]
_tln.TLN_GetTilemapRows.restype = c_int
_tln.TLN_GetTilemapTile.argtypes = [c_void_p, c_int, c_int, POINTER(Tile)]
_tln.TLN_GetTilemapTile.restype = c_bool
_tln.TLN_SetTilemapTile.argtypes = [c_void_p, c_int, c_int, POINTER(Tile)]
_tln.TLN_SetTilemapTile.restype = c_bool
_tln.TLN_CopyTiles.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_void_p, c_int, c_int]
_tln.TLN_CopyTiles.restype = c_bool
_tln.TLN_DeleteTilemap.argtypes = [c_void_p]
_tln.TLN_DeleteTilemap.restype = c_bool

def CreateTilemap(rows, cols, tiles):
	handle = _tln.TLN_CreateTilemap(rows, cols, tiles)
	if handle != None:
		return Tilemap(handle)
	else:
		return None		

def LoadTilemap(filename, layer_name=None):
	handle = _tln.TLN_LoadTilemap(_encode_string(filename), _encode_string(layer_name))
	if handle != None:
		return Tilemap(handle)
	else:
		return None		

class Tilemap:
	def __init__(self, handle):
		self._as_parameter_ = handle
		self.rows = _tln.TLN_GetTilemapRows(handle)
		self.cols = _tln.TLN_GetTilemapCols(handle)

	def Clone(self):
		handle = _tln.TLN_CloneTilemap(self)
		if handle != None:
			return Tilemap(handle)
		else:
			return None		

	def GetTile(self, entry, tile_info):
		return _tln.TLN_GetTilemapTile(self, entry, tile_info)

	def SetTile(self, entry, tile_info):
		return _tln.TLN_SetTilemapTile(self, entry, tile_info)
		
	def CopyTiles(self, src_row, sr_ccol, num_rows, num_cols, dst_tilemap, dst_row, dst_col):
		return _tln.TLN_CopyTiles(self, src_row, sr_ccol, num_rows, num_cols, dst_tilemap, dst_row, dst_col)

	def Delete(self):
		return _tln.TLN_DeleteTilemap(self)
	
# color tables management -----------------------------------------------------
_tln.TLN_CreatePalette.argtypes = [c_int]
_tln.TLN_CreatePalette.restype = c_void_p
_tln.TLN_LoadPalette.argtypes = [c_char_p]
_tln.TLN_LoadPalette.restype = c_void_p
_tln.TLN_ClonePalette.argtypes = [c_void_p]
_tln.TLN_ClonePalette.restype = c_void_p
_tln.TLN_SetPaletteColor.argtypes = [c_void_p, c_int, c_ubyte, c_ubyte, c_ubyte]
_tln.TLN_SetPaletteColor.restype = c_bool
_tln.TLN_MixPalettes.argtypes = [c_void_p, c_void_p, c_void_p, c_ubyte]
_tln.TLN_MixPalettes.restype = c_bool
_tln.TLN_AddPaletteColor.argtypes = [c_void_p, c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_ubyte]
_tln.TLN_AddPaletteColor.restype = c_bool
_tln.TLN_SubPaletteColor.argtypes = [c_void_p, c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_ubyte]
_tln.TLN_SubPaletteColor.restype = c_bool
_tln.TLN_ModPaletteColor.argtypes = [c_void_p, c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_ubyte]
_tln.TLN_ModPaletteColor.restype = c_bool
_tln.TLN_GetPaletteData.argtypes = [c_void_p, c_int]
_tln.TLN_GetPaletteData.restype = POINTER(c_ubyte)
_tln.TLN_DeletePalette.argtypes = [c_void_p]
_tln.TLN_DeletePalette.restype = c_bool

def CreatePalette(num_entries):
	handle = _tln.TLN_CreatePalette(num_entries)
	if handle != None:
		return Palette(handle)
	else:
		return None

def LoadPalette(filename):
	handle = _tln.TLN_LoadPalette(_encode_string(filename))
	if handle != None:
		return Palette(handle)
	else:
		return None
		
class Palette:
	def __init__(self, handle):
		self._as_parameter_ = handle
	
	def Clone(self):
		handle = _tln.TLN_ClonePalette(self)
		if handle != None:
			return Palette(handle)
		else:
			return None

	def SetColor(self, entry, r, g, b):
		return _tln.TLN_SetPaletteColor(self, entry, r, g, b)

	def Mix(self, src_palette1, src_palette2, factor):
		return _tln.TLN_MixPalettes(src_palette1, src_palette2, self, factor)

	def AddColor(self, first, count, r, g, b):
		return _tln.TLN_AddPaletteColor(self, first, count, r, g, b)

	def SubColor(self, first, count, r, g, b):
		return _tln.TLN_SubPaletteColor(self, first, count, r, g, b)

	def ModColor(self, first, count, r, g, b):
		return _tln.TLN_ModPaletteColor(self, first, count, r, g, b)
		
	def GetData(self, entry):
		return _tln.TLN_GetPaletteData(self, entry)

	def Delete(self):
		return _tln.TLN_DeletePalette(self)
	
# bitmaps ---------------------------------------------------------------------
_tln.TLN_CreateBitmap.argtypes = [c_int, c_int, c_int]
_tln.TLN_CreateBitmap.restype = c_void_p
_tln.TLN_LoadBitmap.argtypes = [c_char_p]
_tln.TLN_LoadBitmap.restype = c_void_p
_tln.TLN_CloneBitmap.argtypes = [c_void_p]
_tln.TLN_CloneBitmap.restype = c_void_p
_tln.TLN_GetBitmapPtr.argtypes = [c_void_p, c_int, c_int]
_tln.TLN_GetBitmapPtr.restype = POINTER(c_ubyte)
_tln.TLN_GetBitmapWidth.argtypes = [c_void_p]
_tln.TLN_GetBitmapWidth.restype = c_int
_tln.TLN_GetBitmapHeight.argtypes = [c_void_p]
_tln.TLN_GetBitmapHeight.restype = c_int
_tln.TLN_GetBitmapDepth.argtypes = [c_void_p]
_tln.TLN_GetBitmapDepth.restype = c_int
_tln.TLN_GetBitmapPitch.argtypes = [c_void_p]
_tln.TLN_GetBitmapPitch.restype = c_int
_tln.TLN_GetBitmapPalette.argtypes = [c_void_p]
_tln.TLN_GetBitmapPalette.restype = c_void_p
_tln.TLN_DeleteBitmap.argtypes = [c_void_p]
_tln.TLN_DeleteBitmap.restype = c_bool

def CreateBitmap(width, height, bpp):
	handle = _tln.TLN_CreateBitmap(width, height, bpp)
	if handle != None:
		return Bitmap(handle)
	else:
		return None

def LoadBitmap(filename):
	handle = _tln.TLN_LoadBitmap(_encode_string(filename))
	if handle != None:
		return Bitmap(handle)
	else:
		return None
	
class Bitmap:
	def __init__(self, handle):
		self._as_parameter_ = handle
		self.width   = _tln.TLN_GetBitmapWidth(handle)
		self.height  = _tln.TLN_GetBitmapHeight(handle)
		self.depth   = _tln.TLN_GetBitmapDepth(handle)
		self.pitch   = _tln.TLN_GetBitmapPitch(handle)
		self.palette = _tln.TLN_GetBitmapPalette(handle)

	def Clone(bitmap):
		handle = _tln.TLN_CloneBitmap(self)
		if handle != None:
			return Bitmap(handle)
		else:
			return None

	def GetData(self, x, y):
		return _tln.TLN_GetBitmapPtr(self, x, y)

	def Delete(self):
		return _tln.TLN_DeleteBitmap(self)

# sequences management --------------------------------------------------------
_tln.TLN_CreateSequence.argtypes = [c_char_p, c_int, c_int, c_int, POINTER(c_int)]
_tln.TLN_CreateSequence.restype = c_void_p
_tln.TLN_CreateCycle.argtypes = [c_char_p, c_int, POINTER(ColorStrip)]
_tln.TLN_CreateCycle.restype = c_void_p
_tln.TLN_CloneSequence.argtypes = [c_void_p]
_tln.TLN_CloneSequence.restype = c_void_p
_tln.TLN_DeleteSequence.argtypes = [c_void_p]
_tln.TLN_DeleteSequence.restype = c_bool

def CreateSequence(name, delay, first, num_frames, data):
	handle =  _tln.TLN_CreateSequence(_encode_string(name), delay, first, num_frames, data)
	if handle != None:
		return Sequence(handle)
	else:
		return None

def CreateCycle(name, num_strips, strips):
	handle = _tln.TLN_CreateCycle(_encode_string(name), num_strips, strips)
	if handle != None:
		return Sequence(handle)
	else:
		return None

class Sequence:
	def __init__(self, handle):
		self._as_parameter_ = handle

	def Clone(sequence):
		handle = _tln.TLN_CloneSequence(sequence)
		if handle != None:
			return Sequence(handle)
		else:
			return None

	def Delete(sequence):
		return _tln.TLN_DeleteSequence(sequence)

# sequence pack management --------------------------------------------------------
_tln.TLN_CreateSequencePack.restype = c_void_p
_tln.TLN_LoadSequencePack.argtypes = [c_char_p]
_tln.TLN_LoadSequencePack.restype = c_void_p
_tln.TLN_FindSequence.argtypes = [c_void_p, c_char_p]
_tln.TLN_FindSequence.restype = c_void_p
_tln.TLN_AddSequenceToPack.argtypes = [c_void_p, c_void_p]
_tln.TLN_AddSequenceToPack.restype = c_bool
_tln.TLN_DeleteSequencePack.argtypes = [c_void_p]
_tln.TLN_DeleteSequencePack.restype = c_bool

def CreateSequencePack():
	handle = _tln.TLN_CreateSequencePack()
	if handle != None:
		return SequencePack(handle)
	else:
		return None

def LoadSequencePack(filename):
	handle = _tln.TLN_LoadSequencePack(_encode_string(filename))
	if handle != None:
		return SequencePack(handle)
	else:
		return None
	
class SequencePack:

	def __init__(self, handle):
		self._as_parameter_ = handle

	def FindSequence(sequence_pack, name):
		handle = _tln.TLN_FindSequence(sequence_pack, _encode_string(name))
		if (handle != None):
			return Sequence(handle)
		else:
			return None			

	def AddSequence(sequence_pack, sequence):
		return _tln.TLN_AddSequenceToPack(sequence_pack, sequence)

	def Delete(sequence_pack):
		return _tln.TLN_DeleteSequencePack(sequence_pack)

# layer management ------------------------------------------------------------
_tln.TLN_SetLayer.argtypes = [c_int, c_void_p, c_void_p]
_tln.TLN_SetLayer.restype = c_bool
_tln.TLN_SetLayerPalette.argtypes = [c_int, c_void_p]
_tln.TLN_SetLayerPalette.restype = c_bool
_tln.TLN_SetLayerPosition.argtypes = [c_int, c_int, c_int]
_tln.TLN_SetLayerPosition.restype = c_bool
_tln.TLN_SetLayerScaling.argtypes = [c_int, c_float, c_float]
_tln.TLN_SetLayerScaling.restype = c_bool
_tln.TLN_SetLayerAffineTransform.argtypes = [c_int, POINTER(Affine)]
_tln.TLN_SetLayerAffineTransform.restype = c_bool
_tln.TLN_SetLayerTransform.argtypes = [c_int, c_float, c_float, c_float, c_float, c_float]
_tln.TLN_SetLayerTransform.restype = c_bool
_tln.TLN_SetLayerPixelMapping.argtypes = [c_int, POINTER(PixelMap)];
_tln.TLN_SetLayerPixelMapping.restype = c_bool
_tln.TLN_ResetLayerMode.argtypes = [c_int]
_tln.TLN_ResetLayerMode.restype = c_bool
_tln.TLN_SetLayerBlendMode.argtypes = [c_int, c_int, c_ubyte]
_tln.TLN_SetLayerBlendMode.restype = c_bool
_tln.TLN_SetLayerColumnOffset.argtypes = [c_int, POINTER(c_int)]
_tln.TLN_SetLayerColumnOffset.restype = c_bool
_tln.TLN_SetLayerClip.argtypes = [c_int, c_int, c_int, c_int, c_int]
_tln.TLN_SetLayerClip.restype = c_bool
_tln.TLN_DisableLayerClip.argtypes = [c_int]
_tln.TLN_DisableLayerClip.restype = c_bool
_tln.TLN_SetLayerMosaic.argtypes = [c_int, c_int, c_int]
_tln.TLN_SetLayerMosaic.restype = c_bool
_tln.TLN_DisableLayerMosaic.argtypes = [c_int]
_tln.TLN_DisableLayerMosaic.restype = c_bool
_tln.TLN_DisableLayer.argtypes = [c_int]
_tln.TLN_DisableLayer.restype = c_bool
_tln.TLN_GetLayerPalette.argtypes = [c_int]
_tln.TLN_GetLayerPalette.restype = c_void_p
_tln.TLN_GetLayerTile.argtypes = [c_int, c_int, c_int, POINTER(TileInfo)]
_tln.TLN_GetLayerTile.restype = c_bool

class Layer:
	def __init__(self, index):
		self._as_parameter_ = index

	def Setup(self, tileset, tilemap):
		return _tln.TLN_SetLayer(self, tileset, tilemap)
		
	def SetPalette(self, palette):
		return _tln.TLN_SetLayerPalette(self, palette)

	def SetPosition(self, x, y):
		return _tln.TLN_SetLayerPosition(self, int(x), int(y))

	def SetScaling(self, hor_factor, ver_factor):
		return _tln.TLN_SetLayerScaling(self, hor_factor, ver_factor)

	def SetTransform(self, angle, x, y, sx, sy):
		return _tln.TLN_SetLayerTransform(self, angle, x, y, sx, sy)

	def SetPixelMapping(self, pixel_map):
		return _tln.TLN_SetLayerPixelMapping(self, pixel_map)
		
	def ResetMode(self):
		return _tln.TLN_ResetLayerMode(self)

	def SetBlendMode(self, mode, factor=0):
		return _tln.TLN_SetLayerBlendMode(self, mode, 0)

	def SetColumnOffset(self, offsets):
		return _tln.TLN_SetLayerColumnOffset(self, offsets)

	def SetClip(self, x1, y1, x2, y2):
		return _tln.TLN_SetLayerClip(self, x1, y1, x2, y2)

	def DisableClip(self):
		return _tln.TLN_DisableLayerClip(self)

	def SetMosaic(self, pixel_w, pixel_h):
		return _tln.TLN_SetLayerMosaic(self, pixel_w, pixel_h)

	def DisableMosaic(self):
		return _tln.TLN_DisableLayerMosaic(self)
		
	def Disable(self):
		return _tln.TLN_DisableLayer(self)

	def GetPalette(self):
		return Palette(_tln.TLN_GetLayerPalette(self))
		
	def GetTile(self, x, y, tile_info):
		return _tln.TLN_GetLayerTile(self, x, y, tile_info)

def CreateTileInfoPtr(info):
	return POINTER(TileInfo)(info)
	
# sprite management -----------------------------------------------------------
_tln.TLN_ConfigSprite.argtypes = [c_int, c_void_p, c_ushort]
_tln.TLN_ConfigSprite.restype = c_bool
_tln.TLN_SetSpriteSet.argtypes = [c_int, c_void_p]
_tln.TLN_SetSpriteSet.restype = c_bool
_tln.TLN_SetSpriteFlags.argtypes = [c_int, c_ushort]
_tln.TLN_SetSpriteFlags.restype = c_bool
_tln.TLN_SetSpritePosition.argtypes = [c_int, c_int, c_int]
_tln.TLN_SetSpritePosition.restype = c_bool
_tln.TLN_SetSpritePicture.argtypes = [c_int, c_int]
_tln.TLN_SetSpritePicture.restype = c_bool
_tln.TLN_SetSpritePalette.argtypes = [c_int, c_void_p]
_tln.TLN_SetSpritePalette.restype = c_bool
_tln.TLN_SetSpriteBlendMode.argtypes = [c_int, c_int, c_ubyte]
_tln.TLN_SetSpriteBlendMode.restype = c_bool
_tln.TLN_SetSpriteScaling.argtypes = [c_int, c_float, c_float]
_tln.TLN_SetSpriteScaling.restype = c_bool
_tln.TLN_ResetSpriteScaling.argtypes = [c_int]
_tln.TLN_ResetSpriteScaling.restype = c_bool
_tln.TLN_GetSpritePicture.argtypes = [c_int]
_tln.TLN_GetSpritePicture.restype = c_int
_tln.TLN_GetAvailableSprite.restype = c_int
_tln.TLN_EnableSpriteCollision.argtypes = [c_int, c_bool]
_tln.TLN_EnableSpriteCollision.restype = c_bool
_tln.TLN_GetSpriteCollision.argtypes = [c_int]
_tln.TLN_GetSpriteCollision.restype = c_bool
_tln.TLN_DisableSprite.argtypes = [c_int]
_tln.TLN_DisableSprite.restype = c_bool
_tln.TLN_GetSpritePalette.argtypes = [c_int]
_tln.TLN_GetSpritePalette.restype = c_void_p

def GetAvailableSprite():
	return _tln.TLN_GetAvailableSprite()

class Sprite:
	def __init__(self, index):
		self._as_parameter_ = index

	def Setup(self, spriteset, flags):
		return _tln.TLN_ConfigSprite(self, spriteset, flags)

	def SetSpriteSet(self, spriteset):
		return _tln.TLN_SetSpriteSet(self, spriteset)

	def SetFlags(self, flags):
		return _tln.TLN_SetSpriteFlags(self, flags)
		
	def SetPosition(self, x, y):
		return _tln.TLN_SetSpritePosition(self, x, y)
		
	def SetPicture(self, picture):
		return _tln.TLN_SetSpritePicture(self, picture)

	def SetPalette(self, palette):
		return _tln.TLN_SetSpritePalette(self, palette)

	def SetBlendMode(self, mode):
		return _tln.TLN_SetSpriteBlendMode(self, mode)

	def SetScaling(self, sx, sy):
		return _tln.TLN_SetSpriteScaling(self, sx, sy)

	def ResetScaling(self):
		return _tln.TLN_ResetSpriteScaling(self)

	def GetPicture(self):
		return _tln.TLN_GetSpritePicture(self)

	def EnableCollision(self, mode):
		return _tln.TLN_EnableSpriteCollision(self, mode)
		
	def GetCollision(self):
		return _tln.TLN_GetSpriteCollision(self)
		
	def Disable(self):
		return _tln.TLN_DisableSprite(self)

	def GetPalette(self):
		return Palette(_tln.TLN_GetSpritePalette(self))

# animation engine ------------------------------------------------------------
_tln.TLN_SetPaletteAnimation.argtypes = [c_int, c_void_p, c_void_p, c_bool]
_tln.TLN_SetPaletteAnimation.restype = c_bool
_tln.TLN_SetPaletteAnimationSource.argtypes = [c_int, c_void_p]
_tln.TLN_SetPaletteAnimationSource = c_bool
_tln.TLN_SetTilesetAnimation.argtypes = [c_int, c_int, c_void_p]
_tln.TLN_SetTilesetAnimation.restype = c_bool
_tln.TLN_SetTilemapAnimation.argtypes = [c_int, c_int, c_void_p]
_tln.TLN_SetTilemapAnimation.restype = c_bool
_tln.TLN_SetSpriteAnimation.argtypes = [c_int, c_int, c_void_p, c_int]
_tln.TLN_SetSpriteAnimation.restype = c_bool
_tln.TLN_GetAnimationState.argtypes = [c_int]
_tln.TLN_GetAnimationState.restype = c_bool
_tln.TLN_SetAnimationDelay.argtypes = [c_int, c_int]
_tln.TLN_SetAnimationDelay.restype = c_bool
_tln.TLN_GetAvailableAnimation.restype = c_int
_tln.TLN_DisableAnimation.argtypes = [c_int]
_tln.TLN_DisableAnimation.restype = c_bool

def GetAvailableAnimation():
	return _tln.TLN_GetAvailableAnimation()

class Animation:

	def __init__(self, index):
		self._as_parameter_ = index

	def SetPaletteAnimation(self, palette, sequence, blend):
		return _tln.TLN_SetPaletteAnimation(self, palette, sequence, blend)

	def SetPaletteAnimationSource(self, palette):
		return _tln.TLN_SetPaletteAnimationSource(self, palette)

	def SetTilesetAnimation(self, layer, sequence):
		return _tln.TLN_SetTilesetAnimation(self, layer, sequence)

	def SetTilemapAnimation(self, layer, sequence):
		return _tln.TLN_SetTilemapAnimation(self, layer, sequence)

	def SetSpriteAnimation(self, sprite, sequence, loop):
		return _tln.TLN_SetSpriteAnimation(self, sprite, sequence, loop)

	def GetState(self):
		return _tln.TLN_GetAnimationState(self)

	def SetDelay(self, delay):
		return _tln.TLN_SetAnimationDelay(self, delay)

	def Disable(self):
		return _tln.TLN_DisableAnimation(self)