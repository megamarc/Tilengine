#include <stdlib.h>
#include "Shooter.h"
#include "Ship.h"
#include "Enemy.h"
#include "Explosion.h"

/* datos extra ship */
typedef struct
{
	int lives;
	Actor* claws[2];
	char weapon;
	bool claw;
}
Ship;

/* datos extra claw */
typedef struct
{
	int radius;
	int angle;
}
Claw;

void ShipTasks (Actor* actor);
void ClawTasks (Actor* actor);
void ShotTasks (Actor* actor);

/*
*******************************************************************************
	Ship
*******************************************************************************
*/

Actor* CreateShip (void)
{
	Actor* actor;
	Ship* ship;
	
	actor = SetActor (ACTOR_SHIP, TYPE_SHIP, 100,100, 32,16, ShipTasks);
	TLN_ConfigSprite (actor->index, spritesets[SPRITESET_MAIN], 0);
	TLN_SetSpritePicture (actor->index, 3);

	ship = (Ship*)actor->usrdata;
	ship->claws[0] = CreateClaw (ACTOR_CLAW1);
	ship->claws[1] = CreateClaw (ACTOR_CLAW2);
	ship->claw = true;

	return actor;
}

void ShipTasks (Actor* actor)
{
	Ship* ship = (Ship*)actor->usrdata;

	if (TLN_GetInput(INPUT_LEFT) && actor->x > 0)
		actor->vx = -2;
	else if (TLN_GetInput(INPUT_RIGHT) && actor->x < 390)
		actor->vx = 2;
	else
		actor->vx = 0;

	if (TLN_GetInput(INPUT_UP) && actor->y > 0)
		actor->vy = -2;
	else if (TLN_GetInput(INPUT_DOWN) && actor->y < 224)
		actor->vy = 2;
	else
		actor->vy = 0;

	/* movimiento */
	if (GetActorTimeout (actor, 0))
	{
		int index = TLN_GetSpritePicture (actor->index);
		SetActorTimeout (actor, 0, 6);

		/* hacia arriba */
		if (actor->vy < 0)
		{
			if (index > 0)
				TLN_SetSpritePicture (actor->index, index - 1);
		}
		
		/* hacia abajo */
		else if (actor->vy > 0)
		{
			if (index < 6)
				TLN_SetSpritePicture (actor->index, index + 1);
		}

		/* centrado */
		else
		{
			if (index > 3)
				TLN_SetSpritePicture (actor->index, index - 1);
			else if (index < 3)
				TLN_SetSpritePicture (actor->index, index + 1);
		}

		/* izquierda */
		if (actor->vx < 0)
		{
			if (actor->x < 0)
				actor->x = 0;
		}

		/* derecha */
		if (actor->vx > 0)
		{
			if (actor->x > 396)
				actor->x = 396;
		}
	}

	/* disparo */
	if (GetActorTimeout (actor, 1) && TLN_GetInput(INPUT_A))
	{
		SetActorTimeout (actor, 1, 10);
		CreateShot (TYPE_BLADEB, actor->x+32, actor->y + rand()%10 - 5);
		if (ship->claw)
		{
			CreateShot (TYPE_BLADES, ship->claws[0]->x + 16, ship->claws[0]->y);
			CreateShot (TYPE_BLADES, ship->claws[1]->x + 16, ship->claws[1]->y);
		}
	}
}

/*
*******************************************************************************
	Claws
*******************************************************************************
*/

Actor* CreateClaw (int id)
{
	Actor* actor = SetActor (id, TYPE_CLAW, 0,0, 16,16, ClawTasks);
	Claw* claw = (Claw*)actor->usrdata;

	TLN_ConfigSprite (actor->index, spritesets[SPRITESET_MAIN], 0);
	TLN_SetSpriteAnimation (actor->index, sequences[SEQ_CLAW], 0);

	claw->angle = id==ACTOR_CLAW1? 360:180;
	claw->radius = 0;

	return actor;
}

void ClawTasks (Actor* actor)
{
	Claw* claw = (Claw*)actor->usrdata;
	Actor* parent = GetActor (ACTOR_SHIP);

	/* extensión */
	if (claw->radius < 32)
		claw->radius++;

	/* giro */
	else
	{
		claw->angle -= 3;
		if (claw->angle == 0)
			claw->angle = 360;
	}

	/* posicion */
	actor->x = parent->x + CalcSin(claw->angle, claw->radius) + 8;
	actor->y = parent->y + CalcCos(claw->angle, claw->radius);
}

/*
*******************************************************************************
	Player shots
*******************************************************************************
*/

Actor* CreateShot (int type, int x, int y)
{
	Actor* actor;
	int index = GetAvailableActor (ACTOR_BULLET1, MAX_BULLETS);
	int size;
	int seq;

	if (index==-1)
		return NULL;

	if (type==TYPE_BLADEB)
	{
		seq = SEQ_BLADE1;
		size = 32;
	}
	else
	{
		seq = SEQ_BLADE2;
		size = 16;
	}

	actor = SetActor (index, type, x,y, size,size, ShotTasks);
	actor->vx = 8;
	actor->vy = 0;
	TLN_ConfigSprite (actor->index, spritesets[SPRITESET_MAIN], 0);
	TLN_SetSpriteAnimation (actor->index, sequences[seq], 0);

	return actor;
}

void ShotTasks (Actor* actor)
{
	int c,last;
	int power;

	/* escape de pantalla */
	if (actor->x > 430)
	{
		ReleaseActor (actor);
		return;
	}

	/* colisiones */
	if (actor->type == TYPE_BLADEB)
		power = 2;
	else
		power = 1;
	last = ACTOR_ENEMY1 + MAX_ENEMIES;
	for (c=ACTOR_ENEMY1; c<last; c++)
	{
		Actor* target = GetActor (c);
		if (target->state!=0 && target->type==TYPE_ENEMY && CheckActorCollision (actor, target))
		{
			HitEnemy (target, power);
			{
				Actor* explo = CreateExplosion (actor->index, actor->x,actor->y, sequences[SEQ_EXPLO2]);
				actor->vx = target->vx;
				actor->vy = target->vy;
			}
			return;
		}
	}

	/* colisiones jefe */
	last = ACTOR_BOSS + 8;
	for (c=ACTOR_BOSS; c<last; c++)
	{
		Actor* target = GetActor (c);
		if (target->state!=0 && CheckActorCollision (actor, target))
		{
			Actor* explo = CreateExplosion (actor->index, actor->x,actor->y, sequences[SEQ_EXPLO2]);
			actor->vx = target->vx;
			actor->vy = target->vy;
			return;
		}
	}
}

