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

#ifdef WIN32
#include <Windows.h>
#endif
#include <string.h>
#include "SDL2/SDL.h"
#include "Tilengine.h"
#include "crt.h"
#include "Engine.h"

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
	bool enable;
	bool blur;
	bool scanlines;
}
static crt_params = { CRT_APERTURE, true, false, true };

#define MAX_PATH	260

/* Window manager */
typedef struct
{
	int width;
	int height;
	int flags;
	volatile int retval;
	uint32_t t0;			/* frame start time for non-vsync pacing */
	uint32_t min_delay;		/* actual granularity of SDL_Delay() */
	uint32_t fps_t0;		/* for actual FPS calc */
	uint32_t fps_frames;
	uint32_t fps_average;
}
WndParams;

static WndParams wnd_params;

typedef union
{
	uint8_t value;
	struct
	{
		bool fullscreen : 1;
		bool vsync : 1;
		uint8_t factor : 4;
		bool nearest : 1;
		bool novsync : 1;
	};
}
WindowFlags;

/* local prototypes */
static bool create_window (void);
static void delete_window (void);

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
static bool create_window(void)
{
	SDL_DisplayMode mode;
	SDL_Surface* surface = NULL;
	int rflags;
	char quality[2] = { 0 };
	Uint32 format = 0;
	WindowFlags flags;
	flags.value = wnd_params.flags;

	/*  gets desktop size and maximum window size */
	SDL_GetDesktopDisplayMode(0, &mode);
	if (!flags.fullscreen)
	{
		rflags = 0;
		if (flags.factor == 0)
		{
			flags.factor = 1;
			while (wnd_params.width*(flags.factor + 1) < mode.w && wnd_params.height*(flags.factor + 1) < mode.h && flags.factor < 3)
				flags.factor += 1;
		}

		wnd_width = wnd_params.width * flags.factor;
		wnd_height = wnd_params.height * flags.factor;

		dstrect.x = 0;
		dstrect.y = 0;
		dstrect.w = wnd_width;
		dstrect.h = wnd_height;
		wnd_params.flags = flags.value;
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
		delete_window();
		return false;
	}

	/* one time init, avoid being forgotten in Alt+TAB */
	if (init == false)
	{
		/* Default input PLAYER 1 */
		TLN_EnableInput(PLAYER1, true);
		TLN_DefineInputKey(PLAYER1, INPUT_UP, SDLK_UP);
		TLN_DefineInputKey(PLAYER1, INPUT_DOWN, SDLK_DOWN);
		TLN_DefineInputKey(PLAYER1, INPUT_LEFT, SDLK_LEFT);
		TLN_DefineInputKey(PLAYER1, INPUT_RIGHT, SDLK_RIGHT);
		TLN_DefineInputKey(PLAYER1, INPUT_X, SDLK_z);
		TLN_DefineInputKey(PLAYER1, INPUT_A, SDLK_x);
		TLN_DefineInputKey(PLAYER1, INPUT_Y, SDLK_c);
		TLN_DefineInputKey(PLAYER1, INPUT_B, SDLK_v);
		TLN_DefineInputKey(PLAYER1, INPUT_START, SDLK_RETURN);
		TLN_DefineInputKey(PLAYER1, INPUT_QUIT, SDLK_ESCAPE);
		TLN_DefineInputKey(PLAYER1, INPUT_CRT, SDLK_BACKSPACE);

		/* joystick */
		if (SDL_NumJoysticks() > 0)
		{
			SDL_JoystickEventState(SDL_ENABLE);
			TLN_AssignInputJoystick(PLAYER1, 0);
			
			/* default for X-Input */
			for (int c = 0; c < 12; c += 1)
				TLN_DefineInputButton(PLAYER1, (TLN_Input)(INPUT_BUTTON1 + c), c);
		}

		/* capture actual granularity for SDL_Delay() */
		if (flags.novsync)
		{
#if defined WIN32
			timeBeginPeriod(1);
#endif
			int c;
			uint32_t delay = 0;
			uint32_t t0;
			SDL_Delay(1);
			t0 = SDL_GetTicks();
			for (c = 0; c < 8; c += 1)
			{
				SDL_Delay(1);
			}
			wnd_params.min_delay = (SDL_GetTicks() - t0) / c;

			/* capture actual monitor fps */
			SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED + SDL_RENDERER_PRESENTVSYNC);
			if (renderer != NULL)
			{
				int target_fps = 0;
				SDL_RenderPresent(renderer);
				t0 = SDL_GetTicks();
				for (c = 0; c < 20; c += 1)
					SDL_RenderPresent(renderer);
				target_fps = (c * 1000) / (SDL_GetTicks() - t0);
				SDL_DestroyRenderer(renderer);

				/* try "snapping" for common rates */
				uint8_t rates[] = { 24,30,60,75,144,200,240 };
				for (c = 0; c < sizeof(rates); c += 1)
				{
					if (abs(target_fps - (int)rates[c]) < 4)
					{
						target_fps = rates[c];
						break;
					}
				}
				engine->target_fps = target_fps;
			}

#if defined WIN32
			timeEndPeriod(1);
#endif
		}
		init = true;
	}

	/* create render context */
	rflags = SDL_RENDERER_ACCELERATED;
	if (!(wnd_params.flags & CWF_NOVSYNC))
		rflags |= SDL_RENDERER_PRESENTVSYNC;
	renderer = SDL_CreateRenderer(window, -1, rflags);
	if (!renderer)
	{
		delete_window();
		return false;
	}

	/* setup backbuffer & crt effect */
	SetupBackBuffer();
	crt = CRTCreate(renderer, backbuffer, crt_params.type, wnd_width, wnd_height, crt_params.blur, crt_params.scanlines);

	if (wnd_params.flags & CWF_FULLSCREEN)
		SDL_ShowCursor(SDL_DISABLE);


	done = false;
	return true;
}

