/*
Tilengine - 2D Graphics library with raster effects
Copyright (c) 2015-2017 Marc Palacios Domènech (megamarc@hotmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*!
 ******************************************************************************
 *
 * \file
 * \brief Built-in window and user input using libSDL2
 * https://www.libsdl.org/
 *
 * \author Megamarc
 * \date 20 sep 2015
 *
 * Public Tilengine source code
 * http://www.tilengine.org
 *
 ******************************************************************************
 */

#ifndef TLN_EXCLUDE_WINDOW

#include "SDL2/SDL.h"
#include "Tilengine.h"

static SDL_Window*   window;
static SDL_Renderer* renderer;
static SDL_Texture*	 backbuffer;
static SDL_Texture*	 overlay;
static SDL_Thread*   thread;
static SDL_mutex*	 lock;
static SDL_cond*	 cond;
static SDL_Joystick* joy;
static SDL_Rect		 dstrect;

static bool			 done;
static int			 wnd_width;
static int			 wnd_height;
static int			 tln_width;
static int			 tln_height;
static int			 tln_bpp;
static int			 instances = 0;
static bool			 blur = false;
static BYTE*		 rt_pixels;
static int			 rt_pitch;

static int			inputs;
static int			last_key;

/* Window manager */
typedef struct
{
	int width;
	int height;
	int bpp;
	TLN_WindowFlags flags;
	const char* file_overlay;
	volatile int retval;
}
WndParams;

/* local prototypes */
static SDL_Surface* CreateOverlaySurface (const char* filename, int dstw, int dsth, int bpp);
static SDL_Texture* LoadTexture (char* filename);
static void hblur (BYTE* scan, int width, int height, int pitch);

static bool CreateWindow (int width, int height, int bpp, TLN_WindowFlags flags, const char* file_overlay)
{
	SDL_DisplayMode mode;
	SDL_Surface* surface = NULL;
	int factor;
	int rflags;
	char quality;
	Uint32 format = 0;

	if (SDL_Init (SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) != 0)
		return false;

	/* obtiene tamaño escritorio y tamaño máximo de ventana*/
	SDL_GetDesktopDisplayMode (0, &mode);
	if (!(flags & CWF_FULLSCREEN))
	{
		factor = (flags >> 2) & 0x07;
		if (!factor)
		{
			factor = 1;
			while (width*(factor + 1) < mode.w && height*(factor + 1) < mode.h && factor < 3)
				factor++;
		}
		
		wnd_width = width*factor;
		wnd_height = height*factor;

		dstrect.x = 0;
		dstrect.y = 0;
		dstrect.w = wnd_width;
		dstrect.h = wnd_height;
	}
	else
	{
		wnd_width = mode.w;
		wnd_height = wnd_width*height/width;
		if (wnd_height > mode.h)
		{
			wnd_height = mode.h;
			wnd_width = wnd_height*width/height;
		}
		factor = wnd_height / height;

		dstrect.x = (mode.w - wnd_width) >> 1;
		dstrect.y = (mode.h - wnd_height) >> 1;
		dstrect.w = wnd_width;
		dstrect.h = wnd_height;
	}

	rflags = 0;

	/* ventana */
	window = SDL_CreateWindow ("Tilengine window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wnd_width,wnd_height, rflags);
	if (!window)
	{
		TLN_DeleteWindow ();
		return false;
	}

	/* contexto de render */
	rflags = SDL_RENDERER_ACCELERATED;
	if (flags & CWF_VSYNC)
		rflags |= SDL_RENDERER_PRESENTVSYNC;
	renderer = SDL_CreateRenderer (window, -1, rflags);
	if (!renderer)
	{
		TLN_DeleteWindow ();
		return false;
	}
	quality = 1;
	SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, &quality);
	
	/* textura para recibir los pixeles de Tilengine */
	if (bpp == 32)
		format = SDL_PIXELFORMAT_ARGB8888;
	else if (bpp == 16)
		format = SDL_PIXELFORMAT_RGB565;
	backbuffer = SDL_CreateTexture (renderer, format, SDL_TEXTUREACCESS_STREAMING, width,height);
	if (!backbuffer)
	{
		TLN_DeleteWindow ();
		return false;
	}

	/* textura de overlay RGB */
	if (file_overlay)
		surface = CreateOverlaySurface (file_overlay, wnd_width, wnd_height, bpp);
	if (surface)
	{
		overlay = SDL_CreateTextureFromSurface (renderer, surface);
		SDL_SetTextureBlendMode (overlay, SDL_BLENDMODE_MOD);
		SDL_FreeSurface (surface);
	}
	
	if (flags & CWF_FULLSCREEN)
	{
		SDL_SetWindowDisplayMode (window, &mode);
		SDL_SetWindowFullscreen (window, SDL_WINDOW_FULLSCREEN);
		SDL_ShowCursor (SDL_DISABLE);
	}

	done = false;

	/* joystick */
	if (SDL_NumJoysticks () > 0)
	{
	    joy = SDL_JoystickOpen(0);
		SDL_JoystickEventState (SDL_ENABLE);
	}

	return true;
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
	SDL_SetWindowTitle (window, title);
}

