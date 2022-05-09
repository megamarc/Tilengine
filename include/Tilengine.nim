##
##  Tilengine - The 2D retro graphics engine with raster effects
##  Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
##  All rights reserved
##
##  This Source Code Form is subject to the terms of the Mozilla Public
##  License, v. 2.0. If a copy of the MPL was not distributed with this
##  file, You can obtain one at http://mozilla.org/MPL/2.0/.
##

## *
##  \defgroup types
##  \brief Common data types
##  @{
##  Tilengine shared

when defined(_MSC_VER):
  when defined(LIB_EXPORTS):
    discard
  else:
    const
      __declspec* = cast[dllexport](__declspec(dllimport))
  when _MSC_VER >= 1600:
    discard
  else:
    type
      int8_t* = char
    ## !< signed 8-bit wide data
    type
      int16_t* = cshort
    ## !< signed 16-bit wide data
    type
      int32_t* = cint
    ## !< signed 32-bit wide data
    type
      uint8_t* = cuchar
    ## !< unsigned 8-bit wide data
    type
      uint16_t* = cushort
    ## !< unsigned 16-bit wide data
    type
      uint32_t* = cuint
    ## !< unsigned 32-bit wide data
  when _MSC_VER >= 1800:
    discard
  else:
    type
      bool* = cuchar
    ## !< C++ bool type for C language
    const
      false* = 0
      true* = 1
else:
  when defined(LIB_EXPORTS):
    const
      __declspec* = cast[dllexport](__attribute__((visibility("default"))))
  else:
    const
      __declspec* = (dllexport)
##  version

const
  TILENGINE_VER_MAJ* = 2
  TILENGINE_VER_MIN* = 9
  TILENGINE_VER_REV* = 4
  TILENGINE_HEADER_VERSION* = ((TILENGINE_VER_MAJ shl 16) or
      (TILENGINE_VER_MIN shl 8) or TILENGINE_VER_REV)

template BITVAL*(n: untyped): untyped =
  (1 shl (n))

## ! tile/sprite flags. Can be none or a combination of the following:

type
  TLN_TileFlags* {.size: sizeof(cint).} = enum
    FLAG_NONE = 0,              ## !< no flags
    FLAG_FLIPX = BITVAL(15),    ## !< horizontal flip
    FLAG_FLIPY = BITVAL(14),    ## !< vertical flip
    FLAG_ROTATE = BITVAL(13),   ## !< row/column flip (unsupported, Tiled compatibility)
    FLAG_PRIORITY = BITVAL(12), ## !< tile goes in front of sprite layer
    FLAG_MASKED = BITVAL(11)    ## !< sprite won't be drawn inside masked region


## !
##  layer blend modes. Must be one of these and are mutually exclusive:
##

type
  TLN_Blend* {.size: sizeof(cint).} = enum
    BLEND_NONE,               ## !< blending disabled
    BLEND_MIX25,              ## !< color averaging 1
    BLEND_MIX50,              ## !< color averaging 2
    BLEND_MIX75,              ## !< color averaging 3
    BLEND_ADD,                ## !< color is always brighter (simulate light effects)
    BLEND_SUB,                ## !< color is always darker (simulate shadow effects)
    BLEND_MOD,                ## !< color is always darker (simulate shadow effects)
    BLEND_CUSTOM,             ## !< user provided blend function with TLN_SetCustomBlendFunction()
    MAX_BLEND

const
  BLEND_MIX* = BLEND_MIX50

## !
##  layer type retrieved by \ref TLN_GetLayerType
##

type
  TLN_LayerType* {.size: sizeof(cint).} = enum
    LAYER_NONE,               ## !< undefined
    LAYER_TILE,               ## !< tilemap-based layer
    LAYER_OBJECT,             ## !< objects layer
    LAYER_BITMAP              ## !< bitmapped layer


## ! Affine transformation parameters

type
  TLN_Affine* {.importc: "TLN_Affine", header: "Tilengine.h", bycopy.} = object
    angle* {.importc: "angle".}: cfloat ## !< rotation in degrees
    dx* {.importc: "dx".}: cfloat ## !< horizontal translation
    dy* {.importc: "dy".}: cfloat ## !< vertical translation
    sx* {.importc: "sx".}: cfloat ## !< horizontal scaling
    sy* {.importc: "sy".}: cfloat ## !< vertical scaling


## ! Tile item for Tilemap access methods

type
  INNER_C_STRUCT_Tilengine_131* {.importc: "Tile::no_name", header: "Tilengine.h",
                                 bycopy.} = object
    index* {.importc: "index".}: uint16_t ## !< tile index
    flags* {.importc: "flags".}: uint16_t ## !< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY)

  Tile* {.importc: "Tile", header: "Tilengine.h", bycopy, union.} = object
    value* {.importc: "value".}: uint32_t
    ano_Tilengine_131* {.importc: "ano_Tilengine_131".}: INNER_C_STRUCT_Tilengine_131


## ! frame animation definition

type
  TLN_SequenceFrame* {.importc: "TLN_SequenceFrame", header: "Tilengine.h", bycopy.} = object
    index* {.importc: "index".}: cint ## !< tile/sprite index
    delay* {.importc: "delay".}: cint ## !< time delay for next frame


## ! color strip definition

type
  TLN_ColorStrip* {.importc: "TLN_ColorStrip", header: "Tilengine.h", bycopy.} = object
    delay* {.importc: "delay".}: cint ## !< time delay between frames
    first* {.importc: "first".}: uint8_t ## !< index of first color to cycle
    count* {.importc: "count".}: uint8_t ## !< number of colors in the cycle
    dir* {.importc: "dir".}: uint8_t ## !< direction: 0=descending, 1=ascending


## ! sequence info returned by TLN_GetSequenceInfo

type
  TLN_SequenceInfo* {.importc: "TLN_SequenceInfo", header: "Tilengine.h", bycopy.} = object
    name* {.importc: "name".}: array[32, char] ## !< sequence name
    num_frames* {.importc: "num_frames".}: cint ## !< number of frames


## ! Sprite creation info for TLN_CreateSpriteset()

type
  TLN_SpriteData* {.importc: "TLN_SpriteData", header: "Tilengine.h", bycopy.} = object
    name* {.importc: "name".}: array[64, char] ## !< entry name
    x* {.importc: "x".}: cint    ## !< horizontal position
    y* {.importc: "y".}: cint    ## !< vertical position
    w* {.importc: "w".}: cint    ## !< width
    h* {.importc: "h".}: cint    ## !< height


