#include <stdarg.h>
#include "Tilengine.h"
#include "Engine.h"
#include "Tileset.h"

/*! */
void TLN_DefineFont(TLN_Font* font, TLN_Tileset tileset, int firstgid, char firstchar, int charheight)
{
	if (font)
	{
		font->firstgid = firstgid;
		font->firstchar = firstchar;
		font->glyph_height = charheight;
		font->glyphs_row = tileset->tiles_per_row;
	}
}

/*! */
bool TLN_WriteText(int nlayer, TLN_Font* font, int row, int col, const char* format, ...)
{
	char buffer[1000];
	char* current;
	Tile tile = { 0 };
	TLN_Tilemap tilemap = engine->layers[nlayer].tilemap;
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	
	current = buffer;
	while (*current)
	{
		const int index = *current - font->firstchar;
		const int glyph_row = index / font->glyphs_row;
		const int glyph_col = index % font->glyphs_row;

		tile.index = font->firstgid + index + (glyph_row * font->glyphs_row) + glyph_col;
		TLN_SetTilemapTile(tilemap, row, col, &tile);
		if (font->glyph_height == 2)
		{
			tile.index += font->glyphs_row;
			TLN_SetTilemapTile(tilemap, row + 1, col, &tile);
		}
		current += 1;
	}
	TLN_SetLastError(TLN_ERR_OK);
	return true;
}
