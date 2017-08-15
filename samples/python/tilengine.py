"""
Tilengine - 2D Graphics library with raster effects
Copyright (c) 2015-2017 Marc Palacios Domenech (megamarc@hotmail.com)
All rights reserved.
"""
"""
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
"""

from sys import platform as _platform
from ctypes import *

"""
Python wrapper for Tilengine retro graphics engine
Updated to library version 1.14.0
http://www.tilengine.org
"""


# constants --------------------------------------------------------------------

class WindowFlags(object):
	"""
	List of flag values for window creation
	"""
	FULLSCREEN = (1 << 0)
	VSYNC = (1 << 1)
	S1 = (1 << 2)
	S2 = (2 << 2)
	S3 = (3 << 2)
	S4 = (4 << 2)
	S5 = (5 << 2)


class Flags(object):
	"""
	List of flags for tiles and sprites
	"""
	FLIPX = (1 << 15)  # horizontal flip
	FLIPY = (1 << 14)  # vertical flip
	ROTATE = (1 << 13)  # row/column flip (unsupported, Tiled compatibility)
	PRIORITY = (1 << 12)  # tile goes in front of sprite layer


class Error(object):
	"""
	List of possible error codes returned by Engine::get_last_error()
	"""
	OK = 0  # No error
	OUT_OF_MEMORY = 1  # Not enough memory
	IDX_LAYER = 2  # Layer index out of range
	IDX_SPRITE = 3  # Sprite index out of range
	IDX_ANIMATION = 4  # Animation index out of range
	IDX_PICTURE = 5  # Picture or tile index out of range
	REF_TILESET = 6  # Invalid Tileset reference
	REF_TILEMAP = 7  # Invalid Tilemap reference
	REF_SPRITESET = 8  # Invalid Spriteset reference
	REF_PALETTE = 9  # Invalid Palette reference
	REF_SEQUENCE = 10  # Invalid SequencePack reference
	REF_SEQPACK = 11  # Invalid Sequence reference
	REF_BITMAP = 12  # Invalid Bitmap reference
	NULL_POINTER = 13  # Null pointer as argument
	FILE_NOT_FOUND = 14  # Resource file not found
	WRONG_FORMAT = 15  # Resource file has invalid format
	WRONG_SIZE = 16  # A width or height parameter is invalid
	UNSUPPORTED = 17  # Unsupported function


class Blend(object):
	"""
	Available blending modes
	"""
	NONE = 0
	MIX25 = 1
	MIX50 = 2
	MIX75 = 3
	ADD = 4
	SUB = 5
	MOD = 6
	CUSTOM = 7
	MIX = MIX50


class Input(object):
	"""
	Available inputs to query in Window::get_input()
	"""
	NONE = 0
	UP = 1
	DOWN = 2
	LEFT = 3
	RIGHT = 4
	A = 5
	B = 6
	C = 7
	D = 8


class Overlay(object):
	"""
	Available CRT overlay patterns
	"""
	NONE = 0
	SHADOWMASK = 1
	APERTURE = 2
	SCANLINES = 3
	CUSTOM = 4


class TilengineException(Exception):
	"""
	Tilengine exception class
	"""
	def __init__(self, value):
		self.value = value

	def __str__(self):
		return repr(self.value)


# structures ------------------------------------------------------------------
class Tile(Structure):
	"""
	Tile data contained in each cell of a Tilemap object
	"""
	_fields_ = [
		("index", c_ushort),
		("flags", c_ushort)
	]


class ColorStrip(Structure):
	"""
	Data used to define each frame of a color cycle for Sequence objects 
	"""
	_fields_ = [
		("delay", c_int),
		("first", c_ubyte),
		("count", c_ubyte),
		("dir", c_ubyte)
	]


class SequenceFrame(Structure):
	"""
	Data used to define each frame of an animation for Sequence objects
	"""
	_fields_ = [
		("index", c_int),
		("delay", c_int)
	]


class SpriteInfo(Structure):
	"""
	Data returned by Spriteset::get_sprite_info() with dimensions of the required sprite 
	"""
	_fields_ = [
		("w", c_int),
		("h", c_int)
	]


class TileInfo(Structure):
	"""
	Data returned by Layer::get_tile() about a given tile inside a background layer
	"""
	_fields_ = [
		("index", c_ushort),
		("flags", c_ushort),
		("row", c_int),
		("col", c_int),
		("xoffset", c_int),
		("yoffset", c_int),
		("color", c_ubyte),
		("type", c_ubyte),
		("empty", c_bool)
	]


class SpriteData(Structure):
	"""
	Data used to create Spriteset objects
	"""
	_fields_ = [
		("name", c_char_p),
		("x", c_int),
		("y", c_int),
		("w", c_int),
		("h", c_int)
	]


class PixelMap(Structure):
	"""
	Data passed to Layer::set_pixel_mapping() in a list
	"""
	_fields_ = [
		("dx", c_short),
		("dy", c_short)
	]


class Color(object):
	"""
	Represents a color value in RGB format
	"""
	def __init__(self, r, g, b):
		self.r = r
		self.g = g
		self.b = b


# module internal variables
_tln = None			# handle to shared native library
_engine = None		# singleton engine
_window = None		# singleton window

# load native library
if _platform == "linux" or _platform == "linux2":
	_tln = cdll.LoadLibrary("libTilengine.so")
elif _platform == "win32":
	_tln = cdll.LoadLibrary("Tilengine.dll")
elif _platform == "darwin":
	_tln = cdll.LoadLibrary("Tilengine.dylib")

# callback types for user functions
_raster_callback_function = CFUNCTYPE(None, c_int)
_blend_function = CFUNCTYPE(c_ubyte, c_ubyte, c_ubyte)


# return c_char_p compatible string
def _encode_string(string):
	if string is not None:
		return string.encode()
	else:
		return None


# error handling --------------------------------------------------------------
_tln.TLN_GetLastError.restype = c_int
_tln.TLN_GetErrorString.argtypes = [c_int]
_tln.TLN_GetErrorString.restype = c_char_p


# raises exception depending on error code
def _raise_exception(result=False):
	if result is not True:
		error = _tln.TLN_GetLastError()
		error_string = _tln.TLN_GetErrorString(error)
		raise TilengineException(error_string.decode())


