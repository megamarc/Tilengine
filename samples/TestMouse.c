/******************************************************************************
*
* Tilengine sample
* 2018 Marc Palacios
* http://www.tilengine.org
*
* This sample illustrates the SDL callbacks introduced in release 1.21.
* It creates a list of game entities and scatters them randomly on the playfield.
* It creates a custom palette based on the sprite's base palette but highlighted.
* When the user clicks on any entity with the mouse, it gets highlighted.
* When the user releases the mouse, it gets back to iths default color.
*
******************************************************************************/

#include <stdlib.h>
#include "Tilengine.h"
#define _STDINT_H_
#include "SDL2/SDL_events.h"

#define WIDTH			400
#define HEIGHT			240
#define MAX_ENTITIES	20

/* entity structure */
typedef struct Entity
{
	bool enabled;		/* entity is alive */
	int guid;			/* for game logic management */
	int sprite_index;	/* Tilengine sprite index */
	int x, y;			/* screen position */
	int w, h;			/* size */
	// TODO other attributes: type, health, state...
}
Entity;

static Entity entities[MAX_ENTITIES];	/* entities list */
static Entity* selected_entity;			/* pointer to currently selected entity (if any) */
static TLN_Palette palette_select;		/* color palette for selected entity */
static TLN_Palette palette_sprite;		/* color palette for regular entity */

/* execute this when an entity is clicked */
void on_entity_click(Entity* entity)
{
	// TODO what to do when clicked?
	TLN_SetSpritePalette(entity->sprite_index, palette_select);
	printf("Entity %d is clicked\n", entity->guid);
}

/* execute this when an entity is un-clicked */
void on_entity_release(Entity* entity)
{
	// TODO what to do when un-clicked?
	TLN_SetSpritePalette(entity->sprite_index, palette_sprite);
	printf("Entity %d is un-clicked\n", entity->guid);
}

/* SDL event callback */
static void sdl_callback(SDL_Event* evt)
{
	if (evt->type == SDL_MOUSEBUTTONDOWN)
	{
		SDL_MouseButtonEvent* mouse = (SDL_MouseButtonEvent*)evt;
		int c;
		
		/* scale from window space to framebuffer space */
		mouse->x = (mouse->x*WIDTH)/TLN_GetWindowWidth();
		mouse->y = (mouse->y*HEIGHT)/TLN_GetWindowHeight();		

		/* search clicked entity */
		for (c = 0; c < MAX_ENTITIES; c++)
		{
			Entity* entity = &entities[c];
			if (entity->enabled &&
				mouse->x >= entity->x && mouse->y >= entity->y &&
				mouse->x < entity->x + entity->w && mouse->y < entity->y + entity->h)
			{
				selected_entity = entity;
				on_entity_click(selected_entity);
			}
		}
	}

	else if (evt->type == SDL_MOUSEBUTTONUP)
	{
		if (selected_entity != NULL)
		{
			on_entity_release(selected_entity);
			selected_entity = NULL;
		}
	}
}

int main(int argc, char* argv[])
{
	TLN_Spriteset spriteset;
	TLN_SpriteInfo sprite_info;
	int c;
	int frame = 0;

	TLN_Init(WIDTH, HEIGHT, 0, MAX_ENTITIES, 0);
	spriteset = TLN_LoadSpriteset("assets/smw/smw_sprite.png");
	palette_sprite = TLN_GetSpritesetPalette(spriteset);
	palette_select = TLN_ClonePalette(palette_sprite);
	TLN_AddPaletteColor(palette_select, 64, 64, 64, 1, 32);
	TLN_GetSpriteInfo(spriteset, 0, &sprite_info);

	/* create list of entities at random positions */
	for (c = 0; c < MAX_ENTITIES; c++)
	{
		Entity* entity = &entities[c];
		entity->guid = c;	/* whatever you want */
		entity->enabled = true;
		entity->x = rand() % WIDTH;
		entity->y = rand() % HEIGHT;
		entity->w = sprite_info.w;
		entity->h = sprite_info.h;
		entity->sprite_index = c;

		/* show on Tilengine */
		TLN_ConfigSprite(entity->sprite_index, spriteset, 0);
		TLN_SetSpritePosition(entity->sprite_index, entity->x, entity->y);
		TLN_SetSpritePicture(entity->sprite_index, 0);
	}

	/* windows and main loop */
	TLN_CreateWindow(NULL, 0);
	TLN_SetSDLCallback(sdl_callback);
	while (TLN_ProcessWindow())
		TLN_DrawFrame(frame++);

	TLN_Deinit();
	return 0;
}
