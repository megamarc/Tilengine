"""
Tilengine python example:
	Layer scaling demo
"""

from tilengine import *

# constants
WIDTH = 400
HEIGHT = 240
MIN_SCALE = 50
MAX_SCALE = 200

# module variables
xpos = 0
ypos = 192
scale = 100


# linear interpolation
def lerp(x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0) * (x - x0) / (x1 - x0)


# load layer assets and basic setup
def setup_layer(layer, name):
	tilemap = Tilemap.fromfile(name)
	layer.setup(tilemap)
	engine.set_background_color(tilemap)


# setup engine
engine = Engine.create(WIDTH, HEIGHT, 2, 0, 0)
foreground = engine.layers[0]
background = engine.layers[1]

# setup layers
engine.set_load_path("../assets/fox")
setup_layer(foreground, "psycho.tmx")
setup_layer(background, "rolo.tmx")

# main loop
window = Window.create()
while window.process():
	# user input
	if window.get_input(Input.LEFT):
		xpos -= 1
	elif window.get_input(Input.RIGHT):
		xpos += 1
	if window.get_input(Input.UP) and ypos > 0:
		ypos -= 1
	elif window.get_input(Input.DOWN):
		ypos += 1
	if window.get_input(Input.A) and scale < MAX_SCALE:
		scale += 1
	elif window.get_input(Input.B) and scale > MIN_SCALE:
		scale -= 1

	# calculate scale factor from fixed point base
	fgscale = scale / 100.0
	bgscale = lerp(scale, MIN_SCALE, MAX_SCALE, 0.75, 1.5)

	# scale dependant lower clipping
	maxy = 640 - (HEIGHT * 100 / scale)
	if ypos > maxy:
		ypos = maxy

	# update position and scaling
	bgypos = lerp(scale, MIN_SCALE, MAX_SCALE, 0, 80)
	foreground.set_position(xpos * 2, ypos)
	background.set_position(xpos, bgypos)
	foreground.set_scaling(fgscale, fgscale)
	background.set_scaling(bgscale, bgscale)

engine.delete()
