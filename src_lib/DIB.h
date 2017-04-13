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
	uint16_t	Type;
	uint32_t	Size;
	uint32_t	Reserved;
	uint32_t	OffsetData;
}
BITMAPFILEHEADER;

typedef struct
{ 
	uint32_t        bV5Size; 
	uint32_t        bV5Width; 
	uint32_t        bV5Height; 
	uint16_t         bV5Planes; 
	uint16_t         bV5BitCount; 
	uint32_t        bV5Compression; 
	uint32_t        bV5SizeImage; 
	uint32_t        bV5XPelsPerMeter; 
	uint32_t        bV5YPelsPerMeter; 
	uint32_t        bV5ClrUsed; 
	uint32_t        bV5ClrImportant; 
	uint32_t        bV5RedMask; 
	uint32_t        bV5GreenMask; 
	uint32_t        bV5BlueMask; 
	uint32_t        bV5AlphaMask; 
}
BITMAPV5HEADER;

#pragma pack(pop)

#endif
