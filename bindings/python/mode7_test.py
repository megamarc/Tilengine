"""Interactive version of the mode7 sample to load from the interpreter"""

# imports
import tilengine as tln
import math
from ctypes import *

# constants
LAYER_FOREGROUND = 0
LAYER_BACKGROUND = 1
MAP_HORIZON		 = 0
MAP_TRACK		 = 1
WIDTH		= 400
HEIGHT		= 240
MIN_SCALE	= 0.2
MAX_SCALE	= 5.0

# module variables
xpos = -136
ypos = 336
angle = 0
speed = 0

affine = tln.Affine(angle=0, sx=1, sy=1, dx=WIDTH/2,dy=HEIGHT)

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)
	
def raster_callback (line):
	global xpos, ypos
	
	# line 0: begin of frame
	if line == 0:
		tln.SetLayer (LAYER_FOREGROUND, tileset_horizon, tilemap_horizon)
		tln.SetLayer (LAYER_BACKGROUND, tileset_horizon, tilemap_horizon)
		tln.SetLayerPosition (LAYER_FOREGROUND, lerp(angle*2, 0,360, 0,256), 24)
		tln.SetLayerPosition (LAYER_BACKGROUND, lerp(angle, 0,360, 0,256), 0)
		tln.ResetLayerMode (LAYER_BACKGROUND)
		affine.angle = angle
		
		rad = math.radians(angle)
		xpos += (math.sin (rad) * speed)
		ypos -= (math.cos (rad) * speed)		
	
	if line == 24:
		tln.SetLayer (LAYER_BACKGROUND, tileset_track, tilemap_track);
		tln.SetLayerPosition (LAYER_BACKGROUND, int(xpos), int(ypos));
		tln.DisableLayer (LAYER_FOREGROUND);

	if line >= 24:
		scale = lerp (line, 24,HEIGHT, MIN_SCALE,MAX_SCALE);
		affine.sx = scale;
		affine.sy = scale;		
		tln.SetLayerTransform (LAYER_BACKGROUND, affine.angle, affine.dx, affine.dy, affine.sx, affine.sy)
		
def SetScale (min,max):
	""""Sets the upper (min) and lower (max) scaling factor of the road"""
	global MIN_SCALE
	global MAX_SCALE
	MIN_SCALE = min
	MAX_SCALE = max
	
def SetPosition (x,y):
	"""Sets the location of the road"""
	global xpos
	global ypos
	xpos = x
	ypos = y
	
def SetAngle (a):
	"""Sets the rotation angle (degrees) of the road"""
	global angle
	angle = a%360
	
def SetSpeed (s):
	""""Sets the movement speed of the road"""
	global speed
	speed = s

# setup engine
tln.Init (WIDTH, HEIGHT, 2,0,0)
tln.CreateWindowThread ("overlay2.bmp", tln.CWF_S2)
tln.SetBGColor (0,0,0)

# load resources
tileset_horizon = tln.LoadTileset ("track1_bg.tsx")
tilemap_horizon = tln.LoadTilemap ("track1_bg.tmx", "Layer 1")
tileset_track   = tln.LoadTileset ("track1.tsx")
tilemap_track   = tln.LoadTilemap ("track1.tmx", "Layer 1")

# set raster callback
CB_FUNC_TYPE = CFUNCTYPE (None, c_int)
cb_func = CB_FUNC_TYPE(raster_callback)
tln.SetRasterCallback (cb_func)
