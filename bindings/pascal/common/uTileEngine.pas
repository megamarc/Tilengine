{ Object Pascal wrapper for Tilengine by Turrican turric4n@turric4n.com }

unit uTileEngine;

interface

uses
    sysutils;

const
  dllname = 'Tilengine.dll';

type

  TTileFlags =
  (
    FLAG_NONE = (0),
    FLAG_FLIPX = (1 shl 15),
    FLAG_FLIPY = (1 shl 14),
    FLAG_ROTATE = (1 shl 13),
    FLAG_PRIORITY = (1 shl 12)
  );

  TTile = record
    index : ShortInt;
    flags : TTileFlags;
  end;

  PTile = ^TTile;

  TTileArr = array of TTile;

  TIntArr = array of Integer;

  TRasterCallback = procedure(line : Integer);

  TError =
  (
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
    MAX_ERR
  );

  TWindowFlags =
  (
    CWF_NONE = (0),
    CWF_FULLSCREEN = (1 shl 0),
    CWF_VSYNC = (1 shl 1),
    CWF_S1 = (1 shl 2),
    CWF_S2 = (2 shl 2),
    CWF_S3 = (3 shl 2),
    CWF_S4 = (4 shl 2),
    CWF_S5 = (5 shl 2)
  );

  TInput =
  (
    INPUT_NONE,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_A,
    INPUT_B,
    INPUT_C,
    INPUT_D
  );

  TBlend =
  (
    BLEND_NONE,
    BLEND_MIX,
    BLEND_ADD,
    BLEND_SUB,
    MAX_BLEND
  );

  TRect = record
    x : Integer;
    y : Integer;
    w : Integer;
    h : Integer;
  end;

  PRect = ^TRect;

  TSpriteInfo = record
    offset : Integer;
    w : Integer;
    h : Integer;
  end;

  PSpriteInfo = ^TSpriteInfo;

  TAffine = record
    angle : Single;
    dx : Single;
    dy : Single;
    sx : Single;
    sy : Single;
  end;

  PAffine = ^TAffine;

  TTileInfo = record
    index : ShortInt;
    flags : ShortInt;
    xoffset : Integer;
    yoffset : Integer;
  end;

  PTileInfo = ^TTileInfo;

  TColorStrip = record
    delay : Integer;
    first : Byte;
    count : Byte;
    dir : Byte;
  end;

  TColorStripArray = array of TColorStrip;

  TRectArray = array of TRect;

// Main
function Init(hres, vres, numlayers, numsprites, numanimations : Integer) : Boolean; cdecl; external dllname name 'TLN_Init';
procedure Deinit; cdecl; external dllname name 'TLN_Deinit';
function GetWidth : Integer; cdecl; external dllname name 'TLN_GetWidth';
function GetHeight : Integer; cdecl; external dllname name 'TLN_GetHeight';
function GetNumObjects : UInt32; cdecl; external dllname name 'TLN_GetNumObjects';
function GetUsedMemory : UInt32; cdecl; external dllname name 'TLN_GetUsedMemory';
function GetVersion : UInt32; cdecl; external dllname name 'TLN_GetVersion';
function GetNumLayers : Integer; cdecl; external dllname name 'TLN_GetNumLayers';
function GetNumSprites : Integer; cdecl; external dllname name 'TLN_GetNumSprites';
procedure SetBGColor(r, g , b: Integer); cdecl; external dllname name 'TLN_SetBGColor';
function SetBGBitmap(bitmap : Pointer) : Boolean; cdecl; external dllname name 'TLN_SetBGBitmap';
procedure SetBGPalette(palette : Pointer); cdecl; external dllname name 'TLN_SetBGPalette';
procedure SetRasterCallback(callback : Pointer); cdecl; external dllname name 'TLN_SetRasterCallback';
procedure SetRenderTarget(data : PByte; pitch : Integer); cdecl; external dllname name 'TLN_SetRenderTarget';
procedure UpdateFrame(time : Integer); cdecl; external dllname name 'TLN_UpdateFrame';
procedure BeginFrame(time : Integer); cdecl; external dllname name 'TLN_BeginFrame';
function DrawNextScanline : Boolean; cdecl; external dllname name 'TLN_DrawNextScanline';

// Error handling
procedure SetLastError(error : TError); cdecl; external dllname name 'TLN_SetLastError';
function GetLastError : TError; cdecl; external dllname name 'TLN_GetLastError';

