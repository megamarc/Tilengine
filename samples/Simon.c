#include "Tilengine.h"
#include "Simon.h"

typedef enum
{
	SIMON_IDLE,
	SIMON_WALKING,
	SIMON_JUMPING
}
SimonState;

typedef enum
{
	DIR_NONE,
	DIR_LEFT,
	DIR_RIGHT,
}
Direction;

TLN_Spriteset simon;
TLN_SequencePack sp;
TLN_Sequence walk;

int x,y;
int sy = 0;
int xworld;
SimonState state;
Direction direction;

void SimonInit (void)
{
	simon = TLN_LoadSpriteset ("Simon");
	sp = TLN_LoadSequencePack ("Simon.sqx");
	walk = TLN_FindSequence (sp, "walk");
	
	TLN_SetSpriteSet (0, simon);
	TLN_SetSpritePosition (0, x,y);
	
	SimonSetState (SIMON_IDLE);
	direction = DIR_RIGHT;
	x = 64;
	y = -48;
}

void SimonDeinit (void)
{
	TLN_DeleteSequencePack (sp);
	TLN_DeleteSpriteset (simon);
}

void SimonSetState (int s)
{
	if (state == s)
		return;

	state = s;
	switch (state)
	{
	case SIMON_IDLE:
		TLN_DisableSpriteAnimation (0);
		TLN_SetSpritePicture (0, 0);
		break;

	case SIMON_WALKING:
		TLN_SetSpriteAnimation (0, walk, 0);
		break;

	case SIMON_JUMPING:
		TLN_DisableSpriteAnimation (0);
		TLN_SetSpritePicture (0, 7);
		sy = -18;
		break;
	}
}

void SimonTasks (void)
{
	int y2, s0, c;
	Direction input = 0;
	bool jump = false;

	/* input */
	if (TLN_GetInput (INPUT_LEFT))
		input = DIR_LEFT;
	else if (TLN_GetInput (INPUT_RIGHT))
		input = DIR_RIGHT;
	if (TLN_GetInput (INPUT_A))
		jump = true;

	/* direction flags */
	if (input==DIR_RIGHT && direction==DIR_LEFT)
	{
		direction = input;
		TLN_SetSpriteFlags (0, 0);
	}
	if (input==DIR_LEFT && direction==DIR_RIGHT)
	{
		direction = input;
		TLN_SetSpriteFlags (0, FLAG_FLIPX);
	}

	switch (state)
	{
	case SIMON_IDLE:
		if (input)
			SimonSetState (SIMON_WALKING);
		break;

	case SIMON_WALKING:
	case SIMON_JUMPING:
		if (input == DIR_RIGHT)
		{
			if (x < 120)
				x++;
			else
				xworld++;
		}
		else if (input == DIR_LEFT)
			xworld--;

		if (state==SIMON_WALKING && !input)
			SimonSetState (SIMON_IDLE);
		break;
	}

	if (jump && state!=SIMON_JUMPING)
		SimonSetState (SIMON_JUMPING);

	/* gravity */
	s0 = sy;
	if (sy < 10)
		sy++;
	y2 = y + (sy>>2);

	/* check tiles below */
	for (c=8; c<24; c+=8)
	{
		TLN_TileInfo ti;
		TLN_GetLayerTile (0, x+c+xworld, y2+48, &ti);
		if (ti.index)
		{
			if (ti.yoffset!=0)
				sy = 0;
			sy = 0;
			y2 -= ti.yoffset;
			break;
		}
	}

	if (s0>0 && sy==0)
		SimonSetState (SIMON_IDLE);
	y = y2;

	TLN_SetSpritePosition (0, x,y);
}

int SimonGetPosition (void)
{
	return xworld;
}
