local TLN = require("tileengine_h")
local ffi = require("ffi")
local C = ffi.load'Tilengine'

--local tln = {C=C}

ffi.cdef [[
    TLN_Tilemap tilemap;
]]

map ={}
local function init(hres, vres, numlayers, numsprites, numanimations )
    return C.TLN_Init(hres, vres, numlayers, numsprites, numanimations)
end 

local function set_loadPath(path)
    local path = ffi.string(path, #path)
    return C.TLN_SetLoadPath(path)
end 


local function load_tilemap(path,layername )
    return C.TLN_LoadTilemap(path, layername)
end


init(400, 240, 1,0,0)
local frame = 0
local path = "../assets/sonic"
set_loadPath(path)
map = load_tilemap("Sonic_md_fg1.tmx", nil)

C.TLN_SetLayer(0, nil, map)
C.TLN_SetBGColor(32, 32, 128)


C.TLN_CreateWindow(NULL,0)

while C.TLN_ProcessWindow() do
    C.TLN_SetLayerPosition(0, frame, 0);
    C.TLN_DrawFrame(frame)
    frame = frame + 1
end

C.TLN_DeleteTilemap(map)
C.TLN_Deinit()