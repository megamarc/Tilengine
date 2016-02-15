using System;
using System.Runtime.InteropServices;

public static class TLN
{
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct Tile
    {
        public ushort index;
        public ushort flags;
    }

    public delegate void RasterCallback(int line);

    public enum Error
    {
        ERR_OK,
        ERR_OUT_OF_MEMORY,
        ERR_IDX_LAYER,
        ERR_IDX_SPRITE,
        ERR_IDX_ANIMATION,
        ERR_IDX_PICTURE,
        ERR_REF_TILESET,
        ERR_REF_TILEMAP,
        ERR_REF_SPRITESET,
        ERR_REF_PALETTE,
        ERR_REF_SEQUENCE,
        ERR_REF_SEQPACK,
        ERR_REF_BITMAP,
        ERR_NULL_POINTER,
        ERR_FILE_NOT_FOUND,
        ERR_WRONG_FORMAT,
        ERR_WRONG_SIZE,
        ERR_UNSUPPORTED,
        MAX_ERR,
    }

    public enum WindowFlags
    {
        CWF_FULLSCREEN = (1 << 0),
        CWF_VSYNC = (1 << 1),
        CWF_S1 = (1 << 2),
        CWF_S2 = (2 << 2),
        CWF_S3 = (3 << 2),
        CWF_S4 = (4 << 2),
        CWF_S5 = (5 << 2),
    }

    public enum Input
    {
        INPUT_NONE,
        INPUT_UP,
        INPUT_DOWN,
        INPUT_LEFT,
        INPUT_RIGHT,
        INPUT_A,
        INPUT_B,
        INPUT_C,
        INPUT_D,
    }

    public enum Blend
    {
        BLEND_NONE,
        BLEND_MIX,
        BLEND_ADD,
        BLEND_SUB,
        MAX_BLEND,
    }

    public enum TileFlags
    {
        FLAG_NONE = (0),
        FLAG_FLIPX = (1 << 15),
        FLAG_FLIPY = (1 << 14),
        FLAG_ROTATE = (1 << 13),
        FLAG_PRIORITY = (1 << 12),
    }

    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct Rect
    {
        public int x;
        public int y;
        public int w;
        public int h;
    }

    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct SpriteInfo
    {
        public int offset;
        public int w;
        public int h;
    }

    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct Affine
    {
        public float angle;
        public float dx;
        public float dy;
        public float sx;
        public float sy;
    }

    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct TileInfo
    {
        public ushort index;
        public ushort flags;
        public int xoffset;
        public int yoffset;
    }

    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct ColorStrip
    {
        public int delay;
        public byte first;
        public byte count;
        public byte dir;
    }

