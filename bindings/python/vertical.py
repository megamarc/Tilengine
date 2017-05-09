'''
Tilengine python example:
	Use raster effects and clipping to fake progressive perspective scaling
'''

from tilengine import *
from math import tan,radians

# helper constants
WIDTH 		 = 360
HEIGHT 		 = 400
SKY_HEIGHT 	 = 64
LAND_HEIGHT  = HEIGHT - SKY_HEIGHT

# variables
world_y = 6144 - 240

# load layer assets and basic setup
def setup_layer(layer, base_name):
	tileset = Tileset.fromfile (base_name + ".tsx")
	tilemap = Tilemap.fromfile (base_name + ".tmx")
	layer.setup (tileset, tilemap)

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)
	
# raster effect function
def raster_effect (line):
	if line >= SKY_HEIGHT:
		line -= SKY_HEIGHT
		clouds.set_position (0,world_y*2 + rows[line] - line)
		terrain.set_position (0,world_y/2 + rows[line] - line)

# init
tln = Engine.create (WIDTH,HEIGHT, 3,0,0)
tln.set_background_color (Color(0,0,0))
skyfog  = tln.layers[0]
clouds  = tln.layers[1]
terrain = tln.layers[2]

# setup layers
setup_layer (skyfog,  "sky")
setup_layer (clouds,  "clouds")
setup_layer (terrain, "zelda")
skyfog.set_blend_mode (BLEND_ADD)
skyfog.set_clip  (0,0, WIDTH,SKY_HEIGHT + 63)
clouds.set_clip  (0,SKY_HEIGHT, WIDTH,HEIGHT)
terrain.set_clip (0,SKY_HEIGHT, WIDTH,HEIGHT)

# setup raster callback
my_raster_callback = raster_callback_function(raster_effect)
tln.set_raster_callback (my_raster_callback)

# precalc raster effect values and store in rows list
rows = [int(tan(radians(lerp(n, 0,LAND_HEIGHT, 105.0,180.0))) * 240) for n in range(LAND_HEIGHT)]

# main loop
window = Window.create()
while window.process ():
	window.draw_frame ()
	world_y -= 1

tln.delete()
