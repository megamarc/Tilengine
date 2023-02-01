/******************************************************************************
*
* Tilengine sample
* 2023 Marc Palacios
* http://www.tilengine.org
*
* This example showcases layer window feature introduced in Tilengine 2.14
* - button 1 : toggles window inversion
* - button 2 : toggles color processing
* - button 3 : toggles color blending
* - d-pad    : moves the window
*
******************************************************************************/

#include "Tilengine.h"

#define HRES	424
#define VRES	240

/* layers, must mach "map.tmx" layer structure! */
enum
{
	LAYER_PROPS,		/* object layer */
	LAYER_FOREGROUND,	/* main foreground layer (tiles) */
	LAYER_MIDDLEGROUND,	/* middle (bitmap) */
	LAYER_BACKGROUND,	/* back ( bitmap) */
	NUM_LAYERS
};

/* holds current combination of window modes */
struct
{
	bool color : 1;
	bool blend : 1;
	bool invert : 1;
}
state;

/* sliding window */
int window_x = 48;
int window_y = 36;
int window_width = HRES - 96;
int window_height = VRES - 72;

/* returns button press event */
TLN_Input get_press(void)
{
	static TLN_Input last = INPUT_NONE;
	TLN_Input input = INPUT_NONE;

	if (TLN_GetInput(INPUT_BUTTON1))
		input = INPUT_BUTTON1;
	else if (TLN_GetInput(INPUT_BUTTON2))
		input = INPUT_BUTTON2;
	else if (TLN_GetInput(INPUT_BUTTON3))
		input = INPUT_BUTTON3;

	if (input != last)
	{
		last = input;
		return input;
	}
	return INPUT_NONE;
}

void update_window(void)
{
	TLN_SetLayerWindow(LAYER_FOREGROUND, window_x, window_y, window_x + window_width, window_y + window_height, state.invert);
	if (state.color)
		TLN_SetLayerWindowColor(LAYER_FOREGROUND, 0, 128, 224, state.blend? BLEND_MIX : BLEND_NONE);
	else
		TLN_DisableLayerWindowColor(LAYER_FOREGROUND);
}

int main(int arg, char* argv[])
{
	/* load world */ 
	TLN_Init(HRES, VRES, NUM_LAYERS, 8, 0);
	TLN_SetLoadPath("assets/forest");
	TLN_LoadWorld("map.tmx", 0);
	TLN_SetWorldPosition(760,0);
	update_window();

	/* create window & main loop */
	TLN_CreateWindow(NULL, 0);
	while (TLN_ProcessWindow())
	{
		TLN_Input input = get_press();
		if (input == INPUT_BUTTON1)
			state.invert ^= 1;
		else if (input == INPUT_BUTTON2)
			state.color ^= 1;
		else if (input == INPUT_BUTTON3)
			state.blend ^= 1;

		/* move window with d-pad */
		if (TLN_GetInput(INPUT_LEFT))
			window_x -= 1;
		else if (TLN_GetInput(INPUT_RIGHT))
			window_x += 1;
		if (TLN_GetInput(INPUT_UP))
			window_y -= 1;
		else if (TLN_GetInput(INPUT_DOWN))
			window_y += 1;
		update_window();

		TLN_DrawFrame(0);
	}

	/* release resources */
	TLN_ReleaseWorld();
	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
