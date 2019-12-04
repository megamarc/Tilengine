-- minimal script that integrates with tilengine_libretro core

require("Tilengine_h")
local ffi = require("ffi")
local tln = ffi.load 'tilengine_libretro'

-- tilengine configuration
config = {
	hres = 320,
	vres = 240,
	numlayers = 4,
	numsprites = 128,
	numanimations = 32,
}

-- called once to load assets
function game_load()
	tln.TLN_SetLoadPath("../samples/assets/sonic")
	foreground = tln.TLN_LoadTilemap("Sonic_md_fg1.tmx", nil)
	background = tln.TLN_LoadTilemap("Sonic_md_bg1.tmx", nil)
	tln.TLN_SetLayer(0, nil, foreground);
	tln.TLN_SetLayer(1, nil, background);
	print("game_load")
end

-- called every frame
function game_loop(frame)
	tln.TLN_SetLayerPosition(0, frame*2, 0);
	tln.TLN_SetLayerPosition(1, frame, 0);
	tln.TLN_SetBGColor(frame/4, frame/2, frame/4);
end

-- called at end
function game_unload()
	tln.TLN_DeleteTilemap(foreground);
	tln.TLN_DeleteTilemap(background);
	print("game_unload")
end
