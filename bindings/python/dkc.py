""" Donkey Kong Country background recreation with Tilengine """

from tilengine import *

# helper constants
WIDTH 	= 400
HEIGHT 	= 240

bg_colors = (
	Color(0x00,0x00,0x32),
	Color(0xf8,0xf8,0xf0),
	Color(0xd8,0xd0,0x70),
	Color(0x40,0x68,0x48),
	Color(0x00,0x00,0x00),
	Color(0x00,0x28,0x00),
	Color(0x00,0x00,0x00))

bg3_colors = (
	Color(0x68,0x80,0x60), 
	Color(0x28,0x30,0x28), 
	Color(0x20,0x40,0x20), 
	Color(0x00,0x00,0x00))

# load layer assets and basic setup
def setup_layer(layer, base_name):
	tileset = Tileset.fromfile (base_name + ".tsx")
	tilemap = Tilemap.fromfile (base_name + ".tmx")
	layer.setup (tileset, tilemap)

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)
	
# interpolates a color between other two
def interpolate_color(x, x0, x1, color1, color2):
	r = lerp(x, x0,x1, color1.r, color2.r)
	g = lerp(x, x0,x1, color1.g, color2.g)
	b = lerp(x, x0,x1, color1.b, color2.b)
	return Color(r,g,b)
	
def clamp(value,min,max):
	if value < min:
		value = min
	elif value > max:
		value = max
	return value

# raster effect function: generate sky and parallax background
def raster_effect (line):

	# background color
	if line < 56:
		tln.set_background_color(interpolate_color(line, 0,56, bg_colors[0], bg_colors[1]))
	elif line < 72:
		tln.set_background_color(interpolate_color(line, 56,72, bg_colors[1], bg_colors[2]))
	elif line < 88:
		tln.set_background_color(interpolate_color(line, 72,88, bg_colors[3], bg_colors[4]))
	elif line < 96:
		tln.set_background_color(interpolate_color(line, 88,96, bg_colors[4], bg_colors[5]))
	elif line < 112:
		tln.set_background_color(interpolate_color(line, 96,112, bg_colors[5], bg_colors[6]))
		
	# bg3 color 1
	if line >= 66 and line < 76:
		bg3_palette.set_color(1, interpolate_color(line, 66,76, bg3_colors[0], bg3_colors[1]))
	elif line >= 82 and line < 96:
		bg3_palette.set_color(1, interpolate_color(line, 82,96, bg3_colors[2], bg3_colors[3]))
	
	# bg3 horizontal position
	if line == 64:
		bg3.set_position(x_world/7, 0)
	elif line == 72:
		bg3.set_position(x_world/6, 0)
	elif line == 80:
		bg3.set_position(x_world/5, 0)
	elif line == 96:
		bg3.set_position(x_world/4, 0)		
	
	return
	
# init
tln = Engine.create (WIDTH,HEIGHT, 3,0,0)
tln.set_background_color (Color(0,0,0))
bg1 = tln.layers[0]
bg2 = tln.layers[1]
bg3 = tln.layers[2]

# setup layers
setup_layer(bg1, "dkc_bg1")
setup_layer(bg2, "dkc_bg2")
setup_layer(bg3, "dkc_bg3")
bg3_palette = bg3.get_palette()
bg3.set_clip(0,0, WIDTH,112)

# setup raster effect callback
my_raster_callback = raster_callback_function(raster_effect)
tln.set_raster_callback (my_raster_callback)

# init game variables
x_world = 0
y_world = 0
x_speed = 0
y_speed = 0
max_speed = 3
accel = 0.2
brake = 0.05

# create window and main loop
window = Window.create()
while window.process ():

	# user input: movement
	if window.get_input(INPUT_RIGHT):
		x_speed = min(x_speed + accel, max_speed)
	elif x_speed > 0:
		x_speed = max(x_speed - brake, 0)
	
	if window.get_input(INPUT_LEFT):
		x_speed = max(x_speed - accel, -max_speed)
	elif x_speed < 0:
		x_speed = min(x_speed + brake, 0)
	
	if window.get_input(INPUT_DOWN):
		y_speed = min(y_speed + accel, max_speed)
	elif y_speed > 0:
		y_speed = max(y_speed - brake, 0)
	
	if window.get_input(INPUT_UP):
		y_speed = max(y_speed - accel, -max_speed)
	elif y_speed < 0:
		y_speed = min(y_speed + brake, 0)

	# update world
	x_world += x_speed
	y_world += y_speed
	x_world = clamp(x_world,0,4976)
	y_world = clamp(y_world,0,240)
	bg1.set_position(x_world, y_world)
	bg2.set_position(x_world/2, y_world/3)
	window.draw_frame ()

tln.delete ()
