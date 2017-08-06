import tilengine as tln
	
engine = tln.Engine.create(400,240,1,0,20)
engine.set_load_path("../assets/sonic")
foreground = tln.Tilemap.fromfile("sonic_md_fg1.tmx")
engine.layers[0].setup(foreground)

frame = 0
window = tln.Window.create(None, tln.WindowFlags.S1)
while window.process():
	window.draw_frame(frame)
	frame += 1

engine.delete()
