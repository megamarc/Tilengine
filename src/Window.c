/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2019 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
* */

#ifndef TLN_EXCLUDE_WINDOW
#define MAX_PLAYERS	4		/* number of unique players */
#define MAX_INPUTS	32		/* number of inputs per player */
#define INPUT_MASK	(MAX_INPUTS - 1)

#include <string.h>
#include "SDL2/SDL.h"
#include "Tilengine.h"
#include "crt.h"

static SDL_Window*   window;
static SDL_Renderer* renderer;
static SDL_Texture*	 backbuffer;
static SDL_Thread*   thread;
static SDL_mutex*	 lock;
static SDL_cond*	 cond;
static SDL_Joystick* joy;
static CRTHandler	 crt;
static SDL_Rect		 dstrect;

static bool			 init;
static bool			 done;
static int			 wnd_width;
static int			 wnd_height;
static int			 instances = 0;
static uint8_t*		 rt_pixels;
static int			 rt_pitch;
static char*		 window_title;

static int			last_key;
static TLN_SDLCallback sdl_callback = NULL;

/* player input */
typedef struct
{
	bool enabled;
	uint8_t joystick_id;
	SDL_Joystick* joy;
	SDL_Keycode keycodes[MAX_INPUTS];
	uint8_t joybuttons[MAX_INPUTS];
	uint32_t inputs;
}
PlayerInput;

static PlayerInput player_inputs[MAX_PLAYERS];

struct
{
	CRTType type;
	bool blur;
	bool enable;
}
static crt_params = { CRT_SHADOW, false, true };

#define MAX_PATH	260

/* Window manager */
typedef struct
{
	int width;
	int height;
	int flags;
	volatile int retval;
}
WndParams;

static WndParams wnd_params;

/* local prototypes */
static bool CreateWindow (void);
static void DeleteWindow (void);

#ifndef _MSC_VER
extern char* strdup(const char* s);
#endif

static void SetupBackBuffer(void)
{
	/* create framebuffer texture */
	if (backbuffer != NULL)
		SDL_DestroyTexture(backbuffer);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, crt_params.enable ? "1" : "0");
	backbuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, wnd_params.width, wnd_params.height);
}

/* create window delegate */
static bool CreateWindow(void)
{
	SDL_DisplayMode mode;
	SDL_Surface* surface = NULL;
	int factor;
	int rflags;
	char quality[2] = { 0 };
	Uint32 format = 0;

	/*  gets desktop size and maximum window size */
	SDL_GetDesktopDisplayMode(0, &mode);
	if (!(wnd_params.flags & CWF_FULLSCREEN))
	{
		rflags = 0;
		factor = (wnd_params.flags >> 2) & 0x07;
		if (!factor)
		{
			factor = 1;
			while (wnd_params.width*(factor + 1) < mode.w && wnd_params.height*(factor + 1) < mode.h && factor < 3)
				factor++;
		}

		wnd_width = wnd_params.width*factor;
		wnd_height = wnd_params.height*factor;

		dstrect.x = 0;
		dstrect.y = 0;
		dstrect.w = wnd_width;
		dstrect.h = wnd_height;
	}
	else
	{
		rflags = SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS;
#if SDL_VERSION_ATLEAST(2,0,5)
		rflags |= SDL_WINDOW_ALWAYS_ON_TOP;
#endif
		wnd_width = mode.w;
		wnd_height = wnd_width * wnd_params.height / wnd_params.width;
		if (wnd_height > mode.h)
		{
			wnd_height = mode.h;
			wnd_width = wnd_height * wnd_params.width / wnd_params.height;
		}
		factor = wnd_height / wnd_params.height;

		dstrect.x = (mode.w - wnd_width) >> 1;
		dstrect.y = (mode.h - wnd_height) >> 1;
		dstrect.w = wnd_width;
		dstrect.h = wnd_height;
	}

	/* create window */
	if (window_title == NULL)
		window_title = strdup("Tilengine window");
	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wnd_width, wnd_height, rflags);
	if (!window)
	{
		DeleteWindow();
		return false;
	}

	/* create render context */
	rflags = SDL_RENDERER_ACCELERATED;
	if (wnd_params.flags & CWF_VSYNC)
		rflags |= SDL_RENDERER_PRESENTVSYNC;
	renderer = SDL_CreateRenderer(window, -1, rflags);
	if (!renderer)
	{
		DeleteWindow();
		return false;
	}

	/* setup backbuffer & crt effect */
	SetupBackBuffer();
	crt = CRTCreate(renderer, backbuffer, crt_params.type, wnd_width, wnd_height, crt_params.blur);

	if (wnd_params.flags & CWF_FULLSCREEN)
		SDL_ShowCursor(SDL_DISABLE);

	/* one time init, avoid being forgotten in Alt+TAB */
	if (init == false)
	{
		/* Default input PLAYER 1 */
		TLN_EnableInput(PLAYER1, true);
		TLN_DefineInputKey(PLAYER1, INPUT_UP, SDLK_UP);
		TLN_DefineInputKey(PLAYER1, INPUT_DOWN, SDLK_DOWN);
		TLN_DefineInputKey(PLAYER1, INPUT_LEFT, SDLK_LEFT);
		TLN_DefineInputKey(PLAYER1, INPUT_RIGHT, SDLK_RIGHT);
		TLN_DefineInputKey(PLAYER1, INPUT_BUTTON1, SDLK_z);
		TLN_DefineInputKey(PLAYER1, INPUT_BUTTON2, SDLK_x);
		TLN_DefineInputKey(PLAYER1, INPUT_BUTTON3, SDLK_c);
		TLN_DefineInputKey(PLAYER1, INPUT_BUTTON4, SDLK_v);
		TLN_DefineInputKey(PLAYER1, INPUT_START, SDLK_RETURN);
		TLN_DefineInputKey(PLAYER1, INPUT_QUIT, SDLK_ESCAPE);
		TLN_DefineInputKey(PLAYER1, INPUT_CRT, SDLK_BACKSPACE);

		/* joystick */
		if (SDL_NumJoysticks() > 0)
		{
			SDL_JoystickEventState(SDL_ENABLE);
			TLN_AssignInputJoystick(PLAYER1, 0);
			TLN_DefineInputButton(PLAYER1, INPUT_BUTTON1, 1);
			TLN_DefineInputButton(PLAYER1, INPUT_BUTTON2, 0);
			TLN_DefineInputButton(PLAYER1, INPUT_BUTTON3, 2);
			TLN_DefineInputButton(PLAYER1, INPUT_BUTTON4, 3);
			TLN_DefineInputButton(PLAYER1, INPUT_START, 5);
		}
		init = true;
	}

	done = false;
	return true;
}

