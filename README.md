![Tilengine logo](Tilengine.png)
# tilengine_libretro - The 2D retro graphics engine core for libretro
[![License: MPL 2.0](https://img.shields.io/badge/License-MPL%202.0-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0)
[![Build Status](https://travis-ci.org/megamarc/Tilengine.svg?branch=libretro)](https://travis-ci.org/megamarc/Tilengine)
[![Build status](https://ci.appveyor.com/api/projects/status/yvx7koa32qyjm1b0?svg=true)](https://ci.appveyor.com/project/megamarc/tilengine)
[![Coverity Scan](https://scan.coverity.com/projects/16964/badge.svg)](https://scan.coverity.com/projects/megamarc-tilengine)

Tilengine is an open source, cross-platform 2D graphics engine for creating classic/retro games with tile maps, sprites and palettes. Its unique scanline-based rendering algorithm makes raster effects a core feature, a technique used by many games running on real 2D graphics chips. It integrates inside the RetroArch emulator (https://www.retroarch.com/) and loads games written in lua (http://www.lua.org/).

This version has the same exact functionality than standard Tilengine, but removing the built-in window support, and adding Retroarch integration

http://www.tilengine.org

# Contents
- [tilengine_libretro - The 2D retro graphics engine core for libretro](#tilenginelibretro---the-2d-retro-graphics-engine-core-for-libretro)
- [Contents](#contents)
- [Features](#features)
- [Minimal game template](#minimal-game-template)
  - [Checking player input](#checking-player-input)
  - [Enabling raster effects](#enabling-raster-effects)
- [Running the game (Windows)](#running-the-game-windows)
- [Documentation](#documentation)

# Features
* Written in portable C (C99)
* MPL 2.0 license: free for any project, including commercial ones, allows console development
* Cross platform: available builds for Windows (32/64), Linux PC(32/64), Mac OS X and Raspberry Pi
* Streamlined, easy to learn API that requires very little lines of code
* Loads assets from open standard standard file formats
* Create or modify graphic assets procedurally at run time
* True raster effects: modify render parameters between scanlines
* Background layer scaling and rotation
* Sprite scaling
* Several blending modes for layers and sprites
* Pixel accurate sprite vs sprite and sprite vs layer collision detection
* Special effects: per-column offset, mosaic, per-pixel displacement, CRT emulation...
* Supports packaged assets with optional AES-128 encryption

# Minimal game template

Use this lua template for your own games:

```lua
-- required dependencies
require("tilengine_libretro")
local ffi = require("ffi")
local tln = ffi.load 'tilengine_libretro'

-- tilengine sample configuration
config = {
	hres = 320,
	vres = 240,
	numlayers = 2,
	numsprites = 80,
	numanimations = 32
}

-- called once at the beginning: load game assets and init
function game_load()

end

-- called once every frame: do game logic and update visuals
function game_loop(frame)

end

-- called once at end: release resources
function game_unload()

end
```

## Checking player input
```lua
-- check if player 1 is pressing left input
if tln.CheckRetroInput(tln.PLAYER1, tln.INPUT_LEFT) then

end
```

## Enabling raster effects
```lua
-- your custom raster function is called every scanline
function my_rasters(line)

end

-- register your raster function
tln.SetRasterCallbackName("my_rasters")
```

# Running the game (Windows)
To run the game, RetroArch emulator must be installed and accessible in %path%. Open a command terminal inside `lua_game` folder and type:

```
retroarch -L tilengine_libretro.dll
```

# Documentation
For extended help and examples refer to the main Tilengine project online documentation:

http://www.tilengine.org/doc/