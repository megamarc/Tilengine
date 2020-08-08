# Sprites

[TOC]

## Introduction

Sprites are any moving object in a videogame that are not a background: a character, enemy, bullet, player, ball, special effect... They can be animated and placed freely anywhere.

Sprites are referenced by an index, starting at 0 up to number of sprites minus 1. The total number of sprites is pre-allocated when the engine is initialized with \ref TLN_Init and cannot be changed later at runtime.

## Basic setup
A sprite needs at least a [spriteset](spritesets.md), a collection of related images packed together inside a single bitmap. It's common to pack together all the animation frames of a single character inside the same spriteset, so to change the displayed graphic it's only needed to change the image index.

Spritesets are loaded with \ref TLN_LoadSpriteset (read more about [spritesets](spritesets.md)). Once loaded, call \ref TLN_SetSpriteSet to attach it to the sprite, passing the sprite index and the reference to the spriteset:

```c
TLN_Spriteset spriteset = TLN_LoadSpriteset ("FireLeo.png");
TLN_SetSpriteSet (0, spriteset);
```

Now the sprite is displayed at position 0,0 by default (top-left corner) with the first graphic inside the spriteset. Each graphic inside a spriteset has a unique index, starting from 0, and an unique name. To change the graphic show, call \ref TLN_SetSpritePicture passing the sprite index and the graphic index inside the spriteset, starting with 0. For example to set sprite 0 with the 4th graphic inside the spriteset:

```c
TLN_SetSpritePicture (0, 3);
```

Sometimes it's convenient to use the graphic name instead of the index, because the latter can be difficult to know. To get the index of a given name, call \ref TLN_FindSpritesetSprite. This will return the graphic index (staring from 0), or -1 if not found:

```C
int index = TLN_FindSpritesetSprite(spriteset, "ship4");
if (index != -1)
    TLN_SetSpritePicture(0, index);
```

This searches de index of graphic with name "ship4" inside spriteset, and if found, assigns it to sprite 0.

## Moving around
To move the sprite to a different location, call the \ref TLN_SetSpritePosition, passing the sprite index, and the x,y coordinates. These values are absolute screen coordinates. For example to move sprite 3 to 160,120:
```c
TLN_SetSpritePosition (3, 160,120);
```
## Special attributes
There are some special modifiers that control sprite flipping, priority and masking. Sprite flipping allows to draw a sprite upside down and/or horizontally mirrored. For example a platformer game just needs to have sprites drawn facing to the right, when character need to walk to the left, just set the horizontal flipping flag.

**Priority** will draw the sprite in front of priority layers, instead of behind them. To set attributes, call \ref TLN_EnableSpriteFlag passing the sprite index and a combination of \ref TLN_TileFlags. For example to draw sprite 0 upside down:

```c
TLN_EnableSpriteFlag (0, FLAG_FLIPY);
```
Flipping modes: a) 0, b) FLAG_FLIPX, c) FLAG_FLIPY, d) FLAG_FLIPX + FLAG_FLIPY:

![Flipping modes](img/sprite_flags.png)

**Masking** will mark the sprite as affected by mask region if flagged with \ref FLAG_MASKED. Read "sprite masking" section below to know more about masking.

## Setting the palette
By default, a sprite is assigned the associated palette of its spriteset, but this can be changed calling \ref TLN_SetSpritePalette passing the sprite index and a \ref TLN_Palette reference:

```c
TLN_Palette palette = TLN_LoadPalette ("palette.act")
TLN_SetSpritePalette (0, palette);
```

## Blending
Blending is supported in sprites, with different modes and effects. To get extended information about the modes and their effects, please refer to [Blending section](blending.md).

To enable blending, call \ref TLN_SetSpriteBlendMode passing the sprite index and the blending mode. For example, to set 50%/50% blending in sprite 0:
```c
TLN_SetSpriteBlendMode (0, BLEND_MIX, 0);
```
The last parameter, *factor*, is kept for compatibility but isn't used.

To disable blending, call the same function with blending mode set to BLEND_NONE :
```c
TLN_SetSpriteBlendMode (0, BLEND_NONE, 0);
```

## Scaling
Sprites can be drawn upscaled or downscaled with an arbitrary factor. To enable scaling, call \ref TLN_SetSpriteScaling passing the sprite index and two floating point values with the horizontal and vertical factor, respectively. Values greater than 1.0 upscale, and smaller than 1.0 downscale. For example to set an horizontal downscaling of 0.5 and vertical upscaling of 1.5 for layer 0:
```c
TLN_SetSpriteScaling (0, 0.5f, 1.5f);
```
Different sprite scaling examples:

![Different sprite scaling examples](img/sprite_scaling.png)

To disable scaling, call \ref TLN_ResetSpriteScaling passing the sprite index. For example to disable scaling in sprite 0:
```c
TLN_ResetSpriteScaling (0);
```

## Collision detection
A basic action on any game is checking if two given sprites collide. For example, if our hero is hit by any enemy bullet. A quick way to determine a collision is to check if their bounding boxes overlap (a *bounding box* is the rectangular area that fully encloses a sprite). This method is fast and easy to implement, but sometimes the bounding boxes of two sprites can overlap, but in regions where there aren't solid pixels, just transparent ones. In this case, you see that the bullet isn't going to hit your hero, but it gets actually hit without touching it. A common solution is to use bounding boxes that are *smaller* than the sprite, but this can have the opposite effect: missing collisions that actually happen.

To solve this, Tilengine implements pixel-based collision detection. With this feature enabled, you know that a sprite gets involved in a collision only if there are actual non-transparent pixels in both sprites overlapping. But this method also has a limitation: it can tell you that a given sprite is per-pixel colliding with another sprite, but you don't know with *which* sprite.

