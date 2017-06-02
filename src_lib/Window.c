/*
Tilengine - 2D Graphics library with raster effects
Copyright (c) 2015-2017 Marc Palacios Domenech (megamarc@hotmail.com)
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

#include <string.h>
#include "SDL2/SDL.h"
#include "Tilengine.h"

/* linear interploation */
#define lerp(x, x0,x1, fx0,fx1) \
	fx0 + (fx1-fx0)*(x-x0)/(x1-x0)

static SDL_Window*   window;
static SDL_Renderer* renderer;
static SDL_Texture*	 backbuffer;
static SDL_Surface*	 resize_half_width;
static SDL_Thread*   thread;
static SDL_mutex*	 lock;
static SDL_cond*	 cond;
static SDL_Joystick* joy;
static SDL_Rect		 dstrect;

static bool			 done;
static int			 wnd_width;
static int			 wnd_height;
static int			 instances = 0;
static uint8_t*		 rt_pixels;
static int			 rt_pitch;

static int			inputs;
static int			last_key;

/* CRT effect */
struct
{
	bool enable;
	bool gaussian;
	bool table[256];
	TLN_Overlay overlay_id;
	SDL_Texture* glow;
	SDL_Texture* overlay;
	SDL_Surface* overlays[TLN_MAX_OVERLAY];
	SDL_Surface* blur;
	uint8_t glow_factor;
}
static crt;

/* Window manager */
typedef struct
{
	int width;
	int height;
	TLN_WindowFlags flags;
	char file_overlay[128];
	volatile int retval;
}
WndParams;

static WndParams wnd_params;

#define RED		0xFF,0x00,0x00,0xFF
#define GREEN	0x00,0xFF,0x00,0xFF
#define BLUE	0x00,0x00,0xFF,0xFF
#define BLACK	0x00,0x00,0x00,0xFF

static char pattern_aperture[] =
{
	RED,   GREEN, BLUE,  RED,   GREEN, BLUE,
	RED,   GREEN, BLUE,  BLACK, BLACK, BLACK,
	RED,   GREEN, BLUE,  RED,   GREEN, BLUE,
	BLACK, BLACK, BLACK, RED,   GREEN, BLUE
};

static char pattern_shadowmask[] =
{
	RED, RED, GREEN, GREEN, BLUE, BLUE,
	GREEN, BLUE, BLUE, RED, RED, GREEN
};

static char pattern_scanlines[] =
{
	RED, GREEN, BLUE,
	RED, GREEN, BLUE,
	BLACK, BLACK, BLACK,
	BLACK, BLACK, BLACK,
};

/* local prototypes */
static bool CreateWindow (void);
static void DeleteWindow (void);
static SDL_Surface* CreateOverlaySurface (const char* filename, int dstw, int dsth);
static SDL_Texture* LoadTexture (char* filename);
static void hblur (uint8_t* scan, int width, int height, int pitch);
static void Downsample2 (uint8_t* src, uint8_t* dst, int width, int height, int src_pitch, int dst_pitch);
static void BuildFullOverlay (SDL_Texture* texture, SDL_Surface* pattern, uint8_t factor);

/* external prototypes */
void GaussianBlur (uint8_t* src, uint8_t* dst, int width, int height, int pitch, int radius);

