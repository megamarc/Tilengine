![Tilengine logo](Tilengine.png)
# tilengine_libretro - The 2D retro graphics engine core for libretro
[![License: MPL 2.0](https://img.shields.io/badge/License-MPL%202.0-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0)
[![Build Status](https://travis-ci.org/megamarc/Tilengine.svg?branch=libretro)](https://travis-ci.org/megamarc/Tilengine)
[![Build status](https://ci.appveyor.com/api/projects/status/yvx7koa32qyjm1b0?svg=true)](https://ci.appveyor.com/project/megamarc/tilengine)
[![Coverity Scan](https://scan.coverity.com/projects/16964/badge.svg)](https://scan.coverity.com/projects/megamarc-tilengine)

Tilengine is an open source, cross-platform 2D graphics engine for creating classic/retro games with tile maps, sprites and palettes. Its unique scanline-based rendering algorithm makes raster effects a core feature, a technique used by many games running on real 2D graphics chips. It integrates inside the RetroArch emulator (https://www.retroarch.com/) and loads games written in lua (http://www.lua.org/)

http://www.tilengine.org

# Contents
- [tilengine_libretro - The 2D retro graphics engine core for libretro](#tilenginelibretro---the-2d-retro-graphics-engine-core-for-libretro)
- [Contents](#contents)
- [Features](#features)
- [Creating your first program](#creating-your-first-program)
  - [Running the game (Windows)](#running-the-game-windows)
- [Documentation](#documentation)
- [Editing assets](#editing-assets)
- [Creating packages](#creating-packages)

# Features
* Written in portable C (C99)
* MPL 2.0 license: free for any project, including commercial ones, allows console development
* Cross platform: available builds for Windows (32/64), Linux PC(32/64), Mac OS X and Raspberry Pi
* High performance: all samples run at 60 fps with CRT emulation enabled on a Raspberry Pi 3
* Streamlined, easy to learn API that requires very little lines of code
* Built-in SDL-based windowing for quick tests
* Integrate inside any existing framework as a slave renderer
* Loads assets from open standard standard file formats
* Create or modify graphic assets procedurally at run time
* True raster effects: modify render parameters between scanlines
* Background layer scaling and rotation
* Sprite scaling
* Several blending modes for layers and sprites
* Pixel accurate sprite vs sprite and sprite vs layer collision detection
* Special effects: per-column offset, mosaic, per-pixel displacement, CRT emulation...
* Supports packaged assets with optional AES-128 encryption

# Creating your first program
The following section shows how to create from scratch and execute a simple game application that does the following:
1. Include required dependencies
2. Initialize the engine with a resolution of 320x240, one layer, no sprites and 20 animation slots
3. Set the loading path to the assets folder
4. Load a *tilemap*, the asset that contains background layer data
5. Attach the loaded tilemap to the allocated background layer
6. Run the window loop, updating the display at each iteration until the window is closed
7. Release allocated resources

![Test](test.png)

Create a file called `game.lua` and type the following code:

```lua
-- required dependencies
require("tilengine_libretro")
local ffi = require("ffi")
local tln = ffi.load 'tilengine_libretro'

-- tilengine configuration
config = {
	hres = 320,
	vres = 240,
	numlayers = 1,
	numsprites = 0,
	numanimations = 20
}

-- called once at the beginning to load assets
function game_load()
	tln.TLN_SetLoadPath("../samples/assets/sonic")
	foreground = tln.TLN_LoadTilemap("Sonic_md_fg1.tmx", nil)
	tln.TLN_SetLayer(0, nil, foreground)
    tln.TLN_SetBGColor(0x1B, 0x00, 0x8B)
end

-- called once every frame
function game_loop(frame)
    -- do game logic and update visuals here
end

-- called once at end: release resources
function game_unload()
	tln.TLN_DeleteTilemap(foreground)
end
```

## Running the game (Windows)
To run the game, RetroArch emulator must be installed and accessible in %path%. Open a command terminal inside `lua_game` folder and type:

```
retroarch_debug -L tilengine_libretro.dll game.lua -v
```

# Documentation
Doxygen-based documentation and API reference can be found in the following link:
http://www.tilengine.org/doc

# Editing assets
tilengine_libretro is just a game core library that doesn't come with any editor, but the files it loads are made with standard open-source tools. Samples come bundled with several ready-to-use assets, but these are the tools you'll need to edit or create new ones:
* Source code: [VSCode](), [Notepad++]()...
* Graphics: [GIMP](http://www.gimp.org), [Grafx2](http://pulkomandy.tk/projects/GrafX2), [Paint.NET](https://www.getpaint.net/), or any graphic editor with 8-bit (256 colors) png support
* Backgrounds: [Tiled Map Editor](https://www.mapeditor.org/) is the official editor
* Sprites: Online tool [Lesy SpriteSheet](https://www.leshylabs.com/blog/posts/2013-12-03-Leshy_SpriteSheet_Tool.html)
* Sequences: Any standard XML editor

# Creating packages
To create a package with all the assets, the open-source tool [ResourcePacker](https://github.com/megamarc/ResourcePacker) must be used. It's a corss-platform, easy to use command line tool that creates packages with files keeping the same directory structure. Tilengine has built-in support for loading assets from these packages just as if they still were stand-alone files.
