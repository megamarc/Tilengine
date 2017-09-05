# Installing {#install}
[TOC]
# Installing {#install}
1. Download tilengine from [GitHub](https://github.com/megamarc/Tilengine/archive/master.zip) or [itch.io](https://megamarc.itch.io/tilengine)
2. Open a console window in Tilengine root directory and type the suitable command depending on your platform:

## Windows (any architecture) {#install_windows}

```
> install
```
## Unix-like (Linux, Raspberry Pi, Apple OS X) {#install_unix}

```
> ./sudo install
```

**Note for OS X users**: Tilengine depends on having SDL2 in Framework format installed on your system, that by default is not installed. Please download it here: https://www.libsdl.org/release/SDL2-2.0.5.dmg. Homebrew port won't work, use the official DMG file instead.

## Running the C/C++ samples {#install_samples_c}
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

## Running the Python samples {#install_samples_python}
Python samples are located in `Tilengine/samples/python` folder. Make sure that Python v2 or v3 is installed on your system before proceeding.

Open a terminal window in the python samples folder, and run any sample by typing python *name_of_the_sample*.py, for example:
```
> python platformer.py
```

## Running the C# samples {#install_samples_csharp}
C# samples are located in `Tilengine/samples/csharp` folder. Make sure that [Mono](http://www.mono-project.com/download/) or .NET Framework (Windows only) is installed on your system. The following guide assumes Mono usage, please check Microsoft website for .NET Framework related info. C# has separate build and run steps like C, but these steps are the same on all platforms, as in Python.

In order to build the provided sample, open a console window in the C# samples folder and type the following command:
```
> mcs *.cs
```
This will produce `Platform.exe` that can be run with this command:
```
> mono Platformer.exe
```
