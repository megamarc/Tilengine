"""Interactive version of the scaling & blend sample to load from the interpreter"""

# imports
import tilengine as tln

# constants
LAYER_FOREGROUND = 0
LAYER_BACKGROUND = 1
WIDTH		= 400
HEIGHT		= 240
MIN_SCALE	= 0.5
MAX_SCALE	= 2.0

# module variables
xpos = 0
ypos = 192
scale = 1.0
alpha = 255
frame = 0

# linear interpolation
def lerp (x, x0, x1, fx0, fx1):
	return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0)

# setup layer helper
def SetupLayer (nlayer, name):
	tileset = tln.LoadTileset (name + ".tsx")
	tilemap = tln.LoadTilemap (name + ".tmx", "Layer 1")
	tln.SetLayer (nlayer, tileset, tilemap)	

def Update ():
	global ypos
	
	# calculate background scale factor
	fgscale = scale
	bgscale = lerp(scale, MIN_SCALE,MAX_SCALE, 0.75,1.5)

	# scale dependant lower clipping
	maxy = 640 - (HEIGHT/scale)
	if ypos > maxy:
		ypos = maxy
	
	# update position
	bgypos = lerp(scale,MIN_SCALE,MAX_SCALE, 0,80)
	tln.SetLayerPosition (LAYER_FOREGROUND, int(xpos*2), int(ypos))
	tln.SetLayerPosition (LAYER_BACKGROUND, int(xpos), int(bgypos))
	tln.SetLayerScaling (LAYER_FOREGROUND, fgscale, fgscale)
	tln.SetLayerScaling (LAYER_BACKGROUND, bgscale, bgscale)

def SetPosition (x,y):
	global xpos,ypos
	xpos = x
	ypos = y
	Update ()
	
def SetScale (s):
	global scale
	if s > MAX_SCALE:
		s = MAX_SCALE
	if s < MIN_SCALE:
		s = MIN_SCALE
	scale = s
	Update ()	
	
def SetAlpha (alpha):
	if alpha < 255:
		tln.SetLayerBlendMode (LAYER_FOREGROUND, tln.BLEND_MIX, alpha)
	else:
		tln.SetLayerBlendMode (LAYER_FOREGROUND, tln.BLEND_NONE, 0)

# setup engine
tln.Init (WIDTH, HEIGHT, 2,0,0)
tln.CreateWindowThread ("overlay2.bmp", tln.CWF_S2)
tln.SetBGColor (34,136,170)

# setup layers
SetupLayer (LAYER_FOREGROUND, "psycho")
SetupLayer (LAYER_BACKGROUND, "rolo")
Update ()
		