## ! Sprite information

type
  TLN_SpriteInfo* {.importc: "TLN_SpriteInfo", header: "Tilengine.h", bycopy.} = object
    w* {.importc: "w".}: cint    ## !< width of sprite
    h* {.importc: "h".}: cint    ## !< height of sprite


## ! Tile information returned by TLN_GetLayerTile()

type
  TLN_TileInfo* {.importc: "TLN_TileInfo", header: "Tilengine.h", bycopy.} = object
    index* {.importc: "index".}: uint16_t ## !< tile index
    flags* {.importc: "flags".}: uint16_t ## !< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY)
    row* {.importc: "row".}: cint ## !< row number in the tilemap
    col* {.importc: "col".}: cint ## !< col number in the tilemap
    xoffset* {.importc: "xoffset".}: cint ## !< horizontal position inside the title
    yoffset* {.importc: "yoffset".}: cint ## !< vertical position inside the title
    color* {.importc: "color".}: uint8_t ## !< color index at collision point
    `type`* {.importc: "type".}: uint8_t ## !< tile type
    empty* {.importc: "empty".}: bool ## !< cell is empty


## ! Object item info returned by TLN_GetObjectInfo()

type
  TLN_ObjectInfo* {.importc: "TLN_ObjectInfo", header: "Tilengine.h", bycopy.} = object
    id* {.importc: "id".}: uint16_t ## !< unique ID
    gid* {.importc: "gid".}: uint16_t ## !< graphic ID (tile index)
    flags* {.importc: "flags".}: uint16_t ## !< attributes (FLAG_FLIPX, FLAG_FLIPY, FLAG_PRIORITY)
    x* {.importc: "x".}: cint    ## !< horizontal position
    y* {.importc: "y".}: cint    ## !< vertical position
    width* {.importc: "width".}: cint ## !< horizontal size
    height* {.importc: "height".}: cint ## !< vertical size
    `type`* {.importc: "type".}: uint8_t ## !< type property
    visible* {.importc: "visible".}: bool ## !< visible property
    name* {.importc: "name".}: array[64, char] ## !< name property


## ! Tileset attributes for TLN_CreateTileset()

type
  TLN_TileAttributes* {.importc: "TLN_TileAttributes", header: "Tilengine.h", bycopy.} = object
    `type`* {.importc: "type".}: uint8_t ## !< tile type
    priority* {.importc: "priority".}: bool ## !< priority flag set


## ! overlays for CRT effect

type
  TLN_Overlay* {.size: sizeof(cint).} = enum
    TLN_OVERLAY_NONE,         ## !< no overlay
    TLN_OVERLAY_SHADOWMASK,   ## !< Shadow mask pattern
    TLN_OVERLAY_APERTURE,     ## !< Aperture grille pattern
    TLN_OVERLAY_SCANLINES,    ## !< Scanlines pattern
    TLN_OVERLAY_CUSTOM,       ## !< User-provided when calling TLN_CreateWindow()
    TLN_MAX_OVERLAY


## ! pixel mapping for TLN_SetLayerPixelMapping()

type
  TLN_PixelMap* {.importc: "TLN_PixelMap", header: "Tilengine.h", bycopy.} = object
    dx* {.importc: "dx".}: int16_t ## ! horizontal pixel displacement
    dy* {.importc: "dy".}: int16_t ## ! vertical pixel displacement

  TLN_Engine* = ptr Engine

## !< Engine context

type
  TLN_Tile* = ptr Tile

## !< Tile reference

type
  TLN_Tileset* = ptr Tileset

## !< Opaque tileset reference

type
  TLN_Tilemap* = ptr Tilemap

## !< Opaque tilemap reference

type
  TLN_Palette* = ptr Palette

## !< Opaque palette reference

type
  TLN_Spriteset* = ptr Spriteset

## !< Opaque sspriteset reference

type
  TLN_Sequence* = ptr Sequence

## !< Opaque sequence reference

type
  TLN_SequencePack* = ptr SequencePack

## !< Opaque sequence pack reference

type
  TLN_Bitmap* = ptr Bitmap

## !< Opaque bitmap reference

type
  TLN_ObjectList* = ptr ObjectList

## !< Opaque object list reference
## ! Image Tile items for TLN_CreateImageTileset()

type
  TLN_TileImage* {.importc: "TLN_TileImage", header: "Tilengine.h", bycopy.} = object
    bitmap* {.importc: "bitmap".}: TLN_Bitmap
    id* {.importc: "id".}: uint16_t
    `type`* {.importc: "type".}: uint8_t


## ! Sprite state

type
  TLN_SpriteState* {.importc: "TLN_SpriteState", header: "Tilengine.h", bycopy.} = object
    x* {.importc: "x".}: cint    ## !< Screen position x
    y* {.importc: "y".}: cint    ## !< Screen position y
    w* {.importc: "w".}: cint    ## !< Actual width in screen (after scaling)
    h* {.importc: "h".}: cint    ## !< Actual height in screen (after scaling)
    flags* {.importc: "flags".}: uint32_t ## !< flags
    palette* {.importc: "palette".}: TLN_Palette ## !< assigned palette
    spriteset* {.importc: "spriteset".}: TLN_Spriteset ## !< assigned spriteset
    index* {.importc: "index".}: cint ## !< graphic index inside spriteset
    enabled* {.importc: "enabled".}: bool ## !< enabled or not
    collision* {.importc: "collision".}: bool ## !< per-pixel collision detection enabled or not


##  callbacks

type
  TLN_VideoCallback* = proc (scanline: cint)
  TLN_BlendFunction* = proc (src: uint8_t; dst: uint8_t): uint8_t
  TLN_SDLCallback* = proc (a1: ptr SDL_Event)

## ! Player index for input assignment functions

type
  TLN_Player* {.size: sizeof(cint).} = enum
    PLAYER1,                  ## !< Player 1
    PLAYER2,                  ## !< Player 2
    PLAYER3,                  ## !< Player 3
    PLAYER4                   ## !< Player 4


## ! Standard inputs query for TLN_GetInput()

