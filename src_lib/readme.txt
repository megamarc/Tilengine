*******************************************************************************
About source code included
*******************************************************************************

This directory contains the source code for the outer functionality of Tilengine: windowing and asset loaders. All this functions are based on other people's file format, or require third-party libraries. They cannot be built because the core engine is still closed source, but they can be used as a reference on how to implement custom loaders for different file formats or other windowing environments.

* Bitmap loading uses libpng library (www.libpng.org)
* Windowing and user input uses SDL2 library (www.libsdl.org)
* XML loaders use simplexml sources by Bruno Essmann (simplexml.sourceforge.net)
* The tileset and tilemap file formats are standard in Tiled (www.mapeditor.org)
* The spriteset format and packing tool was created by Nick Gravelyn (http://spritesheetpacker.codeplex.com/)
* Base64 decoding is public domain (https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64)

Megamarc 20 sep 2015