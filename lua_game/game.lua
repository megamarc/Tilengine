-- minimal script that integrates with tilengine_libretro core

require("tilengine_libretro")
local ffi = require("ffi")
local tln = ffi.load 'tilengine_libretro'

x = 0

-- tilengine configuration
config = {
	hres = 320,
	vres = 240,
	numlayers = 4,
	numsprites = 128,
	numanimations = 32
}

-- called once to load assets
function game_load()
	tln.TLN_SetLoadPath("../samples/assets/sonic")
	foreground = tln.TLN_LoadTilemap("Sonic_md_fg1.tmx", nil)
	background = tln.TLN_LoadTilemap("Sonic_md_bg1.tmx", nil)
	tln.TLN_SetLayer(0, nil, foreground)
	tln.TLN_SetLayer(1, nil, background)
	tln.LUA_SetRasterCallback("rasters")
	print("game_load")
end

-- called every frame
function game_loop(frame)
	-- update visuals
	tln.TLN_SetLayerPosition(0, x*2, 0)
	tln.TLN_SetLayerPosition(1, x, 0)
	tln.TLN_SetBGColor(0x1B, 0x00, 0x8B)
	
	-- process input
	if LUA_CheckInput(0, INPUT_LEFT) then x = x - 2 end
	if LUA_CheckInput(0, INPUT_RIGHT) then x = x + 2 end
end

-- called at end
function game_unload()
	tln.TLN_DeleteTilemap(foreground)
	tln.TLN_DeleteTilemap(background)
	print("game_unload")
end

-- optional raster callback, registered with LUA_SetRasterCallback()
function rasters(line)
	if line == 144 then tln.TLN_SetBGColor(0x24, 0x92, 0xDB) end
end
