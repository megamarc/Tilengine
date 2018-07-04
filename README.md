![Tilengine logo](Tilengine.png)
# Tilengine - The 2D retro graphics engine
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)

Tilengine is an open source, cross-platform 2D graphics engine for creating classic/retro games with tile maps, sprites and palettes. Its unique scanline-based rendering algorithm makes raster effects a core feature, a technique used by many games running on real 2D graphics chips.

http://www.tilengine.org

# Contents
- [Features](#features)
- [Getting binaries](#getting-binaries)
- [Installing](#installing)
    - [Windows](#windows)
    - [Unix-like](#unix-like)
- [Running the samples](#running-the-samples)
    - [C/C++](#cc)
        - [Windows](#windows-1)
        - [Unix-like](#unix-like-1)
    - [Python](#python)
    - [C-Sharp](#c-sharp)
- [The tilengine window](#the-tilengine-window)
- [Creating your first program](#creating-your-first-program)
    - [C/C++](#cc-1)
        - [Windows 32-bit](#windows-32-bit)
        - [Linux (32-bit, 64-bit or Raspberry Pi)](#linux-32-bit-64-bit-or-raspberry-pi)
        - [Apple OS X](#apple-os-x)
    - [Python](#python-1)
    - [C-Sharp](#c-sharp-1)
- [Contributors](#contributors)

# Features
* Written in portable C (C99)
* LGPL v2.1 license: free for any project, including commercial ones
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

# Getting binaries

## Download from itch.io
The best way to get prebuilt binaries ready to run is grabbing them from official itch.io acount: https://megamarc.itch.io/tilengine. Just download the package for your platform.

## Build from source
You can also build the library from source. You'll need to provide libpng 1.6 and SDL2 development libraries (headers and binaries) into the suitable folders inside the `/src_lib` folder. They're not distributed with Tilengine source. See `readme.txt` files inside `/src_lib/libpng` and `/src_lib/sdl` for detailed instructions.
* **Windows users**: open the /Tilengine.sln project, you'll need Visual Studio Community. x86 and x64 targets provided.
* **Linux/OSX users**: run the suitable Makefile_xxx for your platform. You'll need GNU Make and GCC toolchain installed.

# Installing
After building from source, open a console window in Tilengine root directory and type the suitable command depending on your platform:

## Windows
```
> install
```

## Unix-like
```
> sudo ./install
```

**Note for OS X users**: Tilengine depends on having SDL2 in Framework format installed on your system, that by default is not installed. Please download it here: https://www.libsdl.org/release/SDL2-2.0.5.dmg. Homebrew port won't work, use the official DMG file instead.

# Running the samples

## C/C++
C samples are located in `Tilengine/samples/c` folder. To build them you need the gcc compiler suite, and/or Visual C++ in windows.
* **Linux**: the GCC compiler suite is already installed by default
* **Windows**: you must install [MinGW](http://www.mingw.org/) or [Visual Studio Community](https://www.visualstudio.com/vs/community/)
* **Apple OS X**: You must install [Command-line tools for Xcode](https://developer.apple.com/xcode/). An Apple ID account is required.

Once installed, open a console window in the C samples folder and type the suitable command depending on your platform:

### Windows
```
> mingw32-make
```

### Unix-like
```
> make
```

## Python
Python samples are located in `Tilengine/samples/python` folder. Make sure that Python v2 or v3 is installed on your system before proceeding.

Open a terminal window in the python samples folder, and run any sample by typing python *name_of_the_sample*.py, for example:
```
> python platformer.py
```

## C-Sharp
C# samples are located in `Tilengine/samples/csharp` folder. Make sure that [Mono](http://www.mono-project.com/download/) or .NET Framework (Windows only) is installed on your system. The following guide assumes Mono usage, please check Microsoft website for .NET Framework related info. C# has separate build and run steps like C, but these steps are the same on all platforms, as in Python.

In order to build the provided sample, open a console window in the C# samples folder and type the following command:
```
> mcs *.cs
```
This will produce `Platform.exe` that can be run with this command:
```
> mono Platformer.exe
```

# The tilengine window
The following actions can be done in the created window:
* Press <kbd>Esc</kbd> to close the window
* Press <kbd>Alt</kbd> + <kbd>Enter</kbd> to toggle full-screen/windowed
* Press <kbd>Backspace</kbd> to toggle built-in CRT effect (enabled by default)

# Creating your first program
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

## C/C++
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

### Windows 32-bit
```
> mingw32-gcc test.c -o test.exe -I"../../include" ../../lib/win32/Tilengine.lib
> test.exe
```

### Linux (32-bit, 64-bit or Raspberry Pi)
```
> gcc test.c -o test -lTilengine -lm
> ./test
```

### Apple OS X
```
> gcc test.c -o test "/usr/local/lib/Tilengine.dylib" -lm
> ./test
```

## Python
Create a file called `test.py` in `Tilengine/samples/python` folder, and type the following code:
```python
import tilengine as tln

engine = tln.Engine.create(400, 240, 1, 0, 20)
engine.set_load_path("../assets/sonic")
foreground = tln.Tilemap.fromfile("sonic_md_fg1.tmx")
engine.layers[0].setup(foreground)

window = tln.Window.create()
while window.process():
    window.draw_frame()
```
Python does not need separate build/run steps, and unlike C language, the procedure is the same for any platform. Just open a console window in the python samples folder and type the following command:
```
> python test.py
```

## C-Sharp
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

# Editing assets
Tilengine doesn't come with an editor, but the files it loads are made with standard open-source tools. Samples come bundled with several ready-to-use assets, but these are the tools you'll need to edit or create new ones:
* Source code: [VSCode](), [Notepad++]()...
* Graphics: [GIMP](http://www.gimp.org), [Grafx2](http://pulkomandy.tk/projects/GrafX2), [Paint.NET](https://www.getpaint.net/), or any graphic editor with 8-bit (256 colors) png support
* Backgrounds: [Tiled Map Editor](https://www.mapeditor.org/) is the official editor
* Sprites: Online tool [Lesy SpriteSheet](https://www.leshylabs.com/blog/posts/2013-12-03-Leshy_SpriteSheet_Tool.html)
* Sequences: Any standard XML editor

# Contributors
These people contributed to tilengine:

@turric4an - the Pascal wrapper<br>
@davideGiovannini - help with the Linux-x86_64 port<br>
@shayneoneill - help with the OS X port<br>
@adtennant - provided cmake and pkg-config support<br>
@tvasenin - improved C# binding<br>
@tyoungjr - LUA/FFI binding<br>
