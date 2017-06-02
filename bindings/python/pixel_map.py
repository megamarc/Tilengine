'''
Tilengine python example:
	Pixel mapping transform demo
'''

from tilengine import *
from math import sin,cos,radians
import random

# helper constants
WIDTH 		 = 400
HEIGHT 		 = 240

# load layer assets and basic setup
def setup_layer(layer, name):
	tilemap = Tilemap.fromfile (name)
	layer.set_map (tilemap)
	layer.set_background_color_tilemap (tilemap)

# init
tln = Engine.create (WIDTH,HEIGHT, 1,0,0)
foreground = tln.layers[0]

# setup layers
setup_layer (foreground, "zelda.tmx")

# build pixel mapping table of WIDTH*HEIGHT PixelMap elements
num_pixels = WIDTH*HEIGHT
pixel_map = (PixelMap*num_pixels)()
for y in range(HEIGHT):
	for x in range(WIDTH):
		index = y*WIDTH + x
		pixel_map[index].dx = x + int(sin(radians(x+y*3))*16)
		pixel_map[index].dy = y + int(sin(radians(x-y*3))*16)
foreground.set_pixel_mapping (pixel_map)

# main loop
frame = 0
window = Window.create()
while window.process ():
	foreground.set_position (40,frame)
	window.draw_frame ()
	frame += 1
	
tln.delete()