/* destroy window delegate */
static void DeleteWindow (void)
{
	if (SDL_JoystickGetAttached(joy))
		SDL_JoystickClose(joy);

	CRTDelete(crt);
	crt = NULL;

	if (backbuffer)
	{
		SDL_DestroyTexture (backbuffer);
		backbuffer = NULL;
	}
	
	if (renderer)
	{
		SDL_DestroyRenderer (renderer);
		renderer = NULL;
	}

	if (window)
	{
		SDL_DestroyWindow (window);
		window = NULL;
	}
}

/*!
 * \brief
 * Sets window title
 * 
 * \param title
 * Text with the title to set
 * 
 */
void TLN_SetWindowTitle (const char* title)
{
	if (window != NULL)
		SDL_SetWindowTitle (window, title);
	if (window_title != NULL)
	{
		free(window_title);
		window_title = NULL;
	}
	if (title != NULL)
		window_title = strdup(title);
}

static int WindowThread (void* data)
{
	bool ok;

	ok = CreateWindow ();
	if (ok == true)
		wnd_params.retval = 1;
	else
	{
		wnd_params.retval = 2;
		return 0;
	}

	/* main loop */
	while (TLN_IsWindowActive())
	{
		SDL_LockMutex (lock);
		TLN_DrawFrame (0);
		SDL_CondSignal (cond);
		SDL_UnlockMutex (lock);
		TLN_ProcessWindow ();
	}
	return 0;
}

/*!
 * \brief
 * Creates a window for rendering
 * 
 * \param overlay
 * Deprecated parameter in 2.10, kept for compatibility. Set to NULL
 * 
 * \param flags
 * Mask of the possible creation flags:
 * CWF_FULLSCREEN, CWF_VSYNC, CWF_S1 - CWF_S5 (scaling factor, none = auto max)
 * 
 * \returns
 * True if window was created or false if error
 * 
 * Creates a host window with basic user input for tilengine. If fullscreen, it uses the desktop
 * resolution and stretches the output resolution with aspect correction, letterboxing or pillarboxing
 * as needed. If windowed, it creates a centered window that is the maximum possible integer multiply of
 * the resolution configured at TLN_Init()
 * 
 * \remarks
 * Using this feature is optional, Tilengine is designed to output its rendering to a user-provided surface
 * so it can be used as a backend renderer of an already existing framework. But it is provided for convenience,
 * so it isn't needed to provide external components to run the examples or do engine tests.
 * 
 * \see
 * TLN_DeleteWindow(), TLN_ProcessWindow(), TLN_GetInput(), TLN_DrawFrame()
 */