/* destroy window delegate */
static void delete_window (void)
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

	ok = create_window ();
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
	wnd_params.flags = flags;

	crt_params.enable = (wnd_params.flags & CWF_NEAREST) == 0;
	ok = create_window ();
	if (ok)
		instances++;
	return ok;
}

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
	wnd_params.flags = flags;

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

void TLN_DeleteWindow (void)
{
	/* single instance, delete when reach 0 */
	if (!instances)
		return;
	instances--;
	if (instances)
		return;

	delete_window ();
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
				delete_window();
				wnd_params.flags ^= CWF_FULLSCREEN;
				create_window();
			}
			
			/* override window scale */
			for (c = 1; c <= 5; c += 1)
			{
				if (keybevt->keysym.sym == (SDL_Keycode)('0' + c) && keybevt->keysym.mod & KMOD_ALT)
				{
					WindowFlags flags;
					flags.value = wnd_params.flags;
					if (c != flags.factor)
					{
						flags.factor = c;
						flags.fullscreen = false;
						wnd_params.flags = flags.value;
						delete_window();
						create_window();
					}
				}
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

bool TLN_IsWindowActive (void)
{
	return !done;
}

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

void TLN_ConfigCRTEffect(TLN_CRT type, bool blur, bool scanlines)
{
	if (crt != NULL)
		CRTDelete(crt);

	crt_params.type = (CRTType)type;
	crt_params.blur = blur;
	crt_params.scanlines = scanlines;
	crt_params.enable = true;

	SetupBackBuffer();
	crt = CRTCreate(renderer, backbuffer, crt_params.type, wnd_width, wnd_height, crt_params.blur, crt_params.scanlines);
}

void TLN_DisableCRTEffect (void)
{
	crt_params.enable = false;
	SetupBackBuffer();
}

bool TLN_GetInput (TLN_Input input)
{
	const TLN_Player player = (TLN_Player)(input >> 5);
	const uint32_t mask = (player_inputs[player].inputs & (1 << (input & INPUT_MASK)));
	if (mask)
		return true;
	return false;
}

void TLN_EnableInput (TLN_Player player, bool enable)
{
	player_inputs[player].enabled = enable;
}

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

void TLN_DefineInputKey (TLN_Player player, TLN_Input input, uint32_t keycode)
{
	player_inputs[player].keycodes[input & INPUT_MASK] = keycode;
}

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
	wnd_params.t0 = SDL_GetTicks();
	SDL_LockTexture (backbuffer, NULL, (void**)&rt_pixels, &rt_pitch);
	TLN_SetRenderTarget (rt_pixels, rt_pitch);
	if (wnd_params.fps_t0 == 0)
		wnd_params.fps_t0 = SDL_GetTicks();
}

static void EndWindowFrame(void)
{
	WindowFlags flags;
	flags.value = wnd_params.flags;

	if (flags.fullscreen)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
	}

	if (crt_params.enable && crt != NULL && (flags.factor > 1 || flags.fullscreen))
		CRTDraw(crt, rt_pixels, rt_pitch, &dstrect);

	else
	{
		SDL_UnlockTexture(backbuffer);
		SDL_SetTextureBlendMode(backbuffer, SDL_BLENDMODE_NONE);
		SDL_RenderCopy(renderer, backbuffer, NULL, &dstrect);
	}

	/* no vsync: timed sync */
	if (flags.novsync)
	{
#if defined WIN32
		timeBeginPeriod(1);
#endif
		Engine* context = TLN_GetContext();
		uint32_t due_time = wnd_params.t0 + (1000 / context->target_fps);
		uint32_t now = SDL_GetTicks();
		while (now < due_time)
		{
			if (due_time - now > wnd_params.min_delay)
				SDL_Delay(wnd_params.min_delay);
			now = SDL_GetTicks();
		}
#if defined WIN32
		timeEndPeriod(1);
#endif
	}

	SDL_RenderPresent(renderer);

	/* update averaged fps */
	const uint32_t now = SDL_GetTicks();
	const uint32_t elapsed = now - wnd_params.fps_t0;
	wnd_params.fps_frames += 1;
	if (elapsed >= 500)
	{
		wnd_params.fps_average = (wnd_params.fps_frames * 1000) / elapsed;
		wnd_params.fps_frames = 0;
		wnd_params.fps_t0 = now;
	}
}

void TLN_DrawFrame (int frame)
{
	BeginWindowFrame ();
	TLN_UpdateFrame(frame);
	EndWindowFrame ();
}

uint32_t TLN_GetTicks (void)
{
	return SDL_GetTicks ();
}

void TLN_Delay (uint32_t time)
{
	SDL_Delay (time);
}

int TLN_GetWindowWidth(void)
{
	return wnd_width;
}

int TLN_GetWindowHeight(void)
{
	return wnd_height;
}

void TLN_SetSDLCallback(TLN_SDLCallback callback)
{
	sdl_callback = callback;
}

uint32_t TLN_GetAverageFps(void)
{
	return wnd_params.fps_average;
}

int TLN_GetWindowScaleFactor(void)
{
	WindowFlags flags;
	flags.value = wnd_params.flags;
	return flags.factor;
}

void TLN_SetWindowScaleFactor(int factor)
{
	WindowFlags flags;
	flags.value = wnd_params.flags;
	flags.factor = factor;
	wnd_params.flags = flags.value;
}

#endif