/* create window delegate */
static bool CreateWindow (void)
{
	SDL_DisplayMode mode;
	SDL_Surface* surface = NULL;
	int factor;
	int rflags;
	char quality;
	Uint32 format = 0;
	void* pixels;
	int pitch;

	/* obtiene tamaño escritorio y tamaño máximo de ventana*/
	SDL_GetDesktopDisplayMode (0, &mode);
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
		rflags = CWF_FULLSCREEN;
		wnd_width = mode.w;
		wnd_height = wnd_width*wnd_params.height/wnd_params.width;
		if (wnd_height > mode.h)
		{
			wnd_height = mode.h;
			wnd_width = wnd_height*wnd_params.width/wnd_params.height;
		}
		factor = wnd_height / wnd_params.height;

		dstrect.x = (mode.w - wnd_width) >> 1;
		dstrect.y = (mode.h - wnd_height) >> 1;
		dstrect.w = wnd_width;
		dstrect.h = wnd_height;
	}

	/* ventana */
	window = SDL_CreateWindow ("Tilengine window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wnd_width,wnd_height, rflags);
	if (!window)
	{
		DeleteWindow ();
		return false;
	}

	/* contexto de render */
	rflags = SDL_RENDERER_ACCELERATED;
	if (wnd_params.flags & CWF_VSYNC)
		rflags |= SDL_RENDERER_PRESENTVSYNC;
	renderer = SDL_CreateRenderer (window, -1, rflags);
	if (!renderer)
	{
		DeleteWindow ();
		return false;
	}
	quality = 1;
	SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, &quality);
	
	/* textura para recibir los pixeles de Tilengine */
	format = SDL_PIXELFORMAT_ARGB8888;
	backbuffer = SDL_CreateTexture (renderer, format, SDL_TEXTUREACCESS_STREAMING, wnd_params.width,wnd_params.height);
	if (!backbuffer)
	{
		DeleteWindow ();
		return false;
	}
	SDL_SetTextureAlphaMod (backbuffer, 0);

	/* texturas CRT effect */
	crt.overlay_id = TLN_OVERLAY_NONE;
	crt.glow = SDL_CreateTexture (renderer, format, SDL_TEXTUREACCESS_STREAMING, wnd_params.width/2,wnd_params.height/2);
	crt.blur = SDL_CreateRGBSurface (0, wnd_params.width/2,wnd_params.height/2,32, 0,0,0,0);
	SDL_SetTextureBlendMode (crt.glow, SDL_BLENDMODE_ADD);
	SDL_LockTexture (crt.glow, NULL, &pixels, &pitch);
	memset (pixels, 0, pitch*wnd_params.height/2);
	SDL_UnlockTexture (crt.glow);
	crt.overlay = SDL_CreateTexture (renderer, format, SDL_TEXTUREACCESS_STREAMING, wnd_width, wnd_height);
	SDL_SetTextureBlendMode (crt.overlay, SDL_BLENDMODE_MOD);
	crt.overlays[TLN_OVERLAY_APERTURE  ] = SDL_CreateRGBSurfaceFrom (pattern_aperture, 6,4,32,24, 0,0,0,0);
	crt.overlays[TLN_OVERLAY_SHADOWMASK] = SDL_CreateRGBSurfaceFrom (pattern_shadowmask, 6,2,32,24, 0,0,0,0);
	crt.overlays[TLN_OVERLAY_SCANLINES ] = SDL_CreateRGBSurfaceFrom (pattern_scanlines, 3,4,32,12, 0,0,0,0);
	if (wnd_params.file_overlay[0])
		crt.overlays[TLN_OVERLAY_CUSTOM] = SDL_LoadBMP (wnd_params.file_overlay);

	TLN_EnableCRTEffect (TLN_OVERLAY_APERTURE, 128, 192, 0,64, 64,128, false, 255);

	/* temporal downsample surface */
	resize_half_width = SDL_CreateRGBSurface (0, wnd_params.width/2, wnd_params.height, 32, 0,0,0,0);
	memset (resize_half_width->pixels, 255, resize_half_width->pitch * resize_half_width->h);
	
	if (wnd_params.flags & CWF_FULLSCREEN)
		SDL_ShowCursor (SDL_DISABLE);

	done = false;

	/* joystick */
	if (SDL_NumJoysticks () > 0)
	{
	    joy = SDL_JoystickOpen(0);
		SDL_JoystickEventState (SDL_ENABLE);
	}

	return true;
}

