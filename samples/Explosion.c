#include "Shooter.h"
#include "Explosion.h"

void ExplosionTasks (Actor* actor)
{
	if (!TLN_GetAnimationState (actor->index))
		ReleaseActor (actor);
}

Actor* CreateExplosion (int index, int x, int y, TLN_Sequence sequence)
{
	Actor* actor = GetActor (index);
	SetActor (index, TYPE_EXPLOSION, x,y, 32,32, ExplosionTasks);
	TLN_ConfigSprite (index, spritesets[SPRITESET_MAIN], 0);
	TLN_SetSpriteAnimation (index, sequence, 1);
	//TLN_SetSpriteBlendMode (index, BLEND_ADD);
	return actor;
}