The final solution consists in combining both methods as they compliment each other: first determine coarse collision with bounding boxes, and then check per-pixel collision detection in those sprites.

Per-pixel collision detection requires more CPU cycles that regular sprites, so it's an optional feature that is disabled by default. You can enable it for each sprite calling \ref TLN_EnableSpriteCollision passing the sprite index and a boolean value with *true* to enable or *false* to disable the feature. For example, to enable collision detection for sprite 0 and disable it for sprite 3:
```c
TLN_EnableSpriteCollision (0, true);
TLN_EnableSpriteCollision (3, false);
```
To query the collision status for a given sprite, call \ref TLN_GetSpriteCollision passing the sprite index to check. It returns a boolean value indicating the collision status:
```c
bool collision = TLN_GetSpriteCollision (0);
```

## Sprite drawing order

By default, each sprite activated is added to the end of a list of sprites that are drawn from first to last, following [painter's algorithm](https://en.wikipedia.org/wiki/Painter%27s_algorithm). That means dat sprites added later will overlap the ones added first. For example if sprites 0, 1, 2, 3 are added in sequence:

```
0 -> 1 -> 2 -> 3
```

Sprite 0 will be drawn first, sprite 3 will be drawn last, overlapping the others.

This order can be changed with \ref TLN_SetFirstSprite and \ref TLN_SetNextSprite functions.

To set the first sprite in the list, call \ref TLN_SetFirstSprite passing the index of first sprite. In the above example, to set 2 at the beginning:

```C
TLN_SetFirstSprite(2);
```

The list becomes:

```C
2 -> 0 -> 1 -> 3
```

To change any other sprite, call \ref TLN_SetNextSprite, passing the current sprite, and which one goes next. Taking the previous list, to move sprite 0 from its position to be drawn after sprite 3:

```C
TLN_SetNextSprite(3, 0);
```

The list becomes:


```C
2 -> 1 -> 3 -> 0
```

Now sprite 0 overlaps sprite 3

## Sprite masking

Sprite masking allows defining a rectangular region that spans the whole frame width, where selected sprites won't be drawn when they cross this region.

To define the masking area, call \ref TLN_SetSpritesMaskRegion, passing the bottom and top scanlines that define the exclusion area. For example to create a masking area that goes from y = 120 to y = 160, call:

```C
TLN_SetSpritesMaskRegion(120, 160);
```

To disable masking region, pass 0, 0:

```C
TLN_SetSpritesMaskRegion(0, 0);
```

Only sprites flagged with TLN_MASKED flag will disappear inside the mask region. Use \ref TLN_EnableSpriteFlag to enable or disable FLAG_MASKED flag. 

## Animation
Although it is possible to animate a sprite manually using the \ref TLN_SetSpritePicture function at timed intervals, tilengine has built-in animation support. To animate a sprite it is necessary to have a \ref TLN_Sequence object describing the animation. See the chapter [Sequences](sequences.md) to see how to create a \ref TLN_Sequence object from a \ref TLN_Spriteset object.

For example, assuming a spriteset called `spriteset`, containing frames numbered sequentially from `walk1` to `walk8`, you can create the sequence with these frames, with a cadence of 6 fps at 60 Hz:

```c
TLN_Sequence walk_sequence = TLN_CreateSpriteSequence ("walking", spriteset, "walk", 60/6);
```

Once created, set the animation to the sprite with \ref TLN_SetSpriteAnimation, indicating the index of the sprite, the \ref TLN_Sequence object, and the number of times the animation should loop, indicating 0 for it to repeat indefinitely. For example, to animate sprite 0 with the `walk_sequence` previously created and to be repeated continuously:

```c
TLN_SetSpriteAnimation (0, walk_sequence, 0);
```

**NOTE**: the sprite must have been previously assigned with the same spriteset used to create the sequence.

To finish playing an ongoing animation, call \ref TLN_DisableSpriteAnimation passing the index of the sprite to stop animation:

```c
TLN_DisableSpriteAnimation(0);
```

## Disabling
To disable a sprite so it is not rendered, just call \ref TLN_DisableSprite passing the sprite index:
```c
TLN_DisableSprite (0);
```

## Summary
This is a quick reference of related functions in this chapter:

|Function                        | Quick description
|--------------------------------|-------------------------------------
|\ref TLN_SetSpriteSet           |Assigns the spriteset and its palette to a given sprite
|\ref TLN_EnableSpriteFlag       |Sets flags for a given sprite
|\ref TLN_SetSpritePosition      |Sets the sprite position inside the viewport
|\ref TLN_SetSpritePicture       |Sets the actual graphic to the sprite
|\ref TLN_SetSpritePalette       |Assigns a palette to a sprite
|\ref TLN_SetSpriteBlendMode     |Sets the blending mode (transparency effect)
|\ref TLN_SetSpriteScaling       |Sets the scaling factor of the sprite
|\ref TLN_ResetSpriteScaling     |Disables scaling for a given sprite
|\ref TLN_GetSpritePicture       |Returns the index of the assigned picture from the spriteset
|\ref TLN_GetAvailableSprite     |Returns the first available (unused) sprite
|\ref TLN_EnableSpriteCollision  |Enable sprite collision checking at pixel level
|\ref TLN_GetSpriteCollision     |Gets the collision status of a given sprite
|\ref TLN_SetSpritesMaskRegion   |Defines masking region to hide FLAG_MASKED sprites
|\ref TLN_SetSpriteAnimation     |Starts a sprite animation
|\ref TLN_DisableSpriteAnimation |Disables animation of sprite
|\ref TLN_GetSpritePalette       |Returns the current palette of a sprite
|\ref TLN_DisableSprite          |Disables the sprite so it is not drawn
