# API reference overview

[TOC]

Tilengine uses eight types of elements, arranged in two groups:
* **Static items** are reserved at startup and cannot be modified at runtime. These elements are layers (the backgrounds), sprites(the objects) and the animations. They are referenced by an index, starting at 0 for the first element up to num_elements - 1. They represent the "hardware" capabilities of Tilengine. Each one of these elements can be enabled or disabled as they are needed, but cannot be created or destroyed.
* **Dynamic assets** can be freely loaded, cloned and deleted at runtime. They are referenced by a pointer, and represent the "assets" that are assigned to the static elements

## Setup & run
This section shows the basic functions to initialise, manage and query Tilengine. Read at least the functions \ref TLN_Init, \ref TLN_SetRenderTarget and \ref TLN_UpdateFrame

## Windowing & user input
Tilengine is designed to act as a back-end renderer for other gaming environments, but it has a built-in windowing system for easy setup and testing without having to rely on user provided components. Read at lest the functions \ref TLN_CreateWindow, \ref TLN_ProcessWindow, \ref TLN_DrawFrame and \ref TLN_GetInput (for user input)

## Background layers
Layers represent the scrolling planes (or backgrounds) composed of square tiles. A layer must be assigned a tileset, a tilemap and optionally a palette. Read at least the functions \ref TLN_SetLayer and \ref TLN_SetLayerPosition

## Sprites
Sprites represent the movable objects that are not part of the backgrounds and where the gameplay occurs. A sprite must be assigned a spriteset and optionally a palette. Read at least the functions \ref TLN_ConfigSprite, \ref TLN_SetSpritePicture and \ref TLN_SetSpritePosition.

## Animations
Animations represent the built-in sequencers that play sequences. An animation must be assigned a sequence and one of the following: a layer (for tileset animation), a sprite (for sprite animation) or a palette (for color animation). Read at least the functions \ref TLN_SetSpriteAnimation and \ref TLN_SetPaletteAnimation.

## Tilesets
Tilesets are the building blocks of the backgrounds, the individual tiles themselves packed together. They are loaded at runtime from Tiled *tsx* files with the \ref TLN_LoadTileset function.

## Tilemaps
Tilemaps are a gird layout of tiles (from a tileset) that form a background. They are loaded at runtime from Tiled *tmx* files with the \ref TLN_LoadTilemap function.

## Spritesets
Spritesets are collections of related individual images for sprites grouped together inside a big picture, for example all the animation frames of the main character. They are loaded at runtime from *png/txt* pairs with the \ref TLN_LoadSpriteset function.

## Palettes
Palettes are the color tables used by the graphics in layers and sprites. They can be loaded at runtime from standard adobe *.act* files with the function \ref TLN_LoadPalette, retrieved from tilesets with \ref TLN_GetTilesetPalette or from spritesets with \ref TLN_GetSpritesetPalette.

## Sequences
A sequence is a serie of frames played in succession at a given speed by an animation. They can be loaded at runtime from *.sqx* files (xml) with the \ref TLN_LoadSequencePack function.