type
  TLN_Input* {.size: sizeof(cint).} = enum
    INPUT_NONE,               ## !< no input
    INPUT_UP,                 ## !< up direction
    INPUT_DOWN,               ## !< down direction
    INPUT_LEFT,               ## !< left direction
    INPUT_RIGHT,              ## !< right direction
    INPUT_BUTTON1,            ## !< 1st action button
    INPUT_BUTTON2,            ## !< 2nd action button
    INPUT_BUTTON3,            ## !< 3th action button
    INPUT_BUTTON4,            ## !< 4th action button
    INPUT_BUTTON5,            ## !< 5th action button
    INPUT_BUTTON6,            ## !< 6th action button
    INPUT_START,              ## !< Start button
    INPUT_QUIT,               ## !< Window close (only Player 1 keyboard)
    INPUT_CRT,                ## !< CRT toggle (only Player 1 keyboard)
              ##  ... up to 32 unique inputs
    INPUT_P1 = (PLAYER1 shl 5),   ## !< request player 1 input (default)
    INPUT_P2 = (PLAYER2 shl 5),   ## !< request player 2 input
    INPUT_P3 = (PLAYER3 shl 5),   ## !< request player 3 input
    INPUT_P4 = (PLAYER4 shl 5)    ## !< request player 4 input
                          ##  compatibility symbols for pre-1.18 input model

const
  INPUT_A* = INPUT_BUTTON1
  INPUT_B* = INPUT_BUTTON2
  INPUT_C* = INPUT_BUTTON3
  INPUT_D* = INPUT_BUTTON4
  INPUT_E* = INPUT_BUTTON5
  INPUT_F* = INPUT_BUTTON6

## ! CreateWindow flags. Can be none or a combination of the following:

const
  CWF_FULLSCREEN* = (1 shl 0)     ## !< create a fullscreen window
  CWF_VSYNC* = (1 shl 1)          ## !< sync frame updates with vertical retrace
  CWF_S1* = (1 shl 2)             ## !< create a window the same size as the framebuffer
  CWF_S2* = (2 shl 2)             ## !< create a window 2x the size the framebuffer
  CWF_S3* = (3 shl 2)             ## !< create a window 3x the size the framebuffer
  CWF_S4* = (4 shl 2)             ## !< create a window 4x the size the framebuffer
  CWF_S5* = (5 shl 2)             ## !< create a window 5x the size the framebuffer
  CWF_NEAREST* = (1 shl 6)        ## <! unfiltered upscaling

## ! Error codes

type
  TLN_Error* {.size: sizeof(cint).} = enum
    TLN_ERR_OK,               ## !< No error
    TLN_ERR_OUT_OF_MEMORY,    ## !< Not enough memory
    TLN_ERR_IDX_LAYER,        ## !< Layer index out of range
    TLN_ERR_IDX_SPRITE,       ## !< Sprite index out of range
    TLN_ERR_IDX_ANIMATION,    ## !< Animation index out of range
    TLN_ERR_IDX_PICTURE,      ## !< Picture or tile index out of range
    TLN_ERR_REF_TILESET,      ## !< Invalid TLN_Tileset reference
    TLN_ERR_REF_TILEMAP,      ## !< Invalid TLN_Tilemap reference
    TLN_ERR_REF_SPRITESET,    ## !< Invalid TLN_Spriteset reference
    TLN_ERR_REF_PALETTE,      ## !< Invalid TLN_Palette reference
    TLN_ERR_REF_SEQUENCE,     ## !< Invalid TLN_Sequence reference
    TLN_ERR_REF_SEQPACK,      ## !< Invalid TLN_SequencePack reference
    TLN_ERR_REF_BITMAP,       ## !< Invalid TLN_Bitmap reference
    TLN_ERR_NULL_POINTER,     ## !< Null pointer as argument
    TLN_ERR_FILE_NOT_FOUND,   ## !< Resource file not found
    TLN_ERR_WRONG_FORMAT,     ## !< Resource file has invalid format
    TLN_ERR_WRONG_SIZE,       ## !< A width or height parameter is invalid
    TLN_ERR_UNSUPPORTED,      ## !< Unsupported function
    TLN_ERR_REF_LIST,         ## !< Invalid TLN_ObjectList reference
    TLN_MAX_ERR


## ! Debug level

type
  TLN_LogLevel* {.size: sizeof(cint).} = enum
    TLN_LOG_NONE,             ## !< Don't print anything (default)
    TLN_LOG_ERRORS,           ## !< Print only runtime errors
    TLN_LOG_VERBOSE           ## !< Print everything


## *@}

## *
##  \defgroup setup
##  \brief Basic setup and management
##  @{

proc TLN_Init*(hres: cint; vres: cint; numlayers: cint; numsprites: cint;
              numanimations: cint): TLN_Engine {.importc: "TLN_Init",
    header: "Tilengine.h".}
proc TLN_Deinit*() {.importc: "TLN_Deinit", header: "Tilengine.h".}
proc TLN_DeleteContext*(context: TLN_Engine): bool {.importc: "TLN_DeleteContext",
    header: "Tilengine.h".}
proc TLN_SetContext*(context: TLN_Engine): bool {.importc: "TLN_SetContext",
    header: "Tilengine.h".}
proc TLN_GetContext*(): TLN_Engine {.importc: "TLN_GetContext", header: "Tilengine.h".}
proc TLN_GetWidth*(): cint {.importc: "TLN_GetWidth", header: "Tilengine.h".}
proc TLN_GetHeight*(): cint {.importc: "TLN_GetHeight", header: "Tilengine.h".}
proc TLN_GetNumObjects*(): uint32_t {.importc: "TLN_GetNumObjects",
                                   header: "Tilengine.h".}
proc TLN_GetUsedMemory*(): uint32_t {.importc: "TLN_GetUsedMemory",
                                   header: "Tilengine.h".}
proc TLN_GetVersion*(): uint32_t {.importc: "TLN_GetVersion", header: "Tilengine.h".}
proc TLN_GetNumLayers*(): cint {.importc: "TLN_GetNumLayers", header: "Tilengine.h".}
proc TLN_GetNumSprites*(): cint {.importc: "TLN_GetNumSprites", header: "Tilengine.h".}
proc TLN_SetBGColor*(r: uint8_t; g: uint8_t; b: uint8_t) {.importc: "TLN_SetBGColor",
    header: "Tilengine.h".}
proc TLN_SetBGColorFromTilemap*(tilemap: TLN_Tilemap): bool {.
    importc: "TLN_SetBGColorFromTilemap", header: "Tilengine.h".}
