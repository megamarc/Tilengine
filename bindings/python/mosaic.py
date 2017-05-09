'''
Tilengine python example:
	Mosaic effect demo
'''

from tilengine import *

# helper constants
WIDTH 		 = 400
HEIGHT 		 = 240

# variables
frame = 0
pixel_size = 0		# pixel size
pixel_delta = 0.4	# pixel size variation per frame

# load layer assets and basic setup
def setup_layer(layer, base_name):
	tileset = Tileset.fromfile (base_name + ".tsx")
	tilemap = Tilemap.fromfile (base_name + ".tmx")
	layer.setup (tileset, tilemap)

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)
	
# init
tln = Engine.create (WIDTH,HEIGHT,3,0,0)
tln.set_background_color (Color(28,0,140))
foreground = tln.layers[0]
background = tln.layers[1]

# setup layers
setup_layer(foreground, "Sonic_md_fg1")
setup_layer(background, "Sonic_md_bg1")

# main window loop
target_layer = foreground
window = Window.create()
while window.process ():
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
	
	target_layer.set_mosaic (size,size)
	foreground.set_position (int(frame),0)
	background.set_position (int(frame/2),0)
	window.draw_frame(frame)

tln.delete()
