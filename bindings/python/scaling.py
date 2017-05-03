# imports
import tilengine as tln

# constants
WIDTH		= 400
HEIGHT		= 240
MIN_SCALE	= 50
MAX_SCALE	= 200

# module variables
xpos = 0
ypos = 192
scale = 100

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)

# load layer assets and basic setup
class Layer:
	def __init__(self, index, base_name):
		self.tileset = tln.LoadTileset (base_name + ".tsx")
		self.tilemap = tln.LoadTilemap (base_name + ".tmx")
		tln.layers[index].Setup (self.tileset, self.tilemap)

# setup engine
tln.Init (WIDTH, HEIGHT, 2,0,0)
tln.SetBGColor (34,136,170)

# setup layers
Layer (0, "psycho")
Layer (1, "rolo")
foreground = tln.layers[0]
background = tln.layers[1]

# main loop
window = tln.CreateWindow (None, tln.CWF_VSYNC)
while window.Process():
	# user input
	if window.GetInput (tln.INPUT_LEFT):
		xpos -= 1
	elif window.GetInput (tln.INPUT_RIGHT):
		xpos += 1
	if window.GetInput (tln.INPUT_UP) and ypos > 0:
		ypos -= 1
	elif window.GetInput (tln.INPUT_DOWN):
		ypos += 1
	if window.GetInput (tln.INPUT_A) and scale < MAX_SCALE:
		scale += 1
	elif window.GetInput (tln.INPUT_B) and scale > MIN_SCALE:
		scale -= 1

	# calculate scale factor from fixed point base
	fgscale = scale/100.0
	bgscale = lerp(scale, MIN_SCALE,MAX_SCALE, 0.75,1.5)

	# scale dependant lower clipping
	maxy = 640 - (HEIGHT*100/scale)
	if ypos > maxy:
		ypos = maxy
	
	# update position and scaling
	bgypos = lerp(scale,MIN_SCALE,MAX_SCALE, 0,80)
	foreground.SetPosition (xpos*2, ypos)
	background.SetPosition (xpos, bgypos)
	foreground.SetScaling (fgscale, fgscale)
	background.SetScaling (bgscale, bgscale)

	# render to the window
	window.DrawFrame ()
	
tln.Deinit ()
