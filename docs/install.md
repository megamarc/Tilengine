# Installation

[TOC]

1. Download tilengine from [GitHub](https://github.com/megamarc/Tilengine/archive/master.zip) or [itch.io](https://megamarc.itch.io/tilengine)
2. Open a console window in Tilengine root directory and type the suitable command depending on your platform:

## Windows

```
install
```

## Debian-based linux and OSX

```
./sudo install
```

**Note for OS X users**: Tilengine depends on having SDL2 in Framework format installed on your system, that by default is not installed. Please download it here: https://www.libsdl.org/release/SDL2-2.0.5.dmg. Homebrew port won't work, use the official DMG file instead.

## Running the samples
C samples are located in `Tilengine/samples` folder. To build them you need the gcc compiler suite, and/or Visual C++ in windows.
* **Linux**: the GCC compiler suite is already installed by default
* **Windows**: you must install [MinGW](http://www.mingw.org/) or [Visual Studio Community](https://www.visualstudio.com/vs/community/)
* **Apple OS X**: You must install [Command-line tools for Xcode](https://developer.apple.com/xcode/). An Apple ID account is required.

Once installed, open a console window in the C samples folder and type the suitable command depending on your platform:

### Windows (any architecture)
```
mingw32-make
```
### Unix-like (Linux, Raspberry Pi, Apple OS X)
```
make
```