proc TLN_DisableBGColor*() {.importc: "TLN_DisableBGColor", header: "Tilengine.h".}
proc TLN_SetBGBitmap*(bitmap: TLN_Bitmap): bool {.importc: "TLN_SetBGBitmap",
    header: "Tilengine.h".}
proc TLN_SetBGPalette*(palette: TLN_Palette): bool {.importc: "TLN_SetBGPalette",
    header: "Tilengine.h".}
proc TLN_SetRasterCallback*(a1: TLN_VideoCallback) {.
    importc: "TLN_SetRasterCallback", header: "Tilengine.h".}
proc TLN_SetFrameCallback*(a1: TLN_VideoCallback) {.
    importc: "TLN_SetFrameCallback", header: "Tilengine.h".}
proc TLN_SetRenderTarget*(data: ptr uint8_t; pitch: cint) {.
    importc: "TLN_SetRenderTarget", header: "Tilengine.h".}
proc TLN_UpdateFrame*(frame: cint) {.importc: "TLN_UpdateFrame",
                                  header: "Tilengine.h".}
proc TLN_SetLoadPath*(path: cstring) {.importc: "TLN_SetLoadPath",
                                    header: "Tilengine.h".}
proc TLN_SetCustomBlendFunction*(a1: TLN_BlendFunction) {.
    importc: "TLN_SetCustomBlendFunction", header: "Tilengine.h".}
proc TLN_SetLogLevel*(log_level: TLN_LogLevel) {.importc: "TLN_SetLogLevel",
    header: "Tilengine.h".}
proc TLN_OpenResourcePack*(filename: cstring; key: cstring): bool {.
    importc: "TLN_OpenResourcePack", header: "Tilengine.h".}
proc TLN_CloseResourcePack*() {.importc: "TLN_CloseResourcePack",
                              header: "Tilengine.h".}
## *@}
## *
##  \defgroup errors
##  \brief Basic setup and management
##  @{

proc TLN_SetLastError*(error: TLN_Error) {.importc: "TLN_SetLastError",
                                        header: "Tilengine.h".}
proc TLN_GetLastError*(): TLN_Error {.importc: "TLN_GetLastError",
                                   header: "Tilengine.h".}
proc TLN_GetErrorString*(error: TLN_Error): cstring {.importc: "TLN_GetErrorString",
    header: "Tilengine.h".}
## *@}
## *
##  \defgroup windowing
##  \brief Built-in window and input management
##  @{

proc TLN_CreateWindow*(overlay: cstring; flags: cint): bool {.
    importc: "TLN_CreateWindow", header: "Tilengine.h".}
proc TLN_CreateWindowThread*(overlay: cstring; flags: cint): bool {.
    importc: "TLN_CreateWindowThread", header: "Tilengine.h".}
proc TLN_SetWindowTitle*(title: cstring) {.importc: "TLN_SetWindowTitle",
                                        header: "Tilengine.h".}
proc TLN_ProcessWindow*(): bool {.importc: "TLN_ProcessWindow", header: "Tilengine.h".}
proc TLN_IsWindowActive*(): bool {.importc: "TLN_IsWindowActive",
                                header: "Tilengine.h".}
proc TLN_GetInput*(id: TLN_Input): bool {.importc: "TLN_GetInput",
                                      header: "Tilengine.h".}
proc TLN_EnableInput*(player: TLN_Player; enable: bool) {.importc: "TLN_EnableInput",
    header: "Tilengine.h".}
proc TLN_AssignInputJoystick*(player: TLN_Player; index: cint) {.
    importc: "TLN_AssignInputJoystick", header: "Tilengine.h".}
proc TLN_DefineInputKey*(player: TLN_Player; input: TLN_Input; keycode: uint32_t) {.
    importc: "TLN_DefineInputKey", header: "Tilengine.h".}
proc TLN_DefineInputButton*(player: TLN_Player; input: TLN_Input; joybutton: uint8_t) {.
    importc: "TLN_DefineInputButton", header: "Tilengine.h".}
proc TLN_DrawFrame*(frame: cint) {.importc: "TLN_DrawFrame", header: "Tilengine.h".}
proc TLN_WaitRedraw*() {.importc: "TLN_WaitRedraw", header: "Tilengine.h".}
proc TLN_DeleteWindow*() {.importc: "TLN_DeleteWindow", header: "Tilengine.h".}
proc TLN_EnableBlur*(mode: bool) {.importc: "TLN_EnableBlur", header: "Tilengine.h".}
proc TLN_EnableCRTEffect*(overlay: TLN_Overlay; overlay_factor: uint8_t;
                         threshold: uint8_t; v0: uint8_t; v1: uint8_t; v2: uint8_t;
                         v3: uint8_t; blur: bool; glow_factor: uint8_t) {.
    importc: "TLN_EnableCRTEffect", header: "Tilengine.h".}
proc TLN_DisableCRTEffect*() {.importc: "TLN_DisableCRTEffect",
                             header: "Tilengine.h".}
proc TLN_SetSDLCallback*(a1: TLN_SDLCallback) {.importc: "TLN_SetSDLCallback",
    header: "Tilengine.h".}
proc TLN_Delay*(msecs: uint32_t) {.importc: "TLN_Delay", header: "Tilengine.h".}
proc TLN_GetTicks*(): uint32_t {.importc: "TLN_GetTicks", header: "Tilengine.h".}
proc TLN_GetWindowWidth*(): cint {.importc: "TLN_GetWindowWidth",
                                header: "Tilengine.h".}
proc TLN_GetWindowHeight*(): cint {.importc: "TLN_GetWindowHeight",
                                 header: "Tilengine.h".}
## *@}
## *
##  \defgroup spriteset
##  \brief Spriteset resources management for sprites
##  @{

proc TLN_CreateSpriteset*(bitmap: TLN_Bitmap; data: ptr TLN_SpriteData;
                         num_entries: cint): TLN_Spriteset {.
    importc: "TLN_CreateSpriteset", header: "Tilengine.h".}
proc TLN_LoadSpriteset*(name: cstring): TLN_Spriteset {.
    importc: "TLN_LoadSpriteset", header: "Tilengine.h".}
proc TLN_CloneSpriteset*(src: TLN_Spriteset): TLN_Spriteset {.
    importc: "TLN_CloneSpriteset", header: "Tilengine.h".}
proc TLN_GetSpriteInfo*(spriteset: TLN_Spriteset; entry: cint;
                       info: ptr TLN_SpriteInfo): bool {.
    importc: "TLN_GetSpriteInfo", header: "Tilengine.h".}