bool TLN_CreateWindow (const char* overlay, int flags)
{
	bool ok;

	/* allow single instance */
	if (instances)
	{
		instances++;
		return true;
	}

	if (SDL_Init (SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) != 0)
		return false;

	/* fill parameters for window creation */
	wnd_params.width = TLN_GetWidth ();
	wnd_params.height = TLN_GetHeight ();
	wnd_params.flags = flags|CWF_VSYNC;

	crt_params.enable = (wnd_params.flags & CWF_NEAREST) == 0;
	ok = CreateWindow ();
	if (ok)
		instances++;
	return ok;
}

/*!
 * \brief
 * Creates a multithreaded window for rendering
 * 
 * \param overlay
 * Deprecated parameter in 2.10, kept for compatibility. Set to NULL
 * 
 * \param flags
 * Mask of the possible creation flags:
 * CWF_FULLSCREEN, CWF_VSYNC, CWF_S1 - CWF_S5 (scaling factor, none = auto max)
 * 
 * \returns
 * True if window was created or false if error
 * 
 * Creates a host window with basic user input for tilengine. If fullscreen, it uses the desktop
 * resolution and stretches the output resolution with aspect correction, letterboxing or pillarboxing
 * as needed. If windowed, it creates a centered window that is the maximum possible integer multiply of
 * the resolution configured at TLN_Init()
 * 
 * \remarks
 * Unlike TLN_CreateWindow, This window runs in its own thread
 * 
 * \see
 * TLN_DeleteWindow(), TLN_IsWindowActive(), TLN_GetInput(), TLN_UpdateFrame()
 */
bool TLN_CreateWindowThread (const char* overlay, int flags)
{
	bool ok;

	/* allow single instance */
	if (instances)
	{
		instances++;
		return true;
	}

	if (SDL_Init (SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) != 0)
		return false;

	/* fill parameters for window creation */
	wnd_params.retval = 0;
	wnd_params.width = TLN_GetWidth ();
	wnd_params.height = TLN_GetHeight ();
	wnd_params.flags = flags | CWF_VSYNC;

	crt_params.enable = (wnd_params.flags & CWF_NEAREST) == 0;
	lock = SDL_CreateMutex ();
	cond = SDL_CreateCond ();

	/* init thread & wait window creation result */
	thread = SDL_CreateThread (WindowThread, "WindowThread", &wnd_params);
	while (wnd_params.retval == 0)
		SDL_Delay (10);

	if (wnd_params.retval == 1)
		return true;
	else
		return false;

	if (ok)
		instances++;
	return ok;
}

/*!
 * \brief
 * Deletes the window previoulsy created with TLN_CreateWindow() or TLN_CreateWindowThread()
 * 
 * \see
 * TLN_CreateWindow()
 */
void TLN_DeleteWindow (void)
{
	/* single instance, delete when reach 0 */
	if (!instances)
		return;
	instances--;
	if (instances)
		return;

	DeleteWindow ();
	SDL_Quit ();
	printf(" ");
}

/* marks input as pressed */
static void SetInput (TLN_Player player, TLN_Input input)
{
	player_inputs[player].inputs |= (1 << input);
	last_key = input;
}

/* marks input as unpressed */
static void ClrInput (TLN_Player player, TLN_Input input)
{
	player_inputs[player].inputs &= ~(1 << input);
}

/* process keyboard input */
static void ProcessKeycodeInput (TLN_Player player, SDL_Keycode keycode, uint8_t state)
{
	int c;
	PlayerInput* player_input = &player_inputs[player];
	TLN_Input input = INPUT_NONE;
	
	/* search input */
	for (c=INPUT_UP; c<MAX_INPUTS && input == INPUT_NONE; c++)
	{
		if (player_input->keycodes[c] == keycode)
			input = (TLN_Input)c;
	}

	/* update */
	if (input != INPUT_NONE)
	{
		if (state == SDL_PRESSED)
			SetInput (player, input);
		else
			ClrInput (player, input);
	}
}

