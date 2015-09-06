#include <stdio.h>
#include <string.h>
#include "Tilengine.h"

#define MAX_SCENE	35
#define RES_PATH	"cycles\\"

const char* const files[] =
{
	"CORAL",
	"V01",
	"V02",
	"V03",
	"V04",
	"V05HAUNT",
	"V05RAIN",
	"V07",
	"V08",
	"V08AM",
	"V08PM",
	"V08RAIN",
	"V09",
	"V10",
	"V11AM",
	"V13",
	"V14",
	"V15",
	"V16",
	"V16PM",
	"V16RAIN",
	"V17",
	"V19",
	"V19AURA",
	"V19PM",
	"V20",
	"V25HEAT",
	"V26SNOW",
	"V27",
	"V28",
	"V29",
	"V29FOG",
	"V29PM",
	"V30",
	"V30RAIN",
};

TLN_Bitmap background;
TLN_SequencePack sp;

int frame = 0;
int pos = 0;
bool inc = false;
bool dec = false;

static void SetScene (int pos)
{
	char filename[32];

	if (background)
		TLN_DeleteBitmap (background);
	sprintf (filename, RES_PATH "%s.PNG", files[pos]);
	background = TLN_LoadBitmap (filename);
	TLN_SetBGBitmap (background);

	if (sp)
		TLN_DeleteSequencePack (sp);
	sprintf (filename, RES_PATH "%s.SQX", files[pos]);
	sp = TLN_LoadSequencePack (filename);
	TLN_DisableAnimation (0);
	TLN_SetPaletteAnimation (0, TLN_GetBitmapPalette (background), TLN_FindSequence (sp, files[pos]), true);
}

int main (int argc, char* argv[])
{
	TLN_Init (640,480,0,0,1);
	TLN_CreateWindow (NULL, CWF_VSYNC);
	SetScene (pos);

	while (TLN_ProcessWindow ())
	{
		if (TLN_GetInput (INPUT_LEFT))
		{
			if (dec == false)
			{
				dec = true;
				pos = (pos - 1 + MAX_SCENE) % MAX_SCENE;
				SetScene (pos);
			}
		}
		else
			dec = false;

		if (TLN_GetInput (INPUT_RIGHT))
		{
			if (inc == false)
			{
				inc = true;
				pos = (pos + 1) % MAX_SCENE;
				SetScene (pos);
			}
		}
		else
			inc = false;

		TLN_DrawFrame (frame);
		frame++;
	}

	TLN_DeleteBitmap (background);
	TLN_DeleteSequencePack (sp);
	TLN_DeleteWindow ();
	TLN_Deinit ();
	return 0;
}