proc TLN_GetSpritesetPalette*(spriteset: TLN_Spriteset): TLN_Palette {.
    importc: "TLN_GetSpritesetPalette", header: "Tilengine.h".}
proc TLN_FindSpritesetSprite*(spriteset: TLN_Spriteset; name: cstring): cint {.
    importc: "TLN_FindSpritesetSprite", header: "Tilengine.h".}
proc TLN_SetSpritesetData*(spriteset: TLN_Spriteset; entry: cint;
                          data: ptr TLN_SpriteData; pixels: pointer; pitch: cint): bool {.
    importc: "TLN_SetSpritesetData", header: "Tilengine.h".}
proc TLN_DeleteSpriteset*(Spriteset: TLN_Spriteset): bool {.
    importc: "TLN_DeleteSpriteset", header: "Tilengine.h".}
## *@}
## *
##  \defgroup tileset
##  \brief Tileset resources management for background layers
##  @{

proc TLN_CreateTileset*(numtiles: cint; width: cint; height: cint;
                       palette: TLN_Palette; sp: TLN_SequencePack;
                       attributes: ptr TLN_TileAttributes): TLN_Tileset {.
    importc: "TLN_CreateTileset", header: "Tilengine.h".}
proc TLN_CreateImageTileset*(numtiles: cint; images: ptr TLN_TileImage): TLN_Tileset {.
    importc: "TLN_CreateImageTileset", header: "Tilengine.h".}
proc TLN_LoadTileset*(filename: cstring): TLN_Tileset {.importc: "TLN_LoadTileset",
    header: "Tilengine.h".}
proc TLN_CloneTileset*(src: TLN_Tileset): TLN_Tileset {.importc: "TLN_CloneTileset",
    header: "Tilengine.h".}
proc TLN_SetTilesetPixels*(tileset: TLN_Tileset; entry: cint; srcdata: ptr uint8_t;
                          srcpitch: cint): bool {.importc: "TLN_SetTilesetPixels",
    header: "Tilengine.h".}
proc TLN_GetTileWidth*(tileset: TLN_Tileset): cint {.importc: "TLN_GetTileWidth",
    header: "Tilengine.h".}
proc TLN_GetTileHeight*(tileset: TLN_Tileset): cint {.importc: "TLN_GetTileHeight",
    header: "Tilengine.h".}
proc TLN_GetTilesetNumTiles*(tileset: TLN_Tileset): cint {.
    importc: "TLN_GetTilesetNumTiles", header: "Tilengine.h".}
proc TLN_GetTilesetPalette*(tileset: TLN_Tileset): TLN_Palette {.
    importc: "TLN_GetTilesetPalette", header: "Tilengine.h".}
proc TLN_GetTilesetSequencePack*(tileset: TLN_Tileset): TLN_SequencePack {.
    importc: "TLN_GetTilesetSequencePack", header: "Tilengine.h".}
proc TLN_DeleteTileset*(tileset: TLN_Tileset): bool {.importc: "TLN_DeleteTileset",
    header: "Tilengine.h".}
## *@}
## *
##  \defgroup tilemap
##  \brief Tilemap resources management for background layers
##  @{

proc TLN_CreateTilemap*(rows: cint; cols: cint; tiles: TLN_Tile; bgcolor: uint32_t;
                       tileset: TLN_Tileset): TLN_Tilemap {.
    importc: "TLN_CreateTilemap", header: "Tilengine.h".}
proc TLN_LoadTilemap*(filename: cstring; layername: cstring): TLN_Tilemap {.
    importc: "TLN_LoadTilemap", header: "Tilengine.h".}
proc TLN_CloneTilemap*(src: TLN_Tilemap): TLN_Tilemap {.importc: "TLN_CloneTilemap",
    header: "Tilengine.h".}
proc TLN_GetTilemapRows*(tilemap: TLN_Tilemap): cint {.
    importc: "TLN_GetTilemapRows", header: "Tilengine.h".}
proc TLN_GetTilemapCols*(tilemap: TLN_Tilemap): cint {.
    importc: "TLN_GetTilemapCols", header: "Tilengine.h".}
proc TLN_GetTilemapTileset*(tilemap: TLN_Tilemap): TLN_Tileset {.
    importc: "TLN_GetTilemapTileset", header: "Tilengine.h".}
proc TLN_GetTilemapTile*(tilemap: TLN_Tilemap; row: cint; col: cint; tile: TLN_Tile): bool {.
    importc: "TLN_GetTilemapTile", header: "Tilengine.h".}
proc TLN_SetTilemapTile*(tilemap: TLN_Tilemap; row: cint; col: cint; tile: TLN_Tile): bool {.
    importc: "TLN_SetTilemapTile", header: "Tilengine.h".}
proc TLN_CopyTiles*(src: TLN_Tilemap; srcrow: cint; srccol: cint; rows: cint; cols: cint;
                   dst: TLN_Tilemap; dstrow: cint; dstcol: cint): bool {.
    importc: "TLN_CopyTiles", header: "Tilengine.h".}
proc TLN_DeleteTilemap*(tilemap: TLN_Tilemap): bool {.importc: "TLN_DeleteTilemap",
    header: "Tilengine.h".}
## *@}
## *
##  \defgroup palette
##  \brief Color palette resources management for sprites and background layers
##  @{

proc TLN_CreatePalette*(entries: cint): TLN_Palette {.importc: "TLN_CreatePalette",
    header: "Tilengine.h".}
proc TLN_LoadPalette*(filename: cstring): TLN_Palette {.importc: "TLN_LoadPalette",
    header: "Tilengine.h".}
proc TLN_ClonePalette*(src: TLN_Palette): TLN_Palette {.importc: "TLN_ClonePalette",
    header: "Tilengine.h".}
proc TLN_SetPaletteColor*(palette: TLN_Palette; color: cint; r: uint8_t; g: uint8_t;
                         b: uint8_t): bool {.importc: "TLN_SetPaletteColor",
    header: "Tilengine.h".}
proc TLN_MixPalettes*(src1: TLN_Palette; src2: TLN_Palette; dst: TLN_Palette;
                     factor: uint8_t): bool {.importc: "TLN_MixPalettes",
    header: "Tilengine.h".}