/* process joystick button input */
static void ProcessJoybuttonInput (TLN_Player player, uint8_t button, uint8_t state)
{
	int c;
	PlayerInput* player_input = &player_inputs[player];
	TLN_Input input = INPUT_NONE;

	/* search input */
	for (c=INPUT_BUTTON1; c<MAX_INPUTS && input == INPUT_NONE; c++)
	{
		if (player_input->joybuttons[c] == button)
			input = (TLN_Input)c;
	}

	/* update */
	if (input != INPUT_NONE)
	{
		if (state == SDL_PRESSED)
			SetInput (player, input);
		else
			ClrInput (player, input);
	}
}

/* process joystic axis input */
static void ProcessJoyaxisInput (TLN_Player player, uint8_t axis, int value)
{
	if (axis == 0)
	{
		ClrInput (player, INPUT_LEFT);
		ClrInput (player, INPUT_RIGHT);
		if (value > 1000)
			SetInput (player, INPUT_RIGHT);
		else if (value < -1000)
			SetInput (player, INPUT_LEFT);
	}
	else if (axis == 1)
	{
		ClrInput (player, INPUT_UP);
		ClrInput (player, INPUT_DOWN);
		if (value > 1000)
			SetInput (player, INPUT_DOWN);
		else if (value < -1000)
			SetInput (player, INPUT_UP);
	}
}

/*!
 * \brief
 * Does basic window housekeeping in signgle-threaded window
 * 
 * \returns
 * True if window is active or false if the user has requested to end the application (by pressing Esc key
 * or clicking the close button)
 * 
 * If a window has been created with TLN_CreateWindow, this function must be called periodically (call it inside
 * the main loop so it gets called regularly). If the window was created with TLN_CreateWindowThread, do not use it
 * 
 * \see
 * TLN_CreateWindow()
 */
bool TLN_ProcessWindow (void)
{
	SDL_Event evt;
	SDL_KeyboardEvent* keybevt;
	SDL_JoyButtonEvent* joybuttonevt;
	SDL_JoyAxisEvent* joyaxisevt;
	int input = 0;
	int c;

	if (done)
		return false;

	/* dispatch message queue */
	while (SDL_PollEvent (&evt))
	{
		switch (evt.type)
		{
		case SDL_QUIT:
			done = true;
			break;

		case SDL_KEYDOWN:
			keybevt = (SDL_KeyboardEvent*)&evt;
			if (keybevt->repeat != 0)
				break;

			/* special inputs */
			if (keybevt->keysym.sym == player_inputs[PLAYER1].keycodes[INPUT_QUIT])
				done = true;
			else if (keybevt->keysym.sym == player_inputs[PLAYER1].keycodes[INPUT_CRT])
			{
				crt_params.enable = !crt_params.enable;
				SetupBackBuffer();
				CRTSetRenderTarget(crt, backbuffer);
			}
			else if (keybevt->keysym.sym == SDLK_RETURN && keybevt->keysym.mod & KMOD_ALT)
			{
				DeleteWindow();
				wnd_params.flags ^= CWF_FULLSCREEN;
				CreateWindow();
			}

			/* regular user input */
			for (c = PLAYER1; c < MAX_PLAYERS; c++)
			{
				if (player_inputs[c].enabled == true)
					ProcessKeycodeInput((TLN_Player)c, keybevt->keysym.sym, keybevt->state);
			}
			break;

		case SDL_KEYUP:
			keybevt = (SDL_KeyboardEvent*)&evt;
			for (c = PLAYER1; c < MAX_PLAYERS; c++)
			{
				if (player_inputs[c].enabled == true)
					ProcessKeycodeInput((TLN_Player)c, keybevt->keysym.sym, keybevt->state);
			}
			break;

		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			joybuttonevt = (SDL_JoyButtonEvent*)&evt;
			for (c=PLAYER1; c<MAX_PLAYERS; c++)
			{
				if (player_inputs[c].enabled == true && player_inputs[c].joystick_id == joybuttonevt->which)
					ProcessJoybuttonInput ((TLN_Player)c, joybuttonevt->button, joybuttonevt->state);
			}
			break;

		case SDL_JOYAXISMOTION:
			joyaxisevt = (SDL_JoyAxisEvent*)&evt;
			for (c=PLAYER1; c<MAX_PLAYERS; c++)
			{
				if (player_inputs[c].enabled == true && player_inputs[c].joystick_id == joyaxisevt->which)
					ProcessJoyaxisInput ((TLN_Player)c, joyaxisevt->axis, joyaxisevt->value);
			}
			break;
    	}

		/* procesa eventos de usuario */
		if (sdl_callback != NULL)
			sdl_callback(&evt);
	}

	/* delete */
	if (done)
		TLN_DeleteWindow ();

	return TLN_IsWindowActive ();
}

