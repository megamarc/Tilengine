#ifndef _CRT_H
#define _CRT_H

#include <stdbool.h>
#include "SDL2/SDL.h"

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

	CRTHandler CRTCreate(SDL_Renderer* renderer, SDL_Texture* framebuffer, CRTType type, int wnd_width, int wnd_height, bool blur);
	void CRTDraw(CRTHandler crt, void* pixels, int pitch, SDL_Rect* dstrect);
	void CRTSetRenderTarget(CRTHandler crt, SDL_Texture* framebuffer);
	void CRTIncreaseGlow(CRTHandler crt);
	void CRTDecreaseGlow(CRTHandler crt);
	void CRTSetBlur(CRTHandler crt, bool blur);
	void CRTDelete(CRTHandler crt);

#ifdef __cplusplus
}
#endif

#endif