# basic management ------------------------------------------------------------
_tln.TLN_Init.argtypes = [c_int, c_int, c_int, c_int, c_int]
_tln.TLN_Init.restype = c_bool
_tln.TLN_GetNumObjects.restype = c_int
_tln.TLN_GetVersion.restype = c_int
_tln.TLN_GetUsedMemory.restype = c_int
_tln.TLN_SetBGColor.argtypes = [c_ubyte, c_ubyte, c_ubyte]
_tln.TLN_SetBGColorFromTilemap.argtypes = [c_void_p]
_tln.TLN_SetBGColorFromTilemap.restype = c_bool
_tln.TLN_SetBGBitmap.argtypes = [c_void_p]
_tln.TLN_SetBGBitmap.restype = c_bool
_tln.TLN_SetBGPalette.argtypes = [c_void_p]
_tln.TLN_SetBGPalette.restype = c_bool
_tln.TLN_SetRenderTarget.argtypes = [c_void_p, c_int]
_tln.TLN_UpdateFrame.argtypes = [c_int]
_tln.TLN_BeginFrame.argtypes = [c_int]
_tln.TLN_DrawNextScanline.restype = c_bool
_tln.TLN_SetLoadPath.argtypes = [c_char_p]


class Engine(object):
	"""
	Main object for engine creation and rendering
	"""

	def __init__(self, num_layers, num_sprites, num_animations):
		self.layers = [Layer(n) for n in range(num_layers)]
		self.sprites = [Sprite(n) for n in range(num_sprites)]
		self.animations = [Animation(n) for n in range(num_animations)]
		self.version = _tln.TLN_GetVersion()
		self.num_layers = num_layers
		self.num_sprites = num_sprites
		self.num_animations = num_animations
		self.cb_raster_func = None
		self.cb_blend_func = None

	@classmethod
	def create(cls, width, height, num_layers, num_sprites, num_animations):
		"""
		Static method that creates a new instance of the engine
		:param width: horizontal resolution in pixels
		:param height: vertical resolution in pixels
		:param num_layers: number of background layers
		:param num_sprites: number of sprites
		:param num_animations: number of animations
		:return: new instance
		"""
		global _engine
		if _engine is not None:
			return _engine
		ok = _tln.TLN_Init(width, height, num_layers, num_sprites, num_animations)
		if ok is True:
			_engine = Engine(num_layers, num_sprites, num_animations)
			return _engine
		else:
			_raise_exception(ok)

	def delete(self):
		"""
		De-initialises engine and frees used resources
		"""
		global _engine
		_engine = None
		_tln.TLN_Deinit()

	def get_num_objects(self):
		"""
		:return: the number of objets used by the engine so far
		"""
		return _tln.TLN_GetNumObjects()

	def get_used_memory(self):
		"""
		:return: the total amount of memory used by the objects
		"""
		return _tln.TLN_GetUsedMemory()

	def set_background_color(self, param):
		"""
		Sets the background color
		:param param: can be a Color object or a Tilemap object. In this case, 
		it assigns de background color as defined inside the tilemap
		"""
		param_type = type(param)
		if param_type is Color:
			_tln.TLN_SetBGColor(param.r, param.g, param.b)
		elif param_type is Tilemap:
			_tln.TLN_SetBGColorFromTilemap(param)
			
	def disable_background_color(self):
		"""
		Disales background color rendering. If you know that the last background layer will always
		cover the entire screen, you can disable it to gain some performance
 		"""
		_tln.TLN_DisableBGColor()

	def set_background_bitmap(self, bitmap):
		"""
		Sets a static bitmap as background
		:param bitmap: Bitmap object to set as background. Set None to disable it.
		"""
		ok = _tln.TLN_SetBGBitmap(bitmap)
		_raise_exception(ok)

	def set_background_palette(self, palette):
		"""
		Sets the palette for the background bitmap. By default it is assigned the palette
		of the bitmap passed in `Engine::set_background_bitmap()` 
		:param palette: Palette object to set
		"""
		ok = _tln.TLN_SetBGPalette(palette)
		_raise_exception(ok)

	def set_raster_callback(self, raster_callback):
		"""
		Enables raster effects processing, where any render parameter can be modified mid frame, between scanlines.
		:param raster_callback: name of the user-defined function to call for each scanline. Set None to disable.
		This function takes one integer parameter that indicates the current scanline, between 0 and vertical resolution:
		```
		# raster callback example
		def raster_callback(num_scanline):
			# do something depending on the value of num_scanline
		```
		"""
		self.cb_raster_func = _raster_callback_function(raster_callback)
		_tln.TLN_SetRasterCallback(self.cb_raster_func)

	def set_render_target(self, pixels, pitch):
		"""
		Sets the output surface for rendering
		:param pixels: Pointer to the start of the target framebuffer
		:param pitch: Number of bytes per each scanline of the framebuffer 
		"""
		_tln.TLN_SetRenderTarget(pixels, pitch)

	def update_frame(self, num_frame=0):
		"""
		Draws the frame to the previously specified render target
		:param num_frame: optional timestamp value (frame number) for animation control
		"""
		_tln.TLN_UpdateFrame(num_frame)

	def begin_frame(self, num_frame=0):
		"""
		Starts active rendering of the current frame, istead of the callback-based `Engine::update_frame()`.
		Used in conjunction with `Engine::draw_next_scanline()` 
		:param num_frame: optional timestamp value (frame number) for animation control
		"""
		_tln.TLN_BeginFrame(num_frame)

	def draw_next_scanline(self):
		"""
		Draws the next scanline of the frame started with `Engine::begin_frame()` or `Window::begin_frame()`
		:return: True if there are still lines to be drawn, or False when the frame is camplete.
		"""
		return _tln.TLN_DrawNextScanline()

	def set_load_path(self, path):
		"""
		Sets base path for all data loading static methods (`::fromfile()`)
		:param path: Base path. Files will load at path/filename. Set None to use current directory
		"""
		_tln.TLN_SetLoadPath(_encode_string(path))

	def set_custom_blend_function(self, blend_function):
		"""
		Sets custom blend function to use in sprites or background layers when `BLEND_CUSTOM` mode
		is selected with the `::set_blend_mode()` method.
		:param blend_function: name of the user-defined function to call when blending that takes
		two integer arguments: source component intensity, destination component intensity, and returns
		the desired intensity.
		````
		# do 50%/50% blending example
		def blend_function(src, dst):
			return (src + dst) / 2
		"""
		self.cb_blend_func = _blend_function(blend_function)
		_tln.TLN_SetCustomBlendFunction(self.cb_blend_func)

	def get_available_sprite(self):
		"""
		:return: Index of the first unused sprite (starting from 0) or -1 if none found
		"""
		return _tln.TLN_GetAvailableSprite()

	def get_available_animation(self):
		"""
		:return: Index of the first unused animation (starting from 0) or -1 if none found 
		"""
		return _tln.TLN_GetAvailableAnimation()


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