/*!
 * \brief
 * Checks window state
 * 
 * \returns
 * True if window is active or false if the user has requested to end the application (by pressing Esc key
 * or clicking the close button)
 * 
 * \see
 * TLN_CreateWindow(), TLN_CreateWindowThread()
 */
bool TLN_IsWindowActive (void)
{
	return !done;
}

/*!
 * \brief
 * Thread synchronization for multithreaded window. Waits until the current
 * frame has ended rendering
 *
 * \see
 * TLN_CreateWindowThread()
 */
void TLN_WaitRedraw (void)
{
	if (lock)
	{
		SDL_LockMutex (lock);
		SDL_CondWait (cond, lock);
		SDL_UnlockMutex (lock);
	}
}

/*!
 * \brief
 * Enables or disables optional horizontal blur in CRT effect
 * 
 * \param mode
 * Enables or disables RF emulation on CRT effect
 */
void TLN_EnableRFBlur (bool mode)
{
	CRTSetBlur(crt, mode);
}

/*!
 * \deprecated Use TLN_ConfigCRTEffect()
 */
void TLN_EnableBlur(bool mode)
{
}

/*!
  * \brief
 * Enables CRT simulation post-processing effect to give true retro appeareance
 *
 * \param type One possible value of \ref TLN_CRT enumeration
 * \param blur Optional RF (horizontal) blur, increases CPU usage
 */

void TLN_ConfigCRTEffect(TLN_CRT type, bool blur)
{
	if (crt != NULL)
		CRTDelete(crt);

	crt_params.type = (CRTType)type;
	crt_params.blur = blur;
	crt_params.enable = true;
	SetupBackBuffer();
	crt = CRTCreate(renderer, backbuffer, crt_params.type, wnd_width, wnd_height, crt_params.blur);
}

/*!
 * \deprecated Use TLN_ConfigCRTEffect() instead
 * \brief
 * Enables CRT simulation post-processing effect to give true retro appeareance
 * 
 * \remarks Parameters have no effect, they're kept for backwards API/ABI compatibility. Original default values are always used.
 */ 
void TLN_EnableCRTEffect (int overlay, uint8_t overlay_factor, uint8_t threshold, uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, bool blur, uint8_t glow_factor)
{
	if (crt != NULL)
		CRTDelete(crt);

	crt_params.type = CRT_SLOT;
	crt_params.blur = true;
	crt_params.enable = true;
	SetupBackBuffer();
	crt = CRTCreate(renderer, backbuffer, crt_params.type, wnd_width, wnd_height, crt_params.blur);
}

/*!
 * \brief
 * Disables the CRT post-processing effect
 * 
 * \see
 * TLN_ConfigCRTEffect
 */ 
void TLN_DisableCRTEffect (void)
{
	crt_params.enable = false;
	SetupBackBuffer();
}

/*!
 * \brief
 * Returns the state of a given input
 * 
 * \param input
 * Input to check state. It can be one of the following values:
 *	 * INPUT_UP
 *	 * INPUT_DOWN
 *	 * INPUT_LEFT
 *	 * INPUT_RIGHT
 *	 * INPUT_BUTTON1 - INPUT_BUTTON6,
 *	 * INPUT_START
 *	 * Optionally combine with INPUT_P1 to INPUT_P4 to request input for specific player
 * 
 * \returns
 * True if that input is pressed or false if not
 * 
 * If a window has been created with TLN_CreateWindow, it provides basic user input.
 * It simulates a classic arcade setup, with 4 directional buttons (INPUT_UP to INPUT_RIGHT),
 * 6 action buttons (INPUT_BUTTON1 to INPUT_BUTTON6) and a start button (INPUT_START). 
 * By default directional buttons are mapped to keyboard cursors and joystick 1 D-PAD, 
 * and the first four action buttons are the keys Z,X,C,V and joystick buttons 1 to 4.
 * 
 * \see
 * TLN_CreateWindow(), TLN_DefineInputKey(), TLN_DefineInputButton()
 */
bool TLN_GetInput (TLN_Input input)
{
	const TLN_Player player = (TLN_Player)(input >> 5);
	const uint32_t mask = (player_inputs[player].inputs & (1 << (input & INPUT_MASK)));
	if (mask)
		return true;
	return false;
}

/*!
 * \brief
 * Enables or disables input for specified player
 *
 * \param player
 * Player number to enable (PLAYER1 - PLAYER4)
 *
 * \param enable
 * Set true to enable, false to disable
 */
