"""
Tilengine python example:
	Raster effect to simulate depth with scroll strips and linescroll
"""

from tilengine import *

# module variables
frame = 0
basepos = 0
speed = 0
pos_background = [0, 0, 0, 0, 0, 0]


# linear interpolation
def lerp(x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0) * (x - x0) / (x1 - x0)


# load layer assets and basic setup
def setup_layer(layer, name):
	tilemap = Tilemap.fromfile(name)
	layer.setup(tilemap)
	engine.set_background_color(tilemap)


# raster effect callback
def raster_effect(line):
	pos = -1

	if line is 0:
		pos = pos_background[0]
	elif line is 32:
		pos = pos_background[1]
	elif line is 48:
		pos = pos_background[2]
	elif line is 64:
		pos = pos_background[3]
	elif line is 112:
		pos = pos_background[4]
	elif line >= 152:
		pos = lerp(line, 152, 224, pos_background[4], pos_background[5])

	if pos is not -1:
		background.set_position(pos, 0)

	if line is 0:
		engine.set_background_color(Color(28, 0, 140))
	elif line is 144:
		engine.set_background_color(Color(0, 128, 238))


# initialise 400x240 resolution, 2 background layers, no sprites and 20 animation slots
engine = Engine.create(400, 240, 2, 0, 20)
foreground = engine.layers[0]
background = engine.layers[1]

# setup layers
engine.set_load_path("../assets/sonic")
setup_layer(foreground, "Sonic_md_fg1.tmx")
setup_layer(background, "Sonic_md_bg1.tmx")

# color cycle animation for water
sequence_pack = SequencePack.fromfile("Sonic_md_seq.sqx")
sequence = sequence_pack.find_sequence("seq_water")
palette = background.get_palette();
animation = engine.get_available_animation()
engine.animations[animation].set_palette_animation(palette, sequence, True);

# setup raster callback
engine.set_raster_callback(raster_effect)

# main loop
window = Window.create()
while window.process():

	# process user input
	if window.get_input(Input.RIGHT):
		speed = min(speed + 0.04, 1.0)
	elif speed > 0:
		speed = max(speed - 0.02, 0.0)

	if window.get_input(Input.LEFT):
		speed = max(speed - 0.04, -1.0)
	elif speed < 0:
		speed = min(speed + 0.02, 0.0)

	# scroll
	basepos += speed
	pos_foreground = basepos * 3
	foreground.set_position(pos_foreground, 0)

	pos_background[0] = basepos * 0.562
	pos_background[1] = basepos * 0.437
	pos_background[2] = basepos * 0.375
	pos_background[3] = basepos * 0.625
	pos_background[4] = basepos * 1.000
	pos_background[5] = basepos * 2.000

	# draw frame
	window.draw_frame(frame)
	frame += 1

engine.delete()
