'''
Tilengine python example:
	Column offset feature demo
'''

import tilengine as tln
from ctypes import c_int
from math import sin,radians

WIDTH  = 400
HEIGHT = 240

# load layer assets and basic setup
def SetupLayer(layer, base_name):
	tileset = tln.LoadTileset (base_name + ".tsx")
	tilemap = tln.LoadTilemap (base_name + ".tmx")
	layer.Setup (tileset, tilemap)

# initialise
tln.Init (WIDTH,HEIGHT,3,0,0)
foreground = tln.layers[0]
background = tln.layers[1]
base_layer = tln.layers[2]

# setup the layers
SetupLayer (foreground, "Sonic_md_fg1")
SetupLayer (background, "Sonic_md_bg1")
SetupLayer (base_layer, "Base")

# create and assign column offset array
num_columns = int(WIDTH/8 + 2)
columns = (c_int*num_columns)()
foreground.SetColumnOffset (columns)

# game loop
x_world = 0
window = tln.CreateWindow (None, tln.CWF_VSYNC)
while window.Process ():
	for n in range(num_columns):
		columns[n] = int(sin(radians(x_world*4 + n*7)) * ((abs(sin(radians(x_world/4))*12)) + 4))
	foreground.SetPosition (x_world, 0)
	background.SetPosition (x_world/4, 0)
	window.DrawFrame ()
	x_world += 2
	
# release resources
tln.Deinit ()