void TLN_EnableInput (TLN_Player player, bool enable)
{
	player_inputs[player].enabled = enable;
}

/*!
 * \brief
 * Assigns a joystick index to the specified player
 *
 * \param player
 * Player number to configure (PLAYER1 - PLAYER4)
 *
 * \param index
 * Joystick index to assign, 0-based index. -1 = disable
 */
void TLN_AssignInputJoystick (TLN_Player player, int index)
{
	PlayerInput* player_input = &player_inputs[player];
	if (player_input->joy != NULL)
	{
		SDL_JoystickClose (player_input->joy);
		player_input->joy = NULL;
	}
	if (index >= 0)
	{
		player_input->joy = SDL_JoystickOpen (index);
		player_input->joystick_id = SDL_JoystickInstanceID (player_input->joy);
	}
}

/*!
 * \brief
 * Assigns a keyboard input to a player
 *
 * \param player
 * Player number to configure (PLAYER1 - PLAYER4)
 *
 * \param input
 * Input to associate to the given key
 *
 * \param keycode
 * ASCII key value or scancode as defined in SDL.h
 */
void TLN_DefineInputKey (TLN_Player player, TLN_Input input, uint32_t keycode)
{
	player_inputs[player].keycodes[input & INPUT_MASK] = keycode;
}

/*!
 * \brief
 * Assigns a button joystick input to a player
 *
 * \param player
 * Player number to configure (PLAYER1 - PLAYER4)
 *
 * \param input
 * Input to associate to the given button
 *
 * \param joybutton
 * Button index
 */
void TLN_DefineInputButton (TLN_Player player, TLN_Input input, uint8_t joybutton)
{
	player_inputs[player].joybuttons[input & INPUT_MASK] = joybutton;
}

/*!
 * \brief
 * Returns the last pressed input button
 * 
 * \see
 * TLN_GetInput()
 */
int TLN_GetLastInput (void)
{
	int retval = last_key;
	last_key = INPUT_NONE;
	return retval;
}

static void BeginWindowFrame (void)
{
	SDL_LockTexture (backbuffer, NULL, (void**)&rt_pixels, &rt_pitch);
	TLN_SetRenderTarget (rt_pixels, rt_pitch);
}

static void EndWindowFrame(void)
{
	if (wnd_params.flags & CWF_FULLSCREEN)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
	}

	if (crt_params.enable && crt != NULL)
		CRTDraw(crt, rt_pixels, rt_pitch, &dstrect);

	else
	{
		SDL_UnlockTexture(backbuffer);
		SDL_SetTextureBlendMode(backbuffer, SDL_BLENDMODE_NONE);
		SDL_RenderCopy(renderer, backbuffer, NULL, &dstrect);
	}
	SDL_RenderPresent(renderer);
}

/*!
 * \brief
 * Draws a frame to the window
 * 
 * \param frame Optional frame number. Set to 0 to autoincrement from previous value
 * 
 * Draws a frame to the window
 * 
 * \remarks
 * If a window has been created with TLN_CreateWindow(), it renders the frame to it. This function is a wrapper to
 * TLN_UpdateFrame which also automatically sets the render target for the window, so when calling this function it is
 * not needed to call TLN_UpdateFrame() too.
 * 
 * \see
 * TLN_CreateWindow(), TLN_UpdateFrame()
 */
void TLN_DrawFrame (int frame)
{
	BeginWindowFrame ();
	TLN_UpdateFrame(frame);
	EndWindowFrame ();
}

/*!
 * \brief
 * Returns the number of milliseconds since application start
 */
uint32_t TLN_GetTicks (void)
{
	return SDL_GetTicks ();
}

/*!
 * \brief
 * Suspends execition for a fixed time
 * \param time Number of milliseconds to wait
 */
void TLN_Delay (uint32_t time)
{
	SDL_Delay (time);
}

/*!
 * \brief
 * Returns horizontal dimension of window after scaling
 */
int TLN_GetWindowWidth(void)
{
	return wnd_width;
}

/*!
 * \brief
 * Returns vertical dimension of window after scaling
 */
int TLN_GetWindowHeight(void)
{
	return wnd_height;
}

/*!
* \brief
* Registers a user-defined callback to capture internal SDL2 events
* \param callback pointer to user funcion with signature void (SDL_Event*)
*/
void TLN_SetSDLCallback(TLN_SDLCallback callback)
{
	sdl_callback = callback;
}

#endif