class Window(object):
	"""
	Built-in window manager for easy setup and testing
	"""

	@classmethod
	def create(cls, overlay=None, flags=WindowFlags.VSYNC):
		"""
		Static method that creates a single-threaded window that must be used in conjunction with
		`Window::process()` and `Window::draw_frame()` in a loop
		:param overlay: name of an optional bitmap for use as overlay by the CRT effect
		:param flags: optional flags combination of `class WindowFlags` values
		:return: instance of the created window
		"""
		global _window
		if _window is not None:
			return _window
		ok = _tln.TLN_CreateWindow(_encode_string(overlay), flags)
		if ok is True:
			_window = Window()
			return _window
		else:
			_raise_exception(ok)

	@classmethod
	def create_threaded(cls, overlay=None, flags=WindowFlags.VSYNC):
		"""
		Static method that creates a multi-threaded window that runs in its own thread without user loop.
		Used mainly in python interactive console
		:param overlay: name of an optional bitmap for use as overlay by the CRT effect
		:param flags: optional flags combination of `class WindowFlags` values
		"""
		global _window
		if _window is not None:
			return _window
		ok = _tln.TLN_CreateWindowThread(_encode_string(overlay), flags)
		if ok is True:
			_window = Window()
			return _window
		else:
			_raise_exception(ok)

	def process(self):
		"""
		Does basic window housekeeping in signgle-threaded window, created with `Window::create()`.
		This method must be called in a loop by the main thread.
		:return: True if window is active or False if the user has requested to end the application
		(by pressing Esc key or clicking the close button)
		"""
		return _tln.TLN_ProcessWindow()

	def is_active(self):
		"""
		:return: True if window is active or False if the user has requested to end the application
		(by pressing Esc key or clicking the close button)
		"""
		return _tln.TLN_IsWindowActive()

	def get_input(self, input_id):
		"""
		Returns the state of a given input
		:param input_id: one of the `class Input` defined values
		:return: True if that input is pressed or False if not
		"""
		return _tln.TLN_GetInput(input_id)

	def draw_frame(self, num_frame=0):
		"""
		Draws a frame to a single-threaded window
		:param num_frame: optional timestamp value (frame number) for animation control
		"""
		_tln.TLN_DrawFrame(num_frame)

	def wait_redraw(self):
		"""
		In multi-threaded windows, it waits until the current frame has finished rendering. 
		"""
		_tln.TLN_WaitRedraw()

	def enable_crt_effect(self, overlay_id, overlay_blend, threshold, v0, v1, v2, v3, blur, glow_factor):
		"""
		Enables CRT simulation post-processing effect to give true retro appearance. Enabled by default.
		:param overlay_id: One of the defined `class Overlay` values. 
		Choosing `Overlay::CUSTOM` selects the image passed when calling `::create()`
		:param overlay_blend: blend factor for overlay image. 0 is full transparent (no effect), 255 is full blending
		:param threshold: Middle point of the brightness mapping function
		:param v0: output brightness when input brightness = 0
		:param v1: output brightness when input brightness = threshold
		:param v2: output brightness when input brightness = threshold (to create discontinuity with v1)
		:param v3: output brightness when input brightness = 255
		:param blur: True to add gaussian blur to brightness overlay, softens image
		:param glow_factor: blend addition factor for brightness overlay. 0 is not addition, 255 is full addition
		"""
		_tln.TLN_EnableCRTEffect(overlay_id, overlay_blend, threshold, v0, v1, v2, v3, blur, glow_factor)

	def disable_crt_effect(self):
		"""
		Disables the CRT post-processing effect enabled with `::enable_crt_effect()`
		"""
		_tln.TLN_DisableCRTEffect()

	def get_ticks(self):
		"""
		:return: the number of milliseconds since application start
		"""
		return _tln.TLN_GetTicks()

	def delay(self, msecs):
		"""
		Suspends execition for a fixed time
		:param msecs: number of milliseconds to pause
		"""
		_tln.TLN_Delay(msecs)

	def begin_frame(self, num_frame):
		"""
		Begins active rendering frame to the window
		:param num_frame: optional timestamp value (frame number) for animation control 
		"""
		_tln.TLN_BeginWindowFrame(num_frame)

	def end_frame(self):
		"""
		Finishes rendering the current frame and updates the window 
		"""
		_tln.TLN_EndWindowFrame()


# spritesets management -----------------------------------------------------------
_tln.TLN_CreateSpriteset.argtypes = [c_void_p, POINTER(SpriteData), c_int]
_tln.TLN_CreateSpriteset.restype = c_void_p
_tln.TLN_LoadSpriteset.argtypes = [c_char_p]
_tln.TLN_LoadSpriteset.restype = c_void_p
_tln.TLN_CloneSpriteset.argtypes = [c_void_p]
_tln.TLN_CloneSpriteset.restype = c_void_p
_tln.TLN_GetSpriteInfo.argtypes = [c_void_p, c_int, POINTER(SpriteInfo)]
_tln.TLN_GetSpriteInfo.restype = c_bool
_tln.TLN_GetSpritesetPalette.argtypes = [c_void_p]
_tln.TLN_GetSpritesetPalette.restype = c_void_p
_tln.TLN_DeleteSpriteset.argtypes = [c_void_p]
_tln.TLN_DeleteSpriteset.restype = c_bool


class Spriteset(object):
	"""
	The Spriteset object holds the graphic data used to render moving objects (sprites)
	"""

	def __init__(self, handle):
		self._as_parameter_ = handle
		self.palette = Palette(_tln.TLN_GetSpritesetPalette(handle))

	@classmethod
	def create(cls, bitmap, sprite_data):
		"""
		Static method that creates an empty spriteset
		:param bitmap: Bitmap object containing the packaged sprite pictures
		:param sprite_data: list of SpriteEntry tuples describing each sprite pictures
		:return: instance of the created object
		"""
		handle = _tln.TLN_CreateSpriteset(bitmap, sprite_data, len(sprite_data))
		if handle is not None:
			return Spriteset(handle)
		else:
			_raise_exception()

	@classmethod
	def fromfile(cls, filename):
		"""
		Static method that loads a spriteset from a pair of png/txt files
		:param filename: png filename with bitmap data
		:return: instance of the created object 
		"""
		handle = _tln.TLN_LoadSpriteset(_encode_string(filename))
		if handle is not None:
			return Spriteset(handle)
		else:
			_raise_exception()

	def clone(self):
		"""
		Creates a copy of the object
		:return: instance of the copy
		"""
		handle = _tln.TLN_CloneSpriteset(self)
		if handle is not None:
			return Spriteset(handle)
		else:
			_raise_exception()

	def get_sprite_info(self, entry, info):
		"""
		Gets info about a given sprite in
		:param entry: 
		:param info: 
		"""
		ok = _tln.TLN_GetSpriteInfo(self, entry, info)
		_raise_exception(ok)

	def delete(self):
		"""
		Destroys the object and releases unmanaged resources
		"""
		ok = _tln.TLN_DeleteSpriteset(self)
		_raise_exception(ok)