// Built-in windowing
procedure CreateWindow(overlay : PChar; flags : TWindowFlags); cdecl; external dllname name 'TLN_CreateWindow';
procedure CreateWindowThread(overlay : AnsiString; flags : TWindowFlags); cdecl; external dllname name 'TLN_CreateWindowThread';
procedure SetWindowTitle(title : string); cdecl; external dllname name 'TLN_SetWindowTitle';
function ProcessWindow : LongBool ; cdecl; external dllname name 'TLN_ProcessWindow';
function IsWindowActive : LongBool ; cdecl; external dllname name 'TLN_IsWindowActive';
function GetInput(id : TInput) : LongBool ; cdecl; external dllname name 'TLN_GetInput';
function GetLastInput : Integer; external dllname name 'TLN_GetLastInput';
procedure DrawFrame(time : Integer); cdecl; external dllname name 'TLN_DrawFrame';
procedure WaitRedraw; cdecl; external dllname name 'TLN_WaitRedraw';
procedure DeleteWindow; cdecl; external dllname name 'TLN_DeleteWindow';
procedure EnableBlur(mode : LongBool ); cdecl; external dllname name 'TLN_EnableBlur';
procedure Delay(msecs : UInt32); cdecl; external dllname name 'TLN_Delay';
function GetTicks : UInt32; cdecl; external dllname name 'TLN_GetTicks';
procedure BeginWindowFrame(frame : Integer); cdecl; external dllname name 'TLN_BeginWindowFrame';
procedure EndWindowFrame; cdecl; external dllname name 'TLN_EndWindowFrame';

// Layer management
function SetLayer(nlayer : Integer; tileset, tilemap : Pointer) : LongBool ; cdecl; external dllname name 'TLN_SetLayer';
function SetLayerPalette(nlayer : Integer; palette : Pointer) : LongBool ; cdecl; external dllname name 'TLN_SetLayerPalette';
function SetLayerPosition(nlayer , hstart, vstart : Integer) : LongBool ; cdecl; external dllname name 'TLN_SetLayerPosition';
function SetLayerScaling(nlayer : Integer; xfactor, yfactor : Single) : LongBool ; cdecl; external dllname name 'TLN_SetLayerScaling';
function SetLayerAffineTransform(nlayer : Integer; affine : PAffine) : LongBool ; cdecl; external dllname name 'TLN_SetLayerAffineTransform';
function SetLayerTransform(layer : Integer; angle, dx, dy, sx, sy : Single) : LongBool ; cdecl; external dllname name 'TLN_SetLayerTransform';
function SetLayerBlendMode(nlayer : Integer; blend : TBlend) : LongBool ; cdecl; external dllname name 'TLN_SetLayerBlendMode'; overload;
function SetLayerBlendMode(nlayer : Integer; blend : TBlend; a : Integer) : LongBool ; cdecl; external dllname name 'TLN_SetLayerBlendMode'; overload;
function SetLayerColumnOffset(nlayer : Integer; offset : TIntArr) : LongBool ; cdecl; external dllname name 'TLN_SetLayerColumnOffset';
function ResetLayerMode(nlayer : Integer) : LongBool ; cdecl; external dllname name 'TLN_ResetLayerMode';
function DisableLayer(nlayer : Integer) : LongBool ; cdecl; external dllname name 'TLN_DisableLayer';
function GetLayerPalette(nlayer : Integer) : Pointer; cdecl; external dllname name 'TLN_GetLayerPalette';
function GetLayerTile(nlayer : Integer; x , y : Integer; info : PTileInfo ) : LongBool ; cdecl; external dllname name 'TLN_GetLayerTile';

// Sprite management
function ConfigSprite(nsprite : Integer; spriteset : Pointer; flags : TTileFlags) : LongBool ; cdecl; external dllname name 'TLN_ConfigSprite';
function SetSpriteSet(nsprite : Integer; spriteset : Pointer) : LongBool ; cdecl; external dllname name 'TLN_SetSpriteSet';
function SetSpriteFlags(nsprite : Integer; flags : TTileFlags) : LongBool ; cdecl; external dllname name 'TLN_SetSpriteFlags';
function SetSpritePosition(nsprite, x, y : Integer) : LongBool ; cdecl; external dllname name 'TLN_SetSpritePosition';
function SetSpritePicture(nsprite, entry : Integer) : LongBool ; cdecl; external dllname name 'TLN_SetSpritePicture';
function SetSpritePalette(nsprite : Integer; palette : PInteger) : LongBool ; cdecl; external dllname name 'TLN_SetSpritePalette';
function SetSpriteBlendMode(nsprite : Integer; mode : TBlend; factor : Byte) : LongBool ; cdecl; external dllname name 'TLN_SetSpriteBlendMode';
function SetSpriteScaling(nsprite : Integer; sx, sy : Single) : LongBool ; cdecl; external dllname name 'TLN_SetSpriteScaling';
function ResetSpriteScaling(nsprite : Integer) : LongBool ; cdecl; external dllname name 'TLN_ResetSpriteScaling';
function GetSpritePicture(nsprite : Integer) : Integer; cdecl; external dllname name 'TLN_GetSpritePicture';
function GetAvailableSprite : Integer; cdecl; external dllname name 'TLN_GetAvailableSprite';
function EnableSpriteCollision(nsprite : Integer; enable : LongBool ) : LongBool ; cdecl; external dllname name 'TLN_EnableSpriteCollision';
function GetSpriteCollision(nsprite : Integer) : LongBool ; cdecl; external dllname name 'TLN_GetSpriteCollision';
function DisableSprite(nsprite : Integer) : LongBool ; cdecl; external dllname name 'TLN_DisableSprite';
function GetSpritePalette(nsprite : Integer) : Pointer; cdecl; external dllname name 'TLN_GetSpritePalette';

