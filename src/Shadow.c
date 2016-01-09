#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

/* linear interploation */
#define lerp(x, x0,x1, fx0,fx1) \
	fx0 + (fx1-fx0)*(x-x0)/(x1-x0)

typedef struct
{
	int r,g,b;
}
RGB;

const RGB sky[4] = 
{
	{0x1D, 0x44, 0x7B},
	{0x7F, 0xA4, 0xD9},
	{0x0B, 0x00, 0x4E},
	{0xEB, 0x99, 0x9D},
};

RGB sky_hi;
RGB sky_lo;

static int frame;
static int xpos;
static int speed = 2;

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

typedef struct
{
	TLN_Tilemap tilemap;
	TLN_Tileset tileset;
}
layer_t;

layer_t layers[MAX_LAYER];
TLN_Spriteset spriteset;
TLN_SequencePack sp;
TLN_Sequence walk;

static void raster_callback (int line);

/* helper for loading a related tileset + tilemap and configure the appropiate layer */
static void LoadLayer (int index, char* name)
{
	char filename[64];
	layer_t* layer = &layers[index];

	/* load tileset */
	sprintf (filename, "%s.tsx", name);
	layer->tileset = TLN_LoadTileset (filename);

	/* load tilemap */
	sprintf (filename, "%s.tmx", name);
	layer->tilemap = TLN_LoadTilemap (filename, "Layer 1");

	TLN_SetLayer (index, layer->tileset, layer->tilemap);
}

/* helper for freeing a tileset + tilemap */ 
static void FreeLayer (int index)
{
	layer_t* layer = &layers[index];
	
	TLN_DeleteTileset (layer->tileset);
	TLN_DeleteTilemap (layer->tilemap);
}

/* entry point */
int main (int argc, char* argv[])
{
	int c;

	/* setup engine */
	TLN_Init (WIDTH, HEIGHT, 2,1,1);
	TLN_SetBGColor (0,128,238);
	TLN_CreateWindow ("overlay.bmp", CWF_VSYNC);

	/* load layer (tileset + tilemap) */
	LoadLayer (LAYER_FOREGROUND, "SOTB_fg");
	LoadLayer (LAYER_BACKGROUND, "SOTB_bg");
	TLN_SetRasterCallback (raster_callback);

	spriteset = TLN_LoadSpriteset ("SOTB");
	sp = TLN_LoadSequencePack ("SOTB.sqx");
	walk = TLN_FindSequence (sp, "walk");

	TLN_ConfigSprite (0, spriteset, 0);
	TLN_SetSpritePosition (0, 200,160);
	TLN_SetSpriteAnimation (0, 0, walk, 0);
	
	xpos = 2000;

	sky_hi.r = sky[0].r;
	sky_hi.g = sky[0].g;
	sky_hi.b = sky[0].b;
	sky_lo.r = sky[1].r;
	sky_lo.g = sky[1].g;
	sky_lo.b = sky[1].b;

	/* main loop */
	while (TLN_ProcessWindow ())
	{
		/* input */
		if (TLN_GetInput (INPUT_LEFT) && xpos > 0)
			xpos -= speed;
		else if (TLN_GetInput (INPUT_RIGHT) && xpos < 4720)
			xpos += speed;

		/* sky gradient */
		if (frame>=300 && frame<=900)
		{
			/* interpolate upper color */
			sky_hi.r = lerp (frame, 300,900, sky[0].r, sky[2].r);
			sky_hi.g = lerp (frame, 300,900, sky[0].g, sky[2].g);
			sky_hi.b = lerp (frame, 300,900, sky[0].b, sky[2].b);

			/* interpolate lower color */
			sky_lo.r = lerp (frame, 300,900, sky[1].r, sky[3].r);
			sky_lo.g = lerp (frame, 300,900, sky[1].g, sky[3].g);
			sky_lo.b = lerp (frame, 300,900, sky[1].b, sky[3].b);
		}

		TLN_SetLayerPosition (LAYER_FOREGROUND, xpos, 0);

		/* render to the window */
		TLN_DrawFrame (frame);
		frame++;
	}

	/* release resources */
	FreeLayer (LAYER_FOREGROUND);
	FreeLayer (LAYER_BACKGROUND);
	TLN_DeleteWindow ();
	TLN_Deinit ();

	return 0;
}

static void raster_callback (int line)
{
	int pos;

	/* sky color */
	if (line < 192)
	{
		RGB color;

		/* interpolate between upper and lower color */
		color.r = lerp (line, 0,191, sky_hi.r, sky_lo.r);
		color.g = lerp (line, 0,191, sky_hi.g, sky_lo.g);
		color.b = lerp (line, 0,191, sky_hi.b, sky_lo.b);
		TLN_SetBGColor (color.r, color.g ,color.b);
	}

	/* background layer */
	pos = -1;
	if (line==0 || line==24 || line==64 || line==88 || line==96)
		pos = (int)lerp (line, 0,96, xpos*0.7f, xpos*0.2);
	else if (line==120)
		pos = xpos/2;
	else if (line==208 || line==216 || line==224 || line==232)
		pos = (int)lerp (line, 208,232, xpos*1.0f, xpos*2.0);

	if (pos != -1)
		TLN_SetLayerPosition (LAYER_BACKGROUND, pos, 0);

	/* foreground layer */
	pos = -1;
	if (line==0)
		pos = xpos;
	else if (line==216)
		pos = xpos*3;
	if (pos != -1)
		TLN_SetLayerPosition (LAYER_FOREGROUND, pos, 0);
}