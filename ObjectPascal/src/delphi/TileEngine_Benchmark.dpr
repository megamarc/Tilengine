program TileEngine_Benchmark;

{$APPTYPE CONSOLE}
{$R *.res}

uses
  System.SysUtils,
  System.Math,
  uTileEngine;

const
  HRES = 400;
  VRES = 240;
  SIZE =	(32*48);
  SPRITES =	250;
  FRAMES = 1000;

var
  palette : Pointer;
  tileset : Pointer;
  tilemap : Pointer;
  spriteset : Pointer;
  frame : Integer;
  pixels : Integer;
  t0, t1 : Integer;

function Profile : Integer;
var
  frame : Integer;
begin
  frame := 0;
  t0 := GetTicks;
  while ProcessWindow and (frame < frames) do
  begin
    Inc(frame);
    UpdateFrame(frame);
  end;
  DrawFrame(frame);
  t1 := GetTicks - t0;

  Result := Round(pixels / t1 * 1000);
end;


procedure FillRandomData(data : PByte; len, coverage : Integer);
var
  c : Integer;
begin
  for c := 0 to len - 1 do
  begin
    if Random(100) < coverage then
      data[c] := Byte(RandomRange(0, 255) + 1)
    else data[c] := Byte(0);
  end;
end;

function CreateRandomTilemap(rows, cols, maxtile, coverage : Integer) : Pointer;
var
  tilemap : Pointer;
  tile : TTile;
  r, c, size : Integer;
  error : TError;
begin
  tile.index := 0;
  tile.flags := TTileFlags.FLAG_NONE;
  tilemap := CreateTilemap(rows, cols, nil);
  size := rows * cols;
  for r := 0 to rows - 1 do
  begin
    for c := 0 to cols - 1 do
    begin
      if Random(100) < coverage then
        tile.index := 1
      else
        tile.index := 1;
      SetTilemapTile(tilemap, r, c, @tile);
    end;
  end;

  Result := tilemap;
end;

function CreateRandomSpriteSet(coverage : Integer; palette : Pointer) : Pointer;
var
  spriteset : Pointer;
  data : PByte;
  rect : TRect;
begin
  GetMem(data, 32 * 48);
  FillRandomData(data, 32*48, coverage);
  rect.x := 0;
  rect.y := 0;
  rect.w := 32;
  rect.h := 48;
  spriteset := CreateSpriteset1(1, @rect, data, 32, 48, 32, palette);
  Dispose(data);

  Result := spriteset;
end;


function CreateRandomTileset(ntiles, coverage : Integer; palette : Pointer) : Pointer;
var
  tileset : Pointer;
  c : Integer;
  data : PByte;
begin
  GetMem(data, 8 * 8);
  tileset := CreateTileset(ntiles, 8, 8, palette);
  for c := 0 to ntiles - 1 do
  begin
    FillRandomData(data, 64, coverage);
    SetTilesetPixels(tileset, c, data, 8)
  end;
  Dispose(data);
  Result := tileset;
end;

function CreateRandomPalette : Pointer;
var
  palette : Pointer;
  c : Integer;
begin
  palette := CreatePalette(256);
  for c := 0 to 255 do
    SetPaletteColor(palette, c, RandomRange(0, 255), RandomRange(0, 255), RandomRange(0 ,255));
  Result := palette;
end;

procedure Benchmark;
var
  c : Integer;
  res : Integer;
  ok : Boolean;
  error : TError;
  framebuffer : PByte;
  pcen : Integer;
  y, x : Integer;
begin
  // setup engine
  Randomize;
  pcen := 70;
  Init(HRES, VRES, 1, SPRITES, 0);
  GetMem(framebuffer, HRES*VRES*4);
  SetRenderTarget(framebuffer , HRES * 4);
  CreateWindow(nil, CWF_NONE);

  error := GetLastError;

  // create assets
  palette := CreateRandomPalette;
  tileset := CreateRandomTileset(1024, pcen, palette);
  tilemap := CreateRandomTilemap(50, 30, 1023, pcen);
  spriteset := CreateRandomSpriteSet(pcen, palette);

  // setup layer
  ok := SetLayer(0 ,tileset, tilemap);
  pixels := FRAMES*HRES*VRES;

  Writeln('Normal layer..........');
  Writeln(Profile.ToString + ' ' + 'pixels/s.');

  Writeln('Scaling layer..........');
  ok := SetLayerScaling(0, 2.0, 2.0);
  Writeln(Profile.ToString + ' ' + 'pixels/s.');

  Writeln('Affine layer..........');
  SetLayerTransform(0, 45.0, 0.0, 0.0, 1.0, 1.0);
  Writeln(Profile.ToString + ' ' + 'pixels/s.');

  Writeln('Blend layer..........');
  ResetLayerMode(0);
  SetLayerBlendMode(0, BLEND_MIX, 128);
  Writeln(Profile.ToString + ' ' + 'pixels/s.');

  Writeln('Scaling blend layer...');
  SetLayerScaling(0, 2.0, 2.0);
  Writeln(Profile.ToString + ' ' + 'pixels/s.');

  Writeln('Affine blend layer....');
  SetLayerTransform(0, 45.0, 0.0, 0.0, 1.0, 1.0);
  Writeln(Profile.ToString + ' ' + 'pixels/s.');

  DisableLayer(0);

  //setup sprites

  for c := 0 to SPRITES do
  begin
    y := c div 25;
    x := c mod 25;
    ok := ConfigSprite(c, spriteset, FLAG_NONE);
    ok := SetSpritePicture(c, 0);
    SetSpritePosition(c, x * 15, y * 21);
  end;

  pixels := FRAMES*SPRITES*SIZE;

  Writeln('Affine blend layer....');
  Writeln(Profile.ToString + ' ' + 'pixels/s.');
  Writeln('Colliding sprites......');
  for c := 0 to sprites do EnableSpriteCollision(c, True);
  Writeln(Profile.ToString + ' ' + 'pixels/s.');

  //Dispose(framebuffer);
  DeleteTilemap(tilemap);
  DeleteTileset(tileset);
  Deinit;
end;

begin
  Writeln('Tilengine benchmark tool ObjectPascal');
  Writeln('http://www.tilengine.org');
  Benchmark;
end.