# tilesets management ---------------------------------------------------------
_tln.TLN_CreateTileset.argtypes = [c_int, c_int, c_int, c_void_p, c_void_p, POINTER(c_byte)]
_tln.TLN_CreateTileset.restype = c_void_p
_tln.TLN_LoadTileset.argtypes = [c_char_p]
_tln.TLN_LoadTileset.restype = c_void_p
_tln.TLN_CloneTileset.argtypes = [c_void_p]
_tln.TLN_CloneTileset.restype = c_void_p
_tln.TLN_SetTilesetPixels.argtypes = [c_void_p, c_int, POINTER(c_byte), c_int]
_tln.TLN_SetTilesetPixels.restype = c_bool
_tln.TLN_CopyTile.argtypes = [c_void_p, c_int, c_int]
_tln.TLN_CopyTile.restype = c_bool
_tln.TLN_GetTileWidth.argtypes = [c_void_p]
_tln.TLN_GetTileWidth.restype = c_int
_tln.TLN_GetTileHeight.argtypes = [c_void_p]
_tln.TLN_GetTileHeight.restype = c_int
_tln.TLN_GetTilesetPalette.argtypes = [c_void_p]
_tln.TLN_GetTilesetPalette.restype = c_void_p
_tln.TLN_GetTilesetSequencePack.argtypes = [c_void_p]
_tln.TLN_GetTilesetSequencePack.restype = c_void_p
_tln.TLN_FindSpritesetSprite.argtypes = [c_void_p, c_char_p]
_tln.TLN_FindSpritesetSprite.restype = c_int
_tln.TLN_DeleteTileset.argtypes = [c_void_p]
_tln.TLN_DeleteTileset.restype = c_bool


class Tileset(object):
	"""
	The Tileset object holds the graphic tiles used to render background layers from a Tilemap
	"""
	def __init__(self, handle):
		self._as_parameter_ = handle
		self.tile_width = _tln.TLN_GetTileWidth(handle)
		self.tile_height = _tln.TLN_GetTileHeight(handle)
		self.palette = Palette(_tln.TLN_GetTilesetPalette(handle))
		self.sequence_pack = SequencePack(_tln.TLN_GetTilesetSequencePack(handle))

	@classmethod
	def create(cls, num_tiles, width, height, palette, sequence_pack=None, types=None):
		"""
		Static method that creates an empty Tileset at runtime
		:param num_tiles: number of unique tiles
		:param width: Width of each tile (must be multiple of 8)
		:param height: Height of each tile (must be multiple of 8)
		:param palette: Palette object
		:param sequence_pack: Optional SequencePack with associated Tileset animations
		:param types: Optional list of bytes with the type of each tile, one tile per byte
		:return: instance of the created object 
		"""
		handle = _tln.TLN_CreateTileset(num_tiles, width, height, palette, sequence_pack, types)
		if handle is not None:
			return Tileset(handle)
		else:
			_raise_exception()

	@classmethod
	def fromfile(cls, filename):
		"""
		Static method that loads a Tiled TSX tileset from file
		:param filename: TSX file with the tileset
		:return: 
		"""
		handle = _tln.TLN_LoadTileset(_encode_string(filename))
		if handle is not None:
			return Tileset(handle)
		else:
			_raise_exception()

	def clone(self):
		"""
		Creates a copy of the object
		:return: instance of the copy
		"""
		handle = _tln.TLN_CloneTileset(self)
		if handle is not None:
			return Tileset(handle)
		else:
			_raise_exception()

	def set_pixels(self, entry, data, pitch):
		"""
		Sets pixel data for a single tile
		:param entry: Number of tile to set [0, num_tiles - 1]
		:param data: List of bytes with pixel data, one byte per pixel
		:param pitch: Number of bytes per line in source data
		"""
		ok = _tln.TLN_SetTilesetPixels(self, entry, data, pitch)
		_raise_exception(ok)

	def copy_tile(self, source, target):
		"""
		Copies tile graphic data inside a Tileset
		:param source: index of source tile
		:param target: index of target tile
		"""
		ok = _tln.TLN_CopyTile(self, source, target)
		_raise_exception(ok)

	def delete(self):
		"""
		Destroys the object and releases unmanaged resources
		"""
		ok = _tln.TLN_DeleteTileset(self)
		_raise_exception(ok)


# tilemaps management ---------------------------------------------------------
_tln.TLN_CreateTilemap.argtypes = [c_int, c_int, POINTER(Tile), c_int, c_void_p]
_tln.TLN_CreateTilemap.restype = c_void_p
_tln.TLN_LoadTilemap.argtypes = [c_char_p]
_tln.TLN_LoadTilemap.restype = c_void_p
_tln.TLN_CloneTilemap.argtypes = [c_void_p]
_tln.TLN_CloneTilemap.restype = c_void_p
_tln.TLN_GetTilemapRows.argtypes = [c_void_p]
_tln.TLN_GetTilemapRows.restype = c_int
_tln.TLN_GetTilemapRows.argtypes = [c_void_p]
_tln.TLN_GetTilemapRows.restype = c_int
_tln.TLN_GetTilemapTileset.argtypes = [c_void_p]
_tln.TLN_GetTilemapTileset.restype = c_void_p
_tln.TLN_GetTilemapTile.argtypes = [c_void_p, c_int, c_int, POINTER(Tile)]
_tln.TLN_GetTilemapTile.restype = c_bool
_tln.TLN_SetTilemapTile.argtypes = [c_void_p, c_int, c_int, POINTER(Tile)]
_tln.TLN_SetTilemapTile.restype = c_bool
_tln.TLN_CopyTiles.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_void_p, c_int, c_int]
_tln.TLN_CopyTiles.restype = c_bool
_tln.TLN_DeleteTilemap.argtypes = [c_void_p]
_tln.TLN_DeleteTilemap.restype = c_bool


