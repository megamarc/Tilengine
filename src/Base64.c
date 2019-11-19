#include "Base64.h"

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

static const unsigned char d[] =
{
	66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
	54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
	29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66
};

int base64decode (const unsigned char* in, int inLen, unsigned char *out, int *outLen)
{
	const unsigned char* end = in + inLen;
	int buf = 1, len = 0;

	while (*in < 'A')
		in++;

	while (in < end)
	{
		unsigned char c = d[*in++];

		switch (c)
		{
		case WHITESPACE:
			continue;   /* skip whitespace */
		case INVALID:
			return 1;   /* invalid input, return error */
		case EQUALS:                 /* pad character, end of data */
			in = end;
			continue;
		default:
			buf = buf << 6 | c;

			/* If the buffer is full, split it into bytes */
			if (buf & 0x1000000)
			{
				if ((len += 3) > *outLen) return 1; /* buffer overflow */
				*out++ = (unsigned char)(buf >> 16);
				*out++ = (unsigned char)(buf >> 8);
				*out++ = (unsigned char)(buf);
				buf = 1;
			}
		}
	}

	if (buf & 0x40000)
	{
		if ((len += 2) > *outLen) return 1; /* buffer overflow */
		*out++ = (unsigned char)(buf >> 10);
		*out++ = (unsigned char)(buf >> 2);
	}
	else if (buf & 0x1000)
	{
		if (++len > *outLen) return 1; /* buffer overflow */
		*out++ = (unsigned char)(buf >> 4);
	}

	*outLen = len; /* modify to reflect the actual output size */
	return 0;
}