/* destroy window delegate */
static void DeleteWindow (void)
{
	int c;

    if (SDL_JoystickGetAttached(joy))
        SDL_JoystickClose(joy);

	/* CRT effect resources */
	SDL_DestroyTexture (crt.glow);
	SDL_DestroyTexture (crt.overlay);
	SDL_FreeSurface (crt.blur);
	for (c=0; c<TLN_MAX_OVERLAY; c++)
	{
		if (crt.overlays[c])
			SDL_FreeSurface (crt.overlays[c]);
	}
	SDL_FreeSurface (resize_half_width);

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
	SDL_SetWindowTitle (window, title);
}

static int WindowThread (void* data)
{
	int time = 0;
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
		TLN_DrawFrame (time++);
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

	if (SDL_Init (SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) != 0)
		return false;

	/* fill parameters for window creation */
	wnd_params.width = TLN_GetWidth ();
	wnd_params.height = TLN_GetHeight ();
	wnd_params.flags = flags|CWF_VSYNC;
	if (overlay)
		strcpy (wnd_params.file_overlay, overlay);

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

	if (SDL_Init (SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) != 0)
		return false;

	/* fill parameters for window creation */
	wnd_params.retval = 0;
	wnd_params.width = TLN_GetWidth ();
	wnd_params.height = TLN_GetHeight ();
	wnd_params.flags = flags|CWF_VSYNC;
	if (overlay)
		strcpy (wnd_params.file_overlay, overlay);

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
			case SDLK_BACKSPACE: crt.enable = !crt.enable; break;
			case SDLK_RETURN:
				if (keybevt->keysym.mod & KMOD_ALT)
				{
					DeleteWindow ();
					wnd_params.flags ^= CWF_FULLSCREEN;
					CreateWindow ();
				}
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
 * Enables or disables a simple horizontal blur effect to emulate noisy composite video
 * 
 * \param mode
 * Enable or disable effect
 * 
 * \remarks
 * As of release 1.12, this function has been deprecated and doesn't have effect anymore. It has been
 * left only for ABI compatibility. Use the new TLN_EnableCRTEffect instead.
 * \see
 * TLN_EnableCRTEffect()
 */
void TLN_EnableBlur (bool mode)
{
}

/*!
 * \brief
 * Enables CRT simulation post-processing effect to give true retro appeareance
 * 
 * \param overlay
 * One of the enumerated TLN_Overlay types. Choosing TLN_OVERLAY_CUSTOM selects the image passed when calling TLN_CreateWindow
 *
 * \param overlay_factor
 * Blend factor for overlay image. 0 is full transparent (no effect), 255 is full blending
 
 * \param threshold
 * Middle point of the brightness mapping function
 *
 * \param v0
 * output brightness for input brightness = 0
 *
 * \param v1
 * output brightness for input brightness = threshold
 *
 * \param v2
 * output brightness for input brightness = threshold
 *
 * \param v3
 * output brightness for input brightness = 255
 *
 * \param blur
 * adds gaussian blur to brightness overlay, softens image
 *
 * \param glow_factor
 * blend addition factor for brightness overlay. 0 is not addition, 255 is full addition
 * 
 * This function combines various effects to simulate the output of a CRT monitor with low CPU/GPU usage.
 * A small horizontal blur is added to the frame, simulating the continuous output of a RF modulator where adjacent pixels got mixed.
 * Many graphic designers use this feature where alternating vertical lines are used to create the illusion of more colors or blending.
 * An secondary image is created with overbright pixels. In a real CRT, brighter colors bleed into surrounding area: the pixel size depends
 * somewhat on its brightness. The threshold and v0 to v3 parametes define a two-segment linear mapping between source and destination 
 * brightness for the overlay. Optionally the overlay can be softened more using a slight gaussian blur filter te create a kind of "bloom"
 * effect, and finally it is added on top of the frame with the glow_factor value.
 * 
 * \see
 * TLN_CreateWindow(),TLN_DisableCRTEffect()
 */ 
void TLN_EnableCRTEffect (TLN_Overlay overlay, uint8_t overlay_factor, uint8_t threshold, uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, bool blur, uint8_t glow_factor)
{
	int c;

	crt.enable = true;
	crt.gaussian = blur;
	crt.glow_factor = glow_factor;
	SDL_SetTextureAlphaMod (crt.glow, glow_factor);

	for (c=0; c<threshold; c++)
		crt.table[c] = lerp (c, 0,threshold, v0,v1);
	for (c=threshold; c<256; c++)
		crt.table[c] = lerp (c, threshold,255, v2,v3);

	if (crt.overlay_id != overlay)
	{
		if (crt.overlays[overlay] != NULL)
		{
			BuildFullOverlay (crt.overlay, crt.overlays[overlay], 255 - overlay_factor);
			crt.overlay_id = overlay;
		}
		else
			crt.overlay_id = TLN_OVERLAY_NONE;
	}
}

/*!
 * \brief
 * Disables the CRT post-processing effect
 * 
 * \see
 * TLN_EnableCRTEffect()
 */ 
void TLN_DisableCRTEffect (void)
{
	crt.enable = false;
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
	/* pixeles con threshold */
	if (crt.enable && crt.glow_factor != 0)
	{
		const int dst_width = wnd_params.width / 2;
		const int dst_height = wnd_params.height / 2;
		uint8_t* pixels_glow;
		int pitch_glow;
		int x,y;

		/* downscale backbuffer */
		SDL_LockTexture (crt.glow, NULL, &pixels_glow, &pitch_glow);
		Downsample2 (rt_pixels, pixels_glow, wnd_params.width,wnd_params.height, rt_pitch, pitch_glow);

		/* replace color vales with LUT mapped */
		for (y=0; y<dst_height; y++)
		{
			uint8_t* pixel = pixels_glow + y*pitch_glow;
			for (x=0; x<dst_width; x++)
			{
				pixel[0] = *(crt.table + pixel[0]);
				pixel[1] = *(crt.table + pixel[1]);
				pixel[2] = *(crt.table + pixel[2]);
				pixel[3] = 255;
				pixel += sizeof(uint32_t);
			}
		}

		/* apply gaussian blur (opitional) */
		if (crt.gaussian)
			GaussianBlur (pixels_glow, crt.blur->pixels, dst_width,dst_height,pitch_glow, 2);

		SDL_UnlockTexture (crt.glow);
	}

	/* horizontal blur in-place */
	if (crt.enable)
		hblur (rt_pixels, wnd_params.width, wnd_params.height, rt_pitch);

	/* end frame and apply overlay */
	SDL_UnlockTexture (backbuffer);

	SDL_RenderClear (renderer);
	SDL_RenderCopy (renderer, backbuffer, NULL, &dstrect);

	if (crt.enable)
	{
		if (crt.overlay_id != TLN_OVERLAY_NONE)
			SDL_RenderCopy (renderer, crt.overlay, NULL, &dstrect);
		if (crt.glow_factor != 0)
			SDL_RenderCopy (renderer, crt.glow, NULL, &dstrect);
	}
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

#define blendfunc(t,a,b) *(t  + ((a)<<8) + (b))
extern uint8_t* SelectBlendTable (TLN_Blend mode, uint8_t factor);

/* fills full-frame overlay texture with repeated pattern */
static void BuildFullOverlay (SDL_Texture* texture, SDL_Surface* pattern, uint8_t factor)
{
	SDL_Surface* src_surface;
	SDL_Surface* dst_surface;
	SDL_Rect rect;
	uint8_t* pixels = NULL;
	uint8_t* add_table = SelectBlendTable (BLEND_ADD, 255);
	int pitch = 0;
	int x,y;

	/* create auxiliar surfaces */
	src_surface = SDL_CreateRGBSurface (0, pattern->w, pattern->h, 32, 0,0,0,0);
	dst_surface = SDL_CreateRGBSurface (0, wnd_width, wnd_height, 32, 0,0,0,0);

	/* modulate overlay brightness in source surface */
	for (y=0; y<pattern->h; y++)
	{
		uint8_t* srcpixel = (uint8_t*)pattern->pixels + y*pattern->pitch;
		uint8_t* dstpixel = (uint8_t*)src_surface->pixels + y*src_surface->pitch;
		for (x=0; x<pattern->w; x++)
		{
			dstpixel[0] = blendfunc(add_table, srcpixel[0], factor);
			dstpixel[1] = blendfunc(add_table, srcpixel[1], factor);
			dstpixel[2] = blendfunc(add_table, srcpixel[2], factor);
			dstpixel[3] = 255;
			srcpixel += sizeof(uint32_t);
			dstpixel += sizeof(uint32_t);
		}
	}

	/* fill destination surface with mosaic of modulated source surface */
	rect.w = pattern->w;
	rect.h = pattern->h;
	for (rect.y=0; rect.y<dst_surface->h; rect.y+=rect.h)
	{
		for (rect.x=0; rect.x<dst_surface->w; rect.x+=rect.w)
			SDL_BlitSurface (src_surface, NULL, dst_surface, &rect);
	}

	/* copy pixels into final texture */
	SDL_LockTexture (texture, NULL, &pixels, &pitch);
	memcpy (pixels, dst_surface->pixels, pitch*dst_surface->h);
	SDL_UnlockTexture (texture);
	
	/* release resources */
	SDL_FreeSurface (dst_surface);
	SDL_FreeSurface (src_surface);
}

/* basic horizontal blur emulating RF blurring */
static void hblur (uint8_t* scan, int width, int height, int pitch)
{
	int x,y;
	uint8_t *pixel;
	
	width -= 1;
	for (y=0; y<height; y++)
	{
		pixel = scan;
		for (x=0; x<width; x++)
		{
			pixel[0] = (pixel[0] + pixel[4]) >> 1;
			pixel[1] = (pixel[1] + pixel[5]) >> 1;
			pixel[2] = (pixel[2] + pixel[6]) >> 1;
			pixel += sizeof(uint32_t);
		}
		scan += pitch;
	}
}

/* resample rápido dividio 2 */
static void Downsample2 (uint8_t* src, uint8_t* dst, int width, int height, int src_pitch, int dst_pitch)
{
	uint8_t* src_pixel;
	uint8_t* dst_pixel;
	int x,y;
	const int dst_width = width/2;
	const int dst_height = height/2;

	/* de src horizontal/2 en tmp */
	for (y=0; y<height; y++)
	{
		src_pixel = src + (y*src_pitch);
		dst_pixel = (uint8_t*)resize_half_width->pixels + (y*resize_half_width->pitch);
		for (x=0; x<dst_width; x++)
		{
			dst_pixel[0] = (src_pixel[0] + src_pixel[4]) >> 1;
			dst_pixel[1] = (src_pixel[1] + src_pixel[5]) >> 1;
			dst_pixel[2] = (src_pixel[2] + src_pixel[6]) >> 1;
			src_pixel += 2*sizeof(uint32_t);
			dst_pixel += sizeof(uint32_t);
		}
	}

	/* de tmp a vertical/2 en dst */
	for (y=0; y<dst_height; y++)
	{
		const int index0 = resize_half_width->pitch + 0;
		const int index1 = resize_half_width->pitch + 1;
		const int index2 = resize_half_width->pitch + 2;

		src_pixel = (uint8_t*)resize_half_width->pixels + ((y << 1)*resize_half_width->pitch);
		dst_pixel = dst + (y*dst_pitch);
		for (x=0; x<dst_width; x++)
		{
			dst_pixel[0] = (src_pixel[0] + src_pixel[index0]) >> 1;
			dst_pixel[1] = (src_pixel[1] + src_pixel[index1]) >> 1;
			dst_pixel[2] = (src_pixel[2] + src_pixel[index2]) >> 1;
			src_pixel += sizeof(uint32_t);
			dst_pixel += sizeof(uint32_t);
		}
	}
}

#endif
