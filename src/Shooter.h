#ifndef _SHOOTER_H
#define _SHOOTER_H

#include "Tilengine.h"
#include "Sin.h"
#include "Actor.h"

#define WIDTH	400
#define HEIGHT	240

/* spritesets */
enum
{
	SPRITESET_MAIN,
	SPRITESET_HELLARM,
	MAX_SPRITESET
};

/* tipos de actores */
enum
{
	TYPE_SHIP = 1,
	TYPE_CLAW,
	TYPE_BLADEB,
	TYPE_BLADES,
	TYPE_ENEMY,
	TYPE_EXPLOSION,
};

#define MAX_BULLETS		20
#define MAX_ENEMIES		10

/* actores */
enum
{
	ACTOR_SHIP,
	ACTOR_CLAW1,
	ACTOR_CLAW2,
	ACTOR_ENEMY1,
	ACTOR_BOSS = ACTOR_ENEMY1 + MAX_ENEMIES,
	ACTOR_BULLET1 = ACTOR_BOSS + 8,
	MAX_ACTOR = ACTOR_BULLET1 + MAX_BULLETS,
};

/* animaciones */
enum
{
	SEQ_CLAW,
	SEQ_BLADE1,
	SEQ_BLADE2,
	SEQ_EXPLO1,
	SEQ_EXPLO2,
	MAX_SEQ
};

extern TLN_Sequence sequences[MAX_SEQ];
extern TLN_Spriteset spritesets[MAX_SPRITESET];
extern unsigned int time;

#endif