class Tilemap(object):
	"""
	The Tilemap object holds the grid of tiles that define the background layout
	"""
	def __init__(self, handle):
		self._as_parameter_ = handle
		self.rows = _tln.TLN_GetTilemapRows(handle)
		self.cols = _tln.TLN_GetTilemapCols(handle)
		tileset_handle = _tln.TLN_GetTilemapTileset(handle)
		if tileset_handle is not None:
			self.tileset = Tileset(tileset_handle)
		else:
			self.tileset = None

	@classmethod
	def create(cls, rows, cols, tiles, background_color=0, tileset=None):
		"""
		Static method that creates an empty tilemap
		:param rows: Number of rows (vertical dimension)
		:param cols: Number of cols (horizontal dimension)
		:param tiles: List of Tile objects with tile data
		:param background_color: optional Color object with default background color
		:param tileset: Optional reference to associated tileset
		:return: instance of the created object 
		"""
		handle = _tln.TLN_CreateTilemap(rows, cols, POINTER(tiles), background_color, tileset)
		if handle is not None:
			return Tilemap(handle)
		else:
			_raise_exception()

	@classmethod
	def fromfile(cls, filename, layer_name=None):
		"""
		Static method that loads a Tiled TMX tilemap from file
		:param filename: TMX file with the tilemap
		:param layer_name: Optional name of the layer to load when the TMX file has more than one layer. 
		By default it loads the first layer inside the TMX
		:return: instance of the created object
		"""
		handle = _tln.TLN_LoadTilemap(_encode_string(filename), _encode_string(layer_name))
		if handle is not None:
			return Tilemap(handle)
		else:
			_raise_exception()

	def clone(self):
		"""
		Creates a copy of the object
		:return: instance of the copy
		"""
		handle = _tln.TLN_CloneTilemap(self)
		if handle is not None:
			return Tilemap(handle)
		else:
			_raise_exception()

	def get_tile(self, row, col, tile_info):
		"""
		Gets data about a given tile
		:param row: Vertical position of the tile (0 <= row < rows)
		:param col: Horizontal position of the tile (0 <= col < cols)
		:param tile_info: pointer to user-provided `Tile` object where to get the data
		"""
		ok = _tln.TLN_GetTilemapTile(self, row, col, POINTER(tile_info))
		_raise_exception(ok)

	def set_tile(self, row, col, tile_info):
		"""
		
		:param row: Vertical position of the tile (0 <= row < rows)
		:param col: Horizontal position of the tile (0 <= col < cols)
		:param tile_info: pointer to user-provided `Tile` object 
		"""
		ok = _tln.TLN_SetTilemapTile(self, row, col, POINTER(tile_info))
		_raise_exception(ok)

	def copy_tiles(self, src_row, src_col, num_rows, num_cols, dst_tilemap, dst_row, dst_col):
		"""
		Copies blocks of tiles between two tilemaps
		:param src_row: Starting row (vertical position) inside the source tilemap
		:param src_col: Starting column (horizontal position) inside the source tilemap
		:param num_rows: Number of rows to copy
		:param num_cols: Number of columns to copy
		:param dst_tilemap: Target tilemap
		:param dst_row: Starting row (vertical position) inside the target tilemap
		:param dst_col: Starting column (horizontal position) inside the target tilemap
		"""
		ok = _tln.TLN_CopyTiles(self, src_row, src_col, num_rows, num_cols, dst_tilemap, dst_row, dst_col)
		_raise_exception(ok)

	def delete(self):
		"""
		Destroys the object and releases unmanaged resources
		"""
		ok = _tln.TLN_DeleteTilemap(self)
		_raise_exception(ok)


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


class Palette(object):
	"""
	The Palette object holds the color tables used by tileesets and spritesets to render sprites and backgrounds
	"""
	def __init__(self, handle):
		self._as_parameter_ = handle

	@classmethod
	def create(cls, num_entries=256):
		"""
		Static method that creates an empty palette
		:param num_entries: optional number of colors to hold (up to 256, default value)
		:return: instance of the created object
		"""
		handle = _tln.TLN_CreatePalette(num_entries)
		if handle is not None:
			return Palette(handle)
		else:
			_raise_exception()

	@classmethod
	def fromfile(cls, filename):
		"""
		Static method that loads a palette from an Adobe Color Table (.act) file
		:param filename: name of the .act file to load
		:return: instance of the created object 
		"""
		handle = _tln.TLN_LoadPalette(_encode_string(filename))
		if handle is not None:
			return Palette(handle)
		else:
			_raise_exception()

	def clone(self):
		"""
		Creates a copy of the object
		:return: instance of the copy
		"""
		handle = _tln.TLN_ClonePalette(self)
		if handle is not None:
			return Palette(handle)
		else:
			_raise_exception()

	def set_color(self, entry, color):
		"""
		Sets the RGB color value of a palette entry
		:param entry: Index of the palette entry to modify (0-255)
		:param color: Color object with the r,g,b components of the color
		"""
		ok = _tln.TLN_SetPaletteColor(self, entry, color.r, color.g, color.b)
		_raise_exception(ok)

	def mix(self, src_palette1, src_palette2, factor):
		"""
		Mixes two palettes
		:param src_palette1: First palette to mix
		:param src_palette2: Second palette to mix
		:param factor: Integer value with percentage of mix (0-100)
		"""
		ok = _tln.TLN_MixPalettes(src_palette1, src_palette2, self, factor)
		_raise_exception(ok)

	def add_color(self, first, count, color):
		"""
		Modifies a range of colors by adding the provided color value to the selected range. 
		The result is always a brighter color.
		:param first: index of the first color entry to modify
		:param count: number of colors from start to modify
		:param color: Color object to add
		"""
		ok = _tln.TLN_AddPaletteColor(self, first, count, color.r, color.g, color.b)
		_raise_exception(ok)

	def sub_color(self, first, count, color):
		"""
		Modifies a range of colors by subtracting the provided color value to the selected range. 
		The result is always a darker color.
		:param first: index of the first color entry to modify
		:param count: number of colors from start to modify
		:param color: Color object to subtract
		"""
		ok = _tln.TLN_SubPaletteColor(self, first, count, color.r, color.g, color.b)
		_raise_exception(ok)

	def mod_color(self, first, count, color):
		"""
		Modifies a range of colors by modulating (normalized product) the provided color value to the selected range. 
		The result is always a darker color.
		:param first: index of the first color entry to modify
		:param count: number of colors from start to modify
		:param color: Color object to modulate
		"""
		ok = _tln.TLN_ModPaletteColor(self, first, count, color.r, color.g, color.b)
		_raise_exception(ok)

	def delete(self):
		"""
		Destroys the object and releases unmanaged resources
		"""
		ok = _tln.TLN_DeletePalette(self)
		_raise_exception(ok)


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


