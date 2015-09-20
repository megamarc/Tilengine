#ifndef _DIB_H
#define _DIB_H

#pragma pack(push)
#pragma pack (2)

typedef struct
{
	BYTE b,g,r;
}
RGBTRIPLE;

typedef struct
{
	BYTE b,g,r,a;
}
RGBQUAD;

typedef struct
{
	WORD	Type;
	DWORD	Size;
	DWORD	Reserved;
	DWORD	OffsetData;
}
BITMAPFILEHEADER;

typedef struct
{ 
	DWORD        bV5Size; 
	DWORD        bV5Width; 
	DWORD        bV5Height; 
	WORD         bV5Planes; 
	WORD         bV5BitCount; 
	DWORD        bV5Compression; 
	DWORD        bV5SizeImage; 
	DWORD        bV5XPelsPerMeter; 
	DWORD        bV5YPelsPerMeter; 
	DWORD        bV5ClrUsed; 
	DWORD        bV5ClrImportant; 
	DWORD        bV5RedMask; 
	DWORD        bV5GreenMask; 
	DWORD        bV5BlueMask; 
	DWORD        bV5AlphaMask; 
}
BITMAPV5HEADER;

#pragma pack(pop)

#endif
