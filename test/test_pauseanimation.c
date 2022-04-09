#include "Tilengine.h"

int main(int argc, char* argv[])
{
	TLN_Init(400, 240, 0, 1, 0);
	TLN_SetLoadPath("assets/forest");

	TLN_Spriteset ss = TLN_LoadSpriteset("atlas");
	TLN_Sequence sequence = TLN_CreateSpriteSequence(NULL, ss, "player-idle/player-idle-", 5);
	TLN_ConfigSprite(0, ss, 0);
	TLN_SetSpriteAnimation(0, sequence, 0);

	uint32_t t0 = TLN_GetTicks();
	TLN_CreateWindow(NULL, CWF_NEAREST);
	while (TLN_ProcessWindow())
	{
		uint32_t elapsed = TLN_GetTicks() - t0;
		if (elapsed >= 2000 && elapsed < 4000)
			TLN_PauseSpriteAnimation(0);
		else if (elapsed >= 4000)
			TLN_ResumeSpriteAnimation(0);
		
		TLN_DrawFrame(0);
	}

	TLN_DeleteWindow();
	TLN_Deinit();
	return 0;
}