class Bitmap(object):
	"""
	The Bitmap object holds graphic data used to build in backgrounds, Tileset and Spriteset objects
	"""
	def __init__(self, handle):
		self._as_parameter_ = handle
		self.width = _tln.TLN_GetBitmapWidth(handle)
		self.height = _tln.TLN_GetBitmapHeight(handle)
		self.depth = _tln.TLN_GetBitmapDepth(handle)
		self.pitch = _tln.TLN_GetBitmapPitch(handle)
		self.palette = Palette(_tln.TLN_GetBitmapPalette(handle))

	@classmethod
	def create(cls, width, height, bpp=8):
		"""
		Static method that creates an empty bitmap
		:param width: Width in pixels
		:param height: Height in pixels
		:param bpp: Optional bits per pixel (8 by default)
		:return: instance of the created object 
		"""
		handle = _tln.TLN_CreateBitmap(width, height, bpp)
		if handle is not None:
			return Bitmap(handle)
		else:
			_raise_exception()

	@classmethod
	def fromfile(cls, filename):
		"""
		Static method that loads a BMP or PNG file
		:param filename: name of the file to load (.bmp or .png)
		:return: instance of the created object 
		"""
		handle = _tln.TLN_LoadBitmap(_encode_string(filename))
		if handle is not None:
			return Bitmap(handle)
		else:
			_raise_exception()

	def clone(self):
		"""
		Creates a copy of the object
		:return: instance of the copy
		"""
		handle = _tln.TLN_CloneBitmap(self)
		if handle is not None:
			return Bitmap(handle)
		else:
			_raise_exception()

	def get_data(self, x, y):
		"""
		Returns a pointer to the starting memory address
		:param x: Starting x position [0, width - 1]
		:param y: Starting y position [0, height - 1]
		:return: pointer
		"""
		return _tln.TLN_GetBitmapPtr(self, x, y)

	def delete(self):
		"""
		Destroys the object and releases unmanaged resources		 
		"""
		ok = _tln.TLN_DeleteBitmap(self)
		_raise_exception(ok)


# sequences management --------------------------------------------------------
_tln.TLN_CreateSequence.argtypes = [c_char_p, c_int, c_int, POINTER(SequenceFrame)]
_tln.TLN_CreateSequence.restype = c_void_p
_tln.TLN_CreateCycle.argtypes = [c_char_p, c_int, POINTER(ColorStrip)]
_tln.TLN_CreateCycle.restype = c_void_p
_tln.TLN_CloneSequence.argtypes = [c_void_p]
_tln.TLN_CloneSequence.restype = c_void_p
_tln.TLN_DeleteSequence.argtypes = [c_void_p]
_tln.TLN_DeleteSequence.restype = c_bool


class Sequence(object):
	"""
	The Sequence object holds the sequences to feed the animation engine
	"""
	def __init__(self, handle):
		self._as_parameter_ = handle

	@classmethod
	def create_sequence(cls, name, target, frames):
		"""
		Static method that creates an empty Sequence for Sprite and Tileset animations
		:param name: String with an unique name to identify the sequence inside a SequencePack object
		:param target: For Tileset animations, the tile index to animate
		:param frames: List with SequenceFrame objects, one for each frame of animation 
		:return: instance of the created object 
		"""
		handle = _tln.TLN_CreateSequence(_encode_string(name), target, len(frames), frames)
		if handle is not None:
			return Sequence(handle)
		else:
			_raise_exception()

	@classmethod
	def create_cycle(cls, name, strips):
		"""
		Static method that creates an empty Sequence for Palette animations
		:param name: String with an unique name to identify the sequence inside a SequencePack object 
		:param strips: List with ColorStrip objects, one for each frame of animation
		:return: instance of the created object 
		"""
		handle = _tln.TLN_CreateCycle(_encode_string(name), len(strips), strips)
		if handle is not None:
			return Sequence(handle)
		else:
			_raise_exception()

	def clone(self):
		"""
		Creates a copy of the object
		:return: instance of the copy
		"""
		handle = _tln.TLN_CloneSequence(self)
		if handle is not None:
			return Sequence(handle)
		else:
			_raise_exception()

	def delete(self):
		"""
		Destroys the object and releases unmanaged resources
		"""
		ok = _tln.TLN_DeleteSequence(self)
		_raise_exception(ok)


# sequence pack management --------------------------------------------------------
_tln.TLN_CreateSequencePack.restype = c_void_p
_tln.TLN_LoadSequencePack.argtypes = [c_char_p]
_tln.TLN_LoadSequencePack.restype = c_void_p
_tln.TLN_FindSequence.argtypes = [c_void_p, c_char_p]
_tln.TLN_FindSequence.restype = c_void_p
_tln.TLN_GetSequence.argtypes = [c_int]
_tln.TLN_GetSequence.restype = c_void_p
_tln.TLN_GetSequencePackCount.argtypes = [c_void_p]
_tln.TLN_GetSequencePackCount.restype = c_int
_tln.TLN_AddSequenceToPack.argtypes = [c_void_p, c_void_p]
_tln.TLN_AddSequenceToPack.restype = c_bool
_tln.TLN_DeleteSequencePack.argtypes = [c_void_p]
_tln.TLN_DeleteSequencePack.restype = c_bool


class SequencePack(object):
	"""
	The SequencePack object holds a collection of Sequence objects
	"""
	def __init__(self, handle):
		self._as_parameter_ = handle
		self.count = _tln.TLN_GetSequencePackCount(self)

	@classmethod
	def create(cls):
		"""
		Static method that creates an empty SequencePack object
		:return: instance of the created object 
		"""
		handle = _tln.TLN_CreateSequencePack()
		if handle is not None:
			return SequencePack(handle)
		else:
			_raise_exception()

	@classmethod
	def fromfile(cls, filename):
		"""
		Static method that loads a SQX file with sequence data (XML-based)
		:param filename: Name of the SQX file to load
		:return: instance of the created object 
		"""
		handle = _tln.TLN_LoadSequencePack(_encode_string(filename))
		if handle is not None:
			return SequencePack(handle)
		else:
			_raise_exception()

	def find_sequence(self, name):
		"""
		Finds a Sequence by its name
		:param name: name of the Sequence to find
		:return: Sequence object if found, or None if error
		"""
		handle = _tln.TLN_FindSequence(self, _encode_string(name))
		if handle is not None:
			return Sequence(handle)
		else:
			_raise_exception()

	def add_sequence(self, sequence):
		"""
		Adds a Sequence to a SequencePack
		:param sequence: Sequence object to add
		"""
		ok = _tln.TLN_AddSequenceToPack(self, sequence)
		_raise_exception(ok)

	def delete(self):
		"""
		Destroys the object and releases unmanaged resources
		"""
		ok = _tln.TLN_DeleteSequencePack(self)
		_raise_exception(ok)


