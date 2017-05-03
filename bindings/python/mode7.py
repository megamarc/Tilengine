"""
Tilengine perspective projection demo
  Cursors or joystick d-pad: scroll
"""

# imports
import tilengine as tln
from math import sin, cos, radians

# constants
WIDTH	= 400
HEIGHT	= 240

#module variables
pos_x = -136
pos_y = 336
speed = 0
angle = 0

class affine:
	angle = 0
	sx = 1
	sy = 1
	dx = WIDTH/2
	dy = HEIGHT

# load map assets
class Map:
	def __init__(self, base_name):
		self.tileset = tln.LoadTileset (base_name + ".tsx")
		self.tilemap = tln.LoadTilemap (base_name + ".tmx")

# linear interpolation
def lerp (pos_x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(pos_x - x0)/(x1 - x0)
	
def raster_callback (line):
	if line == 24:
		background.Setup (map_track.tileset, map_track.tilemap);
		background.SetPosition (pos_x, pos_y);
		foreground.Disable ();

	if line > 24:
		scale = lerp (line, 24,HEIGHT, 0.2,5.0);
		affine.sx = scale;
		affine.sy = scale;		
		background.SetTransform (affine.angle, affine.dx, affine.dy, affine.sx, affine.sy)

# setup engine
tln.Init (WIDTH, HEIGHT, 2,0,0)
tln.SetBGColor (0,0,0)
foreground = tln.layers[0]
background = tln.layers[1]

# load resources
map_horizon = Map("track1_bg")
map_track   = Map("track1")

# set raster callback
cb_func = tln.RasterCallbackFunc(raster_callback)
tln.SetRasterCallback (cb_func)

# main loop
window = tln.CreateWindow (None, tln.CWF_VSYNC)
while window.Process ():
	foreground.Setup (map_horizon.tileset, map_horizon.tilemap)
	background.Setup (map_horizon.tileset, map_horizon.tilemap)
	foreground.SetPosition (lerp(angle*2, 0,360, 0,256), 24)
	background.SetPosition (lerp(angle, 0,360, 0,256), 0)
	background.ResetMode ()

	# input: turn left/right
	if window.GetInput (tln.INPUT_LEFT):
		angle -= 2
	elif window.GetInput (tln.INPUT_RIGHT):
		angle += 2
	
	# input: accelerate forward
	if window.GetInput (tln.INPUT_UP):
		speed = min(speed + 0.2, 2)
	elif speed > 0:
		speed = max(speed - 0.1, 0)
	
	# input: accelerate backwards
	if window.GetInput (tln.INPUT_DOWN):
		speed = max(speed - 0.2, -2)
	elif speed < 0:
		speed = min(speed + 0.1, 0)

	# displacement
	if speed != 0:
		angle = (angle + 360) % 360
		rad = radians(angle)
		pos_x += (sin (rad) * speed)
		pos_y -= (cos (rad) * speed)

	affine.angle = angle

	# render to window
	window.DrawFrame ()
	
tln.Deinit ()
