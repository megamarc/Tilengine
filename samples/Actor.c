#include <stdlib.h>
#include <string.h>
#include "Tilengine.h"
#include "Actor.h"

/* local variables */
static Actor* actors;
static int count = 0;
static unsigned int time;

/* local prototypes */
static void TasksActor (Actor* actor);

/* create actors array */
bool CreateActors (int num)
{
	int size = sizeof(Actor)*num;
	
	if (actors != NULL)
		return false;

	actors = malloc (size);
	if (actors)
	{
		count = num;
		memset (actors, 0, size);
		return true;
	}
	return false;
}

/* delete actors array */
bool DeleteActors (void)
{
	if (actors != NULL)
	{
		free (actors);
		actors = NULL;
		count = 0;
		return true;
	}
	return false;
}

/* returns index of first available actor (-1 = all used) */
int GetAvailableActor (int first, int len)
{
	int c, last;

	if (!actors)
		return -1;

	last = first + len;
	for (c=first; c<last; c++)
	{
		if (actors[c].state==0)
			return c;
	}
	return -1;
}

/* gets actor pointer from index */
Actor* GetActor (int index)
{
	if (actors && index<count)
		return &actors[index];
	else
		return NULL;
}

/* sets actor properties */
Actor* SetActor (int index, int type, int x, int y, int w, int h, void (*callback)(struct Actor*))
{
	if (actors && index<count)
	{
		Actor* actor = &actors[index];
		actor->index = index;
		actor->type = type;
		actor->callback = callback;
		actor->state = 1;
		actor->x = x;
		actor->y = y;
		actor->w = w;
		actor->h = h;
		UpdateActorHitbox (actor);
		return actor;
	}
	return NULL;
}

/* releases actor */
void ReleaseActor (Actor* actor)
{
	TLN_SetSpriteBlendMode (actor->index, BLEND_NONE, 0);
	actor->state = 0;
}

/* sets collision box */
void UpdateActorHitbox (Actor* actor)
{
	actor->hitbox.x1 = actor->x;
	actor->hitbox.y1 = actor->y;
	actor->hitbox.x2 = actor->x + actor->w;
	actor->hitbox.y2 = actor->y + actor->h;
}

/* Periodic tasks */
void TasksActors (unsigned int t)
{
	int c;

	if (!actors)
		return;

	time = t;

	for (c=0; c<count; c++)
	{
		Actor* actor = &actors[c];
		if (actor->state != 0)
			TasksActor (actor);
	}
}

/* returns collision between two actors */
bool CheckActorCollision (Actor* actor1, Actor* actor2)
{
	return 
		actor1->hitbox.x1 < actor2->hitbox.x2 &&
		actor1->hitbox.x2 > actor2->hitbox.x1 &&
		actor1->hitbox.y1 < actor2->hitbox.y2 &&
		actor1->hitbox.y2 > actor2->hitbox.y1;
}

/* sets generic timeout */
void SetActorTimeout (Actor* actor, int timer, int timeout)
{
	actor->timers[timer] = time + timeout;
}

/* gets generic timeout ended */
bool GetActorTimeout (Actor* actor, int timer)
{
	return time >= actor->timers[timer];
}

/* TasksActor */
static void TasksActor (Actor* actor)
{
	/* motion */
	actor->x += actor->vx;
	actor->y += actor->vy;
	if (actor->callback)
		actor->callback (actor);

	/* updates associated sprite */
	if (actor->state != 0)
	{
		UpdateActorHitbox (actor);
		TLN_SetSpritePosition (actor->index, actor->x, actor->y);
	}
	else
	{
		TLN_DisableSprite (actor->index);
		TLN_DisableSpriteAnimation (actor->index);
	}
}
