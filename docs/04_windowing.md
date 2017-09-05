# Windowing {#window}
[TOC]
# Windowing {#window}
The built-in window in tilengine provides a basic, easy to setup environment for quick test and evaluation. It's a very simple environment to ease prototyping but is not intended to be used in production environment.

## Features {#window_features}
* Runs windowed or full-screen
* User input with keyboard or gamepad
* CRT post-processing emulation filter
* Single thread or multi-threaded

## Single threaded window {#window_single}
The single threaded window runs inside the main thread and must be handled inside the game loop for each frame. The window is created with \ref TLN_CreateWindow. With default parameters it creates a window with an integer scaling as large as possible for the desktop resolution, and with CRT emulation effect enabled:
```c
TLN_CreateWindow (NULL, 0);
```
* To toggle windowed/fullscreen mode, press <kbd>Alt + Enter</kbd>
* To toggle CRT effect or on off, press <kbd>Backspace</kbd>
* To close the window, press <kbd>Escape</kbd>

Once the window is created, it must be processed calling \ref TLN_ProcessWindow for each frame. This function returns true while the window is alive, or false when the user has requested to terminate it. Draw the frames with \ref TLN_DrawFrame. This function takes an optional integer value, that represents a timestamp used by the animation engine. If there are not animations it can be left as 0.

This basic sample show how to initialize the engine, create the window and do the window loop until the user requests to exit:
```c
int frame = 0;
TLN_Init (400,240, 2,8,0);   /* init the engine */
TLN_CreateWindow (NULL, 0);  /* create the window */
while (TLN_ProcessWindow())  /* the window loop */
{
    TLN_DrawFrame (frame);   /* draw next frame */
    frame += 1;
}
TLN_Deinit ();               /* release resources */
```

## Multi-threaded window {#window_multi}
The multi-threaded window spans its own thread and runs in parallel, without a window loop. It's mainly used for interactive use within an editor as in python. The multi-threaded window is created with \ref TLN_CreateWindowThread and doesn't require continuous calls to \ref TLN_ProcessWindow or \ref TLN_DrawFrame. Instead you query its active state with \ref TLN_IsWindowActive, and optionally you can sync to it with \ref TLN_WaitRedraw.

This is the same sample with the multi-threaded window:
```c
TLN_Init (400,240, 2,8,0);         /* init the engine */
TLN_CreateWindowThread (NULL, 0);  /* create the window */
while (TLN_IsWindowActive())       /* check window state */
{
    TLN_WaitRedraw ();             /* optionally sync to window drawing for fps control */
}
TLN_Deinit ();                     /* release resources */
```

## User input {#window_input}
User input in tilengine simulates a basic arcade setup, with for directions and four action buttons. It can be controlled with keyboard or joystick/gamepad:
* 4-way direction: keyboard cursors or gamepad D-Pad
* 4 action buttons: <kbd>Z,X,C,V</kbd> or gamepad buttons 1-4.

To check the state of an input, call \ref TLN_GetInput with one of the possible \ref TLN_Input values. For example, to check if left arrow is pushed do this:
```c
if (TLN_GetInput(INPUT_LEFT))
{
    /* do your treatment */
}
```

## Time & delay {#window_timing}
Tilengine window provides some basic timing functions. \ref TLN_GetTicks returns the number of milliseconds elapsed since system started, and \ref TLN_Delay pauses execution for the given amount of milliseconds.

## The CRT effect {#window_crt}
All low resolution, pixel art games were played on CRT displays that provided its unique texture and feel. Trying to watch this type of games in crystal-clear, big square pixels, is just wrong. The CRT effect simulates more or less faithfully the characteristics of a CRT display: visible RGB strips, horizontal blur, bright pixel bloom...

Plain output without CRT:  
![CRT disabled](http://www.tilengine.org/doc/crt_off.jpg)

The same output but with default CRT enabled:  
![CRT enabled](http://www.tilengine.org/doc/crt_on.jpg)

By default the CRT effect is enabled when the window is created, but it can be disabled with the \ref TLN_DisableCRTEffect function. It can be toggled pressing the <kbd>Escape</kbd> key, too.

The effect is highly configurable with the \ref TLN_EnableCRTEffect function. It takes many parameters to customize its appearance and strength. The default parameters are:
```c
TLN_EnableCRTEffect (TLN_OVERLAY_APERTURE, 128, 192, 0,64, 64,128, false, 255);
```

