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

public class Tilengine
{
	static{
		System.loadLibrary ("TilengineJNI");
	}
	
	// CreateWindow flags 
	public static final int CWF_FULLSCREEN	= (1<<0);
	public static final int CWF_VSYNC		= (1<<1);
	public static final int CWF_S1			= (1<<2);
	public static final int CWF_S2			= (2<<2);
	public static final int CWF_S3			= (3<<2);
	public static final int CWF_S4			= (4<<2);
	public static final int CWF_S5			= (5<<2);		
	
	// error codes 
	public static final int TLN_ERR_OK				= 0;	// No error 
	public static final int TLN_ERR_OUT_OF_MEMORY	= 1;	// Not enough memory 
	public static final int TLN_ERR_IDX_LAYER		= 2;	// Layer index out of range 
	public static final int TLN_ERR_IDX_SPRITE		= 3;	// Sprite index out of range 
	public static final int TLN_ERR_IDX_ANIMATION	= 4;	// Animation index out of range 
	public static final int TLN_ERR_IDX_PICTURE		= 5;	// Picture or tile index out of range 
	public static final int TLN_ERR_REF_TILESET		= 6;	// Invalid TLN_Tileset reference 
	public static final int TLN_ERR_REF_TILEMAP		= 7;	// Invalid TLN_Tilemap reference 
	public static final int TLN_ERR_REF_SPRITESET	= 8;	// Invalid TLN_Spriteset reference 
	public static final int TLN_ERR_REF_PALETTE		= 9;	// Invalid TLN_Palette reference 
	public static final int TLN_ERR_REF_SEQUENCE	= 10;	// Invalid TLN_SequencePack reference 
	public static final int TLN_ERR_REF_SEQPACK		= 11;	// Invalid TLN_Sequence reference 
	public static final int TLN_ERR_REF_BITMAP		= 12;	// Invalid TLN_Bitmap reference 
	public static final int TLN_ERR_NULL_POINTER	= 13;	// Null pointer as argument  
	public static final int TLN_ERR_FILE_NOT_FOUND	= 14;	// Resource file not found 
	public static final int TLN_ERR_WRONG_FORMAT	= 15;	// Resource file has invalid format 
	public static final int TLN_ERR_WRONG_SIZE		= 16;	// A width or height parameter is invalid 
	public static final int TLN_ERR_UNSUPPORTED		= 17;
	
	// SetLayerBlendMode & SetSpriteBlendMode modes 
	public static final int BLEND_NONE		= 0;
	public static final int BLEND_MIX		= 1;
	public static final int BLEND_ADD		= 2;
	public static final int BLEND_SUB		= 3;
	public static final int BLEND_MOD		= 4;
	
	// GetInput 
	public static final int INPUT_NONE		= 0;
	public static final int INPUT_UP		= 1;
	public static final int INPUT_DOWN		= 2;
	public static final int INPUT_LEFT		= 3;
	public static final int INPUT_RIGHT		= 4;
	public static final int INPUT_A			= 5;
	public static final int INPUT_B			= 6;
	public static final int INPUT_C			= 7;
	public static final int INPUT_D			= 8;
	
	// affine transform 
	final class Affine{
		float angle;	// rotation 
		float dx,dy;	// translation 
		float sx,sy;	// scale 
	}

	// tile 
	final class Tile{
		short index;		// tile index 
		short flags;		// attributes 
	}
	
	// color strip 
	final class ColorStrip{
		int   delay;		// time delay between frames 
		short first;		// index of first color to cycle 
		short count;		// number of colors in the cycle 
		byte  dir;			// direction: 0=descending, 1=ascending 
	}	

	// Rectangle 
	final class Rect{
		int x,y,w,h;
	}

	final class SpriteInfo	{
		int offset;
		int w,h;
	}

	final class TileInfo{
		short index;
		short flags;
		int row;
		int col;
		int xoffset;
		int yoffset;
		byte color;
	}
	
	// basic management 
	public native boolean Init (int hres, int vres, int numlayers, int numsprites, int numanimations);
	public native boolean InitBPP (int hres, int vres, int bpp, int numlayers, int numsprites, int numanimations);
	public native void Deinit ();
	public native int GetNumObjects ();
	public native int GetUsedMemory ();
	public native int GetVersion ();
	public native int GetNumLayers ();
	public native int GetNumSprites ();
	public native void SetBGColor (int r, int g, int b);
	public native boolean SetBGBitmap (int bitmap);
	public native boolean SetBGPalette (int palette);
	public native void SetRasterCallback (Object obj, String methodname);
	public native void SetRenderTarget (int[] data, int pitch);
	public native void UpdateFrame (int time);
	public native void BeginFrame (int time);
	public native boolean DrawNextScanline ();
	public native void SetLoadPath (String path);
	
	// error handling 
	public native void SetLastError (int error);
	public native int GetLastError ();
	//public native String GetErrorString (int error);

	// window management 
	public native boolean CreateWindow (String overlay, int flags);
	public native boolean CreateWindowThread (String overlay, int flags);
	public native boolean ProcessWindow ();
	public native boolean IsWindowActive ();
	public native boolean GetInput (int id);
	public native void DrawFrame (int time);
	public native void WaitRedraw ();
	public native void DeleteWindow ();
	public native void EnableBlur (boolean mode);
	public native int GetTicks ();
	public native void Delay (int time);
	public native void BeginWindowFrame ();
	public native void EndWindowFrame ();
	
