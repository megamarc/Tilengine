#ifndef _ACTOR_H
#define _ACTOR_H

#include "Tilengine.h"

typedef struct
{
	int x1,y1,x2,y2;
}
Rect;

typedef struct Actor
{
	int index;
	int type;
	int state;
	int w,h;
	int x,y;
	int vx,vy;
	int life;
	Rect hitbox;
	unsigned int timers[4];
	void (*callback)(struct Actor*);
	unsigned char usrdata[64];
}
Actor;

#ifdef __cplusplus
extern "C"{
#endif

bool	CreateActors (int num);
bool	DeleteActors (void);
int		GetAvailableActor (int first, int len);
Actor*	GetActor (int index);
Actor*	SetActor (int index, int type, int x, int y, int w, int h, void (*callback)(struct Actor*));
void	ReleaseActor (Actor* actor);
void	UpdateActorHitbox (Actor* actor);
void	TasksActors (unsigned int time);
bool	CheckActorCollision (Actor* actor1, Actor* actor2);
void	SetActorTimeout (Actor* actor, int timer, int timeout);
bool	GetActorTimeout (Actor* actor, int timer);

#ifdef __cplusplus
}
#endif

#endif