// Animation management
function SetPaletteAnimation(index : Integer; palette : Pointer; sequence : Pointer; blend : LongBool ) : LongBool ; cdecl; external dllname name 'TLN_SetPaletteAnimation';
function SetPaletteAnimationSource(index : Integer; palette : Pointer) : LongBool ; cdecl; external dllname name 'TLN_SetPaletteAnimationSource';
function SetTilemapAnimation(index, nlayer : Integer; sequence : Pointer) : LongBool ; cdecl; external dllname name 'TLN_SetTilemapAnimation';
function SetSpriteAnimation(index, nsprite : Integer; sequence : Pointer; loop : Integer) : LongBool ; cdecl; external dllname name 'TLN_SetSpriteAnimation';
function GetAnimationState(index : Integer) : LongBool ; cdecl; external dllname name 'TLN_GetAnimationState';
function SetAnimationDelay(index : Integer; delay : Integer) : LongBool ; cdecl; external dllname name 'TLN_SetAnimationDelay';
function GetAvailableAnimation : Integer; cdecl; external dllname name 'TLN_GetAvailableAnimation';
function DisableAnimation(index : Integer) : LongBool ; cdecl; external dllname name 'TLN_DisableAnimation';

// Spriteset
function CreateSpriteset(entries : Integer; rects : TRectArray; data : PByte; width, height, pitch : Integer; palette : Pointer) : Pointer; cdecl; external dllname name 'TLN_CreateSpriteset';
function CreateSpriteset1(entries : Integer; rects : PRect; data : PByte; width, height, pitch : Integer; palette : Pointer) : Pointer; cdecl; external dllname name 'TLN_CreateSpriteset';

function LoadSpriteset(name : string) : Pointer; cdecl; external dllname name 'TLN_LoadSpriteset';
function CloneSpriteset(src : Pointer) : Pointer; cdecl; external dllname name 'TLN_CloneSpriteset';
function GetSpriteInfo(spriteset : Pointer; entry : Integer; info : PSpriteInfo) : LongBool ; cdecl; external dllname name 'TLN_GetSpriteInfo';
function GetSpritesetPalette(spriteset : Pointer) : Pointer; cdecl; external dllname name 'TLN_GetSpritesetPalette';
function DeleteSpriteset(spriteset : Pointer) : LongBool ; cdecl; external dllname name 'TLN_DeleteSpriteset';


// Tileset
function CreateTileset(numtiles, width, height : Integer; palette : Pointer) : Pointer; cdecl; external dllname name 'TLN_CreateTileset';
function LoadTileset(filename : string) : Pointer; cdecl; external dllname name 'TLN_LoadTileset';
function CloneTileset(src : Pointer) : Pointer; cdecl; external dllname name 'TLN_CloneTileset';
function SetTilesetPixels(tileset : Pointer; entry : Integer; srcdata : PByte; srcpitch : Integer) : LongBool ; cdecl; external dllname name 'TLN_SetTilesetPixels';
function GetTileWidth(tileset : Pointer) : Integer; cdecl; external dllname name 'TLN_GetTileWidth';
function GetTileHeight(tileset : Pointer) : Integer; cdecl; external dllname name 'TLN_GetTileHeight';
function GetTilesetPalette(tileset : Pointer) : Pointer; cdecl; external dllname name 'TLN_GetTilesetPalette';
function DeleteTileset(tileset : Pointer) : LongBool ; cdecl; external dllname name 'TLN_DeleteTileset';