	// spritesets management 
	public native int CreateSpriteset (int entries, Rect rects, byte[] data, int width, int height, int pitch, int palette);
	public native int LoadSpriteset (String name);
	public native int CloneSpriteset (int src);
	public native boolean GetSpriteInfo (int spriteset, int entry, SpriteInfo info);
	public native int GetSpritesetPalette (int spriteset);
	public native boolean DeleteSpriteset (int spriteset);	
	
	// tilesets management 
	public native int CreateTileset (int numtiles, int width, int height, int palette);
	public native int LoadTileset (String filename);
	public native int CloneTileset (int src);
	public native boolean SetTilesetPixels (int tileset, int entry, byte[] srcdata, int srcpitch);
	public native boolean CopyTile (int tileset, int src, int dst);	
	public native int GetTileWidth (int tileset);
	public native int GetTileHeight (int tileset);
	public native int GetTilesetPalette (int tileset);
	public native boolean DeleteTileset (int tileset);
	
	// tilemaps management 
	public native int CreateTilemap (int rows, int cols, Tile[] tiles);
	public native int LoadTilemap (String filename, String layername);
	public native int CloneTilemap (int src);
	public native int GetTilemapRows (int tilemap);
	public native int GetTilemapCols (int tilemap);
	public native boolean GetTilemapTile (int tilemap, int row, int col, Tile tile);
	public native boolean SetTilemapTile (int tilemap, int row, int col, Tile tile);
	public native void CopyTiles (int src, int srcrow, int srccol, int rows, int cols, int dst, int dstrow, int dstcol);
	public native boolean DeleteTilemap (int tilemap);
	
	// color tables management 
	public native int CreatePalette (int entries);
	public native int LoadPalette (String filename);
	public native int ClonePalette (int src);
	public native void DeletePalette (int palette);
	public native void SetPaletteColor (int palette, int color, byte r, byte g, byte b);
	public native void MixPalettes (int src1, int src2, int dst, byte f);
	
	// bitmaps 
	public native int CreateBitmap (int width, int height, int bpp);
	public native int LoadBitmap (String filename);
	public native int CloneBitmap (int src);
	public native int GetBitmapWidth (int bitmap);
	public native int GetBitmapHeight (int bitmap);
	public native int GetBitmapDepth (int bitmap);
	public native int GetBitmapPitch (int bitmap);
	public native int GetBitmapPalette (int bitmap);
	public native boolean SetBitmapPalette (int bitmap, int palette);
	public native boolean DeleteBitmap (int bitmap);

	// layer management 
	public native boolean SetLayer (int nlayer, int tileset, int tilemap);
	public native boolean SetLayerPalette (int nlayer, int palette);
	public native boolean SetLayerPosition (int nlayer, int hstart, int vstart);
	public native boolean SetLayerScaling (int nlayer, float xfactor, float yfactor);
	public native boolean SetLayerAffineTransform (int nlayer, Affine affine);
	public native boolean SetLayerTransform (int layer, float angle, float dx, float dy, float sx, float sy);
	public native boolean SetLayerBlendMode (int nlayer, int mode, byte factor);
	public native boolean SetLayerColumnOffset (int nlayer, int[] offset);
	public native boolean SetLayerClip (int nlayer, int x1, int y1, int x2, int y2);
	public native boolean DisableLayerClip (int nlayer);
	public native boolean ResetLayerMode (int nlayer);
	public native boolean DisableLayer (int nlayer);
	public native int GetLayerPalette (int nlayer);
	public native boolean GetLayerTile (int nlayer, int x, int y, TileInfo info);

	// sprites management 
	public native boolean ConfigSprite (int nsprite, int spriteset, short flags);
	public native boolean SetSpriteSet (int nsprite, int spriteset);
	public native boolean SetSpriteFlags (int nsprite, short flags);
	public native boolean SetSpritePosition (int nsprite, int x, int y);
	public native boolean SetSpritePicture (int nsprite, int entry);
	public native boolean SetSpritePalette (int nsprite, int palette);
	public native boolean SetSpriteBlendMode (int nsprite, int mode, byte factor);
	public native boolean SetSpriteScaling (int nsprite, float sx, float sy);
	public native boolean ResetSpriteScaling (int nsprite);
	public native int GetSpritePicture (int nsprite);
	public native int GetAvailableSprite ();
	public native boolean EnableSpriteCollision (int nsprite, boolean enable);
	public native boolean GetSpriteCollision (int nsprite);
	public native boolean DisableSprite (int nsprite);
	public native int GetSpritePalette (int nsprite);

	// sequences management 
	public native int CreateSequence (String name, int delay, int first, int num_frames, int[] data);
	public native int CreateCycle (String name, int num_strips, ColorStrip[] strips);
	public native int CloneSequence (int src);
	public native boolean DeleteSequence (int sequence);
	
	// sequence pack management 
	public native int CreateSequencePack ();
	public native int LoadSequencePack (String filename);
	public native int FindSequence (int sp, String name);
	public native boolean AddSequenceToPack (int sp, int sequence);
	public native boolean DeleteSequencePack (int sp);

	// animation engine 
	public native boolean SetPaletteAnimation (int index, int palette, int sequence, boolean blend);
	public native boolean SetPaletteAnimationSource (int index, int palette);
	public native boolean SetTilesetAnimation (int index, int nlayer, int sequence);
	public native boolean SetTilemapAnimation (int index, int nlayer, int sequence);
	public native boolean SetSpriteAnimation (int index, int nsprite, int sequence, int loop);
	public native boolean GetAnimationState (int index);
	public native boolean SetAnimationDelay (int index, int delay);
	public native int GetAvailableAnimation ();
	public native boolean DisableAnimation (int index);	
}