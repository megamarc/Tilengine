'''
Tilengine python example:
	Use raster effects and clipping to fake progressive perspective scaling
'''

import tilengine as tln
from math import tan,radians

# helper constants
WIDTH 		 = 360
HEIGHT 		 = 400
SKY_HEIGHT 	 = 64
LAND_HEIGHT  = HEIGHT - SKY_HEIGHT

# variables
world_y = 6144 - 240

# load layer assets and basic setup
def SetupLayer(layer, base_name):
	tileset = tln.LoadTileset (base_name + ".tsx")
	tilemap = tln.LoadTilemap (base_name + ".tmx")
	layer.Setup (tileset, tilemap)

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)
	
# raster effect function
def raster_effect (line):
	if line >= SKY_HEIGHT:
		line -= SKY_HEIGHT
		clouds.SetPosition (0,world_y*2 + rows[line] - line)
		terrain.SetPosition (0,world_y/2 + rows[line] - line)

# init
tln.Init (WIDTH,HEIGHT, 3,0,0)
tln.SetBGColor (0,0,0)
skyfog  = tln.layers[0]
clouds  = tln.layers[1]
terrain = tln.layers[2]

# setup layers
SetupLayer (skyfog,  "sky")
SetupLayer (clouds,  "clouds")
SetupLayer (terrain, "zelda")
skyfog.SetBlendMode (tln.BLEND_ADD)
skyfog.SetClip  (0,0, WIDTH,SKY_HEIGHT + 63)
clouds.SetClip  (0,SKY_HEIGHT, WIDTH,HEIGHT)
terrain.SetClip (0,SKY_HEIGHT, WIDTH,HEIGHT)

# setup raster callback
my_raster_callback = tln.RasterCallbackFunc(raster_effect)
tln.SetRasterCallback (my_raster_callback)

# precalc raster effect values and store in rows list
rows = [int(tan(radians(lerp(n, 0,LAND_HEIGHT, 105.0,180.0))) * 240) for n in range(LAND_HEIGHT)]

# main loop
window = tln.CreateWindow (None, tln.CWF_VSYNC)
while window.Process ():
	window.DrawFrame ()
	world_y -= 1

tln.Deinit ()
