"""
Tilengine python example:
    Color cycle animation
"""

from tilengine import *

# initialise engine
engine = Engine.create(640, 480, 0, 0, 1)

# load resources
engine.set_load_path("../assets/color")
background = Bitmap.fromfile("beach.png")
sp = SequencePack.fromfile("beach.sqx")

# setup effect
engine.set_background_bitmap(background)
engine.animations[0].set_palette_animation(background.palette, sp.sequences["beach"], True)

# main loop
window = Window.create()
while window.process():
	None
