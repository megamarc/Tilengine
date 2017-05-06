'''
Tilengine python example:
	Mosaic effect demo
'''

import tilengine as tln

# helper constants
WIDTH 		 = 400
HEIGHT 		 = 240

# variables
frame = 0
pixel_size = 0		# pixel size
pixel_delta = 0.4	# pixel size variation per frame

# load layer assets and basic setup
def SetupLayer(layer, base_name):
	tileset = tln.LoadTileset (base_name + ".tsx")
	tilemap = tln.LoadTilemap (base_name + ".tmx")
	layer.Setup (tileset, tilemap)

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)
	
# init
tln.Init (WIDTH,HEIGHT, 3,0,0)
tln.SetBGColor (28,0,140)
foreground = tln.layers[0]
background = tln.layers[1]

# setup layers
SetupLayer(foreground, "Sonic_md_fg1")
SetupLayer(background, "Sonic_md_bg1")

# main window loop
target_layer = foreground
window = tln.CreateWindow (None, tln.CWF_VSYNC)
while window.Process ():
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
	
	target_layer.SetMosaic (size,size)
	foreground.SetPosition (int(frame),0)
	background.SetPosition (int(frame/2),0)
	window.DrawFrame(frame)

tln.Deinit ()
