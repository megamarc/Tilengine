#include "Tilengine.h"

#define WIDTH	398
#define HEIGHT	224

typedef enum
{
	MODE_BASIC,
	MODE_SCALING,
	MODE_AFFINE,
	MODE_PIXELMAP,
	MAX_MODE
}
Mode;

/* mode */
typedef union
{
	struct
	{
		bool invert : 1;
		bool blend : 1;
		bool mosaic : 1;
		uint8_t mode : 2;
	};
	uint8_t value;
}
State;

TLN_PixelMap pixelmap[WIDTH * HEIGHT];

/* set draw state */
static void set_state(State state)
{
	char* mode_names[] = { "basic", "scaling", "affine", "pixel" };

	/* window invert */
	TLN_SetLayerWindow(0, 30, 30, WIDTH - 30, HEIGHT - 30, state.invert);

	/* window color mode */
	if (state.blend)
		TLN_SetLayerWindowColor(0, 0, 128, 0, BLEND_ADD);
	else
		TLN_DisableLayerWindowColor(0);

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
		TLN_SetLayerTransform(0, 30.0f, 0, 0, 1.0f, 1.0f);
		break;

	case MODE_PIXELMAP:
		TLN_SetLayerPixelMapping(0, pixelmap);
		break;
	}

	printf("%02d: %s ", state.value, mode_names[state.mode]);
	if (state.mosaic)
		printf("mosaic ");
	if (state.blend)
		printf("blend ");
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

	/* build pixelmap */
	for (y = 0; y < HEIGHT; y += 1)
	{
		for (x = 0; x < WIDTH; x += 1)
		{
			pixelmap[x*y].dx = WIDTH - x - 1;
			pixelmap[x*y].dy = HEIGHT - y - 1;
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
			if (state.value >= 32)
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
	}

	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
