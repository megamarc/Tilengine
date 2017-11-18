/*
Tilengine - 2D Graphics library with raster effects
Copyright (c) 2015-2017 Marc Palacios Domenech (megamarc@hotmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
*****************************************************************************
* C# Tilengine wrapper - Up to date to library version 1.15
* http://www.tilengine.org
*****************************************************************************
*/

using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Tilengine
{
    /// <summary>
    /// 
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct Tile
    {
        public ushort index;
        public ushort flags;
    }

    public delegate void RasterCallback(int line);
	public delegate byte BlendFunction(byte src, byte dst);

    /// <summary>
    /// 
    /// </summary>
    public enum Error
    {
        Ok,
        OutOfMemory,
        IdxLayer,
        IdxSprite,
        IdxAnimation,
        IdxPicture,
        RefTileset,
        RefTilemap,
        RefSpriteset,
        RefPalette,
        RefSequence,
        RefSequencePack,
        RefBitmap,
        NullPointer,
        FileNotFound,
        WrongFormat,
        WrongSize,
        Unsupported,
        MaxError,
    }

    /// <summary>
    /// 
    /// </summary>
    public enum WindowFlags
    {
        Fullscreen = (1 << 0),
        Vsync = (1 << 1),
        S1 = (1 << 2),
        S2 = (2 << 2),
        S3 = (3 << 2),
        S4 = (4 << 2),
        S5 = (5 << 2),
    }

    /// <summary>
    /// 
    /// </summary>
    public enum Input
    {
        None,
        Up,
        Down,
        Left,
        Right,
        Button_A,
        Button_B,
        Button_C,
        Button_D,
    }

    /// <summary>
    /// 
    /// </summary>
    public enum Blend
    {
        None,
        Mix25,
		Mix50,
		Mix75,
        Add,
        Sub,
		Mod,
		Custom,
		Mix = Mix50
    }

    /// <summary>
    /// 
    /// </summary>
    public enum TileFlags
    {
        None        = (0),
        FlipX       = (1 << 15),
        FlipY       = (1 << 14),
        Rotate      = (1 << 13),
        Priority    = (1 << 12),
    }

    /// <summary>
    /// 
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct SpriteData
    {
        [MarshalAs(UnmanagedType.LPStr, SizeConst = 64)]
        public string Name;
        public int X;
        public int Y;
        public int W;
        public int H;
    }

    /// <summary>
    /// 
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct SpriteInfo
    {
        public int W;
        public int H;
    }

    /// <summary>
    /// 
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct TileInfo
    {
        public ushort Index;
        public ushort Flags;
        public int Row;
        public int Col;
        public int Xoffset;
        public int Yoffset;
        public byte Color;
        public byte Type;
        public bool Empty;
    }

    /// <summary>
    /// Tileset attributes for Tileset constructor
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
	public struct TileAttributes
	{
		public byte type;		// type of tile
		public bool priority;	// priority bit set
	}

    /// <summary>
    /// 
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct SequenceFrame
    {
	    public int index;
	    public int delay;
    }

    /// <summary>
    /// 
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct ColorStrip
    {
        public int Delay;
        public byte First;
        public byte Count;
        public byte Dir;
    }

    /// <summary>
    /// 
    /// </summary>
    public struct Color
    {
        public byte R,G,B;
        public Color(byte r, byte g, byte b)
        {
            R = r;
            G = g;
            B = b;
        }
    }
	
    /// <summary>
    /// overlays for CRT effect
    /// </summary>
	public enum Overlay
	{
		None,
		ShadowMask,
		Aperture,
		Scanlines,
		Custom
	}
	
    /// <summary>
    /// pixel mapping for Layer.SetPixelMapping()
    /// </summary>
	public struct PixelMap
	{
		public ushort dx, dy;
	}

    /// <summary>
    /// Creaton error exception
    /// </summary>
    public class CreationException : Exception
    {
    }

    /// <summary>
    /// 
    /// </summary>
    public class NotFoundException : Exception
    {
    }
    
    /// <summary>
    /// 
    /// </summary>
    public struct Engine
    {    
        // singleton
        private static Engine instance;
        private static bool init;

		public Layer[] Layers;
		public Sprite[] Sprites;
		public Animation[] Animations;

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_Init(int hres, int vres, int numlayers, int numsprites, int numanimations);

        [DllImport("Tilengine")]
        private static extern void TLN_Deinit();

        [DllImport("Tilengine")]
        private static extern int TLN_GetWidth();

        [DllImport("Tilengine")]
        private static extern int TLN_GetHeight();

        [DllImport("Tilengine")]
        private static extern uint TLN_GetNumObjects();

        [DllImport("Tilengine")]
        private static extern uint TLN_GetUsedMemory();

        [DllImport("Tilengine")]
        private static extern uint TLN_GetVersion();

        [DllImport("Tilengine")]
        private static extern int TLN_GetNumLayers();

        [DllImport("Tilengine")]
        private static extern int TLN_GetNumSprites();

        [DllImport("Tilengine")]
        private static extern void TLN_SetBGColor(byte r, byte g, byte b);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetBGColorFromTilemap(IntPtr tilemap);

        [DllImport("Tilengine")]
        private static extern void TLN_DisableBGColor();

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetBGBitmap(IntPtr bitmap);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetBGPalette(IntPtr palette);

        [DllImport("Tilengine")]
        private static extern void TLN_SetRasterCallback(RasterCallback callback);

        [DllImport("Tilengine")]
        private static extern void TLN_SetRenderTarget(byte[] data, int pitch);

        [DllImport("Tilengine")]
        private static extern void TLN_UpdateFrame(int time);

        [DllImport("Tilengine")]
        private static extern void TLN_BeginFrame(int frame);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DrawNextScanline();

        [DllImport("Tilengine")]
        private static extern void TLN_SetLastError(Error error);

        [DllImport("Tilengine")]
        private static extern Error TLN_GetLastError();

        [DllImport("Tilengine")]
        private static extern string TLN_GetErrorString(Error error);

        [DllImport("Tilengine")]
        private static extern int TLN_GetAvailableSprite();

        [DllImport("Tilengine")]
        private static extern void TLN_SetLoadPath(string path);
		
        [DllImport("Tilengine")]
        private static extern void TLN_SetCustomBlendFunction(BlendFunction function);

        private Engine (int hres, int vres, int numLayers, int numSprites, int numAnimations)
        {
            int c;

            Layers = new Layer[numLayers];
            for (c = 0; c < numLayers; c++)
                Layers[c].index = c;

            Sprites = new Sprite[numSprites];
            for (c = 0; c < numSprites; c++)
                Sprites[c].index = c;

            Animations = new Animation[numAnimations];
            for (c = 0; c < numAnimations; c++)
                Animations[c].index = c;
        }

        /// <summary>
        /// Initializes the graphic engine
        /// </summary>
        /// <param name="hres">horizontal resolution in pixels</param>
        /// <param name="vres">vertical resolution in pixels</param>
        /// <param name="numLayers">number of layers</param>
        /// <param name="numSprites">number of sprites</param>
        /// <param name="numAnimations">number of animations</param>
        /// <returns>Engine instance</returns>
        /// <remarks>This is a singleton object: calling Init multiple times will return the same reference</remarks>
        public static Engine Init(int hres, int vres, int numLayers, int numSprites, int numAnimations)
        {
            // singleton
            if (!init)
            {
                bool retval = TLN_Init(hres, vres, numLayers, numSprites, numAnimations);
                if (retval)
                {
                    init = true;
                    instance = new Engine(hres, vres, numLayers, numSprites, numAnimations);
                }
                else
                    throw new CreationException();
            }

            return instance;
        }

        /// <summary>
        /// Deinits engine and frees associated resources
        /// </summary>
        public void Deinit()
        {
            TLN_Deinit();
        }

        /// <summary>
        /// Returns the width in pixels of the framebuffer
        /// </summary>
        public int Width
        {
            get { return TLN_GetWidth(); }
        }

        /// <summary>
        /// Returns the height in pixels of the framebuffer
        /// </summary>
        public int Height
        {
            get { return TLN_GetHeight(); }
        }

        /// <summary>
        /// Returns the number of objets used by the engine so far
        /// </summary>
        public uint NumObjects
        {
            get { return TLN_GetNumObjects(); }
        }

        /// <summary>
        /// Returns the total amount of memory used by the objects so far
        /// </summary>
        public uint UsedMemory
        {
            get { return TLN_GetUsedMemory(); }
        }

        /// <summary>
        /// Retrieves Tilengine dll version, in a 32-bit integer
        /// </summary>
        public uint Version
        {
            get { return TLN_GetVersion(); }
        }

        /// <summary>
        /// Sets the background color, that is the color of the pixel when there isn't any layer or sprite at that position
        /// </summary>
        public void SetBackgroundColor(Color value)
        {
            TLN_SetBGColor(value.R, value.G, value.B);
        }

        /// <summary>
        /// Sets the background color from a tilemap, that is the color of the pixel when there isn't any layer or sprite at that position
        /// </summary>
        public void SetBackgroundColor(Tilemap tilemap)
        {
            TLN_SetBGColorFromTilemap(tilemap.ptr);
        }

        /// <summary>
		/// Disales background color rendering. If you know that the last background layer will always
        /// cover the entire screen, you can disable it to gain some performance
        /// </summary>
        public void DisableBackgroundColor()
        {
            TLN_DisableBGColor();
        }

        /// <summary>
        /// Sets an optional, static bitmap as background instead of a solid color
        /// </summary>
        public Bitmap BackgroundBitmap
        {
            set { TLN_SetBGBitmap(value.ptr); }
        }

        /// <summary>
        /// Sets the palette for the optional background bitmap
        /// </summary>
        public Palette BackgroundPalette
        {
            set { TLN_SetBGPalette(value.ptr); }
        }

        /// <summary>
        /// Sets the output surface for rendering
        /// </summary>
        /// <param name="data">Array of bytes that will hold the render target</param>
        /// <param name="pitch">Number of bytes per each scanline of the framebuffer</param>
        /// <remarks>The render target pixel format must be 32 bits RGBA</remarks>
        public void SetRenderTarget(byte[] data, int pitch)
        {
            TLN_SetRenderTarget(data, pitch);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="callback"></param>
        public void SetRasterCallback(RasterCallback callback)
        {
            TLN_SetRasterCallback(callback);
        }

        /// <summary>
        /// 
        /// </summary>
        public String LoadPath
        {
            set { TLN_SetLoadPath(value); }
        }
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="function"></param>
        public void SetCustomBlendFunction(BlendFunction function)
        {
            TLN_SetCustomBlendFunction(function);
        }

        /// <summary>
        /// Starts active rendering of the current frame
        /// </summary>
        /// <param name="frame">Timestamp value</param>
        /// <remarks>This method is used for active rendering combined with DrawNextScanline(), instead of using delegates for raster effects</remarks>
        public void BeginFrame(int frame)
        {
            TLN_BeginFrame(frame);
        }

        /// <summary>
        /// Draws the next scanline of the frame when doing active rendering (without delegates)
        /// </summary>
        /// <returns>true if there are still scanlines to draw or false when the frame is complete</returns>
        public bool DrawNextScanline()
        {
            return TLN_DrawNextScanline();
        }

        /// <summary>
        /// Returns the last error after an invalid operation
        /// </summary>
        public Error LastError
        {
            set { TLN_SetLastError(value); }
            get { return TLN_GetLastError(); }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="error"></param>
        /// <returns>Returns a string description about the given error</returns>
        public string GetErrorString(Error error)
        {
            return TLN_GetErrorString(error);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public Sprite GetAvailableSprite()
        {
            int index = TLN_GetAvailableSprite();
            if (index >= 0)
                return Sprites[index];
            else
                throw new NotFoundException();
        }
    }

    /// <summary>
    /// Built-in windowing and user input
    /// </summary>
    public struct Window
    {
        // singleton
        private static Window instance;
        private static bool init;

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_CreateWindow(string overlay, WindowFlags flags);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_CreateWindowThread(string overlay, WindowFlags flags);

        [DllImport("Tilengine")]
        private static extern void TLN_SetWindowTitle (string title);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_ProcessWindow();
        
        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_IsWindowActive();

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_GetInput(Input id);

        [DllImport("Tilengine")]
        private static extern Input TLN_GetLastInput();

        [DllImport("Tilengine")]
        private static extern void TLN_DrawFrame(int time);

        [DllImport("Tilengine")]
        private static extern void TLN_WaitRedraw();

        [DllImport("Tilengine")]
        private static extern void TLN_DeleteWindow();
		
		[DllImport("Tilengine")]
		private static extern void TLN_EnableCRTEffect(Overlay overlay, byte overlay_factor, byte threshold, byte v0, byte v1, byte v2, byte v3, bool blur, byte glow_factor);
		
		[DllImport("Tilengine")]
		private static extern void TLN_DisableCRTEffect();		

        [DllImport("Tilengine")]
        private static extern void TLN_Delay(uint msecs);

        [DllImport("Tilengine")]
        private static extern uint TLN_GetTicks();

        [DllImport("Tilengine")]
        private static extern void TLN_BeginWindowFrame(int frame);

        [DllImport("Tilengine")]
        private static extern void TLN_EndWindowFrame();

        /// <summary>
        /// Creates a window for rendering
        /// </summary>
        /// <param name="overlay">Optional path of a bmp file to overlay (for emulating RGB mask, scanlines, etc)</param>
        /// <param name="flags">Combined mask of the possible creation flags</param>
        /// <returns>Window instance</returns>
        /// <remarks>This is a singleton object: calling Init multiple times will return the same reference</remarks>
        public static Window Create(string overlay, WindowFlags flags)
        {
            // singleton
            if (!init)
            {
                bool retval = TLN_CreateWindow (overlay, flags);
                if (retval)
                {
                    init = true;
                    instance = new Window();
                }
                else
                    throw new CreationException();
            }

            return instance;
        }

        /// <summary>
        /// Creates a multithreaded window for rendering
        /// </summary>
        /// <param name="overlay">Optional path of a bmp file to overlay (for emulating RGB mask, scanlines, etc)</param>
        /// <param name="flags">Combined mask of the possible creation flags</param>
        /// <returns>Window instance</returns>
        /// <remarks>This is a singleton object: calling Init multiple times will return the same reference</remarks>
        public static Window CreateThreaded(string overlay, WindowFlags flags)
        {
            // singleton
            if (!init)
            {
                bool retval = TLN_CreateWindowThread (overlay, flags);
                if (retval)
                {
                    init = true;
                    instance = new Window();
                }
                else
                    throw new CreationException();
            }

            return instance;
        }

        /// <summary>
        /// Sets the title of the window
        /// </summary>
        public string Title
        {
            set { TLN_SetWindowTitle(value); }
        }

        /// <summary>
        /// Does basic window housekeeping in signgle-threaded window. Must be called for each frame in game loop
        /// </summary>
        /// <returns>true if window is active or false if the user has requested to end the application (by pressing Esc key or clicking the close button)</returns>
        /// <remarks>This method must be called only for single-threaded windows, created with Create() method.</remarks>
        public bool Process ()
        {
            return TLN_ProcessWindow ();
        }
        
        /// <summary>
        /// true if window is active or false if the user has requested to end the application (by pressing Esc key or clicking the close button)
        /// </summary>
        public bool Active
        {
            get { return TLN_IsWindowActive(); }
        }

        /// <summary>
        /// Returns the state of a given input
        /// </summary>
        /// <param name="id">Input identificator to check state</param>
        /// <returns>true if that input is pressed or false if not</returns>
        public bool GetInput (Input id)
        {
            return TLN_GetInput(id);
        }

        /// <summary>
        /// Returns the last pressed input button
        /// </summary>
        public Input LastInput
        {
            get { return TLN_GetLastInput(); }
        }

        /// <summary>
        /// Begins active rendering frame
        /// </summary>
        /// <param name="frame">Timestamp value</param>
        /// <remarks>Use this method instead of Engine.BeginFrame() when using build-in windowing</remarks>
        public void BeginFrame(int frame)
        {
            TLN_BeginWindowFrame(frame);
        }

        /// <summary>
        /// Draws a frame to the window
        /// </summary>
        /// <param name="time">Timestamp value</param>
        /// <remarks>This method does delegate-driven rendering</remarks>
        public void DrawFrame(int time)
        {
            TLN_DrawFrame(time);
        }

        /// <summary>
        /// 
        /// </summary>
        public void EndFrame()
        {
            TLN_EndWindowFrame();
        }

        /// <summary>
        /// 
        /// </summary>
        public void WaitRedraw()
        {
            TLN_WaitRedraw();
        }

        /// <summary>
        /// Deprecated, use EnableCRTEffect() instead
        /// </summary>
        public bool Blur
        {
            set { return; }
        }
		
        /// <summary>
        /// 
        /// </summary>
		public void EnableCRTEffect(Overlay overlay, byte overlay_factor, byte threshold, byte v0, byte v1, byte v2, byte v3, bool blur, byte glow_factor)
		{
			TLN_EnableCRTEffect(overlay, overlay_factor, threshold, v0, v1, v2, v3, blur, glow_factor);
		}
		
        /// <summary>
        /// 
        /// </summary>
		public void DisableCRTEffect()
		{
			TLN_DisableCRTEffect();
		}		

        /// <summary>
        /// 
        /// </summary>
        /// <param name="msecs"></param>
        public void Delay(uint msecs)
        {
            TLN_Delay(msecs);
        }

        /// <summary>
        /// 
        /// </summary>
        public uint Ticks
        {
            get { return TLN_GetTicks(); }
        }

        /// <summary>
        /// 
        /// </summary>
        public void Delete()
        {
            TLN_DeleteWindow();
        }
    }

    /// <summary>
    /// Layer management
    /// </summary>
    public struct Layer
    {
		internal int index;
		
        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetLayer(int nlayer, IntPtr tileset, IntPtr tilemap);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetLayerPalette(int nlayer, IntPtr palette);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetLayerPosition(int nlayer, int hstart, int vstart);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetLayerScaling(int nlayer, float xfactor, float yfactor);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetLayerTransform(int nlayer, float angle, float dx, float dy, float sx, float sy);
		
        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
		private static extern bool TLN_SetLayerPixelMapping (int nlayer, PixelMap[] table);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetLayerBlendMode(int nlayer, Blend mode, byte factor);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetLayerColumnOffset(int nlayer, int[] offset);
		
        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetLayerClip(int nlayer, int x1, int y1, int x2, int y2);
		
        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DisableLayerClip(int nlayer);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetLayerMosaic (int nlayer, int width, int height);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DisableLayerMosaic(int nlayer);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_ResetLayerMode(int nlayer);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DisableLayer(int nlayer);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetLayerPalette(int nlayer);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_GetLayerTile(int nlayer, int x, int y, out TileInfo info);

        [DllImport("Tilengine")]
        private static extern int TLN_GetLayerWidth(int nlayer);
        
        [DllImport("Tilengine")]
        private static extern int TLN_GetLayerHeight(int nlayer);            

        /// <summary>
        /// 
        /// </summary>
        /// <param name="tileset"></param>
        /// <param name="tilemap"></param>
        /// <returns></returns>
        public bool Setup(Tileset tileset, Tilemap tilemap)
        {
            return TLN_SetLayer(index, tileset.ptr, tilemap.ptr);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="tilemap"></param>
        /// <returns></returns>
        public bool SetMap(Tilemap tilemap)
        {
            return TLN_SetLayer(index, IntPtr.Zero, tilemap.ptr);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        public bool SetPosition(int x, int y)
        {
            return TLN_SetLayerPosition(index, x, y);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sx"></param>
        /// <param name="sy"></param>
        /// <returns></returns>
        public bool SetScaling(float sx, float sy)
        {
            return TLN_SetLayerScaling(index, sx, sy);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="angle"></param>
        /// <param name="dx"></param>
        /// <param name="dy"></param>
        /// <param name="sx"></param>
        /// <param name="sy"></param>
        /// <returns></returns>
        public bool SetTransform(float angle, float dx, float dy, float sx, float sy)
        {
            return TLN_SetLayerTransform(index, angle, dx, dy, sx, sy);
        }

        /// <summary>
        /// 
        /// </summary>
		/// <param name="">map</param>
        /// <returns></returns>
		public bool SetPixelMapping(PixelMap[] map)
		{
			return TLN_SetLayerPixelMapping(index, map);
		}
		
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Reset()
        {
            return TLN_ResetLayerMode(index);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="blend"></param>
        /// <returns></returns>
        public bool SetBlendMode(Blend blend)
        {
            return TLN_SetLayerBlendMode(index, blend, 0);
        }

        /// <summary>
        /// 
        /// </summary>
        public int[] ColumnOffset
        {
            set { TLN_SetLayerColumnOffset(index, value); }
        }
		
        /// <summary>
        /// 
        /// </summary>
		/// <param name="x1"></param>
		/// <param name="y1"></param>
		/// <param name="x2"></param>
		/// <param name="y2"></param>
		public bool SetClip(int x1, int y1, int x2, int y2)
		{
			return TLN_SetLayerClip(index, x1, y1, x2, y2);
		}
		
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
        public bool DisableClip()
		{
			return TLN_DisableLayerClip(index);
		}

        /// <summary>
        /// 
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <returns></returns>
        public bool SetMosaic(int width, int height)
        {
            return TLN_SetLayerMosaic(index, width, height);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool DisableMosaic()
        {
            return TLN_DisableLayerMosaic(index);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="info"></param>
        /// <returns></returns>
        public bool GetTileInfo(int x, int y, out TileInfo info)
        {
            return TLN_GetLayerTile(index, x, y, out info);
        }

        /// <summary>
        /// 
        /// </summary>
        public Palette Palette
        {
            get { return new Palette(TLN_GetLayerPalette(index)); }
            set { TLN_SetLayerPalette(index, value.ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Width
        {
            get { return TLN_GetLayerWidth(index); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Height
        {
            get { return TLN_GetLayerHeight(index); }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Disable()
        {
            return TLN_DisableLayer(index);
        }
    }

    /// <summary>
    /// Sprite management
    /// </summary>
    public struct Sprite
    {
		internal int index;
		
        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_ConfigSprite(int nsprite, IntPtr spriteset, TileFlags flags);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetSpriteSet(int nsprite, IntPtr spriteset);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetSpriteFlags(int nsprite, TileFlags flags);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetSpritePosition(int nsprite, int x, int y);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetSpritePicture(int nsprite, int entry);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetSpritePalette(int nsprite, IntPtr palette);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetSpriteBlendMode(int nsprite, Blend mode, byte factor);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetSpriteScaling(int nsprite, float sx, float sy);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_ResetSpriteScaling(int nsprite);

        [DllImport("Tilengine")]
        private static extern int TLN_GetSpritePicture(int nsprite);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_EnableSpriteCollision(int nsprite, [MarshalAsAttribute(UnmanagedType.I1)] bool enable);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_GetSpriteCollision(int nsprite);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DisableSprite(int nsprite);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetSpritePalette(int nsprite);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="spriteset"></param>
        /// <param name="flags"></param>
        /// <returns></returns>
        public bool Setup(Spriteset spriteset, TileFlags flags)
        {
            return TLN_ConfigSprite(index, spriteset.ptr, flags);
        }

        /// <summary>
        /// 
        /// </summary>
        public Spriteset Spriteset
        {
            set { TLN_SetSpriteSet(index, value.ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public TileFlags Flags
        {
            set { TLN_SetSpriteFlags(index, value); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Picture
        {
            set { TLN_SetSpritePicture(index, value); }
            get { return TLN_GetSpritePicture(index); }
        }

        /// <summary>
        /// 
        /// </summary>
        public Palette Palette
        {
            set { TLN_SetSpritePalette(index, value.ptr); }
            get { return new Palette(TLN_GetSpritePalette(index)); }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <returns></returns>
        public bool SetPosition(int x, int y)
        {
            return TLN_SetSpritePosition(index, x, y);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sx"></param>
        /// <param name="sy"></param>
        /// <returns></returns>
        public bool SetScaling(float sx, float sy)
        {
            return TLN_SetSpriteScaling(index, sx, sy);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Reset()
        {
            return TLN_ResetSpriteScaling(index);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="blend"></param>
        /// <param name="factor"></param>
        /// <returns></returns>
        public bool SetBlendMode(Blend blend, byte factor)
        {
            return TLN_SetSpriteBlendMode(index, blend, factor);
        }

        /// <summary>
        /// 
        /// </summary>
        public bool Collision
        {
            set { TLN_EnableSpriteCollision(index, value); }
            get { return TLN_GetSpriteCollision(index); }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Disable()
        {
            return TLN_DisableSprite(index);
        }
    }

    /// <summary>
    /// Animation management
    /// </summary>
    public struct Animation
    {
		internal int index;
		
        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetPaletteAnimation(int index, IntPtr palette, IntPtr sequence, [MarshalAsAttribute(UnmanagedType.I1)] bool blend);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetPaletteAnimationSource(int index, IntPtr palette);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetTilemapAnimation(int index, int nlayer, IntPtr sequence);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetTilesetAnimation(int index, int nlayer, IntPtr sequence);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetSpriteAnimation(int index, int nsprite, IntPtr sequence, int loop);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_GetAnimationState(int index);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetAnimationDelay(int index, int delay);

        [DllImport("Tilengine")]
        private static extern int TLN_GetAvailableAnimation();

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DisableAnimation(int index);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="palette"></param>
        /// <param name="sequence"></param>
        /// <param name="blend"></param>
        /// <returns></returns>
        public bool SetPaletteAnimation(Palette palette, Sequence sequence, bool blend)
        {
            return TLN_SetPaletteAnimation(index, palette.ptr, sequence.ptr, blend);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="palette"></param>
        /// <returns></returns>
        public bool SetPaletteAnimationSource(Palette palette)
        {
            return TLN_SetPaletteAnimationSource(index, palette.ptr);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="layerIndex"></param>
        /// <param name="sequence"></param>
        /// <returns></returns>
        public bool SetTilesetAnimation(int layerIndex, Sequence sequence)
        {
            return TLN_SetTilesetAnimation(index, layerIndex, sequence.ptr);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="layerIndex"></param>
        /// <param name="sequence"></param>
        /// <returns></returns>
        public bool SetTilemapAnimation(int layerIndex, Sequence sequence)
        {
            return TLN_SetTilemapAnimation(index, layerIndex, sequence.ptr);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="spriteIndex"></param>
        /// <param name="sequence"></param>
        /// <param name="loop"></param>
        /// <returns></returns>
        public bool SetSpriteAnimation(int spriteIndex, Sequence sequence, int loop)
        {
            return TLN_SetSpriteAnimation(index, spriteIndex, sequence.ptr, loop);
        }

        /// <summary>
        /// 
        /// </summary>
        public bool Active
        {
            get { return TLN_GetAnimationState(index); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Delay
        {
            set { TLN_SetAnimationDelay(index, value); }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Disable()
        {
            return TLN_DisableAnimation(index);
        }
    }

    /// <summary>
    /// Spriteset resource
    /// </summary>
    public struct Spriteset
    {
        internal IntPtr ptr;

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CreateSpriteset(IntPtr bitmap, SpriteData[] rects, int entries);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_LoadSpriteset(string name);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CloneSpriteset(IntPtr src);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_GetSpriteInfo(IntPtr spriteset, int entry, out SpriteInfo info);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetSpritesetPalette(IntPtr spriteset);

        [DllImport("Tilengine")]
        private static extern int TLN_FindSpritesetSprite(IntPtr spriteset, string name);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DeleteSpriteset(IntPtr Spriteset);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="res"></param>
        internal Spriteset (IntPtr res)
        {
            ptr = res;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="bitmap"></param>
        /// <param name="data"></param>
        public Spriteset (Bitmap bitmap, SpriteData[] data)
        {
            IntPtr retval = TLN_CreateSpriteset(bitmap.ptr, data, data.Length);
            if (retval != IntPtr.Zero)
                ptr = retval;
            else
                throw new CreationException ();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Spriteset FromFile (string filename)
        {
            IntPtr retval = TLN_LoadSpriteset (filename);
            if (retval != IntPtr.Zero)
                return new Spriteset (retval);
            else
                throw new FileNotFoundException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public Spriteset Clone ()
        {
            IntPtr retval = TLN_CloneSpriteset(ptr);
            if (retval != IntPtr.Zero)
                return new Spriteset (retval);
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="index"></param>
        /// <param name="info"></param>
        /// <returns></returns>
        public bool GetInfo (int index, out SpriteInfo info)
        {
            return TLN_GetSpriteInfo (ptr, index, out info);
        }

        /// <summary>
        /// 
        /// </summary>
        public Palette Palette
        {
            get { return new Palette(TLN_GetSpritesetPalette(ptr)); }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public int FindSprite(string name)
        {
            return TLN_FindSpritesetSprite(ptr, name);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Delete ()
        {
            return TLN_DeleteSpriteset (ptr);
        }
    }

    /// <summary>
    /// Tileset resource
    /// </summary>
    public struct Tileset
    {
        internal IntPtr ptr;

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CreateTileset(int numtiles, int width, int height, IntPtr palette, IntPtr sequencepack, TileAttributes[] attributes);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_LoadTileset(string filename);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CloneTileset(IntPtr src);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_CopyTile(IntPtr tileset, int src, int dst);
        
        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetTilesetPixels(IntPtr tileset, int entry, byte[] srcdata, int srcpitch);

        [DllImport("Tilengine")]
        private static extern int TLN_GetTileWidth(IntPtr tileset);

        [DllImport("Tilengine")]
        private static extern int TLN_GetTileHeight(IntPtr tileset);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetTilesetPalette(IntPtr tileset);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetTilesetSequencePack(IntPtr tileset);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DeleteTileset(IntPtr tileset);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="res"></param>
        internal Tileset (IntPtr res)
        {
            ptr = res;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="numTiles"></param>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <param name="palette"></param>
        /// <param name="palette"></param>
        /// <param name="palette"></param>
        public Tileset(int numTiles, int width, int height, Palette palette, SequencePack sp, TileAttributes[] attributes)
        {
            IntPtr retval = TLN_CreateTileset(numTiles, width, height, palette.ptr, sp.ptr, attributes);
            if (retval != IntPtr.Zero)
                ptr = retval;
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Tileset FromFile(string filename)
        {
            IntPtr retval = TLN_LoadTileset(filename);
            if (retval != IntPtr.Zero)
                return new Tileset(retval);
            else
                throw new FileNotFoundException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public Tileset Clone()
        {
            IntPtr retval = TLN_CloneTileset(ptr);
            if (retval != IntPtr.Zero)
                return new Tileset(retval);
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="entry"></param>
        /// <param name="pixels"></param>
        /// <param name="pitch"></param>
        /// <returns></returns>
        public bool SetPixels(int entry, byte[] pixels, int pitch)
        {
            return TLN_SetTilesetPixels(ptr, entry, pixels, pitch);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="src"></param>
        /// <param name="dst"></param>
        /// <returns></returns>
        public bool CopyTile(int src, int dst)
        {
            return TLN_CopyTile(ptr, src, dst);
        }

        /// <summary>
        /// 
        /// </summary>
        public int Width
        {
            get { return TLN_GetTileWidth(ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Height
        {
            get { return TLN_GetTileHeight(ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public Palette Palette
        {
            get { return new Palette(TLN_GetTilesetPalette(ptr)); }
        }

        /// <summary>
        /// 
        /// </summary>
        public SequencePack SequencePack
        {
            get { return new SequencePack(TLN_GetTilesetSequencePack(ptr)); }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Delete()
        {
            IntPtr value = ptr;
            ptr = IntPtr.Zero;
            return TLN_DeleteTileset(value);
        }
    }

    /// <summary>
    /// Tilemap resource
    /// </summary>
    public struct Tilemap
    {
        internal IntPtr ptr;

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CreateTilemap(int rows, int cols, Tile[] tiles, uint bgcolor, Tileset tileset);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_LoadTilemap(string filename, string layername);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CloneTilemap(IntPtr src);

        [DllImport("Tilengine")]
        private static extern int TLN_GetTilemapRows(IntPtr tilemap);

        [DllImport("Tilengine")]
        private static extern int TLN_GetTilemapCols(IntPtr tilemap);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetTilemapTileset(IntPtr tilemap);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_GetTilemapTile(IntPtr tilemap, int row, int col, out Tile tile);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetTilemapTile(IntPtr tilemap, int row, int col, ref Tile tile);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_CopyTiles(IntPtr src, int srcrow, int srccol, int rows, int cols, IntPtr dst, int dstrow, int dstcol);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DeleteTilemap(IntPtr tilemap);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="res"></param>
        internal Tilemap (IntPtr res)
        {
            ptr = res;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="rows"></param>
        /// <param name="cols"></param>
        /// <param name="tiles"></param>
        /// <param name="bgcolor"></param>
        /// <param name="tileset"></param>
        public Tilemap(int rows, int cols, Tile[] tiles, Color bgcolor, Tileset tileset)
        {
            long color;
            color = 0xFF000000 + (bgcolor.R << 16) + (bgcolor.G << 8) + bgcolor.B;

            IntPtr retval = TLN_CreateTilemap(rows, cols, tiles, (uint)color, tileset);
            if (retval != IntPtr.Zero)
                ptr = retval;
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        /// <param name="layername"></param>
        /// <returns></returns>
        public static Tilemap FromFile(string filename, string layername)
        {
            IntPtr retval = TLN_LoadTilemap(filename, layername);
            if (retval != IntPtr.Zero)
                return new Tilemap(retval);
            else
                throw new FileNotFoundException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public Tilemap Clone()
        {
            IntPtr retval = TLN_CloneTilemap(ptr);
            if (retval != IntPtr.Zero)
                return new Tilemap(retval);
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        public int Cols
        {
            get { return TLN_GetTilemapCols(ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Rows
        {
            get { return TLN_GetTilemapRows(ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public Tileset Tileset
        {
            get { return new Tileset(TLN_GetTilemapTileset(ptr)); }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="row"></param>
        /// <param name="col"></param>
        /// <param name="tile"></param>
        /// <returns></returns>
        public bool SetTile(int row, int col, ref Tile tile)
        {
            return TLN_SetTilemapTile(ptr, row, col, ref tile);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="row"></param>
        /// <param name="col"></param>
        /// <param name="tile"></param>
        /// <returns></returns>
        public bool GetTile(int row, int col, out Tile tile)
        {
            return TLN_GetTilemapTile(ptr, row, col, out tile);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="srcRow"></param>
        /// <param name="srcCol"></param>
        /// <param name="rows"></param>
        /// <param name="cols"></param>
        /// <param name="dst"></param>
        /// <param name="dstRow"></param>
        /// <param name="dstCol"></param>
        /// <returns></returns>
        public bool CopyTiles(int srcRow, int srcCol, int rows, int cols, Tilemap dst, int dstRow, int dstCol)
        {
            return TLN_CopyTiles(ptr, srcRow, srcCol, rows, cols, dst.ptr, dstRow, dstCol);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Delete()
        {
            IntPtr value = ptr;
            ptr = IntPtr.Zero;
            return TLN_DeleteTilemap(value);
        }
    }

    /// <summary>
    /// Palette resource
    /// </summary>
    public struct Palette
    {
        internal IntPtr ptr;

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CreatePalette(int entries);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_LoadPalette(string filename);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_ClonePalette(IntPtr src);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetPaletteColor(IntPtr palette, int index, byte r, byte g, byte b);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_MixPalettes(IntPtr src1, IntPtr src2, IntPtr dst, byte factor);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_AddPaletteColor(IntPtr palette, byte r, byte g, byte b, byte start, byte num);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SubPaletteColor(IntPtr palette, byte r, byte g, byte b, byte start, byte num);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_ModPaletteColor(IntPtr palette, byte r, byte g, byte b, byte start, byte num);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetPaletteData(IntPtr palette, int index);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DeletePalette(IntPtr palette);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="res"></param>
        internal Palette (IntPtr res)
        {
            ptr = res;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="entries"></param>
        public Palette(int entries)
        {
            IntPtr retval = TLN_CreatePalette(entries);
            if (retval != IntPtr.Zero)
                ptr = retval;
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Palette FromFile(string filename)
        {
            IntPtr retval = TLN_LoadPalette(filename);
            if (retval != IntPtr.Zero)
                return new Palette(retval);
            else
                throw new FileNotFoundException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public Palette Clone()
        {
            IntPtr retval = TLN_ClonePalette(ptr);
            if (retval != IntPtr.Zero)
                return new Palette(retval);
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="index"></param>
        /// <param name="color"></param>
        /// <returns></returns>
        public bool SetColor(int index, Color color)
        {
            return TLN_SetPaletteColor(ptr, index, color.R, color.G, color.B);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="src1"></param>
        /// <param name="src2"></param>
        /// <param name="factor"></param>
        /// <returns></returns>
        public bool Mix(Palette src1, Palette src2, byte factor)
        {
            return TLN_MixPalettes(src1.ptr, src2.ptr, ptr, factor);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="color"></param>
        /// <param name="first"></param>
        /// <param name="count"></param>
        /// <returns></returns>
        public bool AddColor(Color color, byte first, byte count)
        {
            return TLN_AddPaletteColor(ptr, color.R, color.G, color.B, first, count);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="color"></param>
        /// <param name="first"></param>
        /// <param name="count"></param>
        /// <returns></returns>
        public bool SubColor(Color color, byte first, byte count)
        {
            return TLN_SubPaletteColor(ptr, color.R, color.G, color.B, first, count);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="color"></param>
        /// <param name="first"></param>
        /// <param name="count"></param>
        /// <returns></returns>
        public bool MulColor(Color color, byte first, byte count)
        {
            return TLN_ModPaletteColor(ptr, color.R, color.G, color.B, first, count);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Delete()
        {
            IntPtr value = ptr;
            ptr = IntPtr.Zero;
            return TLN_DeletePalette(value);
        }
    }

    /// <summary>
    /// Bitmap resource
    /// </summary>
    public struct Bitmap
    {
        internal IntPtr ptr;

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CreateBitmap(int width, int height, int bpp);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_LoadBitmap(string filename);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CloneBitmap(IntPtr src);

        [DllImport("Tilengine")]
        private static extern int TLN_GetBitmapWidth(IntPtr bitmap);

        [DllImport("Tilengine")]
        private static extern int TLN_GetBitmapHeight(IntPtr bitmap);

        [DllImport("Tilengine")]
        private static extern int TLN_GetBitmapDepth(IntPtr bitmap);

        [DllImport("Tilengine")]
        private static extern int TLN_GetBitmapPitch(IntPtr bitmap);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetBitmapPtr(IntPtr bitmap, int x, int y);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetBitmapPalette(IntPtr bitmap);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_SetBitmapPalette(IntPtr bitmap, IntPtr palette);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DeleteBitmap(IntPtr bitmap);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="res"></param>
        internal Bitmap(IntPtr res)
        {
            ptr = res;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <param name="bpp"></param>
        public Bitmap(int width, int height, int bpp)
        {
            IntPtr retval = TLN_CreateBitmap(width, height, bpp);
            if (retval != IntPtr.Zero)
                ptr = retval;
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Bitmap FromFile(string filename)
        {
            IntPtr retval = TLN_LoadBitmap(filename);
            if (retval != IntPtr.Zero)
                return new Bitmap(retval);
            else
                throw new FileNotFoundException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public Bitmap Clone()
        {
            IntPtr retval = TLN_CloneBitmap(ptr);
            if (retval != IntPtr.Zero)
                return new Bitmap(retval);
            else
                throw new CreationException();
        }

        /// <summary>
        /// Raw pixel data
        /// </summary>
        public byte[] PixelData
        {
            get
            {
                byte[] pixelData = new byte[Pitch * Height];
                IntPtr ptrPixelData = TLN_GetBitmapPtr(ptr, 0, 0);
                Marshal.Copy(ptrPixelData, pixelData, 0, pixelData.Length);
                return pixelData;
            }

            set
            {
                if (Pitch * Height != value.Length) { throw new ArgumentException(); }
                IntPtr ptrPixelData = TLN_GetBitmapPtr(ptr, 0, 0);
                Marshal.Copy(value, 0, ptrPixelData, value.Length);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Width
        {
            get { return TLN_GetBitmapWidth(ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Height
        {
            get { return TLN_GetBitmapHeight(ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Depth
        {
            get { return TLN_GetBitmapDepth(ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int Pitch
        {
            get { return TLN_GetBitmapPitch(ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        public Palette Palette
        {
            get { return new Palette(TLN_GetBitmapPalette(ptr)); }
            set { TLN_SetBitmapPalette(ptr, value.ptr); }
        }
    }

    /// <summary>
    /// Sequence resource
    /// </summary>
    public struct Sequence
    {
        internal IntPtr ptr;

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CreateSequence(string name, int target, int num_frames, SequenceFrame[] frames);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CreateCycle(string name, int num_strips, ColorStrip[] strips);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CloneSequence(IntPtr src);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DeleteSequence(IntPtr sequence);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="res"></param>
        internal Sequence (IntPtr res)
        {
            ptr = res;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="delay"></param>
        /// <param name="first"></param>
        /// <param name="data"></param>
        public Sequence(string name, int target, SequenceFrame[] frames)
        {
            IntPtr retval = TLN_CreateSequence(name, target, frames.Length, frames);
            if (retval != IntPtr.Zero)
                ptr = retval;
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <param name="strips"></param>
        public Sequence(string name, ColorStrip[] strips)
        {
            IntPtr retval = TLN_CreateCycle(name, strips.Length, strips);
            if (retval != IntPtr.Zero)
                ptr = retval;
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public Sequence Clone()
        {
            IntPtr retval = TLN_CloneSequence(ptr);
            if (retval != IntPtr.Zero)
                return new Sequence(retval);
            else
                throw new CreationException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Delete()
        {
            IntPtr value = ptr;
            ptr = IntPtr.Zero;
            return TLN_DeleteSequence(value);
        }
    }
     
    /// <summary>
    /// SequencePack resource
    /// </summary>
    public struct SequencePack
    {
        internal IntPtr ptr;

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_CreateSequencePack();

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_LoadSequencePack(string filename);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_FindSequence(IntPtr sp, string name);

        [DllImport("Tilengine")]
        private static extern IntPtr TLN_GetSequence(IntPtr sp, int index);

        [DllImport("Tilengine")]
        private static extern int TLN_GetSequencePackCount(IntPtr sp);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_AddSequenceToPack(IntPtr sp, IntPtr sequence);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_DeleteSequencePack(IntPtr sp);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="res"></param>
        internal SequencePack (IntPtr res)
        {
            ptr = res;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static SequencePack FromFile(string filename)
        {
            IntPtr retval = TLN_LoadSequencePack(filename);
            if (retval != IntPtr.Zero)
                return new SequencePack(retval);
            else
                throw new FileNotFoundException();
        }

        /// <summary>
        /// 
        /// </summary>
        public int NumSequences
        {
            get { return TLN_GetSequencePackCount(ptr); }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public Sequence Find(string name)
        {
            IntPtr retval = TLN_FindSequence(ptr, name);
            if (retval != IntPtr.Zero)
                return new Sequence(retval);
            else
                throw new NotFoundException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="index"></param>
        /// <returns></returns>
        public Sequence Get(int index)
        {
            IntPtr retval = TLN_GetSequence(ptr, index);
            if (retval != IntPtr.Zero)
                return new Sequence(retval);
            else
                throw new NotFoundException();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sequence"></param>
        /// <returns></returns>
        public bool Add(Sequence sequence)
        {
            return TLN_AddSequenceToPack(ptr, sequence.ptr);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public bool Delete()
        {
            IntPtr value = ptr;
            ptr = IntPtr.Zero;
            return TLN_DeleteSequencePack(value);
        }
    }
}
