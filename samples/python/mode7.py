"""
Tilengine python example:
    Raster based affine transform to simulate perspective, SNES Mode-7 style
"""

from tilengine import *
from math import sin, cos, radians

# constants
WIDTH = 400
HEIGHT = 240

# module variables
pos_x = -136
pos_y = 336
speed = 0
angle = 0


class affine:
	angle = 0
	sx = 1
	sy = 1
	dx = WIDTH / 2
	dy = HEIGHT


# linear interpolation
def lerp(pos_x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0) * (pos_x - x0) / (x1 - x0)


def raster_effect(line):
	if line == 24:
		background.setup(map_track)
		background.set_position(pos_x, pos_y)
		foreground.disable()

	if line > 24:
		scale = lerp(line, 24, HEIGHT, 0.2, 5.0)
		affine.sx = scale
		affine.sy = scale
		background.set_transform(affine.angle, affine.dx, affine.dy, affine.sx, affine.sy)


# setup engine
engine = Engine.create(WIDTH, HEIGHT, 2, 0, 0)
engine.set_background_color(Color(0, 0, 0))
foreground = engine.layers[0]
background = engine.layers[1]

# load resources
engine.set_load_path("../assets/smk")
map_horizon = Tilemap.fromfile("track1_bg.tmx")
map_track = Tilemap.fromfile("track1.tmx")

# set raster callback
engine.set_raster_callback(raster_effect)

# main loop
window = Window.create()
while window.process():
	foreground.setup(map_horizon)
	background.setup(map_horizon)
	foreground.set_position(lerp(angle * 2, 0, 360, 0, 256), 24)
	background.set_position(lerp(angle, 0, 360, 0, 256), 0)
	background.reset_mode()

	# input: turn left/right
	if window.get_input(Input.LEFT):
		angle -= 2
	elif window.get_input(Input.RIGHT):
		angle += 2

	# input: accelerate forward
	if window.get_input(Input.UP):
		speed = min(speed + 0.2, 2)
	elif speed > 0:
		speed = max(speed - 0.1, 0)

	# input: accelerate backwards
	if window.get_input(Input.DOWN):
		speed = max(speed - 0.2, -2)
	elif speed < 0:
		speed = min(speed + 0.1, 0)

	# displacement
	if speed != 0:
		angle = (angle + 360) % 360
		rad = radians(angle)
		pos_x += (sin(rad) * speed)
		pos_y -= (cos(rad) * speed)

	affine.angle = angle
