"""
Tilengine python example:
	Use raster effects and clipping to fake progressive perspective scaling
"""

from tilengine import *
from math import tan, radians

# helper constants
WIDTH = 360
HEIGHT = 400
SKY_HEIGHT = 64
LAND_HEIGHT = HEIGHT - SKY_HEIGHT

# variables
world_y = 0


# load layer assets and basic setup
def setup_layer(layer, name):
	tilemap = Tilemap.fromfile(name)
	layer.setup(tilemap)
	engine.set_background_color(tilemap)


# linear interpolation
def lerp(x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0) * (x - x0) / (x1 - x0)


# raster effect function
def raster_effect(line):
	if line >= SKY_HEIGHT:
		line -= SKY_HEIGHT
		clouds.set_position(0, world_y * 2 + row_offsets[line] - line)
		terrain.set_position(0, world_y / 2 + row_offsets[line] - line)


# init
engine = Engine.create(WIDTH, HEIGHT, 3, 0, 0)
skyfog = engine.layers[0]
clouds = engine.layers[1]
terrain = engine.layers[2]

# setup layers
engine.set_load_path("../assets/zelda")
setup_layer(skyfog, "sky.tmx")
setup_layer(clouds, "clouds.tmx")
setup_layer(terrain, "zelda.tmx")
skyfog.set_blend_mode(Blend.ADD)
skyfog.set_clip(0, 0, WIDTH, SKY_HEIGHT + 63)
clouds.set_clip(0, SKY_HEIGHT, WIDTH, HEIGHT)
terrain.set_clip(0, SKY_HEIGHT, WIDTH, HEIGHT)
engine.set_background_color(Color(0, 0, 0))

# setup raster callback
engine.set_raster_callback(raster_effect)

# precalc raster effect values and store in rows list
row_offsets = [int(tan(radians(lerp(n, 0, LAND_HEIGHT, 105.0, 180.0))) * 240) for n in range(LAND_HEIGHT)]

# main loop
world_y = terrain.height - HEIGHT
window = Window.create()
while window.process():
	world_y -= 1
