"""Interactive version of the platformer sample to load from the interpreter"""

# imports
import tilengine as tln
import array
from ctypes import *

# constants
LAYER_FOREGROUND = 0
LAYER_BACKGROUND = 1

# module variables
basepos = 0
speed = 0
pos_background = array.array ('f', [0, 0, 0, 0, 0, 0])

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)
	
# setup layer helper
def SetupLayer (nlayer, name):
	tileset = tln.LoadTileset (name + ".tsx")
	tilemap = tln.LoadTilemap (name + ".tmx", "Layer 1")
	tln.SetLayer (nlayer, tileset, tilemap)	

# raster callback
def raster_callback (line):
	pos = -1
	
	# start of frame
	if line == 0:
		global basepos
		basepos += speed
		pos_foreground = basepos * 3
		tln.SetLayerPosition (LAYER_FOREGROUND, int(pos_foreground), 0);
	
		pos_background[0] = basepos * 0.562
		pos_background[1] = basepos * 0.437
		pos_background[2] = basepos * 0.375
		pos_background[3] = basepos * 0.625
		pos_background[4] = basepos * 1.0
		pos_background[5] = basepos * 2.0
	
		pos = pos_background[0]
		
	elif line == 32:
		pos = pos_background[1]
	elif line == 48:
		pos = pos_background[2]
	elif line == 64:
		pos = pos_background[3]
	elif line == 112:
		pos = pos_background[4]
	elif line >= 152:
		pos = lerp (line, 152,224, pos_background[4], pos_background[5])
		
	if pos != -1:
		tln.SetLayerPosition (LAYER_BACKGROUND, int(pos), 0)

	if line == 0:
		tln.SetBGColor (28,0,140)	
	elif line == 144:
		tln.SetBGColor (0,128,238)

def SetPosition (pos):
	"""Sets the horizontal position inside the map"""
	global basepos
	basepos = pos
	
def SetSpeed (s):
	"""Sets the scrolling speed on the map"""
	global speed
	speed = s
		
# initialise
tln.Init (400,240,2,80,1)
tln.CreateWindowThread ("overlay2.bmp", tln.CWF_S2)

# setup layers
SetupLayer (LAYER_FOREGROUND, "Sonic_md_fg1")
SetupLayer (LAYER_BACKGROUND, "Sonic_md_bg1")

# color cycle animation
sp = tln.LoadSequencePack ("Sonic_md_seq.sqx")
sequence = tln.FindSequence (sp, "seq_water")
palette  = tln.GetLayerPalette (LAYER_BACKGROUND)
tln.SetPaletteAnimation (0, palette, sequence, True)
	
# set raster callback
CB_FUNC_TYPE = CFUNCTYPE (None, c_int)
cb_func = CB_FUNC_TYPE(raster_callback)
tln.SetRasterCallback (cb_func)