proc TLN_AddPaletteColor*(palette: TLN_Palette; r: uint8_t; g: uint8_t; b: uint8_t;
                         start: uint8_t; num: uint8_t): bool {.
    importc: "TLN_AddPaletteColor", header: "Tilengine.h".}
proc TLN_SubPaletteColor*(palette: TLN_Palette; r: uint8_t; g: uint8_t; b: uint8_t;
                         start: uint8_t; num: uint8_t): bool {.
    importc: "TLN_SubPaletteColor", header: "Tilengine.h".}
proc TLN_ModPaletteColor*(palette: TLN_Palette; r: uint8_t; g: uint8_t; b: uint8_t;
                         start: uint8_t; num: uint8_t): bool {.
    importc: "TLN_ModPaletteColor", header: "Tilengine.h".}
proc TLN_GetPaletteData*(palette: TLN_Palette; index: cint): ptr uint8_t {.
    importc: "TLN_GetPaletteData", header: "Tilengine.h".}
proc TLN_DeletePalette*(palette: TLN_Palette): bool {.importc: "TLN_DeletePalette",
    header: "Tilengine.h".}
## *@}
## *
##  \defgroup bitmap
##  \brief Bitmap management
##  @{

proc TLN_CreateBitmap*(width: cint; height: cint; bpp: cint): TLN_Bitmap {.
    importc: "TLN_CreateBitmap", header: "Tilengine.h".}
proc TLN_LoadBitmap*(filename: cstring): TLN_Bitmap {.importc: "TLN_LoadBitmap",
    header: "Tilengine.h".}
proc TLN_CloneBitmap*(src: TLN_Bitmap): TLN_Bitmap {.importc: "TLN_CloneBitmap",
    header: "Tilengine.h".}
proc TLN_GetBitmapPtr*(bitmap: TLN_Bitmap; x: cint; y: cint): ptr uint8_t {.
    importc: "TLN_GetBitmapPtr", header: "Tilengine.h".}
proc TLN_GetBitmapWidth*(bitmap: TLN_Bitmap): cint {.importc: "TLN_GetBitmapWidth",
    header: "Tilengine.h".}
proc TLN_GetBitmapHeight*(bitmap: TLN_Bitmap): cint {.
    importc: "TLN_GetBitmapHeight", header: "Tilengine.h".}
proc TLN_GetBitmapDepth*(bitmap: TLN_Bitmap): cint {.importc: "TLN_GetBitmapDepth",
    header: "Tilengine.h".}
proc TLN_GetBitmapPitch*(bitmap: TLN_Bitmap): cint {.importc: "TLN_GetBitmapPitch",
    header: "Tilengine.h".}
proc TLN_GetBitmapPalette*(bitmap: TLN_Bitmap): TLN_Palette {.
    importc: "TLN_GetBitmapPalette", header: "Tilengine.h".}
proc TLN_SetBitmapPalette*(bitmap: TLN_Bitmap; palette: TLN_Palette): bool {.
    importc: "TLN_SetBitmapPalette", header: "Tilengine.h".}
proc TLN_DeleteBitmap*(bitmap: TLN_Bitmap): bool {.importc: "TLN_DeleteBitmap",
    header: "Tilengine.h".}
## *@}
## *
##  \defgroup objects
##  \brief ObjectList resources management
##  @{

proc TLN_CreateObjectList*(): TLN_ObjectList {.importc: "TLN_CreateObjectList",
    header: "Tilengine.h".}
proc TLN_AddTileObjectToList*(list: TLN_ObjectList; id: uint16_t; gid: uint16_t;
                             flags: uint16_t; x: cint; y: cint): bool {.
    importc: "TLN_AddTileObjectToList", header: "Tilengine.h".}
proc TLN_LoadObjectList*(filename: cstring; layername: cstring): TLN_ObjectList {.
    importc: "TLN_LoadObjectList", header: "Tilengine.h".}
proc TLN_CloneObjectList*(src: TLN_ObjectList): TLN_ObjectList {.
    importc: "TLN_CloneObjectList", header: "Tilengine.h".}
proc TLN_GetListNumObjects*(list: TLN_ObjectList): cint {.
    importc: "TLN_GetListNumObjects", header: "Tilengine.h".}
proc TLN_GetListObject*(list: TLN_ObjectList; info: ptr TLN_ObjectInfo): bool {.
    importc: "TLN_GetListObject", header: "Tilengine.h".}
proc TLN_DeleteObjectList*(list: TLN_ObjectList): bool {.
    importc: "TLN_DeleteObjectList", header: "Tilengine.h".}
## *@}
## *
##  \defgroup layer
##  \brief Background layers management
##  @{

proc TLN_SetLayer*(nlayer: cint; tileset: TLN_Tileset; tilemap: TLN_Tilemap): bool {.
    importc: "TLN_SetLayer", header: "Tilengine.h".}
proc TLN_SetLayerTilemap*(nlayer: cint; tilemap: TLN_Tilemap): bool {.
    importc: "TLN_SetLayerTilemap", header: "Tilengine.h".}
proc TLN_SetLayerBitmap*(nlayer: cint; bitmap: TLN_Bitmap): bool {.
    importc: "TLN_SetLayerBitmap", header: "Tilengine.h".}
proc TLN_SetLayerPalette*(nlayer: cint; palette: TLN_Palette): bool {.
    importc: "TLN_SetLayerPalette", header: "Tilengine.h".}
proc TLN_SetLayerPosition*(nlayer: cint; hstart: cint; vstart: cint): bool {.
    importc: "TLN_SetLayerPosition", header: "Tilengine.h".}
proc TLN_SetLayerScaling*(nlayer: cint; xfactor: cfloat; yfactor: cfloat): bool {.
    importc: "TLN_SetLayerScaling", header: "Tilengine.h".}
proc TLN_SetLayerAffineTransform*(nlayer: cint; affine: ptr TLN_Affine): bool {.
    importc: "TLN_SetLayerAffineTransform", header: "Tilengine.h".}
proc TLN_SetLayerTransform*(layer: cint; angle: cfloat; dx: cfloat; dy: cfloat;
                           sx: cfloat; sy: cfloat): bool {.
    importc: "TLN_SetLayerTransform", header: "Tilengine.h".}
proc TLN_SetLayerPixelMapping*(nlayer: cint; table: ptr TLN_PixelMap): bool {.
    importc: "TLN_SetLayerPixelMapping", header: "Tilengine.h".}
