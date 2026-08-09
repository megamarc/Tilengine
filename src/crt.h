#ifndef _CRT_H
#define _CRT_H

#include <stdbool.h>

#if TILEENGINE_HAVE_SDL2
#include "SDL2/SDL.h"
#endif

typedef enum
{
	CRT_SLOT,
	CRT_APERTURE,
	CRT_SHADOW,
}
CRTType;

typedef struct _CRTHandler* CRTHandler;

#ifdef __cplusplus
extern "C"{
#endif

#if TILEENGINE_HAVE_SDL2
	CRTHandler CRTCreate(SDL_Renderer* renderer, SDL_Texture* framebuffer,
                         CRTType type, int wnd_width, int wnd_height, bool blur);
	void CRTDraw(CRTHandler crt, void* pixels, int pitch, SDL_Rect* dstrect);
	void CRTSetRenderTarget(CRTHandler crt, SDL_Texture* framebuffer);
#endif
	void CRTIncreaseGlow(CRTHandler crt);
	void CRTDecreaseGlow(CRTHandler crt);
	void CRTSetBlur(CRTHandler crt, bool blur);
	void CRTDelete(CRTHandler crt);

#ifdef __cplusplus
}
#endif

#endif
