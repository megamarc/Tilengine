/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2018 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

/*
*****************************************************************************
* C# Tilengine wrapper - Up to date to library version 1.20
* http://www.tilengine.org
*****************************************************************************
*/

using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Tilengine
{
    /// <summary>
    /// Tile data contained in each cell of a cref="Tilemap" object
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct Tile
    {
        public ushort index;
        public ushort flags;
    }

    /// <summary>
    /// List of possible exception error codes
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
    /// List of flag values for window creation
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
        Nearest	= (1 << 6), // unfiltered upscaling
    }

    /// <summary>
    /// Player index for input assignment functions
    /// </summary>
    public enum Player
    {
        P1,
        P2,
        P3,
        P4
    }

    /// <summary>
    /// Standard inputs query for cref="Window.GetInput()"
    /// </summary>
    public enum Input
    {
        None,
        Up,
        Down,
        Left,
        Right,
        Button1,
        Button2,
        Button3,
        Button4,
        Button5,
        Button6,
        Start,

        P1 = (Player.P1 << 4), // request player 1 input (default)
        P2 = (Player.P2 << 4), // request player 2 input
        P3 = (Player.P3 << 4), // request player 3 input
        P4 = (Player.P4 << 4), // request player 4 input

        /* compatibility symbols for pre-1.18 input model */
        Button_A = Button1,
        Button_B = Button2,
        Button_C = Button3,
        Button_D = Button4,
        Button_E = Button5,
        Button_F = Button6,
    }

    /// <summary>
    /// Available blending modes for cref="Layer" and cref="Sprite"
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
    /// List of flags for tiles and sprites
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
    /// Data used to create cref="Spriteset" objects
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
    /// Data returned by cref="Spriteset.GetSpriteInfo" with dimensions of the requested sprite
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct SpriteInfo
    {
        public int W;
        public int H;
    }

    /// <summary>
    /// Data returned by cref="Layer.GetTile" about a given tile inside a background layer
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
    /// cref="Tileset" attributes for constructor
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
	public struct TileAttributes
	{
		public byte type;		// type of tile
		public bool priority;	// priority bit set
	}

    /// <summary>
    /// Data used to define each frame of an animation for cref="Sequence" objects
    /// </summary>
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct SequenceFrame
    {
	    public int index;
	    public int delay;
    }

    /// <summary>
    /// Data used to define each frame of a color cycle for cref="Sequence" objects
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
    /// Sequence info returned by cref="Sequence.GetInfo"
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct SequenceInfo
    {
        [MarshalAs(UnmanagedType.LPStr, SizeConst = 32)]
        public string Name;	    // sequence name
        public int NumFrames;	// number of frames
    }

    /// <summary>
    /// Represents a color value in RGB format
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
    /// pixel mapping for cref="Layer.SetPixelMapping"
    /// </summary>
	public struct PixelMap
	{
		public ushort dx, dy;
	}

    /// <summary>
    /// Generic Tilengine exception
    /// </summary>
    public class TilengineException : Exception
    {
        public TilengineException(string message) : base(message)
        {
        }
    }

    public delegate void VideoCallback(int line);
	public delegate byte BlendFunction(byte src, byte dst);

    /// <summary>
    /// Main object for engine creation and rendering
    /// </summary>
    public struct Engine
    {
        // singleton
        private static Engine instance;
        private static bool init;

		public Layer[] Layers;
		public Sprite[] Sprites;
		public Animation[] Animations;

        public int Width;		// Width in pixels of the framebuffer
        public int Height;		// Height in pixels of the framebuffer
        public uint Version;	// Tilengine dll version, in a 32-bit integer

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
        private static extern void TLN_SetRasterCallback(VideoCallback callback);

        [DllImport("Tilengine")]
        private static extern void TLN_SetFrameCallback(VideoCallback callback);

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

        private Engine (int numLayers, int numSprites, int numAnimations)
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

            Width = 0;
            Height = 0;
            Version = 0;
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
                Engine.ThrowException(retval);
                init = true;
                instance = new Engine(numLayers, numSprites, numAnimations);
                instance.Width = hres;
                instance.Height = vres;
                instance.Version = TLN_GetVersion();
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
            set
            {
                bool ok = TLN_SetBGBitmap(value.ptr);
                Engine.ThrowException(ok);
            }
        }

        /// <summary>
        /// Sets the palette for the optional background bitmap
        /// </summary>
        public Palette BackgroundPalette
        {
            set
            {
                bool ok = TLN_SetBGPalette(value.ptr);
                Engine.ThrowException(ok);
            }
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
        /// Enables raster effects processing, like a virtual HBLANK interrupt where
        /// any render parameter can be modified between scanlines.
        /// </summary>
        /// <param name="callback">name of the user-defined function to call for each scanline. Set Null to disable.</param>
        public void SetRasterCallback(VideoCallback callback)
        {
            TLN_SetRasterCallback(callback);
        }

        /// <summary>
        /// Enables user callback for each drawn frame, like a virtual VBLANK interrupt
        /// </summary>
        /// <param name="callback">name of the user-defined function to call for each frame. Set Null to disable.</param>
        public void SetFrameCallback(VideoCallback callback)
        {
            TLN_SetFrameCallback(callback);
        }

        /// <summary>
        /// Base path for all data loading .FromFile() static methods
        /// </summary>
        public String LoadPath
        {
            set { TLN_SetLoadPath(value); }
        }

        /// <summary>
        /// Sets custom blend function to use in sprites or background layers when cref="Blend.Custom" mode
		/// is selected with the cref="Layer.BlendMode" and cref="Sprite.BlendMode" properties.
        /// </summary>
        /// <param name="function">user-defined function to call when blending that takes
		/// two integer arguments: source component intensity, destination component intensity, and returns
		/// the desired intensity.
        /// </param>
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
        /// Throws a TilengineException after an unsuccessful operation
        /// </summary>
        /// <param name="success"></param>
        internal static void ThrowException(bool success)
        {
            if (success == false)
            {
                Error error = TLN_GetLastError();
                string name = TLN_GetErrorString(error);
                throw new TilengineException(name);
            }
        }

        /// <summary>
        /// Returns reference to first unused sprite
        /// </summary>
        /// <returns></returns>
        public Sprite GetAvailableSprite()
        {
            int index = TLN_GetAvailableSprite();
            Engine.ThrowException(index != -1);
            return Sprites[index];
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
        private static extern void TLN_EnableInput (Player player, bool enable);

        [DllImport("Tilengine")]
        private static extern void TLN_AssignInputJoystick (Player player, int index);

        [DllImport("Tilengine")]
        private static extern void TLN_DefineInputKey (Player player, Input input, int keycode);

        [DllImport("Tilengine")]
        private static extern void TLN_DefineInputButton (Player player, Input input, byte joybutton);

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
                Engine.ThrowException(retval);
                init = true;
                instance = new Window();
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
                Engine.ThrowException(retval);
                init = true;
                instance = new Window();
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
        /// <param name="id">Input identifier to check state</param>
        /// <returns>true if that input is pressed or false if not</returns>
        public bool GetInput(Input id)
        {
            return TLN_GetInput(id);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="player">Player number to configure</param>
        /// <param name="enable"></param>
        public void EnableInput(Player player, bool enable)
        {
            TLN_EnableInput(player, enable);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="player">Player number to configure</param>
        /// <param name="index"></param>
        public void AssignInputJoystick(Player player, int index)
        {
            TLN_AssignInputJoystick (player, index);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="player">Player number to configure</param>
        /// <param name="input"></param>
        /// <param name="keycode"></param>
        public void DefineInputKey(Player player, Input input, int keycode)
        {
            TLN_DefineInputKey (player, input, keycode);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="player">Player number to configure</param>
        /// <param name="input"></param>
        /// <param name="joybutton"></param>
        public void DefineInputButton(Player player, Input input, byte joybutton)
        {
            TLN_DefineInputButton (player, input, joybutton);
        }

        /// <summary>
        /// Begins active rendering frame
        /// </summary>
        /// <param name="frame">Timestamp value</param>
        /// <remarks>Use this method instead of Engine::BeginFrame() when using build-in windowing</remarks>
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
        private static extern bool TLN_SetLayerBitmap(int nlayer, IntPtr bitmap);

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
        public void Setup(Tileset tileset, Tilemap tilemap)
        {
            bool ok = TLN_SetLayer(index, tileset.ptr, tilemap.ptr);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="tilemap"></param>
        public void SetMap(Tilemap tilemap)
        {
            bool ok = TLN_SetLayer(index, IntPtr.Zero, tilemap.ptr);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        public void SetPosition(int x, int y)
        {
            bool ok = TLN_SetLayerPosition(index, x, y);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="sx"></param>
        /// <param name="sy"></param>
        public void SetScaling(float sx, float sy)
        {
            bool ok = TLN_SetLayerScaling(index, sx, sy);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="angle"></param>
        /// <param name="dx"></param>
        /// <param name="dy"></param>
        /// <param name="sx"></param>
        /// <param name="sy"></param>
        public void SetTransform(float angle, float dx, float dy, float sx, float sy)
        {
            bool ok = TLN_SetLayerTransform(index, angle, dx, dy, sx, sy);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
		/// <param name="map"></param>
		public void SetPixelMapping(PixelMap[] map)
		{
            bool ok = TLN_SetLayerPixelMapping(index, map);
            Engine.ThrowException(ok);
		}

        /// <summary>
        ///
        /// </summary>
        public void Reset()
        {
            bool ok = TLN_ResetLayerMode(index);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public Blend BlendMode
        {
            set
            {
                bool ok = TLN_SetLayerBlendMode(index, value, 0);
                Engine.ThrowException(ok);
            }
        }

        /// <summary>
        ///
        /// </summary>
        public void SetColumnOffset(int[] offsets)
        {
            bool ok = TLN_SetLayerColumnOffset(index, offsets);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
		/// <param name="x1"></param>
		/// <param name="y1"></param>
		/// <param name="x2"></param>
		/// <param name="y2"></param>
		public void SetClip(int x1, int y1, int x2, int y2)
		{
            bool ok = TLN_SetLayerClip(index, x1, y1, x2, y2);
            Engine.ThrowException(ok);
		}

		/// <summary>
		///
		/// </summary>
        public void DisableClip()
		{
            bool ok = TLN_DisableLayerClip(index);
            Engine.ThrowException(ok);
		}

        /// <summary>
        ///
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        public void SetMosaic(int width, int height)
        {
            bool ok = TLN_SetLayerMosaic(index, width, height);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public void DisableMosaic()
        {
            bool ok = TLN_DisableLayerMosaic(index);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="info"></param>
        public void GetTileInfo(int x, int y, out TileInfo info)
        {
            bool ok = TLN_GetLayerTile(index, x, y, out info);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public Palette Palette
        {
            get
            {
                IntPtr value = TLN_GetLayerPalette(index);
                Engine.ThrowException(value != IntPtr.Zero);
                return new Palette(value);
            }
            set
            {
                bool ok = TLN_SetLayerPalette(index, value.ptr);
                Engine.ThrowException(ok);
            }
        }

        /// <summary>
        ///
        /// </summary>
        public Bitmap Bitmap
        {
            set
            {
                bool ok = TLN_SetLayerBitmap(index, value.ptr);
                Engine.ThrowException(ok);
            }
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
        public void Disable()
        {
            bool ok = TLN_DisableLayer(index);
            Engine.ThrowException(ok);
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
        public void Setup(Spriteset spriteset, TileFlags flags)
        {
            bool ok = TLN_ConfigSprite(index, spriteset.ptr, flags);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public Spriteset Spriteset
        {
            set
            {
                bool ok = TLN_SetSpriteSet(index, value.ptr);
                Engine.ThrowException(ok);
            }
        }

        /// <summary>
        ///
        /// </summary>
        public TileFlags Flags
        {
            set
            {
                bool ok = TLN_SetSpriteFlags(index, value);
                Engine.ThrowException(ok);
            }
        }

        /// <summary>
        ///
        /// </summary>
        public int Picture
        {
            set
            {
                bool ok = TLN_SetSpritePicture(index, value);
                Engine.ThrowException(ok);
            }
            get
            {
                int value = TLN_GetSpritePicture(index);
                Engine.ThrowException(value != -1);
                return value;
            }
        }

        /// <summary>
        ///
        /// </summary>
        public Palette Palette
        {
            set
            {
                bool ok = TLN_SetSpritePalette(index, value.ptr);
                Engine.ThrowException(ok);
            }
            get
            {
                IntPtr value = TLN_GetSpritePalette(index);
                Engine.ThrowException(value != IntPtr.Zero);
                return new Palette(value);
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        public void SetPosition(int x, int y)
        {
            bool ok = TLN_SetSpritePosition(index, x, y);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="sx"></param>
        /// <param name="sy"></param>
        public void SetScaling(float sx, float sy)
        {
            bool ok = TLN_SetSpriteScaling(index, sx, sy);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public void Reset()
        {
            bool ok = TLN_ResetSpriteScaling(index);
            Engine.ThrowException(false);
        }

        /// <summary>
        /// Sets blending mode
        /// </summary>
        public Blend BlendMode
        {
            set
            {
                bool ok = TLN_SetSpriteBlendMode(index, value, 0);
                Engine.ThrowException(ok);
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="mode"></param>
        public void EnableCollision(bool mode)
        {
            bool ok = TLN_EnableSpriteCollision(index, mode);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public bool Collision
        {
            get { return TLN_GetSpriteCollision(index); }
        }

        /// <summary>
        ///
        /// </summary>
        public void Disable()
        {
            bool ok = TLN_DisableSprite(index);
            Engine.ThrowException(ok);
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
        public void SetPaletteAnimation(Palette palette, Sequence sequence, bool blend)
        {
            bool ok = TLN_SetPaletteAnimation(index, palette.ptr, sequence.ptr, blend);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="palette"></param>
        public void SetPaletteAnimationSource(Palette palette)
        {
            bool ok = TLN_SetPaletteAnimationSource(index, palette.ptr);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="layerIndex"></param>
        /// <param name="sequence"></param>
        public void SetTilesetAnimation(int layerIndex, Sequence sequence)
        {
            bool ok = TLN_SetTilesetAnimation(index, layerIndex, sequence.ptr);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="layerIndex"></param>
        /// <param name="sequence"></param>
        public void SetTilemapAnimation(int layerIndex, Sequence sequence)
        {
            bool ok = TLN_SetTilemapAnimation(index, layerIndex, sequence.ptr);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="spriteIndex"></param>
        /// <param name="sequence"></param>
        /// <param name="loop"></param>
        public void SetSpriteAnimation(int spriteIndex, Sequence sequence, int loop)
        {
            bool ok = TLN_SetSpriteAnimation(index, spriteIndex, sequence.ptr, loop);
            Engine.ThrowException(ok);
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
            set
            {
                bool ok = TLN_SetAnimationDelay(index, value);
                Engine.ThrowException(ok);
            }
        }

        /// <summary>
        ///
        /// </summary>
        public void Disable()
        {
            bool ok = TLN_DisableAnimation(index);
            Engine.ThrowException(ok);
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
        private static extern bool TLN_SetSpritesetData(IntPtr spriteset, int entry, SpriteData[] data, IntPtr pixels, int pitch);

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
            Engine.ThrowException(retval != IntPtr.Zero);
            ptr = retval;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Spriteset FromFile (string filename)
        {
            IntPtr retval = TLN_LoadSpriteset (filename);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Spriteset (retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        public Spriteset Clone ()
        {
            IntPtr retval = TLN_CloneSpriteset(ptr);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Spriteset(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="index"></param>
        /// <param name="info"></param>
        public void GetInfo (int index, out SpriteInfo info)
        {
            bool ok = TLN_GetSpriteInfo (ptr, index, out info);
            Engine.ThrowException(ok);
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
            int index = TLN_FindSpritesetSprite(ptr, name);
            Engine.ThrowException(index != -1);
            return index;

        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="entry"></param>
        /// <param name="data"></param>
        /// <param name="pixels"></param>
        /// <param name="pitch"></param>
        public void TLN_SetSpritesetData(int entry, SpriteData[] data, IntPtr pixels, int pitch)
        {
            bool ok = TLN_SetSpritesetData(ptr, entry, data, pixels, pitch);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public void Delete ()
        {
            bool ok = TLN_DeleteSpriteset (ptr);
            Engine.ThrowException(ok);
            ptr = IntPtr.Zero;
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
        /// <param name="sp"></param>
        /// <param name="attributes"></param>
        public Tileset(int numTiles, int width, int height, Palette palette, SequencePack sp, TileAttributes[] attributes)
        {
            IntPtr retval = TLN_CreateTileset(numTiles, width, height, palette.ptr, sp.ptr, attributes);
            Engine.ThrowException(retval != IntPtr.Zero);
            ptr = retval;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Tileset FromFile(string filename)
        {
            IntPtr retval = TLN_LoadTileset(filename);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Tileset(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        public Tileset Clone()
        {
            IntPtr retval = TLN_CloneTileset(ptr);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Tileset(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="entry"></param>
        /// <param name="pixels"></param>
        /// <param name="pitch"></param>
        public void SetPixels(int entry, byte[] pixels, int pitch)
        {
            bool ok = TLN_SetTilesetPixels(ptr, entry, pixels, pitch);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="src"></param>
        /// <param name="dst"></param>
        public void CopyTile(int src, int dst)
        {
            bool ok = TLN_CopyTile(ptr, src, dst);
            Engine.ThrowException(ok);
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
        public void Delete()
        {
            bool ok = TLN_DeleteTileset(ptr);
            Engine.ThrowException(ok);
            ptr = IntPtr.Zero;
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
            Engine.ThrowException(retval != IntPtr.Zero);
            ptr = retval;
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
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Tilemap(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        public Tilemap Clone()
        {
            IntPtr retval = TLN_CloneTilemap(ptr);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Tilemap(retval);
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
        public void SetTile(int row, int col, ref Tile tile)
        {
            bool ok = TLN_SetTilemapTile(ptr, row, col, ref tile);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="row"></param>
        /// <param name="col"></param>
        /// <param name="tile"></param>
        public void GetTile(int row, int col, out Tile tile)
        {
            bool ok = TLN_GetTilemapTile(ptr, row, col, out tile);
            Engine.ThrowException(ok);
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
        public void CopyTiles(int srcRow, int srcCol, int rows, int cols, Tilemap dst, int dstRow, int dstCol)
        {
            bool ok = TLN_CopyTiles(ptr, srcRow, srcCol, rows, cols, dst.ptr, dstRow, dstCol);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public void Delete()
        {
            bool ok = TLN_DeleteTilemap(ptr);
            Engine.ThrowException(ok);
            ptr = IntPtr.Zero;
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
            Engine.ThrowException(retval != IntPtr.Zero);
            ptr = retval;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Palette FromFile(string filename)
        {
            IntPtr retval = TLN_LoadPalette(filename);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Palette(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        public Palette Clone()
        {
            IntPtr retval = TLN_ClonePalette(ptr);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Palette(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="index"></param>
        /// <param name="color"></param>
        public void SetColor(int index, Color color)
        {
            bool ok = TLN_SetPaletteColor(ptr, index, color.R, color.G, color.B);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="src1"></param>
        /// <param name="src2"></param>
        /// <param name="factor"></param>
        public void Mix(Palette src1, Palette src2, byte factor)
        {
            bool ok = TLN_MixPalettes(src1.ptr, src2.ptr, ptr, factor);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="color"></param>
        /// <param name="first"></param>
        /// <param name="count"></param>
        public void AddColor(Color color, byte first, byte count)
        {
            bool ok = TLN_AddPaletteColor(ptr, color.R, color.G, color.B, first, count);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="color"></param>
        /// <param name="first"></param>
        /// <param name="count"></param>
        public void SubColor(Color color, byte first, byte count)
        {
            bool ok = TLN_SubPaletteColor(ptr, color.R, color.G, color.B, first, count);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="color"></param>
        /// <param name="first"></param>
        /// <param name="count"></param>
        public void MulColor(Color color, byte first, byte count)
        {
            bool ok =  TLN_ModPaletteColor(ptr, color.R, color.G, color.B, first, count);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public void Delete()
        {
            bool ok = TLN_DeletePalette(ptr);
            Engine.ThrowException(ok);
            ptr = IntPtr.Zero;
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
            Engine.ThrowException(retval != IntPtr.Zero);
            ptr = retval;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Bitmap FromFile(string filename)
        {
            IntPtr retval = TLN_LoadBitmap(filename);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Bitmap(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        public Bitmap Clone()
        {
            IntPtr retval = TLN_CloneBitmap(ptr);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Bitmap(retval);
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
            set
            {
                bool ok = TLN_SetBitmapPalette(ptr, value.ptr);
                Engine.ThrowException(ok);
            }
        }

        /// <summary>
        ///
        /// </summary>
        public void Delete()
        {
            bool ok = TLN_DeleteBitmap(ptr);
            Engine.ThrowException(ok);
            ptr = IntPtr.Zero;
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
        private static extern IntPtr TLN_CloneSequence(IntPtr sequence);

        [DllImport("Tilengine")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        private static extern bool TLN_GetSequenceInfo(IntPtr sequence, ref SequenceInfo info);

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
        /// <param name="target"></param>
        /// <param name="frames"></param>
        public Sequence(string name, int target, SequenceFrame[] frames)
        {
            IntPtr retval = TLN_CreateSequence(name, target, frames.Length, frames);
            Engine.ThrowException(retval != IntPtr.Zero);
            ptr = retval;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="name"></param>
        /// <param name="strips"></param>
        public Sequence(string name, ColorStrip[] strips)
        {
            IntPtr retval = TLN_CreateCycle(name, strips.Length, strips);
            Engine.ThrowException(retval != IntPtr.Zero);
            ptr = retval;
        }

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
        public Sequence Clone()
        {
            IntPtr retval = TLN_CloneSequence(ptr);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Sequence(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="info"></param>
        public void GetInfo(ref SequenceInfo info)
        {
            bool ok = TLN_GetSequenceInfo(ptr, ref info);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public void Delete()
        {
            bool ok = TLN_DeleteSequence(ptr);
            Engine.ThrowException(ok);
            ptr = IntPtr.Zero;
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
            Engine.ThrowException(retval != IntPtr.Zero);
            return new SequencePack(retval);
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
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Sequence(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="index"></param>
        /// <returns></returns>
        public Sequence Get(int index)
        {
            IntPtr retval = TLN_GetSequence(ptr, index);
            Engine.ThrowException(retval != IntPtr.Zero);
            return new Sequence(retval);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="sequence"></param>
        public void Add(Sequence sequence)
        {
            bool ok = TLN_AddSequenceToPack(ptr, sequence.ptr);
            Engine.ThrowException(ok);
        }

        /// <summary>
        ///
        /// </summary>
        public void Delete()
        {
            bool ok = TLN_DeleteSequencePack(ptr);
            Engine.ThrowException(ok);
            ptr = IntPtr.Zero;
        }
    }
}
