# External rendering

[TOC]

## Setting the target surface
The framebuffer can be any 32 bpp RGBA bitmap, with the dimensions specified during initialization with \ref TLN_Init where tilengine performs the rendering. This surface is user-allocated and can be of any origin: a locked OpenGL texture, an SDL video surface, a Java canvas... whatever you choose. The \ref TLN_SetRenderTarget function takes two parameters: a pointer to the start of pixel data in the surface, and the number of bytes per scanline, the so-called *pitch*. This value is usually the horizontal resolution multipliead by four (the number of bytes required by each pixel) rounded to the next multiple of four. The following example allocates an in-memory framebuffer and sets it:
```c
const int hres = 400;
const int vres = 240;
const int pitch = hres*sizeof(uint32_t);
void* framebuffer = malloc (pitch * vres);
TLN_SetRenderTarget (framebuffer, pitch);
```

## Drawing frames
Once the framebuffer is set, to update the frame use the \ref TLN_UpdateFrame function. It takes an optional time parameter that's used by animation engine to keep track of frames. For now you can set it to 0 or as an increasing counter:
```c
TLN_UpdateFrame (0);
```
Now the previously created `framebuffer`surface holds the rendered frame.

## Basic example
This example creates a 400x240 framebuffer in memory, initializes the engine, does the main loop and exits:
```c
#include <stdio.h>
#include <stdlib.h>
#include "Tilengine.h"

int main (int argc, char* argv[])
{
    const int hres = 400;
    const int vres = 240;
    const int pitch = hres * sizeof(uint32_t);
    int frame = 0;
    void* framebuffer;

    /* init and set framebuffer */
    TLN_Init (hres, vres, 2, 80, 0);
    framebuffer = malloc (pitch * vres);
    TLN_SetRenderTarget (framebuffer, pitch);

    /* main loop */
    while (1)
    {
        /* your game stuff goes here */

        TLN_UpdateFrame (frame);
        frame += 1;
    }

    /* deallocate */
    free (framebuffer);
    TLN_Deinit ();
    return 0;
}
```

## Summary
This is a quick reference of related functions in this chapter:

|Function                       | Quick description
|-------------------------------|-------------------------------------
|\ref TLN_SetRenderTarget       |Defines a 32 bpp RGBA surface to hold the framebuffer
|\ref TLN_UpdateFrame           |Draws a frame to the framebuffer
