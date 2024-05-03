#include "Tilengine.h"

int main(int argc, char* argv[])
{
	int x = 0;
	int y = 0;

	TLN_Init(320, 240, 0, 1, 0);
	TLN_Spriteset spriteset = TLN_LoadSpriteset("assets/tf4/FireLeo.png");
	TLN_ConfigSprite(0, spriteset, FLAG_ROTATE);
	TLN_SetSpritePicture(0, TLN_FindSpritesetSprite(spriteset, "claw1"));
	TLN_CreateWindow(NULL, 0);
	while (TLN_ProcessWindow())
	{
		TLN_DrawFrame(0);
		if (TLN_GetInput(INPUT_UP))
			y -= 1;
		else if (TLN_GetInput(INPUT_DOWN))
			y += 1;
		if (TLN_GetInput(INPUT_LEFT))
			x -= 1;
		else if (TLN_GetInput(INPUT_RIGHT))
			x += 1;
		TLN_SetSpritePosition(0, x, y);
	}
	TLN_DeleteSpriteset(spriteset);
	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
