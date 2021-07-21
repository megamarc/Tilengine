#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

int main (int argc, char* argv[])
{
	int x = 0;
	float sf = 0.1f;
	TLN_Spriteset spriteset;
	TLN_Spriteset axis;
	int index = 0;
	int frame = 0;
	bool show = true;

	TLN_Init(WIDTH, HEIGHT, 8, 80, 0);
	TLN_SetLoadPath("assets/racer");
	spriteset = TLN_LoadSpriteset("trees.png");
	axis = TLN_LoadSpriteset("axis.png");
	
	TLN_ConfigSprite(0, spriteset, 0);
	TLN_ConfigSprite(1, spriteset, 0);
	TLN_ConfigSprite(3, axis, 0);
	TLN_ConfigSprite(4, axis, 0);

	TLN_SetSpritePicture(0, index);
	TLN_SetSpritePicture(1, index);
	TLN_SetSpritePicture(3, 0);
	TLN_SetSpritePicture(4, 1);

	TLN_SetSpritePivot(0, 0, 0);
	TLN_SetSpritePivot(1, 0.5f, 1.0f);
	TLN_SetSpritePivot(4, 0.5f, 1.0f);

	TLN_SetSpritePosition(0, 8, 8);
	TLN_SetSpritePosition(1, WIDTH / 2, HEIGHT - 16);

	TLN_SetSpritePosition(3, 0, 0);
	TLN_SetSpritePosition(4, WIDTH / 2, HEIGHT - 16);

	TLN_LoadWorld("map.tmx", 0);

	TLN_CreateWindow(NULL, CWF_S1 | CWF_NEAREST);
	TLN_Delay(2000);
	while (TLN_ProcessWindow())
	{
		TLN_SetSpriteScaling(0, sf, sf);
		TLN_SetSpriteScaling(1, sf, sf);
		TLN_SetSpriteScaling(2, sf, sf);
		sf += 0.003f;

		if ((frame % 10) == 0)
		{
			show = !show;
			if (show)
				TLN_EnableLayer(3);
			else
				TLN_DisableLayer(3);
		}
		frame += 1;
		TLN_DrawFrame(0);
	}

	TLN_DeleteWindow();
	TLN_Deinit ();
	return 0;
}
