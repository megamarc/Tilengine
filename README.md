![Tilengine logo](Tilengine.png)
# Tilengine - The 2D retro graphics engine
Tilengine is a free, cross-platform 2D graphics engine for creating classic/retro games with tile maps, sprites and palettes. Its unique scanline-based rendering algorithm makes raster effects a core feature, a technique used by many games running on real 2D graphics chips.

## Features
* Written in portable C (C99)
* MIT-Licensed: free for any project, including commercial ones
* Cross platform: available builds for Windows (32/64), Linux PC(32/64), Mac OS X and Raspberry Pi
* High performance: all samples run at 60 fps with CRT emulation enabled on a Raspberry Pi 3
* Multiple language bindings: C/C++, C#, Python, Java, Pascal
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

## Installing
Open a console window in Tilengine root directory  and type the suitable command depending on your platform:
### Windows (any architecture)
```> install```
### Unix-like (Linux, Raspberry Pi, Apple OS X)
```> ./sudo install```

**Note for OS X users**: Tilengine depends on having SDL2 in Framework format installed on your system, that by default is not installed. Please download it here: https://www.libsdl.org/release/SDL2-2.0.5.dmg. Homebrew port won't work, use the official DMG file instead.
## Running the samples
### C/C++
C samples are located in `Tilengine/samples/c` folder. To build them you need the gcc compiler suite, and/or Visual C++ in windows.
* **Linux**: the GCC compiler suite is already installed by default
* **Windows**: you must install [MinGW](http://www.mingw.org/) or [Visual Studio Community](https://www.visualstudio.com/vs/community/)
* **Apple OS X**: You must install [Command-line tools for Xcode](https://developer.apple.com/xcode/). An Apple ID account is required.

Once installed, open a console window in the C samples folder and type the suitable command depending on your platform:
### Windows (any architecture)
```
> mingw32-make
```
### Unix-like (Linux, Raspberry Pi, Apple OS X)
```
> make
```
### Python
Python samples are located in `Tilengine/samples/python` folder. Make sure that Python v2 or v3 is installed on your system before proceeding.

Open a terminal window in the python samples folder, and run any sample by typing python *name_of_the_sample*.py, for example:
```
> python platformer.py
```
### C#
C# samples are located in `Tilengine/samples/csharp` folder. Make sure that [Mono](http://www.mono-project.com/download/) or .NET Framework (Windows only) is installed on your system. The following guide assumes Mono usage, please check Microsoft website for .NET Framework related info. C# has separate build and run steps like C, but these steps are the same on all platforms, as in Python.

In order to build the provided sample, open a console window in the C# samples folder and type the following command:
```
> mcs *.cs
```
This will produce `Platform.exe` that can be run with this command:
```
> mono Platformer.exe
```

## The tilengine window
The following actions can be done in the created window:
* Press <kbd>Esc</kbd> to close the window
* Press <kbd>Alt</kbd> + <kbd>Enter</kbd> to toggle full-screen/windowed
* Press <kbd>Backspace</kbd> to toggle built-in CRT effect (enabled by default)

## Creating your first program
The following section shows how to create from scratch and execute a simple tilengine application with its three main languages. All programs are equivalent and do the same things:
1. Reference the inclusion of Tilengine module
2. Initialize the engine with a resolution of 400x240, one layer, no sprites and 20 animation slots
3. Set the loading path to the assets folder
4. Load a *tilemap*, the asset that contains background layer data
5. Attach the loaded tilemap to the allocated background layer
6. Create a display window with default parameters: windowed, auto scale and CRT effect enabled
7. Run the window loop, updating the display at each iteration until the window is closed
8. Release allocated resources

![Test](test.png)
### C/C++
Create a file called `test.c` in `Tilengine/samples/c` folder, and type the following code:
```c
#include "Tilengine.h"

void main(void)
{
    TLN_Tilemap foreground;
    int frame = 0;
    
    TLN_Init (400,240,1,0,20);
    TLN_SetLoadPath ("../assets/sonic");
    foreground = TLN_LoadTilemap ("sonic_md_fg1.tmx", NULL);
    TLN_SetLayer (0, NULL, foreground);
    
    TLN_CreateWindow (NULL, 0);
    while (TLN_ProcessWindow())
    {
        TLN_DrawFrame (frame);
        frame += 1;     
    }
    
    TLN_Deinit ();  
}
```
Now the program must be built to produce an executable. Open a console window in the C samples folder and type the suitable command for your platform:
#### Windows 32-bit
```
> mingw32-gcc test.c -o test.exe -I"../../include" ../../lib/win32/Tilengine.lib
> test.exe
```
#### Linux (32-bit, 64-bit or Raspberry Pi)
```
> gcc test.c -o test -lTilengine -lm
> ./test
```
#### Apple OS X
```
> gcc test.c -o test "/usr/local/lib/Tilengine.dylib" -lm
> ./test
```
### Python
Create a file called `test.py` in `Tilengine/samples/python` folder, and type the following code:
```python
import tilengine as tln
    
engine = tln.Engine.create(400,240,1,0,20)
engine.set_load_path("../assets/sonic")
foreground = tln.Tilemap.fromfile("sonic_md_fg1.tmx")
engine.layers[0].setup(foreground)

frame = 0
window = tln.Window.create()
while window.process():
    window.draw_frame(frame)
    frame += 1

engine.delete()
```
Python doesn't need separate build/run steps, and unlike C language, the procedure is the same for any platform. Just open a console window in the python samples folder and type the following command:
```
> python test.py
```
### C#
Create a file called `test.cs` in `Tilengine/samples/csharp` folder, and type the following code:
```cs
using Tilengine;

class Test
{
    static void Main(string[] args)
    {
        Engine engine = Engine.Init(400,240,1,0,20);
        engine.LoadPath = "../assets/sonic";
        Tilemap background = Tilemap.FromFile("Sonic_md_fg1.tmx", null);
        engine.Layers[0].SetMap(background);
        
        int frame = 0;
        Window window = Window.Create(null, 0);
        while (window.Process ())
        {
            window.DrawFrame(frame);
            frame++;
        }
        engine.Deinit();
    }
}
```
Open a console window in the C# samples folder and type the following commands:
```
> mcs test.cs Tilengine.cs
> mono test.exe
```
## Contributors
Some people contributed to improve tilengine:

@turric4an for the Pascal wrapper<br>
@davideGiovannini for helping with the Linux-x86_64 port<br>
@shayneoneill for helping with the OS X port<br>
@adtennant for providing cmake and pkg-config support<br>