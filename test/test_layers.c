#include "Tilengine.h"

void main(void) {
	TLN_Tilemap tilemap_fg;
	TLN_Tilemap tilemap_bg;

	TLN_Init(400, 240, 4, 0, 0);

	TLN_SetLoadPath("assets/sonic");
	tilemap_bg = TLN_LoadTilemap("Sonic_md_full.tmx", "BG");
	tilemap_fg = TLN_LoadTilemap("Sonic_md_full.tmx", "FG");
	printf(TLN_GetErrorString(TLN_GetLastError()));

	TLN_SetLayerTilemap(0, tilemap_fg);
	TLN_SetLayerTilemap(1, tilemap_bg);

	TLN_CreateWindow(NULL, CWF_VSYNC);
	TLN_SetBGColor(36, 73, 219);

	while (TLN_ProcessWindow()) {
		TLN_DrawFrame(0);
	}

	TLN_DeleteTilemap(tilemap_fg);
	TLN_DeleteTilemap(tilemap_bg);
	TLN_Deinit();
}