    /* Main */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_Init")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool Init(int hres, int vres, int numlayers, int numsprites, int numanimations);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_Deinit")]
    public static extern void Deinit();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetWidth")]
    public static extern int GetWidth();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetHeight")]
    public static extern int GetHeight();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetNumObjects")]
    public static extern uint GetNumObjects();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetUsedMemory")]
    public static extern uint GetUsedMemory();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetVersion")]
    public static extern uint GetVersion();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetNumLayers")]
    public static extern int GetNumLayers();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetNumSprites")]
    public static extern int GetNumSprites();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetBGColor")]
    public static extern void SetBGColor(int r, int g, int b);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetBGBitmap")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetBGBitmap(IntPtr bitmap);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetBGPalette")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetBGPalette(IntPtr palette);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetRasterCallback")]
    public static extern void SetRasterCallback(RasterCallback callback);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetRenderTarget")]
    public static extern void SetRenderTarget(byte[] data, int pitch);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_UpdateFrame")]
    public static extern void UpdateFrame(int time);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_BeginFrame")]
    public static extern void BeginFrame(int frame);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DrawNextScanline")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DrawNextScanline();

    /* Error handling */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetLastError")]
    public static extern void SetLastError(Error error);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetLastError")]
    public static extern Error GetLastError();

    /* Built-in windowing */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreateWindow")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool CreateWindow(string overlay, WindowFlags flags);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreateWindowThread")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool CreateWindowThread(string overlay, WindowFlags flags);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetWindowTitle")]
    public static extern void SetWindowTitle(string title);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_ProcessWindow")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool ProcessWindow();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_IsWindowActive")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool IsWindowActive();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetInput")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool GetInput(Input id);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetLastInput")]
    public static extern int GetLastInput();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DrawFrame")]
    public static extern void DrawFrame(int time);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_WaitRedraw")]
    public static extern void WaitRedraw();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DeleteWindow")]
    public static extern void DeleteWindow();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_EnableBlur")]
    public static extern void EnableBlur([MarshalAsAttribute(UnmanagedType.I1)] bool mode);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_Delay")]
    public static extern void Delay(uint msecs);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetTicks")]
    public static extern uint GetTicks();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_BeginWindowFrame")]
    public static extern void BeginWindowFrame(int frame);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_EndWindowFrame")]
    public static extern void EndWindowFrame();

    /* Layer management */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetLayer")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetLayer(int nlayer, IntPtr tileset, IntPtr tilemap);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetLayerPalette")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetLayerPalette(int nlayer, IntPtr palette);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetLayerPosition")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetLayerPosition(int nlayer, int hstart, int vstart);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetLayerScaling")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetLayerScaling(int nlayer, float xfactor, float yfactor);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetLayerAffineTransform")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetLayerAffineTransform(int nlayer, ref Affine affine);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetLayerTransform")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetLayerTransform(int layer, float angle, float dx, float dy, float sx, float sy);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetLayerBlendMode")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetLayerBlendMode(int nlayer, Blend mode, byte factor);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetLayerColumnOffset")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetLayerColumnOffset(int nlayer, int[] offset);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_ResetLayerMode")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool ResetLayerMode(int nlayer);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DisableLayer")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DisableLayer(int nlayer);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetLayerPalette")]
    public static extern IntPtr GetLayerPalette(int nlayer);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetLayerTile")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool GetLayerTile(int nlayer, int x, int y, ref TileInfo info);

    /* Sprite management */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_ConfigSprite")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool ConfigSprite(int nsprite, IntPtr spriteset, TileFlags flags);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetSpriteSet")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetSpriteSet(int nsprite, IntPtr spriteset);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetSpriteFlags")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetSpriteFlags(int nsprite, TileFlags flags);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetSpritePosition")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetSpritePosition(int nsprite, int x, int y);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetSpritePicture")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetSpritePicture(int nsprite, int entry);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetSpritePalette")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetSpritePalette(int nsprite, IntPtr palette);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetSpriteBlendMode")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetSpriteBlendMode(int nsprite, Blend mode, byte factor);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetSpriteScaling")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetSpriteScaling(int nsprite, float sx, float sy);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_ResetSpriteScaling")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool ResetSpriteScaling(int nsprite);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetSpritePicture")]
    public static extern int GetSpritePicture(int nsprite);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetAvailableSprite")]
    public static extern int GetAvailableSprite();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_EnableSpriteCollision")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool EnableSpriteCollision(int nsprite, [MarshalAsAttribute(UnmanagedType.I1)] bool enable);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetSpriteCollision")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool GetSpriteCollision(int nsprite);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DisableSprite")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DisableSprite(int nsprite);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetSpritePalette")]
    public static extern IntPtr GetSpritePalette(int nsprite);

    /* Animation management */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetPaletteAnimation")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetPaletteAnimation(int index, IntPtr palette, IntPtr sequence, [MarshalAsAttribute(UnmanagedType.I1)] bool blend);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetPaletteAnimationSource")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetPaletteAnimationSource(int index, IntPtr palette);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetTilemapAnimation")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetTilemapAnimation(int index, int nlayer, IntPtr sequence);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetSpriteAnimation")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetSpriteAnimation(int index, int nsprite, IntPtr sequence, int loop);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetAnimationState")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool GetAnimationState(int index);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetAnimationDelay")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetAnimationDelay(int index, int delay);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetAvailableAnimation")]
    public static extern int GetAvailableAnimation();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DisableAnimation")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DisableAnimation(int index);

    /* Spriteset */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreateSpriteset")]
    public static extern IntPtr CreateSpriteset(int entries, Rect[] rects, byte[] data, int width, int height, int pitch, IntPtr palette);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_LoadSpriteset")]
    public static extern IntPtr LoadSpriteset(string name);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CloneSpriteset")]
    public static extern IntPtr CloneSpriteset(IntPtr src);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetSpriteInfo")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool GetSpriteInfo(IntPtr spriteset, int entry, ref SpriteInfo info);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetSpritesetPalette")]
    public static extern IntPtr GetSpritesetPalette(IntPtr spriteset);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DeleteSpriteset")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DeleteSpriteset(IntPtr Spriteset);

    /* Tileset */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreateTileset")]
    public static extern IntPtr CreateTileset(int numtiles, int width, int height, IntPtr palette);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_LoadTileset")]
    public static extern IntPtr LoadTileset(string filename);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CloneTileset")]
    public static extern IntPtr CloneTileset(IntPtr src);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetTilesetPixels")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetTilesetPixels(IntPtr tileset, int entry, byte[] srcdata, int srcpitch);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetTileWidth")]
    public static extern int GetTileWidth(IntPtr tileset);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetTileHeight")]
    public static extern int GetTileHeight(IntPtr tileset);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetTilesetPalette")]
    public static extern IntPtr GetTilesetPalette(IntPtr tileset);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DeleteTileset")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DeleteTileset(IntPtr tileset);

    /* Tilemap */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreateTilemap")]
    public static extern IntPtr CreateTilemap(int rows, int cols, Tile[] tiles);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_LoadTilemap")]
    public static extern IntPtr LoadTilemap(string filename, string layername);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CloneTilemap")]
    public static extern IntPtr CloneTilemap(IntPtr src);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetTilemapRows")]
    public static extern int GetTilemapRows(IntPtr tilemap);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetTilemapCols")]
    public static extern int GetTilemapCols(IntPtr tilemap);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetTilemapTile")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool GetTilemapTile(IntPtr tilemap, int row, int col, ref Tile tile);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetTilemapTile")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetTilemapTile(IntPtr tilemap, int row, int col, ref Tile tile);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CopyTiles")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool CopyTiles(IntPtr src, int srcrow, int srccol, int rows, int cols, IntPtr dst, int dstrow, int dstcol);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DeleteTilemap")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DeleteTilemap(IntPtr tilemap);

    /* Palette */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreatePalette")]
    public static extern IntPtr CreatePalette(int entries);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_LoadPalette")]
    public static extern IntPtr LoadPalette(string filename);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_ClonePalette")]
    public static extern IntPtr ClonePalette(IntPtr src);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetPaletteColor")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetPaletteColor(IntPtr palette, int color, byte r, byte g, byte b);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_MixPalettes")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool MixPalettes(IntPtr src1, IntPtr src2, IntPtr dst, byte factor);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetPaletteData")]
    public static extern IntPtr GetPaletteData(IntPtr palette, int index);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DeletePalette")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DeletePalette(IntPtr palette);

    /* Bitmap */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreateBitmap")]
    public static extern IntPtr CreateBitmap(int width, int height, int bpp);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_LoadBitmap")]
    public static extern IntPtr LoadBitmap(string filename);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CloneBitmap")]
    public static extern IntPtr CloneBitmap(IntPtr src);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetBitmapWidth")]
    public static extern int GetBitmapWidth(IntPtr bitmap);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetBitmapHeight")]
    public static extern int GetBitmapHeight(IntPtr bitmap);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetBitmapDepth")]
    public static extern int GetBitmapDepth(IntPtr bitmap);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetBitmapPitch")]
    public static extern int GetBitmapPitch(IntPtr bitmap);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_GetBitmapPalette")]
    public static extern IntPtr GetBitmapPalette(IntPtr bitmap);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_SetBitmapPalette")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool SetBitmapPalette(IntPtr bitmap, IntPtr palette);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DeleteBitmap")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DeleteBitmap(IntPtr bitmap);

    /* Sequence */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreateSequence")]
    public static extern IntPtr CreateSequence(string name, int delay, int first, int num_frames, int[] data);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreateCycle")]
    public static extern IntPtr CreateCycle(string name, int num_strips, ColorStrip[] strips);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CloneSequence")]
    public static extern IntPtr CloneSequence(IntPtr src);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DeleteSequence")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DeleteSequence(IntPtr sequence);

    /* SequencePack */

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_CreateSequencePack")]
    public static extern IntPtr CreateSequencePack();

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_LoadSequencePack")]
    public static extern IntPtr LoadSequencePack(string filename);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_FindSequence")]
    public static extern IntPtr FindSequence(IntPtr sp, string name);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_AddSequenceToPack")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool AddSequenceToPack(IntPtr sp, IntPtr sequence);

    [DllImportAttribute("Tilengine.dll", EntryPoint = "TLN_DeleteSequencePack")]
    [return: MarshalAsAttribute(UnmanagedType.I1)]
    public static extern bool DeleteSequencePack(IntPtr sp);
}
