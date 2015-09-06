public class Tilengine
{
	static{
		System.loadLibrary ("TilengineJNI");
	}
	
	/* CreateWindow flags */
	public static final int CWF_FULLSCREEN	= (1<<0);
	public static final int CWF_VSYNC		= (1<<1);
	public static final int CWF_S1			= (1<<2);
	public static final int CWF_S2			= (2<<2);
	public static final int CWF_S3			= (3<<2);
	public static final int CWF_S4			= (4<<2);
	public static final int CWF_S5			= (5<<2);		
	
	/* SetLayerBlendMode & SetSpriteBlendMode modes */
	public static final int BLEND_NONE		= 0;
	public static final int BLEND_MIX		= 1;
	public static final int BLEND_ADD		= 2;
	public static final int BLEND_SUB		= 3;
	
	/* GetInput */
	public static final int INPUT_NONE		= 0;
	public static final int INPUT_UP		= 1;
	public static final int INPUT_DOWN		= 2;
	public static final int INPUT_LEFT		= 3;
	public static final int INPUT_RIGHT		= 4;
	public static final int INPUT_A			= 5;
	public static final int INPUT_B			= 6;
	public static final int INPUT_C			= 7;
	public static final int INPUT_D			= 8;
	
	/* affine transform */
	final class Affine{
		float angle;	/* rotation */
		float dx,dy;	/* translation */
		float sx,sy;	/* scale */
	}

	/* tile */
	final class Tile{
		short index;		/* tile index */
		short flags;		/* attributes */
	}

	/* Rectangle */
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
		int xoffset;
		int yoffset;
	}
	
	/* basic management */
	public native void Init (int hres, int vres, int numlayers, int numsprites, int numanimations);
	public native void Deinit ();
	public native int GetNumObjects ();
	public native int GetUsedMemory ();
	public native int GetNumLayers ();
	public native int GetNumSprites ();
	public native int GetVersion ();

	/* window management */
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
	
	/* image generation */
	public native void SetBGColor (int r, int g, int b);
	public native void SetBGBitmap (int bitmap);
	public native void SetBGPalette (int palette);
	public native void SetRasterCallback (Object obj, String methodname);
	public native void SetRenderTarget (int[] data, int pitch);
	public native void UpdateFrame (int time);
	
	/* spritesets management */
	public native int CreateSpriteset (int entries, Rect rects, byte[] data, int width, int height, int pitch, int palette);
	public native int LoadSpriteset (String name);
	public native int CloneSpriteset (int src);
	public native boolean GetSpriteInfo (int spriteset, int entry, SpriteInfo info);
	public native int GetSpritesetPalette (int spriteset);
	public native void DeleteSpriteset (int spriteset);	
	
	/* tilesets management */
	public native int CreateTileset (int numtiles, int width, int height, int palette);
	public native int LoadTileset (String filename);
	public native int CloneTileset (int src);
	public native boolean SetTilesetPixels (int tileset, int entry, byte[] srcdata, int srcpitch);
	public native int GetTileWidth (int tileset);
	public native int GetTileHeight (int tileset);
	public native int GetTilesetPalette (int tileset);
	public native void DeleteTileset (int tileset);
	
	/* tilemaps management */
	//public native int CreateTilemap (int rows, int cols, Tile tiles);
	public native int LoadTilemap (String filename, String layername);
	public native int CloneTilemap (int src);
	public native int GetTilemapRows (int tilemap);
	public native int GetTilemapCols (int tilemap);
	public native boolean GetTilemapTile (int tilemap, int row, int col, Tile tile);
	public native boolean SetTilemapTile (int tilemap, int row, int col, Tile tile);
	public native void CopyTiles (int src, int srcrow, int srccol, int rows, int cols, int dst, int dstrow, int dstcol);
	public native void DeleteTilemap (int tilemap);
	
	/* color tables management */
	public native int CreatePalette (int entries);
	public native int LoadPalette (String filename);
	public native int ClonePalette (int src);
	public native void DeletePalette (int palette);
	public native void SetPaletteColor (int palette, int color, byte r, byte g, byte b);
	public native void MixPalettes (int src1, int src2, int dst, byte f);
	
	/* bitmaps */
	public native int CreateBitmap (int width, int height, int bpp);
	public native int LoadBitmap (String filename);
	public native int CloneBitmap (int src);
	public native int GetBitmapPalette (int bitmap);
	public native void DeleteBitmap (int bitmap);

	/* layer management */
	public native void SetLayer (int nlayer, int tileset, int tilemap);
	public native void SetLayerPalette (int nlayer, int palette);
	public native void SetLayerPosition (int nlayer, int hstart, int vstart);
	public native void SetLayerScaling (int nlayer, float xfactor, float yfactor);
	public native void SetLayerAffineTransform (int nlayer, Affine affine);
	public native void SetLayerTransform (int layer, float angle, float dx, float dy, float sx, float sy);
	public native void SetLayerBlendMode (int nlayer, int mode, byte factor);
	public native void SetLayerColumnOffset (int nlayer, int[] offset);
	public native void ResetLayerMode (int nlayer);
	public native void DisableLayer (int nlayer);
	public native int GetLayerPalette (int nlayer);
	public native boolean GetLayerTile (int nlayer, int x, int y, TileInfo info);

	/* sprites management */
	public native void ConfigSprite (int nsprite, int spriteset, short flags);
	public native void SetSpriteSet (int nsprite, int spriteset);
	public native void SetSpriteFlags (int nsprite, short flags);
	public native void SetSpritePosition (int nsprite, int x, int y);
	public native void SetSpritePicture (int nsprite, int entry);
	public native void SetSpritePalette (int nsprite, int palette);
	public native void SetSpriteBlendMode (int nsprite, int mode, byte factor);
	public native void SetSpriteScaling (int nsprite, float sx, float sy);
	public native void ResetSpriteScaling (int nsprite);
	public native int  GetSpritePicture (int nsprite);
	public native int  GetAvailableSprite ();
	public native void DisableSprite (int nsprite);
	public native int GetSpritePalette (int nsprite);

	/* sequences management */
	public native int CreateSequence (String name, int delay, int first, int num_frames, int[] data);
	public native int CloneSequence (int src);
	public native void DeleteSequence (int sequence);
	public native int LoadSequencePack (String filename);
	public native int FindSequence (int sp, String name);
	public native void DeleteSequencePack (int sp);

	/* animation engine */
	public native void SetPaletteAnimation (int index, int palette, int sequence, boolean blend);
	public native void SetPaletteAnimationSource (int index, int palette);
	public native void SetTilemapAnimation (int index, int nlayer, int sequence);
	public native void SetSpriteAnimation (int index, int nsprite, int sequence, int loop);
	public native boolean GetAnimationState (int index);
	public native void SetAnimationDelay (int index, int delay);
	public native int  GetAvailableAnimation ();
	public native void DisableAnimation (int index);	
}