'''
Tilengine python example:
	Raster based affine transform to simulate perspective, SNES Mode-7 style
'''

from tilengine import *
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
		self.tileset = Tileset.fromfile (base_name + ".tsx")
		self.tilemap = Tilemap.fromfile (base_name + ".tmx")

# linear interpolation
def lerp (pos_x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(pos_x - x0)/(x1 - x0)
	
def raster_callback (line):
	if line == 24:
		background.setup (map_track.tileset, map_track.tilemap);
		background.set_position (pos_x, pos_y);
		foreground.disable ();

	if line > 24:
		scale = lerp (line, 24,HEIGHT, 0.2,5.0);
		affine.sx = scale;
		affine.sy = scale;		
		background.set_transform (affine.angle, affine.dx, affine.dy, affine.sx, affine.sy)

# setup engine
tln = Engine.create (WIDTH, HEIGHT, 2,0,0)
tln.set_background_color (Color(0,0,0))
foreground = tln.layers[0]
background = tln.layers[1]

# load resources
map_horizon = Map("track1_bg")
map_track   = Map("track1")

# set raster callback
cb_func = raster_callback_function(raster_callback)
tln.set_raster_callback (cb_func)

# main loop
window = Window.create ()
while window.process ():
	foreground.setup (map_horizon.tileset, map_horizon.tilemap)
	background.setup (map_horizon.tileset, map_horizon.tilemap)
	foreground.set_position (lerp(angle*2, 0,360, 0,256), 24)
	background.set_position (lerp(angle, 0,360, 0,256), 0)
	background.reset_mode ()

	# input: turn left/right
	if window.get_input (INPUT_LEFT):
		angle -= 2
	elif window.get_input (INPUT_RIGHT):
		angle += 2
	
	# input: accelerate forward
	if window.get_input (INPUT_UP):
		speed = min(speed + 0.2, 2)
	elif speed > 0:
		speed = max(speed - 0.1, 0)
	
	# input: accelerate backwards
	if window.get_input (INPUT_DOWN):
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
	window.draw_frame ()
	
tln.delete()
