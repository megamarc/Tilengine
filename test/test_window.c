#include <math.h>
#include "Tilengine.h"

#define WIDTH	(29*16)
#define HEIGHT	(16*16)

enum
{
	MODE_BASIC,
	MODE_SCALING,
	MODE_AFFINE,
	MODE_PIXELMAP,
	MAX_MODE
};

enum
{
	WINDOW_NONE,
	WINDOW_CLIP,
	WINDOW_COLOR,
	WINDOW_BLEND,
};

/* mode */
typedef union
{
	struct
	{
		uint8_t window : 2;
		bool invert : 1;
		bool mosaic : 1;
		uint8_t mode : 2;
	};
	uint8_t value;
}
State;

TLN_PixelMap pixelmap[HEIGHT][WIDTH] = { 0 };
float angle = 0.0f;

/* set draw state */
static void set_state(State state)
{
	char* mode_names[] = { "basic", "scaling", "affine", "pixel" };
	char* window_names[] = { "no_window", "clip", "color", "blend" };

	/* window type */
	switch (state.window)
	{
	case WINDOW_NONE:
		TLN_DisableLayerWindow(0);
		TLN_DisableLayerWindowColor(0);
		break;

	case WINDOW_CLIP:
		TLN_DisableLayerWindowColor(0);
		TLN_SetLayerWindow(0, 32, 32, WIDTH - 32, HEIGHT - 32, state.invert);
		break;

	case WINDOW_COLOR:
		TLN_SetLayerWindowColor(0, 0, 128, 0, BLEND_NONE);
		break;

	case WINDOW_BLEND:
		TLN_SetLayerWindow(0, 32, 32, WIDTH - 32, HEIGHT - 32, state.invert);
		TLN_SetLayerWindowColor(0, 0, 128, 0, BLEND_MIX);
		break;
	}

	/* mosaic */
	if (state.mosaic)
		TLN_SetLayerMosaic(0, 4, 4);
	else
		TLN_DisableLayerMosaic(0);

	/* mode */
	TLN_ResetLayerMode(0);
	switch (state.mode)
	{
	case MODE_BASIC:
		break;

	case MODE_SCALING:
		TLN_SetLayerScaling(0, 2.0f, 2.0f);
		break;

	case MODE_AFFINE:
		break;

	case MODE_PIXELMAP:
		TLN_SetLayerPixelMapping(0, (TLN_PixelMap*)pixelmap);
		break;
	}

	printf("%02d: %s %s ", state.value, mode_names[state.mode], window_names[state.window]);
	if (state.mosaic)
		printf("mosaic ");
	if (state.invert)
		printf("invert");
	printf("\n");
}

int main(int argc, char* argv[])
{
	int x, y;
	State state;
	state.value = 0;

	TLN_Init(WIDTH, HEIGHT, 2, 4, 0);
	TLN_SetLoadPath("assets/cliffs");
	TLN_SetBGColor(0x88, 0xDA, 0xF2);

	TLN_Tilemap layer_foreground = TLN_LoadTilemap("layer_foreground.tmx", NULL);
	TLN_Tilemap layer_background = TLN_LoadTilemap("layer_background.tmx", NULL);
	TLN_SetLayer(0, NULL, layer_foreground);
	TLN_SetLayer(1, NULL, layer_background);

	/* build sine wave distortion pixelmap */
	for (y = 0; y < HEIGHT; y += 1)
	{
		for (x = 0; x < WIDTH; x += 1)
		{
			pixelmap[y][x].dx = (int16_t)(x + sin(y / 2) * 3);
			pixelmap[y][x].dy = (int16_t)(y + cos(x / 2) * 3);
		}
	}

	bool pushed = false;
	TLN_CreateWindow(NULL, 0);
	set_state(state);
	while (TLN_ProcessWindow())
	{
		TLN_DrawFrame(0);
		if (!pushed && TLN_GetInput(INPUT_RIGHT))
		{
			pushed = true;
			state.value += 1;
			if (state.value >= 64)
				state.value = 0;
			set_state(state);
		}
		if (!pushed && TLN_GetInput(INPUT_LEFT))
		{
			pushed = true;
			state.value -= 1;
			if (state.value < 0)
				state.value = 0;
			set_state(state);
		}
		if (!TLN_GetInput(INPUT_RIGHT) && !TLN_GetInput(INPUT_LEFT))
			pushed = false;

		angle += 0.2f;
		if (state.mode == MODE_AFFINE)
			TLN_SetLayerTransform(0, angle, WIDTH / 2, HEIGHT / 2, 1.0f, 1.0f);
	}

	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
