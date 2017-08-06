"""
Tilengine python example:
	Mosaic effect demo
"""

from tilengine import *

# helper constants
WIDTH = 400
HEIGHT = 240

# variables
frame = 0
pixel_size = 0  # pixel size
pixel_delta = 0.4  # pixel size variation per frame


# load layer assets and basic setup
def setup_layer(layer, name):
	tilemap = Tilemap.fromfile(name)
	layer.setup(tilemap)
	engine.set_background_color(tilemap)


# linear interpolation
def lerp(x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0) * (x - x0) / (x1 - x0)


# init
engine = Engine.create(WIDTH, HEIGHT, 3, 0, 0)
foreground = engine.layers[0]
background = engine.layers[1]

# setup layers
engine.set_load_path("../assets/sonic")
setup_layer(foreground, "Sonic_md_fg1.tmx")
setup_layer(background, "Sonic_md_bg1.tmx")

# main window loop
target_layer = foreground
window = Window.create()
while window.process():
	frame += 1
	pixel_size += pixel_delta
	if pixel_size >= 16:
		pixel_delta = -pixel_delta
	if pixel_size <= 0:
		pixel_delta = -pixel_delta
		if target_layer == foreground:
			target_layer = background
		else:
			target_layer = foreground

	size = int(pixel_size)

	target_layer.set_mosaic(size, size)
	foreground.set_position(int(frame), 0)
	background.set_position(int(frame / 2), 0)
	window.draw_frame(frame)

engine.delete()
