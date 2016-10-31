#include <stdio.h>
#include <string.h>
#include "SeizeTheDay.h"
#include "Timelines.h"

#define MAX_SCENE	21
#define MAX_PALETTE	50
#define MAX_TIME	(120*60)

#ifdef WIN32
	#define RES_PATH	"cycles\\"
#else
	#define RES_PATH	"cycles/"
#endif

const Scene scenes[] =
{
	{"V26",		"V26",		tl_V26},
	{"V26SNOW", "V26SNOW",	tl_V26SNOW},
	{"V19",		"V19",		tl_V19},
	{"V19",		"V19_2",	tl_V19_2},
	{"V30",		"V30",		tl_V30},
	{"V30RAIN", "V30RAIN",	tl_V30RAIN},
	{"V08",		"V08",		tl_V08},
	{"V08",		"V08_2",	tl_V08_2},
	{"V08RAIN", "V08RAIN",	tl_V08RAIN},
	{"V20JOE",	"V20JOE",	tl_V20JOE},
	{"V25",		"V25",		tl_V25},
	{"V25",		"V25_2",	tl_V25_2},
	{"CORAL",	"CORAL",	tl_CORAL},
	{"V29",		"V29",		tl_V29},
	{"V29",		"V29_2",	tl_V29_2},
	{"V05AM",	"V05AM",	tl_V05AM},
	{"V05AM",	"V05AM_2",	tl_V05AM_2},
	{"V05RAIN", "V05RAIN",	tl_V05RAIN},
	{"V16",		"V16",		tl_V16},
	{"V16RAIN", "V16RAIN",	tl_V16RAIN},
	{"V12BASIC","V12BASIC",	tl_V12BASIC}
};

struct
{
	TLN_Palette palette;
	TLN_Sequence sequence;
	int time;
}
palettes[MAX_PALETTE];

TLN_Bitmap background;
TLN_SequencePack sp;
TLN_Palette palette, palette_int;

int frame = 0;
int pos = 0;
int pal = 0;
int maxpal = 0;
bool inc = false;
bool dec = false;

static int FindTimeIndex (int time)
{
	int c;

	if (time < palettes[0].time)
		return maxpal - 1;

	for (c=0; c<maxpal - 1; c++)
	{
		if (time >= palettes[c].time && time < palettes[c + 1].time)
			return c;
	}
	return maxpal - 1;
}

static void SetPalette (int pal)
{
	TLN_SetPaletteAnimation (0, palette, palettes[0/*pal*/].sequence, true);
	TLN_SetPaletteAnimationSource (0, palette_int);
}

static void SetScene (int pos)
{
	char filename[32];
	char name[32];
	Timeline* timeline;
	maxpal = 0;

	if (background)
		TLN_DeleteBitmap (background);
	sprintf (filename, "%s%s.PNG", RES_PATH, scenes[pos].bitmap);
	background = TLN_LoadBitmap (filename);
	TLN_SetBGBitmap (background);
	TLN_SetBGPalette (palette);

	if (sp)
		TLN_DeleteSequencePack (sp);
	sprintf (filename, "%s%s.SQX", RES_PATH, scenes[pos].sequence);
	sp = TLN_LoadSequencePack (filename);

	maxpal = 0;
	timeline = scenes[pos].timeline;
	while (timeline->palette)
	{
		TLN_Palette palette;

		if (palettes[maxpal].palette)
			TLN_DeletePalette (palettes[maxpal].palette);
		strcpy (name, timeline->palette);
		sprintf(filename, "%s%s.ACT", RES_PATH, name);
		palettes[maxpal].palette = TLN_LoadPalette (filename);
		palettes[maxpal].sequence = TLN_FindSequence (sp, name);
		palettes[maxpal].time = timeline->seconds*MAX_TIME/86400;

		/* remap */
		palette = palettes[maxpal].palette;
		if (pos == 12)
			TLN_SetPaletteColor (palette, 0, 0,0,0);
		else if (pos == 13 || pos == 14)
			TLN_SetPaletteColor (palette, 252, 11,11,11);
		else if (pos == 15 || pos == 16 || pos == 17)
		{
			TLN_SetPaletteColor (palette, 0, 11,11,11);
			TLN_SetPaletteColor (palette, 254, 0,0,0);
		}

		maxpal++;
		timeline++;
	}

	frame = 18000*MAX_TIME/86400;;
	pal = 0;
	TLN_DisableAnimation (0);
}

int main (int argc, char* argv[])
{
	TLN_Init (640,480,0,0,1);
	TLN_CreateWindow (NULL, CWF_VSYNC);
	TLN_EnableBlur (true);

	palette = TLN_CreatePalette (256);
	palette_int = TLN_CreatePalette (256);
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

		/* update palette */
		if (!(frame & 0x01))
		{
			TLN_Palette pal1, pal2;
			int t = frame % MAX_TIME;
			int idx1 = FindTimeIndex (t);
			int idx2 = (idx1 + 1) % maxpal;
			int t1 = palettes[idx1].time;
			int t2 = palettes[idx2].time;

			if (t1 > t2)
			{
				t2 += MAX_TIME;
				if (t < t1)
					t += MAX_TIME;
			}

			t -= t1;
			t2 -= t1;

			pal1 = palettes[idx1].palette;
			pal2 = palettes[idx2].palette;
			TLN_MixPalettes (pal1, pal2, palette_int, t*255/t2); /* rescale t2 to 255 */
			SetPalette (idx1);
		}

		TLN_DrawFrame (frame);
		
		frame++;
	}

	TLN_DeleteBitmap (background);
	TLN_DeletePalette (palette);
	TLN_DeletePalette (palette_int);
	TLN_DeleteWindow ();
	TLN_Deinit ();
	return 0;
}
