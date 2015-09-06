#include <stdlib.h>
#include "Shooter.h"
#include "Explosion.h"

#define MAX_HEIGHT	200

void EnemyTasks (Actor* actor);

void CreateEnemy (void)
{
	Actor* actor;
	int index = GetAvailableActor (ACTOR_ENEMY1, MAX_ENEMIES);

	if (index==-1)
		return;

	actor = SetActor (index, TYPE_ENEMY, WIDTH,rand()%200, 32,16, EnemyTasks);
	actor->vx = -(rand()%3 + 2);
	actor->life = 8;
	SetActorTimeout (actor, 1, 60);
	TLN_ConfigSprite (actor->index, spritesets[SPRITESET_MAIN], 0);
	TLN_SetSpritePicture (actor->index, 25);
}

void EnemyTasks (Actor* actor)
{
	/* animacion */
	if (GetActorTimeout (actor, 0))
	{
		int index = TLN_GetSpritePicture (actor->index);
		SetActorTimeout (actor, 0, 6);

		/* hacia arriba */
		if (actor->vy < 0)
		{
			if (index > 23)
				TLN_SetSpritePicture (actor->index, index - 1);
		}
		
		/* hacia abajo */
		else if (actor->vy > 0)
		{
			if (index < 27)
				TLN_SetSpritePicture (actor->index, index + 1);
		}

		/* centrado */
		else
		{
			if (index > 25)
				TLN_SetSpritePicture (actor->index, index - 1);
			else if (index < 25)
				TLN_SetSpritePicture (actor->index, index + 1);
		}
	}

	/* movimiento vertical */
	if (GetActorTimeout (actor, 1))
	{
		int res;
		SetActorTimeout (actor, 1, 60);
		res = rand()%3;
		if (res==0)
			actor->vy = -1;
		else if (res==1)
			actor->vy = 1;
		else
			actor->vy = 0;
	}

	/* limites de pantalla */
	if (actor->y < 0)
	{
		actor->y = 0;
		actor->vy = 0;
	}
	else if (actor->y > MAX_HEIGHT)
	{
		actor->y = MAX_HEIGHT;
		actor->vy = 0;
	}
	if (actor->x < -32)
		ReleaseActor (actor);
}

bool HitEnemy (Actor* actor, int power)
{
	actor->life -= power;
	if (actor->life < 1)
	{
		CreateExplosion (actor->index, actor->x,actor->y, sequences[SEQ_EXPLO1]);
		return true;
	}
	return false;
}