proc TLN_SetLayerBlendMode*(nlayer: cint; mode: TLN_Blend; factor: uint8_t): bool {.
    importc: "TLN_SetLayerBlendMode", header: "Tilengine.h".}
proc TLN_SetLayerColumnOffset*(nlayer: cint; offset: ptr cint): bool {.
    importc: "TLN_SetLayerColumnOffset", header: "Tilengine.h".}
proc TLN_SetLayerClip*(nlayer: cint; x1: cint; y1: cint; x2: cint; y2: cint): bool {.
    importc: "TLN_SetLayerClip", header: "Tilengine.h".}
proc TLN_DisableLayerClip*(nlayer: cint): bool {.importc: "TLN_DisableLayerClip",
    header: "Tilengine.h".}
proc TLN_SetLayerMosaic*(nlayer: cint; width: cint; height: cint): bool {.
    importc: "TLN_SetLayerMosaic", header: "Tilengine.h".}
proc TLN_DisableLayerMosaic*(nlayer: cint): bool {.
    importc: "TLN_DisableLayerMosaic", header: "Tilengine.h".}
proc TLN_ResetLayerMode*(nlayer: cint): bool {.importc: "TLN_ResetLayerMode",
    header: "Tilengine.h".}
proc TLN_SetLayerObjects*(nlayer: cint; objects: TLN_ObjectList; tileset: TLN_Tileset): bool {.
    importc: "TLN_SetLayerObjects", header: "Tilengine.h".}
proc TLN_SetLayerPriority*(nlayer: cint; enable: bool): bool {.
    importc: "TLN_SetLayerPriority", header: "Tilengine.h".}
proc TLN_SetLayerParent*(nlayer: cint; parent: cint): bool {.
    importc: "TLN_SetLayerParent", header: "Tilengine.h".}
proc TLN_DisableLayerParent*(nlayer: cint): bool {.
    importc: "TLN_DisableLayerParent", header: "Tilengine.h".}
proc TLN_DisableLayer*(nlayer: cint): bool {.importc: "TLN_DisableLayer",
    header: "Tilengine.h".}
proc TLN_EnableLayer*(nlayer: cint): bool {.importc: "TLN_EnableLayer",
                                        header: "Tilengine.h".}
proc TLN_GetLayerType*(nlayer: cint): TLN_LayerType {.importc: "TLN_GetLayerType",
    header: "Tilengine.h".}
proc TLN_GetLayerPalette*(nlayer: cint): TLN_Palette {.
    importc: "TLN_GetLayerPalette", header: "Tilengine.h".}
proc TLN_GetLayerTileset*(nlayer: cint): TLN_Tileset {.
    importc: "TLN_GetLayerTileset", header: "Tilengine.h".}
proc TLN_GetLayerTilemap*(nlayer: cint): TLN_Tilemap {.
    importc: "TLN_GetLayerTilemap", header: "Tilengine.h".}
proc TLN_GetLayerBitmap*(nlayer: cint): TLN_Bitmap {.importc: "TLN_GetLayerBitmap",
    header: "Tilengine.h".}
proc TLN_GetLayerObjects*(nlayer: cint): TLN_ObjectList {.
    importc: "TLN_GetLayerObjects", header: "Tilengine.h".}
proc TLN_GetLayerTile*(nlayer: cint; x: cint; y: cint; info: ptr TLN_TileInfo): bool {.
    importc: "TLN_GetLayerTile", header: "Tilengine.h".}
proc TLN_GetLayerWidth*(nlayer: cint): cint {.importc: "TLN_GetLayerWidth",
    header: "Tilengine.h".}
proc TLN_GetLayerHeight*(nlayer: cint): cint {.importc: "TLN_GetLayerHeight",
    header: "Tilengine.h".}
## *@}
## *
##  \defgroup sprite
##  \brief Sprites management
##  @{

proc TLN_ConfigSprite*(nsprite: cint; spriteset: TLN_Spriteset; flags: uint32_t): bool {.
    importc: "TLN_ConfigSprite", header: "Tilengine.h".}
proc TLN_SetSpriteSet*(nsprite: cint; spriteset: TLN_Spriteset): bool {.
    importc: "TLN_SetSpriteSet", header: "Tilengine.h".}
proc TLN_SetSpriteFlags*(nsprite: cint; flags: uint32_t): bool {.
    importc: "TLN_SetSpriteFlags", header: "Tilengine.h".}
proc TLN_EnableSpriteFlag*(nsprite: cint; flag: uint32_t; enable: bool): bool {.
    importc: "TLN_EnableSpriteFlag", header: "Tilengine.h".}
proc TLN_SetSpritePivot*(nsprite: cint; px: cfloat; py: cfloat): bool {.
    importc: "TLN_SetSpritePivot", header: "Tilengine.h".}
proc TLN_SetSpritePosition*(nsprite: cint; x: cint; y: cint): bool {.
    importc: "TLN_SetSpritePosition", header: "Tilengine.h".}
proc TLN_SetSpritePicture*(nsprite: cint; entry: cint): bool {.
    importc: "TLN_SetSpritePicture", header: "Tilengine.h".}
proc TLN_SetSpritePalette*(nsprite: cint; palette: TLN_Palette): bool {.
    importc: "TLN_SetSpritePalette", header: "Tilengine.h".}
proc TLN_SetSpriteBlendMode*(nsprite: cint; mode: TLN_Blend; factor: uint8_t): bool {.
    importc: "TLN_SetSpriteBlendMode", header: "Tilengine.h".}
proc TLN_SetSpriteScaling*(nsprite: cint; sx: cfloat; sy: cfloat): bool {.
    importc: "TLN_SetSpriteScaling", header: "Tilengine.h".}
proc TLN_ResetSpriteScaling*(nsprite: cint): bool {.
    importc: "TLN_ResetSpriteScaling", header: "Tilengine.h".}
## TLNAPI bool TLN_SetSpriteRotation (int nsprite, float angle);
## TLNAPI bool TLN_ResetSpriteRotation (int nsprite);

proc TLN_GetSpritePicture*(nsprite: cint): cint {.importc: "TLN_GetSpritePicture",
    header: "Tilengine.h".}
proc TLN_GetAvailableSprite*(): cint {.importc: "TLN_GetAvailableSprite",
                                    header: "Tilengine.h".}
proc TLN_EnableSpriteCollision*(nsprite: cint; enable: bool): bool {.
    importc: "TLN_EnableSpriteCollision", header: "Tilengine.h".}
