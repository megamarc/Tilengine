# First steps

[TOC]

## Initialize
First of all, you have to include the header at the top of the file:
```c
#include "Tilengine.h
```

To initialize the engine, use the function \ref TLN_Init to set the framebuffer resolution, and the number of layers, sprites, and animators. Once set, these values are immutable and cannot be changed. For example, to set a 400x240 framebuffer with 2 layers, 80 sprites and no animations -Sega Genesis setup- you should do:
```c
TLN_Init (400,240, 2,80,0);
```

## Setting the background
The background is what is shown when there isn't any layer or sprite at a given location. Tilengine supports two types of backgrounds: solid color and static bitmap. By default the background is black color.

### Solid color background
To set a solid color there is the function \ref TLN_SetBGColor that takes the three components of a RGB color, between 0 and 255. For example to set a dark blue background you can do:
```c
TLN_SetBGColor (0,32,96);
```

It is also possible to set the background color defined inside a tilemap object with the \ref TLN_SetBGColorFromTilemap function. Tilemaps may specify a default background color that can be used here. To see how to load and manipulate tilemaps, please refer to [Tilemaps section](tilemaps.md). For now, to load a tilemap called "tilemap.tmx" and use its default background color, you have to do the following:
```c
TLN_Tilemap tilemap = TLN_LoadTilemap ("tilemap.tmx");
TLN_SetBGColorFromTilemap (tilemap);
```

### Bitmap background
To set a bitmap, there is the function \ref TLN_SetBGBitmap that takes the \ref TLN_Bitmap reference of a loaded bitmap. To see how to load an manipulate bitmaps, please refer to [Bitmaps section](bitmaps.md). For now, to load a bitmap called "Background.png" and set it as the background, you have to do the following:
```c
TLN_Bitmap background = TLN_LoadBitmap ("Background.png");
TLN_SetBGBitmap (background);
```
It's possible to change the default palette provided by the bitmap. To do so, use the \ref TLN_SetBGPalette function that takes a \ref TLN_Palette object. To see how to load and manipulate palettes, please refer to [Palettes section](palettes.md). Assuming you have an alternative palette file called "Background.act", do the following to set it:
```c
TLN_Palette palette = TLN_LoadPalette ("Background.act");
TLN_SetBGPalette (palette);
```

### Disabling background
It is possible to disable background at all if you know that the last layer covers the entire screen without holes, to gain some performance:
```c
TLN_DisableBGColor ()
```

## Setting the assets path
By default tilengine loads all graphic assets in the same directory where the executable is. If you want to set your assets in a structured tree of folders -which is recommended-, you can set it with the \ref TLN_SetLoadPath function. It accepts relative or absolute paths, and interprets slash and backslash as path separator on any platform. For example:
```c
TLN_SetLoadPath ("./assets/level1");
```

## Error handling
Most functions in tilengine return either a reference to a requested object, or a boolean value signaling if the operation was successful or not. When an operation fails you can get an specific error code with the \ref TLN_GetLastError, whereas the \ref TLN_GetErrorString returns a string with a description about the requested error code:
```c
int error = TLN_GetLastError ();
char* description = TLN_GetErrorString (error);
printf ("Last operation returned with code %d: %s\n", error, description);
```

## Getting runtime info
Tilengine keeps track about the memory being used, the number of assets, the framebuffer size, etc:
* \ref TLN_GetVersion : returns the engine version number
* \ref TLN_GetWidth : returns the pixel width of the framebuffer (horizontal size)
* \ref TLN_GetHeight : returns the pixel height of the framebuffer (vertical size)
* \ref TLN_GetUsedMemory : returns the total amount of memory used by tilengine and loaded assets
* \ref TLN_GetNumObjects : returns the combined number of loaded assets

## Debugging
Tilengine does sanity check on each parameter and silently ignores a function call when there are some mistakes on the parameters (indexes out of range, wrong object types, etc). Each function that can fail returns a `false` boolean that can be further examinated with \ref TLN_GetLastError and \ref TLN_GetErrorString. However this approach requires much work and manual test when something isn't working as expected. To ease the debugging of your program, Tilengine supports writing messages to the standard output. This behavior is selected with \ref TLN_SetLogLevel, with three possible values:

|Value           | Effect
|----------------|---------------------------------------------------------------
|TLN_LOG_NONE    |Doesn't output anything (default value)
|TLN_LOG_ERRORS  |Print only wrong function calls
|TLN_LOG_VERBOSE |Print wrong function calls and every asset creation/destruction

```c
/* enable basic logging, just errors: */
TLN_SetLogLevel (TLN_LOG_ERRORS);

/* force an error: "Tilengine: Invalid object address is (nil)" */
TLN_SetLayer (0, NULL, NULL);
```

## Cleanup
Once done, you should explicitly close tilengine to release memory and resources:
```c
TLN_Deinit ();
```

## Multiple contexts
Since release 2.0.0, Tilengine supports multiple instances using a *global context* mechanism. The \ref TLN_Init function returns a handler to a newly created context (a \ref TLN_Engine object), that is selected as active by default. To change the active context use the \ref TLN_SetContext function, passing the handler of the desired context. To delete a context, use \ref TLN_DeleteContext.

```c
TLN_Engine instance1 = TLN_Init(480, 240, 2,8, 0);    /* instance 1 */
TLN_Engine instance2 = TLN_Init(360, 160, 1,8, 0);    /* instance 2 */

/* do stuff on instance 1: */
TLN_SetContext(instance1);
/* ... do your stuff with regular TLN_xxx functions */

/* do stuff on instance 2: */
TLN_SetContext(instance2);
/* ... do your stuff with regular TLN_xxx functions */

/* release */
TLN_DeleteContext(instance1);
TLN_DeleteContext(instance2);
```

## Summary
This is a quick reference of related functions in this chapter:

|Function                       | Quick description
|-------------------------------|-------------------------------------
|\ref TLN_GetVersion            |Returns library version
|\ref TLN_Init                  |Creates a Tilengine rendering context
|\ref TLN_Deinit                |Destroys the current rendering context
|\ref TLN_SetContext            |Selects the active context
|\ref TLN_GetContext            |Returns the active context
|\ref TLN_DeleteContext         |Destroys the specified context
|\ref TLN_SetLoadPath           |Sets defautl load path for asset loading
|\ref TLN_GetWidth              |Returns the width of the created framebuffer
|\ref TLN_GetHeight             |Returns the height of the created framebuffer
|\ref TLN_GetNumObjects         |Returns the number of runtime assets
|\ref TLN_GetUsedMemory         |Returns the amount of memory used by runtime assets
|\ref TLN_GetNumLayers          |Returns the number of requested layers
|\ref TLN_GetNumSprites         |Returns the number of requested sprites
|\ref TLN_SetBGColor            |Sets the default background color
|\ref TLN_SetBGColorFromTilemap |Sets the background color as defined in a given tilemap
|\ref TLN_DisableBGColor        |Disables use of background color
|\ref TLN_SetBGBitmap           |Sets a static background bitmap
|\ref TLN_SetBGPalette          |Sets the palette of the static background bitmap
|\ref TLN_SetLogLevel           |Sets the verbosity of debug messages
|\ref TLN_GetLastError          |Returns the code of last error
|\ref TLN_GetErrorString        |Returns the string value of a given error code
