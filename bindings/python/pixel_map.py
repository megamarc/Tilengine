'''
Tilengine python example:
	Pixel mapping transform demo
'''

import tilengine as tln
from math import sin,cos,radians
import random

# helper constants
WIDTH 		 = 400
HEIGHT 		 = 240

# load layer assets and basic setup
def SetupLayer(layer, base_name):
	tileset = tln.LoadTileset (base_name + ".tsx")
	tilemap = tln.LoadTilemap (base_name + ".tmx")
	layer.Setup (tileset, tilemap)

# init
tln.Init (WIDTH,HEIGHT, 1,0,0)
foreground = tln.layers[0]

# setup layers
SetupLayer (foreground, "zelda")

# build pixel mapping table of WIDTH*HEIGHT PixelMap elements
num_pixels = WIDTH*HEIGHT
pixel_map = (tln.PixelMap*num_pixels)()
for y in range(HEIGHT):
	for x in range(WIDTH):
		index = y*WIDTH + x
		pixel_map[index].dx = x + int(sin(radians(x+y*3))*16)
		pixel_map[index].dy = y + int(sin(radians(x-y*3))*16)
foreground.SetPixelMapping (pixel_map)

# main loop
frame = 0
window = tln.CreateWindow (None, tln.CWF_VSYNC)
while window.Process ():
	foreground.SetPosition (40,frame)
	window.DrawFrame ()
	frame += 1
	
tln.Deinit ()
