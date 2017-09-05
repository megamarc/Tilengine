# First steps {#first_steps}
[TOC]
# First steps {#first_steps}
## Initialize {#first_steps_initialize}
First of all, you have to include the header at the top of the file:
```c
#include "Tilengine.h
```

To initialize the engine, use the function \ref TLN_Init to set the framebuffer resolution, and the number of layers, sprites, and animators. Once set, these values are immutable and cannot be changed. For example, to set a 400x240 framebuffer with 2 layers, 80 sprites and no animations -Sega Genesis setup- you should do:
```c
TLN_Init (400,240, 2,80,0);
```

## Setting the background {#first_steps_background}
The background is what is shown when there isn't any layer or sprite at a given location. Tilengine supports two types of backgrounds: solid color and static bitmap. By default the background is black color.

### Solid color background
To set a solid color there is the function \ref TLN_SetBGColor that takes the three components of a RGB color, between 0 and 255. For example to set a dark blue background you can do:
```c
TLN_SetBGColor (0,32,96);
```

It is also possible to set the background color defined inside a tilemap object with the \ref TLN_SetBGColorFromTilemap function. Tilemaps may specify a default background color that can be used here. To see how to load and manipulate tilemaps, please refer to [chapter 10](10_tilemaps.md). For now, to load a tilemap called "tilemap.tmx" and use its default background color, you have to do the following:
```c
TLN_Tilemap tilemap = TLN_LoadTilemap ("tilemap.tmx");
TLN_SetBGColorFromTilemap (tilemap);
```

### Bitmap background
To set a bitmap, there is the function \ref TLN_SetBGBitmap that takes the \ref TLN_Bitmap reference of a loaded bitmap. To see how to load an manipulate bitmaps, please refer to [chapter 14](14_bitmaps.md). For now, to load a bitmap called "Background.png" and set it as the background, you have to do the following:
```c
TLN_Bitmap background = TLN_LoadBitmap ("Background.png");
TLN_SetBGBitmap (background);
```
It's possible to change the default palette provided by the bitmap. To do so, use the \ref TLN_SetBGPalette function that takes a \ref TLN_Palette object. To see how to load and manipulate palettes, please refer to [chapter 12](12_palettes.md). Assuming you have an alternative palette file called "Background.act", do the following to set it:
```c
TLN_Palette palette = TLN_LoadPalette ("Background.act");
TLN_SetBGPalette (palette);
```

### Disabling background
It is possible to disable background at all if you know that the last layer covers the entire screen without holes, to gain some performance:
```c
TLN_DisableBGColor ()
```

## Setting the assets path {#first_steps_path}
By default tilengine loads all graphic assets in the same directory where the executable is. If you want to set your assets in a structured tree of folders -which is recommended-, you can set it with the \ref TLN_SetLoadPath function. It accepts relative or absolute paths, and interprets slash and backslash as path separator on any platform. For example:
```c
TLN_SetLoadPath ("./assets/level1");
```

## Error handling {#first_steps_errors}
Most functions in tilengine return either a reference to a requested object, or a boolean value signaling if the operation was successful or not. When an operation fails you can get an specific error code with the \ref TLN_GetLastError, whereas the \ref TLN_GetErrorString returns a string with a description about the requested error code:
```c
int error = TLN_GetLastError ();
char* description = TLN_GetErrorString (error);
printf ("Last operation returned with code %d: %s\n", error, description);
```

## Getting runtime info {#first_steps_info}
Tilengine keeps track about the memory being used, the number of assets, the framebuffer size, etc:
* \ref TLN_GetVersion : returns the engine version number
* \ref TLN_GetWidth : returns the pixel width of the framebuffer (horizontal size)
* \ref TLN_GetHeight : returns the pixel height of the framebuffer (vertical size)
* \ref TLN_GetUsedMemory : returns the total amount of memory used by tilengine and loaded assets
* \ref TLN_GetNumObjects : returns the combined number of loaded assets

## Cleanup {#first_steps_cleanup}
Once done, you should explicitly close tilengine to release memory and resources:
```c
TLN_Deinit ();
```
