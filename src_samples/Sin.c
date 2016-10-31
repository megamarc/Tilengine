#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265f
#endif

static int sintable[360];
static int costable[360];

void BuildSinTable (void)
{
	int c;
	for (c=0; c<360; c++)
	{
		sintable[c] = (int)(sin(c*M_PI/180)*256);
		costable[c] = (int)(cos(c*M_PI/180)*256);
	}
}

int CalcSin (int angle, int factor)
{
	int val;

	if (angle > 359)
		angle = angle%360;

	val = (sintable[angle]*factor) >> 8;

	return val;
}

int CalcCos (int angle, int factor)
{
	int val;

	if (angle > 359)
		angle = angle%360;

	val = (costable[angle]*factor) >> 8;

	return val;
}
