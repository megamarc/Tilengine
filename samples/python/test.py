import tilengine as tln

engine = tln.Engine.create(400, 240, 1, 0, 20)
engine.set_load_path("../assets/sonic")
foreground = tln.Tilemap.fromfile("Sonic_md_fg1.tmx")
engine.layers[0].setup(foreground)

window = tln.Window.create()
while window.process():
	window.draw_frame()
