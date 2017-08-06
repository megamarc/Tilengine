"""
Tilengine python example:
    Color cycle animation
"""

from tilengine import *

# initialise engine
engine = Engine.create(640,480,0,0,1)

# load resources
engine.set_load_path("../assets/color")
background = Bitmap.fromfile("beach.png")
sp		   = SequencePack.fromfile("beach.sqx")
sequence   = sp.find_sequence("beach")

# setup effect
engine.set_background_bitmap(background)
engine.animations[0].set_palette_animation(background.palette, sequence, True)

# main loop
frame = 0
window = Window.create()
while window.process():
	window.draw_frame(frame)
	frame += 1

# release resources
background.delete()
sp.delete()
engine.delete()