proc TLN_GetSpriteCollision*(nsprite: cint): bool {.
    importc: "TLN_GetSpriteCollision", header: "Tilengine.h".}
proc TLN_GetSpriteState*(nsprite: cint; state: ptr TLN_SpriteState): bool {.
    importc: "TLN_GetSpriteState", header: "Tilengine.h".}
proc TLN_SetFirstSprite*(nsprite: cint): bool {.importc: "TLN_SetFirstSprite",
    header: "Tilengine.h".}
proc TLN_SetNextSprite*(nsprite: cint; next: cint): bool {.
    importc: "TLN_SetNextSprite", header: "Tilengine.h".}
proc TLN_EnableSpriteMasking*(nsprite: cint; enable: bool): bool {.
    importc: "TLN_EnableSpriteMasking", header: "Tilengine.h".}
proc TLN_SetSpritesMaskRegion*(top_line: cint; bottom_line: cint) {.
    importc: "TLN_SetSpritesMaskRegion", header: "Tilengine.h".}
proc TLN_SetSpriteAnimation*(nsprite: cint; sequence: TLN_Sequence; loop: cint): bool {.
    importc: "TLN_SetSpriteAnimation", header: "Tilengine.h".}
proc TLN_DisableSpriteAnimation*(nsprite: cint): bool {.
    importc: "TLN_DisableSpriteAnimation", header: "Tilengine.h".}
proc TLN_DisableSprite*(nsprite: cint): bool {.importc: "TLN_DisableSprite",
    header: "Tilengine.h".}
proc TLN_GetSpritePalette*(nsprite: cint): TLN_Palette {.
    importc: "TLN_GetSpritePalette", header: "Tilengine.h".}
## *@}
## *
##  \defgroup sequence
##  \brief Sequence resources management for layer, sprite and palette animations
##  @{

proc TLN_CreateSequence*(name: cstring; target: cint; num_frames: cint;
                        frames: ptr TLN_SequenceFrame): TLN_Sequence {.
    importc: "TLN_CreateSequence", header: "Tilengine.h".}
proc TLN_CreateCycle*(name: cstring; num_strips: cint; strips: ptr TLN_ColorStrip): TLN_Sequence {.
    importc: "TLN_CreateCycle", header: "Tilengine.h".}
proc TLN_CreateSpriteSequence*(name: cstring; spriteset: TLN_Spriteset;
                              basename: cstring; delay: cint): TLN_Sequence {.
    importc: "TLN_CreateSpriteSequence", header: "Tilengine.h".}
proc TLN_CloneSequence*(src: TLN_Sequence): TLN_Sequence {.
    importc: "TLN_CloneSequence", header: "Tilengine.h".}
proc TLN_GetSequenceInfo*(sequence: TLN_Sequence; info: ptr TLN_SequenceInfo): bool {.
    importc: "TLN_GetSequenceInfo", header: "Tilengine.h".}
proc TLN_DeleteSequence*(sequence: TLN_Sequence): bool {.
    importc: "TLN_DeleteSequence", header: "Tilengine.h".}
## *@}
## *
##  \defgroup sequencepack
##  \brief Sequence pack manager for grouping and finding sequences
##  @{

proc TLN_CreateSequencePack*(): TLN_SequencePack {.
    importc: "TLN_CreateSequencePack", header: "Tilengine.h".}
proc TLN_LoadSequencePack*(filename: cstring): TLN_SequencePack {.
    importc: "TLN_LoadSequencePack", header: "Tilengine.h".}
proc TLN_GetSequence*(sp: TLN_SequencePack; index: cint): TLN_Sequence {.
    importc: "TLN_GetSequence", header: "Tilengine.h".}
proc TLN_FindSequence*(sp: TLN_SequencePack; name: cstring): TLN_Sequence {.
    importc: "TLN_FindSequence", header: "Tilengine.h".}
proc TLN_GetSequencePackCount*(sp: TLN_SequencePack): cint {.
    importc: "TLN_GetSequencePackCount", header: "Tilengine.h".}
proc TLN_AddSequenceToPack*(sp: TLN_SequencePack; sequence: TLN_Sequence): bool {.
    importc: "TLN_AddSequenceToPack", header: "Tilengine.h".}
proc TLN_DeleteSequencePack*(sp: TLN_SequencePack): bool {.
    importc: "TLN_DeleteSequencePack", header: "Tilengine.h".}
## *@}
## *
##  \defgroup animation
##  \brief Color cycle animation
##  @{

proc TLN_SetPaletteAnimation*(index: cint; palette: TLN_Palette;
                             sequence: TLN_Sequence; blend: bool): bool {.
    importc: "TLN_SetPaletteAnimation", header: "Tilengine.h".}
proc TLN_SetPaletteAnimationSource*(index: cint; a2: TLN_Palette): bool {.
    importc: "TLN_SetPaletteAnimationSource", header: "Tilengine.h".}
proc TLN_GetAnimationState*(index: cint): bool {.importc: "TLN_GetAnimationState",
    header: "Tilengine.h".}
proc TLN_SetAnimationDelay*(index: cint; frame: cint; delay: cint): bool {.
    importc: "TLN_SetAnimationDelay", header: "Tilengine.h".}
proc TLN_GetAvailableAnimation*(): cint {.importc: "TLN_GetAvailableAnimation",
                                       header: "Tilengine.h".}
proc TLN_DisablePaletteAnimation*(index: cint): bool {.
    importc: "TLN_DisablePaletteAnimation", header: "Tilengine.h".}
## *@}
## *
##  \defgroup world
##  \brief World management
##  @{

proc TLN_LoadWorld*(tmxfile: cstring; first_layer: cint): bool {.
    importc: "TLN_LoadWorld", header: "Tilengine.h".}
proc TLN_SetWorldPosition*(x: cint; y: cint) {.importc: "TLN_SetWorldPosition",
    header: "Tilengine.h".}
proc TLN_SetLayerParallaxFactor*(nlayer: cint; x: cfloat; y: cfloat): bool {.
    importc: "TLN_SetLayerParallaxFactor", header: "Tilengine.h".}
proc TLN_SetSpriteWorldPosition*(nsprite: cint; x: cint; y: cint): bool {.
    importc: "TLN_SetSpriteWorldPosition", header: "Tilengine.h".}
proc TLN_ReleaseWorld*() {.importc: "TLN_ReleaseWorld", header: "Tilengine.h".}
## *@}
