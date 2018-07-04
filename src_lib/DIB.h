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

#ifndef _DIB_H
#define _DIB_H

#pragma pack(push)
#pragma pack (2)

typedef struct
{
	uint8_t b,g,r;
}
RGBTRIPLE;

typedef struct
{
	uint8_t b,g,r,a;
}
RGBQUAD;

typedef struct
{
	uint16_t Type;
	uint32_t Size;
	uint32_t Reserved;
	uint32_t OffsetData;
}
BITMAPFILEHEADER;

typedef struct
{ 
	uint32_t bV5Size; 
	uint32_t bV5Width; 
	uint32_t bV5Height; 
	uint16_t bV5Planes; 
	uint16_t bV5BitCount; 
	uint32_t bV5Compression; 
	uint32_t bV5SizeImage; 
	uint32_t bV5XPelsPerMeter; 
	uint32_t bV5YPelsPerMeter; 
	uint32_t bV5ClrUsed; 
	uint32_t bV5ClrImportant; 
	uint32_t bV5RedMask; 
	uint32_t bV5GreenMask; 
	uint32_t bV5BlueMask; 
	uint32_t bV5AlphaMask; 
}
BITMAPV5HEADER;

#pragma pack(pop)

#endif
