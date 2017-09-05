# Background layers {#layers}
[TOC]
# Background layers {#layers}
Background layers are one of the two main graphics entities in tilengine, the other are sprites. Background layers are a two dimensional arrangement of tiles, called tilemap, where each tile is a small square bitmap extracted from a bigger palette of tiles, called tileset, plus some flags that modify its appearance. Background layers can have transparent areas, where the underlying layer(s) or background color is seen.

Layers are referenced by an index, starting at 0 for the topmost, increasing up to number of layers minus 1 for the bottom-most.

## Basic setup {#layers_setup}
In order to get displayed, a layer needs to have attached three items: a tilemap, a tileset and a palette. Usually a tilemap has an internal reference to its associated tileset, and a tileset has a palette embedded, we only have to explicitly set the tilemap and the other items are loaded automatically. This is accomplished with the \ref TLN_SetLayer function.

We have to load the tilemap first with \ref TLN_LoadTilemap (read more about tilemaps). Then we use \ref TLN_SetLayer to attach it to the layer
```c
TLN_Tilemap tilemap = TLN_LoadTilemap ("sonic_md_fg1.tmx");
TLN_SetupLayer (0, NULL, tilemap);
```
The first parameter is the index layer, an the second is the tileset. We can just pass a NULL to use the internal tileset referenced by the tilemap. But we can explicitly load a tileset too, with \ref TLN_LoadTileset :
```c
TLN_Tilemap tilemap = TLN_LoadTilemap ("sonic_md_fg1.tmx");
TLN_Tileset tileset = TLN_LoadTileset ("sonic_md_fg1.tsx");
TLN_SetupLayer (0, tileset, tilemap);
```

## Scrolling {#layers_scroll}
Scrolling is the common term for moving the display area inside a bigger map. By default, when a layer is setup for the first time, its located at position 0,0 inside the tilemap. But it can be relocated with \ref TLN_SetLayerPosition. For example, to locate the layer 0 at position 120,16 (120 pixels to the right, 16 pixels down):
```c
TLN_SetLayerPosition (0,120,16);
```

### Smooth scroll
Smooth scrolling consists in moving continuously a few pixels each time. This sample scrolls layer 0 to the right:
```c
int x = 0;
while (TLN_ProcessWindow())
{
    TLN_SetLayerPosition (0, x,16);
    TLN_DrawFrame ();
    x += 1;
}
```

### Parallax scroll
Parallax scrolling consists in scrolling at least two or more layers, where the background layer represents objects that are far away and move slowly, whereas the foreground layer has the objects that are nearer to to player and move faster.
```c
int x = 0;
while (TLN_ProcessWindow())
{
    TLN_SetLayerPosition (0, x,0);     /* move foreground layer two pixels per frame */
    TLN_SetLayerPosition (1, x/2,0);   /* move background layer one pixel per frame */
    TLN_DrawFrame ();
    x += 2;
}
```

## Setting the palette {#layers_palette}
By default, any layer uses the palette that came with the attached spriteset, but it can be changed and use any other palette with \ref TLN_SetLayerPalette. We can explicitly load a palette with \ref TLN_LoadPalette :
```c
TLN_Palette palette = TLN_LoadPalette ("palette.act");
TLN_SetLayerPalette (0, palette);
```

## Blending {#layers_blend}
## Clipping {#layers_clip}
## Column offset {#layers_column}
## Scaling {#layers_scaling}
## Affine trasform {#layers_transform}
## Per-pixel mapping {#layers_mapping}
## Mosaic effect {#layers_mosaic}
## Getting tile data {#layers_info}
## Disabling {#layers_disable}