# layer management ------------------------------------------------------------
_tln.TLN_SetLayer.argtypes = [c_int, c_void_p, c_void_p]
_tln.TLN_SetLayer.restype = c_bool
_tln.TLN_SetLayerPalette.argtypes = [c_int, c_void_p]
_tln.TLN_SetLayerPalette.restype = c_bool
_tln.TLN_SetLayerPosition.argtypes = [c_int, c_int, c_int]
_tln.TLN_SetLayerPosition.restype = c_bool
_tln.TLN_SetLayerScaling.argtypes = [c_int, c_float, c_float]
_tln.TLN_SetLayerScaling.restype = c_bool
_tln.TLN_SetLayerTransform.argtypes = [c_int, c_float, c_float, c_float, c_float, c_float]
_tln.TLN_SetLayerTransform.restype = c_bool
_tln.TLN_SetLayerPixelMapping.argtypes = [c_int, POINTER(PixelMap)]
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
_tln.TLN_GetLayerWidth.argtypes = [c_int]
_tln.TLN_GetLayerWidth.restype = c_int
_tln.TLN_GetLayerHeight.argtypes = [c_int]
_tln.TLN_GetLayerHeight.restype = c_int


class Layer(object):
	"""
	The Layer object manages each tiled background plane
	"""
	def __init__(self, index):
		self._as_parameter_ = index
		self.width = 0
		self.height = 0

	def setup(self, tilemap, tileset=None):
		"""
		Enables a background layer by setting the specified tilemap and optional tileset
		:param tilemap: Tilemap object with background layout 
		:param tileset: Optional Tileset object. If not set, the Tilemap's own Tileset is selected
		"""
		ok = _tln.TLN_SetLayer(self, tileset, tilemap)
		if ok is True:
			self.width = _tln.TLN_GetLayerWidth(self)
			self.height = _tln.TLN_GetLayerHeight(self)
			return ok
		else:
			_raise_exception()

	def set_palette(self, palette):
		"""
		Sets the color palette to the layer
		:param palette: Palette object to assign. By default the Tileset's own palette is used
		"""
		ok = _tln.TLN_SetLayerPalette(self, palette)
		_raise_exception(ok)

	def set_position(self, x, y):
		"""
		Sets the position of the tileset that corresponds to the upper left corner of the viewport
		:param x: horizontal position
		:param y: vertical position
		"""
		ok = _tln.TLN_SetLayerPosition(self, int(x), int(y))
		_raise_exception(ok)

	def set_scaling(self, sx, sy):
		"""
		Enables layer scaling
		:param sx: floating-point value with horizontal scaling factor
		:param sy: floating-point value with vertical scaling factor
		"""
		ok = _tln.TLN_SetLayerScaling(self, sx, sy)
		_raise_exception(ok)

	def set_transform(self, angle, x, y, sx, sy):
		"""
		Enables layer affine transformation (rotation and scaling). All parameters are floating point values
		:param angle: rotation angle in degrees
		:param x: horizontal displacement in screen space where the rotation center is located
		:param y: vertical displacement in screen space where the rotation center is located 
		:param sx: horizontal scaling factor
		:param sy: vertical scaling factor
		"""
		ok = _tln.TLN_SetLayerTransform(self, angle, x, y, sx, sy)
		_raise_exception(ok)

	def set_pixel_mapping(self, pixel_map):
		"""
		Enables pixel mapping displacement table
		:param pixel_map: user-provided list of PixelMap objects of hres*vres size: one item per screen pixel
		"""
		ok = _tln.TLN_SetLayerPixelMapping(self, pixel_map)
		_raise_exception(ok)

	def reset_mode(self):
		"""
		Disables all special effects: scaling, affine transform and pixel mapping, and returns to default render mode.
		"""
		ok = _tln.TLN_ResetLayerMode(self)
		_raise_exception(ok)

	def set_blend_mode(self, mode):
		"""
		Enables blending mode with background objects
		:param mode: One of the class Blend::xxx defined values 
		"""
		ok = _tln.TLN_SetLayerBlendMode(self, mode, 0)
		_raise_exception(ok)

	def set_column_offset(self, offsets):
		"""
		Enables column offset mode for tiles
		:param offsets: User-provided list of integers with offsets, one per column of tiles. Pass None to disable
		"""
		ok = _tln.TLN_SetLayerColumnOffset(self, offsets)
		_raise_exception(ok)

	def set_clip(self, x1, y1, x2, y2):
		"""
		Enables clipping rectangle
		:param x1: left coordinate
		:param y1: top coordinate
		:param x2: right coordinate
		:param y2: bottom coordinate
		"""
		ok = _tln.TLN_SetLayerClip(self, x1, y1, x2, y2)
		_raise_exception(ok)

	def disable_clip(self):
		"""
		Disables clipping rectangle
		"""
		ok = _tln.TLN_DisableLayerClip(self)
		_raise_exception(ok)

	def set_mosaic(self, pixel_w, pixel_h):
		"""
		Enables mosaic effect (pixelation)
		:param pixel_w: horizontal pixel size
		:param pixel_h: vertical pixel size
		"""
		ok = _tln.TLN_SetLayerMosaic(self, pixel_w, pixel_h)
		_raise_exception(ok)

	def disable_mosaic(self):
		"""
		Disables mosaic effect
		"""
		ok = _tln.TLN_DisableLayerMosaic(self)
		_raise_exception(ok)

	def disable(self):
		"""
		Disables the layer so it is not drawn
		"""
		ok = _tln.TLN_DisableLayer(self)
		_raise_exception(ok)

	def get_palette(self):
		"""
		Gets the layer active palette
		:return: Palette object of the Layer
		"""
		handle = _tln.TLN_GetLayerPalette(self)
		if handle is not None:
			return Palette(handle)
		else:
			_raise_exception()

	def get_tile(self, x, y, tile_info):
		"""
		Gets detailed info about the tile located in Tilemap space
		:param x: x position inside the Tilemap
		:param y: y position inside the Tilemap
		:param tile_info: User-provided TileInfo object where to get the data 
		"""
		ok = _tln.TLN_GetLayerTile(self, x, y, tile_info)
		_raise_exception(ok)


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


