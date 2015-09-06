"""
Tilengine perspective projection demo
  Cursors or joystick d-pad: scroll
"""

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

#module variables
x = -136
y = 336
s = 0
a = 0.2
angle = 0
frame = 0

affine = tln.Affine(angle=0, sx=1, sy=1, dx=WIDTH/2,dy=HEIGHT)

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)
	
def raster_callback (line):
	if line == 24:
		tln.SetLayer (LAYER_BACKGROUND, tileset_track, tilemap_track);
		tln.SetLayerPosition (LAYER_BACKGROUND, int(x), int(y));
		tln.DisableLayer (LAYER_FOREGROUND);

	if line >= 24:
		scale = lerp (line, 24,HEIGHT, 0.2,5.0);
		affine.sx = scale;
		affine.sy = scale;		
		tln.SetLayerTransform (LAYER_BACKGROUND, affine.angle, affine.dx, affine.dy, affine.sx, affine.sy)

# setup engine
tln.Init (WIDTH, HEIGHT, 2,0,0)
tln.CreateWindow ("overlay.bmp", tln.CWF_VSYNC)
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

# main loop
while tln.ProcessWindow ():
	tln.SetLayer (LAYER_FOREGROUND, tileset_horizon, tilemap_horizon)
	tln.SetLayer (LAYER_BACKGROUND, tileset_horizon, tilemap_horizon)
	tln.SetLayerPosition (LAYER_FOREGROUND, lerp(angle*2, 0,360, 0,256), 24)
	tln.SetLayerPosition (LAYER_BACKGROUND, lerp(angle, 0,360, 0,256), 0)
	tln.ResetLayerMode (LAYER_BACKGROUND)

	# input
	if tln.GetInput (tln.INPUT_LEFT):
		angle -= 2
	elif tln.GetInput (tln.INPUT_RIGHT):
		angle += 2
	
	if tln.GetInput (tln.INPUT_UP):
		s += a
		if s > 2:
			s = 2
	elif s >= a:
		s -= a
	
	if tln.GetInput (tln.INPUT_DOWN):
		s -= a
		if s < -2:
			s = -2
	elif s <= -a:
		s += a

	# scroll
	if s != 0:
		angle = angle % 360
		if angle < 0:
			angle += 360

		rad = math.radians(angle)
		x += (math.sin (rad) * s)
		y -= (math.cos (rad) * s)

	affine.angle = angle

	# render to window
	tln.DrawFrame (frame)
	frame += 1
	
tln.DeleteWindow ()
tln.Deinit ()