static int WindowThread (void* data)
{
	int time = 0;
	bool ok;
	WndParams* params = (WndParams*)data;

	ok = CreateWindow (params->width, params->height, params->bpp, params->flags, params->file_overlay);
	if (ok == true)
		params->retval = 1;
	else
	{
		params->retval = 2;
		return 0;
	}

	/* main loop */
	while (TLN_IsWindowActive())
	{
		SDL_LockMutex (lock);
		TLN_DrawFrame (time++);
		SDL_CondSignal (cond);
		SDL_UnlockMutex (lock);
		TLN_ProcessWindow ();
	}

	TLN_DeleteWindow ();
	return 0;
}

static bool CreateWindowThread (int width, int height, int bpp, TLN_WindowFlags flags, const char* file_overlay)
{
	WndParams params;

	/* fill parameters for window creation */
	params.width = width;
	params.height = height;
	params.bpp = bpp;
	params.flags = flags|CWF_VSYNC;
	params.file_overlay = file_overlay;
	params.retval = 0;

	lock = SDL_CreateMutex ();
	cond = SDL_CreateCond ();

	/* init thread & wait window creation result */
	thread = SDL_CreateThread (WindowThread, "WindowThread", &params);
	while (params.retval == 0)
		SDL_Delay (10);

	if (params.retval == 1)
		return true;
	else
		return false;
}

/*!
 * \brief
 * Creates a window for rendering
 * 
 * \param overlay
 * Optional path of a bmp file to overlay (for emulating RGB mask, scanlines, etc)
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
bool TLN_CreateWindow (const char* overlay, TLN_WindowFlags flags)
{
	bool ok;

	/* allow single instance */
	if (instances)
	{
		instances++;
		return true;
	}

	tln_width  = TLN_GetWidth ();
	tln_height = TLN_GetHeight ();
	tln_bpp = TLN_GetBPP ();
	ok = CreateWindow (tln_width, tln_height, tln_bpp, flags, overlay);
	if (ok)
		instances++;
	return ok;

}

/*!
 * \brief
 * Creates a multithreaded window for rendering
 * 
 * \param overlay
 * Optional path of a bmp file to overlay (for emulating RGB mask, scanlines, etc)
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
bool TLN_CreateWindowThread (const char* overlay, TLN_WindowFlags flags)
{
	bool ok;

	/* allow single instance */
	if (instances)
	{
		instances++;
		return true;
	}

	tln_width  = TLN_GetWidth ();
	tln_height = TLN_GetHeight ();
	tln_bpp = TLN_GetBPP ();
	ok = CreateWindowThread (tln_width, tln_height, tln_bpp, flags, overlay);
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

    if (SDL_JoystickGetAttached(joy))
        SDL_JoystickClose(joy);

	if (overlay)
	{
		SDL_DestroyTexture (overlay);
		overlay = NULL;
	}

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

	SDL_Quit ();
}

