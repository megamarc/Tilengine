'''
Tilengine python example:
	Raster effect to simulate depth with scroll strips and linescroll
'''

import tilengine as tln
from ctypes import *

# module variables
frame = 0
basepos = 0
speed = 0
pos_background = [0, 0, 0, 0, 0, 0]

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)
	
# load layer assets and basic setup
def SetupLayer(layer, base_name):
	tileset = tln.LoadTileset (base_name + ".tsx")
	tilemap = tln.LoadTilemap (base_name + ".tmx")
	layer.Setup (tileset, tilemap)

# raster effect callback
def raster_effect (line):
	pos = -1
	
	if line == 0:
		pos = pos_background[0]
	elif line == 32:
		pos = pos_background[1]
	elif line == 48:
		pos = pos_background[2]
	elif line == 64:
		pos = pos_background[3]
	elif line == 112:
		pos = pos_background[4]
	elif line >= 152:
		pos = lerp (line, 152,224, pos_background[4], pos_background[5])
		
	if pos != -1:
		background.SetPosition (pos, 0)

	if line == 0:
		tln.SetBGColor (28,0,140)	
	elif line == 144:
		tln.SetBGColor (0,128,238)

# initialise
tln.Init (400,240,2,80,1)
foreground = tln.layers[0]
background = tln.layers[1]

# setup layers
SetupLayer (foreground, "Sonic_md_fg1")
SetupLayer (background, "Sonic_md_bg1")

# color cycle animation
sp = tln.LoadSequencePack ("Sonic_md_seq.sqx")
sequence = sp.FindSequence ("seq_water")
palette  = background.GetPalette ()
tln.animations[0].SetPaletteAnimation (palette, sequence, True)

# setup raster callback
myRasterCallback = tln.RasterCallbackFunc(raster_effect)
tln.SetRasterCallback (myRasterCallback)

# main loop
window = tln.CreateWindow (None, tln.CWF_VSYNC)
while window.Process():

	# process user input
	if window.GetInput (tln.INPUT_RIGHT):
		speed = min(speed + 0.04, 1.0)
	elif speed > 0:
		speed = max(speed - 0.02, 0.0)
		 
	if window.GetInput (tln.INPUT_LEFT):
		speed = max(speed - 0.04, -1.0)
	elif speed < 0:
		speed = min(speed + 0.02, 0.0)

	# scroll
	basepos += speed
	pos_foreground = basepos * 3
	foreground.SetPosition (pos_foreground, 0);

	pos_background[0] = basepos * 0.562
	pos_background[1] = basepos * 0.437
	pos_background[2] = basepos * 0.375
	pos_background[3] = basepos * 0.625
	pos_background[4] = basepos * 1.000
	pos_background[5] = basepos * 2.000
	
	# draw frame line by line doing raster effects
	window.DrawFrame (frame)
	frame += 1
	
tln.Deinit ()
