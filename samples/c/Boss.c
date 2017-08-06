#include <stdlib.h>
#include "Shooter.h"
#include "Boss.h"
#include "Explosion.h"

/* partes del jefe */
enum
{
	PART_CORE,
	PART_UPPER_JOINT,
	PART_UPPER_ARM,
	PART_UPPER_GUN,
	PART_LOWER_JOINT,
	PART_LOWER_ARM,
	PART_LOWER_GUN,
	MAX_PART
};

struct
{
	int picture;
	uint16_t flags;
}
static BossGfx[MAX_PART] =
{
	{7, 0},
	{6, 0},
	{5, 0},
	{0, 0},
	{6, FLAG_FLIPY},
	{5, FLAG_FLIPY},
	{0, 0},
};

typedef struct
{
	Actor* parts[MAX_PART];
}
Boss;

void BossTasks (Actor* actor);

void CreateBoss (void)
{
	TLN_SpriteInfo si;
	const int x = WIDTH + 80;
	const int y = 100;
	Actor* actor = GetActor (ACTOR_BOSS);
	Boss* boss = (Boss*)actor->usrdata;
	int c;	

	for (c=0; c<MAX_PART; c++)
	{
		const int index = ACTOR_BOSS + c;
		Actor* part;
		TLN_GetSpriteInfo (spritesets[SPRITESET_HELLARM], BossGfx[c].picture, &si);
		part = SetActor (index, TYPE_ENEMY, x,y, si.w, si.h, c==0? BossTasks:NULL);
		boss->parts[c] = part;
		TLN_ConfigSprite (part->index, spritesets[SPRITESET_HELLARM], BossGfx[c].flags);
		TLN_SetSpritePicture (part->index, BossGfx[c].picture);
	}
	actor->vx = -1;
}

void BossTasks (Actor* actor)
{
	Boss* boss = (Boss*)actor->usrdata;

	boss->parts[PART_UPPER_JOINT]->x = actor->x + 31;
	boss->parts[PART_UPPER_JOINT]->y = actor->y - 25;
	boss->parts[PART_UPPER_ARM  ]->x = actor->x - 17;
	boss->parts[PART_UPPER_ARM  ]->y = actor->y - 56;
	boss->parts[PART_UPPER_GUN  ]->x = boss->parts[PART_UPPER_ARM]->x - 16;
	boss->parts[PART_UPPER_GUN  ]->y = boss->parts[PART_UPPER_ARM]->y + 8;
	boss->parts[PART_LOWER_JOINT]->x = actor->x + 31;
	boss->parts[PART_LOWER_JOINT]->y = actor->y + 17;
	boss->parts[PART_LOWER_ARM  ]->x = actor->x - 17;
	boss->parts[PART_LOWER_ARM  ]->y = actor->y + 32;
	boss->parts[PART_LOWER_GUN  ]->x = boss->parts[PART_LOWER_ARM]->x - 16;
	boss->parts[PART_LOWER_GUN  ]->y = boss->parts[PART_LOWER_ARM]->y + 8;

	/* detiene */
	if (actor->state==1 && actor->x <= WIDTH - 120)
	{
		actor->vx = 0;
		actor->state = 2;
		SetActorTimeout (actor, 0, 200);
	}

	if (actor->state==2 && GetActorTimeout(actor, 0))
	{
		actor->state = 3;
		SetActorTimeout (actor, 0, 120);
	}

	/* destruye */
	if (actor->state == 3)
	{
		if (!GetActorTimeout (actor, 0))
		{
			actor->x += (rand()%3 - 1);
			actor->y += (rand()%3 - 1);
		}
		else
		{
			actor->vx = 1;
			actor->vy = 1;
		}

		if (GetActorTimeout (actor, 1))
		{
			Actor* part;
			int index;
			int expl;

			SetActorTimeout (actor, 1, 3);
			index = rand() % 7;
			part = boss->parts[index];
			expl = GetAvailableActor (ACTOR_BULLET1, MAX_BULLETS);
			if (expl != -1)
			{
				int x,y;
				Actor* explosion;
				TLN_SpriteInfo si;
				TLN_GetSpriteInfo (spritesets[SPRITESET_MAIN], BossGfx[index].picture, &si);
				x = part->x + (rand()%(si.w - 0)) - (si.w >> 1);
				y = part->y + (rand()%(si.h - 0)) - (si.h >> 1);
				explosion = CreateExplosion (expl, x,y, sequences[SEQ_EXPLO1]);
				explosion->vx = -2;
				explosion->vy = 0;
			}
		}

		/* eliminar */
		if (actor->y > 300)
		{
			int c;
			for (c=0; c<MAX_PART; c++)
				boss->parts[c]->state = 0;
		}
	}
}
