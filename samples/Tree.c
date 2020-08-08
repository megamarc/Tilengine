#include "Racer.h"
#include "Tree.h"

typedef struct
{
	int depth;
	int x,y;
	char side;
}
Tree;

static void TreeTasks (Actor* actor);
static float CalcPerspective (float u, float u0, float u1, float z, float z0, float z1);

Actor* CreateTree (int x, int y, char side)
{
	Actor* actor;
	Tree* tree;
	int index = GetAvailableActor (1, MAX_ACTOR);

	if (index==-1)
		return NULL;

	actor = SetActor (index, 1, 0,0, 136,208, TreeTasks);
	TLN_SetSpriteSet (index, spritesets[SPRITESET_TREES]);
	//TLN_SetFirstSprite (index);
	tree = (Tree*)actor->usrdata;
	tree->x = x;
	tree->y = y;
	tree->depth = Z_FAR;
	tree->side = side;
	if (!side)
		tree->x += pan;
	else
		tree->x -= pan;
	return actor;
}

static void TreeTasks (Actor* actor)
{
	Tree* tree = (Tree*)actor->usrdata;
	float scale;
	actor->x = Z_NEAR*tree->x / tree->depth;
	if (tree->side)
		actor->x += 136;
	else
		actor->x = 136 - actor->x;
	actor->y = tree->y / tree->depth - 52;
	scale = (float)Z_NEAR/tree->depth;
	TLN_SetSpriteScaling (actor->index, scale,scale);
	tree->depth -= speed;

	/* finaliza */
	if (tree->depth<1)
		ReleaseActor (actor);
}

static float CalcPerspective (float u, float u0, float u1, float z, float z0, float z1)
{
	float a = (u - u0)/(u1 - u0);
	float val = 
		((1 - a)*(u0/z0) + a*(u1/z1)) /
		((1 - a)*( 1/z0) + a*( 1/z1));

	return u0 + val*(u1 - u0);
}