class Sprite(object):
	"""
	The Sprite object manages each moving character onscreen
	"""
	def __init__(self, index):
		self._as_parameter_ = index

	def setup(self, spriteset, flags=0):
		"""
		Enables a sprite by setting its Spriteset and optional flags
		:param spriteset: Spriteset object with the graphic data of the sprites
		:param flags: Optional combination of defined class Flag::xxx values, 0 by default
		"""
		ok = _tln.TLN_ConfigSprite(self, spriteset, flags)
		_raise_exception(ok)

	def set_spriteset(self, spriteset):
		"""
		Enables a sprite by setting its Spriteset 
		:param spriteset: Spriteset object with the graphic data of the sprites 
		"""
		ok = _tln.TLN_SetSpriteSet(self, spriteset)
		_raise_exception(ok)

	def set_flags(self, flags):
		"""
		Sets modification flags
		:param flags: Combination of defined class Flag::xxx values
		"""
		ok = _tln.TLN_SetSpriteFlags(self, flags)
		_raise_exception(ok)

	def set_position(self, x, y):
		"""
		Sets the sprite position in screen coordinates
		:param x: Horizontal position
		:param y: Vertical position
		"""
		ok = _tln.TLN_SetSpritePosition(self, x, y)
		_raise_exception(ok)

	def set_picture(self, picture):
		"""
		Sets the actual graphic contained in the Spriteset to the sprite
		:param picture: can be an integer with the index inside the Spriteset, or a string with its name 
		"""
		ok = False
		param_type = type(picture)
		if param_type is int:
			ok = _tln.TLN_SetSpritePicture(self, picture)
		elif param_type is str:
			entry = _tln.TLN_FindSpritesetSprite(picture)
			if entry is not -1:
				ok = _tln.TLN_SetSpritePicture(self, entry)
			else:
				return
		else:
			return
		_raise_exception(ok)

	def set_palette(self, palette):
		"""
		Assigns a Palette object to the sprite. By default it is assigned wit the Spriteset's own palette
		:param palette: Palette object to set
		"""
		ok = _tln.TLN_SetSpritePalette(self, palette)
		_raise_exception(ok)

	def set_blend_mode(self, mode):
		"""
		Enables blending mode with background objects
		:param mode: One of the class Blend::xxx defined values 
		"""
		ok = _tln.TLN_SetSpriteBlendMode(self, mode)
		_raise_exception(ok)

	def set_scaling(self, sx, sy):
		"""
		Enables sprite scaling
		:param sx: floating-point value with horizontal scaling factor
		:param sy: floating-point value with vertical scaling factor
		"""
		ok = _tln.TLN_SetSpriteScaling(self, sx, sy)
		_raise_exception(ok)

	def reset_mode(self):
		"""
		Disables scaling and returns to default render mode.
		"""
		ok = _tln.TLN_ResetSpriteScaling(self)
		_raise_exception(ok)

	def get_picture(self):
		"""
		Returns the index of the assigned picture from the Spriteset
		:return: the graphic index 
		"""
		return _tln.TLN_GetSpritePicture(self)

	def enable_collision(self, mode):
		"""
		Enables pixel-accurate sprite collision detection with other sprites
		:param mode: True for enabling or False for disabling
		"""
		ok = _tln.TLN_EnableSpriteCollision(self, mode)
		_raise_exception(ok)

	def check_collision(self):
		"""
		Gets the collision status of the sprite. Requires ::enable_collision(True)
		:return: True if collision with another sprite detected, or False if not
		"""
		return _tln.TLN_GetSpriteCollision(self)

	def disable(self):
		"""
		Disables the sprite so it is not drawn
		"""
		ok = _tln.TLN_DisableSprite(self)
		_raise_exception(ok)

	def get_palette(self):
		"""
		Gets the sprite active palette
		:return: Palette object of the Sprite
		"""
		handle = _tln.TLN_GetSpritePalette(self)
		if handle is not None:
			return Palette(handle)
		else:
			_raise_exception()


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


class Animation(object):
	"""
	The Animation object manages each animation for the sequencer engine
	"""
	def __init__(self, index):
		self._as_parameter_ = index

	def set_palette_animation(self, palette, sequence, blend):
		"""
		Starts a color cycle animation for Palette object
		:param palette: Palette object to animate
		:param sequence: Sequence object to play on the palette
		:param blend: True for smooth frame interpolation, False for classic coarse mode
		"""
		ok = _tln.TLN_SetPaletteAnimation(self, palette, sequence, blend)
		_raise_exception(ok)

	def set_palette_animation_source(self, palette):
		"""
		Sets the source palette of a color cycle animation already in motion. 
		Useful for combining color cycling and palette interpolation at the same time
		:param palette: Palette object to assign 
		"""
		ok = _tln.TLN_SetPaletteAnimationSource(self, palette)
		_raise_exception(ok)

	def set_tileset_animation(self, layer, sequence):
		"""
		Starts a Tileset animation
		:param layer: Index of layer to animate (0 -> num_layers - 1)
		:param sequence: Sequence object to play on the layer
		"""
		ok = _tln.TLN_SetTilesetAnimation(self, layer, sequence)
		_raise_exception(ok)

	def set_sprite_animation(self, sprite, sequence, loop):
		"""
		Starts a Sprite animation
		:param sprite: Index of sprite to animate (0 -> num_sprites - 1) 
		:param sequence: Sequence object to play on the sprite 
		:param loop: number of times to repeat, 0=infinite
		"""
		ok = _tln.TLN_SetSpriteAnimation(self, sprite, sequence, loop)
		_raise_exception(ok)

	def get_state(self):
		"""
		Gets the state of the animation
		:return: True if still running, False if it has finished
		"""
		return _tln.TLN_GetAnimationState(self)

	def set_delay(self, delay):
		"""
		Sets the playback speed of a given animation
		:param delay: default frame delay to assign
		"""
		ok = _tln.TLN_SetAnimationDelay(self, delay)
		_raise_exception(ok)

	def disable(self):
		"""
		Disables the animation so it doesn't run
		"""
		ok = _tln.TLN_DisableAnimation(self)
		_raise_exception(ok)