/* marks input as pressed */
static void SetInput (int input)
{
	inputs |= (1 << input);
	last_key = input;
}

/* marks input as unpressed */
static void ClrInput (int input)
{
	inputs &= ~(1 << input);
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
			switch (keybevt->keysym.sym)
			{
			case SDLK_ESCAPE: done = true;	break;
			case SDLK_LEFT:	SetInput(INPUT_LEFT); break;
			case SDLK_RIGHT: SetInput(INPUT_RIGHT); break;
			case SDLK_UP: SetInput(INPUT_UP); break;
			case SDLK_DOWN: SetInput(INPUT_DOWN); break;
			case SDLK_z: SetInput(INPUT_A); break;
			case SDLK_x: SetInput(INPUT_B); break;
			case SDLK_c: SetInput(INPUT_C); break;
			case SDLK_v: SetInput(INPUT_D); break;
			}
			break;

		case SDL_KEYUP:
			keybevt = (SDL_KeyboardEvent*)&evt;
			switch (keybevt->keysym.sym)
			{
			case SDLK_LEFT: ClrInput(INPUT_LEFT); break;
			case SDLK_RIGHT: ClrInput(INPUT_RIGHT); break;
			case SDLK_UP: ClrInput(INPUT_UP); break;
			case SDLK_DOWN: ClrInput(INPUT_DOWN); break;
			case SDLK_z: ClrInput(INPUT_A); break;
			case SDLK_x: ClrInput(INPUT_B); break;
			case SDLK_c: ClrInput(INPUT_C); break;
			case SDLK_v: ClrInput(INPUT_D); break;
			}
			break;

		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			joybuttonevt = (SDL_JoyButtonEvent*)&evt;
			input = INPUT_A + joybuttonevt->button;
			if (joybuttonevt->state == 1)
				SetInput (input);
			else
				ClrInput (input);
			break;

		case SDL_JOYAXISMOTION:
			joyaxisevt = (SDL_JoyAxisEvent*)&evt;
			if (joyaxisevt->axis == 0)
			{
				ClrInput (INPUT_LEFT);
				ClrInput (INPUT_RIGHT);
				if (joyaxisevt->value > 1000)
					SetInput (INPUT_RIGHT);
				else if (joyaxisevt->value < -1000)
					SetInput (INPUT_LEFT);
			}
			else if (joyaxisevt->axis == 1)
			{
				ClrInput (INPUT_UP);
				ClrInput (INPUT_DOWN);
				if (joyaxisevt->value > 1000)
					SetInput (INPUT_DOWN);
				else if (joyaxisevt->value < -1000)
					SetInput (INPUT_UP);
			}
			break;
    	}
	}

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
 * Enables or disables a simple horizontal blur effect to emulate noisy composite video
 * 
 * \param mode
 * Enable or disable effect
 * 
 * \remarks
 * Many games in Sega Genesis / Megadrive faked transparencies and more colors by alternating
 * thin vertical strips of colors that were blended together at the RF signal, trading horizontal
 * resolution by color range. Enable this filter to emulate that effect.
 */
