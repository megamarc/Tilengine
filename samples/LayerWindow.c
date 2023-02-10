/******************************************************************************
*
* Tilengine sample
* 2023 Marc Palacios
* http://www.tilengine.org
*
* This example showcases layer window feature introduced in Tilengine 2.14
* - button 1 / Z : toggles window inversion
* - button 2 / X : toggles color processing
* - button 3 / C : toggles color blending
* - d-pad / curs : moves the window
*
******************************************************************************/

#include "Tilengine.h"

#define HRES		640
#define VRES		360

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
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
int window_x = 64;
int window_y = 48;
int window_width = HRES - 128;
int window_height = VRES - 96;

/* forward declarations */
TLN_Input get_press(void);
void update_window(void);

/* entry point */
int main(int arg, char* argv[])
{
	/* init & load assets */ 
	TLN_Init(HRES, VRES, NUM_LAYERS, 0, 0);
	TLN_SetLoadPath("assets/shots");
	TLN_SetLayerBitmap(LAYER_FOREGROUND, TLN_LoadBitmap("zss1.png"));
	TLN_SetLayerBitmap(LAYER_BACKGROUND, TLN_LoadBitmap("zss2.png"));
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
			window_x -= 2;
		else if (TLN_GetInput(INPUT_RIGHT))
			window_x += 2;
		if (TLN_GetInput(INPUT_UP))
			window_y -= 2;
		else if (TLN_GetInput(INPUT_DOWN))
			window_y += 2;
		update_window();

		TLN_DrawFrame(0);
	}

	/* release resources */
	TLN_ReleaseWorld();
	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}

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

/* updates layer window properties */
void update_window(void)
{
	TLN_SetLayerWindow(LAYER_FOREGROUND, window_x, window_y, window_x + window_width, window_y + window_height, state.invert);
	if (state.color)
		TLN_SetLayerWindowColor(LAYER_FOREGROUND, 0, 128, 224, state.blend? BLEND_MIX : BLEND_NONE);
	else
		TLN_DisableLayerWindowColor(LAYER_FOREGROUND);
}