// Tilemap
function CreateTilemap(rows, cols : Integer; tiles : TTileArr) : Pointer; cdecl; external dllname name 'TLN_CreateTilemap';
function LoadTilemap(filename, layername : string) : Pointer; cdecl; external dllname name 'TLN_LoadTilemap';
function CloneTilemap(src : Pointer) : Pointer; cdecl; external dllname name 'TLN_CloneTilemap';
function GetTilemapRows(tilemap : Pointer) : Integer; cdecl; external dllname name 'TLN_GetTilemapRows';
function GetTilemapCols(tilemap : Pointer) : Integer; cdecl; external dllname name 'TLN_GetTilemapCols';
function GetTilemapTile(tilemap : Pointer; row, col : Integer; tile : PTile) : LongBool ; cdecl; external dllname name 'TLN_GetTilemapTile';
function SetTilemapTile(tilemap : Pointer; row, col : Integer; tile : PTile) : LongBool ; cdecl; external dllname name 'TLN_SetTilemapTile';
function CopyTiles(src : Pointer; srcrow, srccol, rows, cols : Integer; dst : Pointer; dstrow, dstcol : Integer) : LongBool ; cdecl; external dllname name 'TLN_CopyTiles';
function DeleteTilemap(tilemap : Pointer) : LongBool ; Register; external dllname name 'TLN_DeleteTilemap';

// Palette
function CreatePalette(entries : Integer) : Pointer; cdecl; external dllname name 'TLN_CreatePalette';
function LoadPalette(filename : String) : Pointer; cdecl; external dllname name 'TLN_LoadPalette';
function ClonePalette(src : Pointer) : Pointer; cdecl; external dllname name 'TLN_ClonePalette';
function SetPaletteColor(palette : Pointer; color : Integer; r, g, b : Byte) : LongBool ; cdecl; external dllname name 'TLN_SetPaletteColor';
function MixPalettes(src1, src2, dest : Pointer; factor : Byte) : LongBool ; cdecl; external dllname name 'TLN_MixPalettes';
function GetPaletteData(palette : Pointer; index : Integer) : Pointer; cdecl; external dllname name 'TLN_GetPaletteData';
function DeletePalette(palette : Pointer) : LongBool ; cdecl; external dllname name 'TLN_DeletePalette';

// Bitmap
function CreateBitmap(width, height, bpp : Integer) : Pointer; cdecl; external dllname name 'TLN_CreateBitmap';
function LoadBitmap(filename : AnsiString) : Pointer; cdecl; external dllname name 'TLN_LoadBitmap';
function CloneBitmap(src : Pointer) : Pointer; cdecl; external dllname name 'TLN_CloneBitmap';
function GetBitmapWidth(bitmap : Pointer) : Integer; cdecl; external dllname name 'TLN_GetBitmapWidth';
function GetBitmapHeight(bitmap : Pointer) : Integer; cdecl; external dllname name 'TLN_GetBitmapHeight';
function GetBitmapDepth(bitmap : Pointer) : Integer; cdecl; external dllname name 'TLN_GetBitmapDepth';
function GetBitmapPitch(bitmap : Pointer) : Integer; cdecl; external dllname name 'TLN_GetBitmapPitch';
function GetBitmapPalette(bitmap : Pointer) : Pointer; cdecl; external dllname name 'TLN_GetBitmapPalette';
function SetBitmapPalette(bitmap, palette : Pointer) : LongBool ; cdecl; external dllname name 'TLN_SetBitmapPalette';
function DeleteBitmap(bitmap : Pointer) : LongBool ; cdecl; external dllname name 'TLN_DeleteBitmap';

// Sequence
function CreateSequence(name : string; delay, first, num_frames : Integer; data : TIntArr) : Pointer; cdecl; external dllname name 'TLN_CreateSequence';
function CreateCycle(name : string; num_strips : Integer; strips : TColorStripArray) : Pointer; cdecl; external dllname name 'TLN_CreateCycle';
function CloneSequence(src : Pointer) : Pointer; cdecl; external dllname name 'TLN_CloneSequence';
function DeleteSequence(sequence : Pointer) : LongBool ; cdecl; external dllname name 'TLN_DeleteSequence';

// Squencepack
function CreateSequencePack : Pointer; cdecl; external dllname name 'TLN_CreateSequencePack';
function LoadSequencePack(filename : string) : Pointer; cdecl; external dllname name 'TLN_LoadSequencePack';
function FindSequence(sp : Pointer; name : string) : Pointer; cdecl; external dllname name 'TLN_FindSequence';
function AddSequenceToPack(sp, sequence : Pointer) : LongBool ; cdecl; external dllname name 'TLN_AddSequenceToPack';
function DeleteSequencePack(sp : Pointer) : LongBool ; cdecl; external dllname name 'TLN_DeleteSequencePack';


implementation


end.