void TLN_EnableBlur (bool mode)
{
	blur = mode && tln_bpp == 32;
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
 *	 * INPUT_A
 *	 * INPUT_B
 *	 * INPUT_C
 *	 * INPUT_D
 * 
 * \returns
 * True if that input is pressed or false if not
 * 
 * If a window has been created with TLN_CreateWindow, it provides basic user input.
 * It simulates a classic arcade setup, with four directional buttons (INPUT_UP to INPUT_RIGHT) 
 * and four action buttons (INPUT_A to INPUT_D). Directional buttons are mapped to keyboard cursors
 * or joystick D-PAD, and the four action buttons are the keys Z,X,C,V or joystick buttons 1 to 4.
 * 
 * \see
 * TLN_CreateWindow()
 */
bool TLN_GetInput (TLN_Input input)
{
	return (inputs & (1 << input)) != 0;
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

/*!
 * \brief Begins active rendering frame in built-in window
 * \param time Timestamp (same value as in TLN_UpdateFrame())
 * \remarks Use this function instead of TLN_BeginFrame() when using the built-in window
 * \see TLN_CreateWindow(), TLN_EndWindowFrame(), TLN_DrawNextScanline()
 */
void TLN_BeginWindowFrame (int time)
{
	SDL_LockTexture (backbuffer, NULL, &rt_pixels, &rt_pitch);
	TLN_SetRenderTarget (rt_pixels, rt_pitch);
	TLN_BeginFrame (time);
}

/*!
 * \brief Finishes rendering the current frame and updates the built-in window
 * \see TLN_CreateWindow(), TLN_BeginWindowFrame(), TLN_DrawNextScanline()
 */
void TLN_EndWindowFrame (void)
{
	if (blur)
		hblur (rt_pixels, tln_width, tln_height, rt_pitch);
	
	/* end frame and apply overlay */
	SDL_UnlockTexture (backbuffer);
	SDL_SetRenderDrawColor (renderer, 0,0,0,255);
	SDL_RenderClear (renderer);
	SDL_SetTextureAlphaMod (backbuffer, 0);
	SDL_RenderCopy (renderer, backbuffer, NULL, &dstrect);
	if (overlay)
		SDL_RenderCopy (renderer, overlay, NULL, &dstrect);
	SDL_RenderPresent (renderer);
}

/*!
 * \brief
 * Draws a frame to the window
 * 
 * \param time
 * Timestamp (same value as in TLN_UpdateFrame())
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
void TLN_DrawFrame (int time)
{
	TLN_BeginWindowFrame (time);
	while (TLN_DrawNextScanline ()){}
	TLN_EndWindowFrame ();
}

/*!
 * \brief
 * Returns the number of milliseconds since application start
 */
DWORD TLN_GetTicks (void)
{
	return SDL_GetTicks ();
}

/*!
 * \brief
 * Suspends execition for a fixed time
 * \param time Number of milliseconds to wait
 */
void TLN_Delay (DWORD time)
{
	SDL_Delay (time);
}

/* loads a BMP image as overlay */
static SDL_Surface* CreateOverlaySurface (const char* filename, int dstw, int dsth, int bpp)
{
	SDL_Surface* srcsurf;
	SDL_Surface* surface;
	SDL_Rect rect;
	int retval;

	/* crea supercicie destino */
	srcsurf = SDL_LoadBMP (filename);
	if (!srcsurf)
		return NULL;
	if (bpp == 16)
		surface = SDL_CreateRGBSurface (0, dstw,dsth,16, 0xF800,0x07E0,0x001F,0x0000);
	else
		surface = SDL_CreateRGBSurface (0, dstw,dsth,32, 0,0,0,0xFF000000);
	if (!surface)
		return NULL;

	/* replica patron original en mosaico detino */
	rect.w = srcsurf->w;
	rect.h = srcsurf->h;
	for (rect.y=0; rect.y<dsth; rect.y+=rect.h)
	{
		for (rect.x=0; rect.x<dstw; rect.x+=rect.w)
			retval = SDL_BlitSurface (srcsurf, NULL, surface, &rect);
	}

	SDL_FreeSurface (srcsurf);

	return surface;
}

/* basic horizontal blur emulating RF blurring */
static void hblur (BYTE* scan, int width, int height, int pitch)
{
	int x,y;
	BYTE *pixel;
	
	width -= 1;
	for (y=0; y<height; y++)
	{
		pixel = scan;
		for (x=0; x<width; x++)
		{
			pixel[0] = (pixel[0] + pixel[4]) >> 1;
			pixel[1] = (pixel[1] + pixel[5]) >> 1;
			pixel[2] = (pixel[2] + pixel[6]) >> 1;
			pixel += sizeof(DWORD);
		}
		scan += pitch;
	}
}

#endif
