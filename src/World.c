#include "Tilengine.h"
#include "Engine.h"
#include "Layer.h"
#include "Sprite.h"
#include "LoadTMX.h"
#include "Palette.h"

#define MAX_TMX_ITEM	100

/* info for current world */
static TMXInfo tmxinfo;
static int first;

/*!
 * \brief Loads and assigns complete TMX file
 * \param filename TMX file to load
 * \param first_layer Starting layer number where place the loaded tmx
 */
bool TLN_LoadWorld(const char* filename, int first_layer)
{
	int c;

	if (!TMXLoad(filename, &tmxinfo))
		return NULL;

	if (tmxinfo.num_layers > MAX_TMX_ITEM)
		tmxinfo.num_layers = MAX_TMX_ITEM;

	/* load and assign each layer type */
	first = first_layer;
	for (c = 0; c < tmxinfo.num_layers; c += 1)
	{
		TMXLayer* tmxlayer = &tmxinfo.layers[c];
		const int layerindex = tmxinfo.num_layers - c - 1 + first;
		switch (tmxlayer->type)
		{
		case LAYER_NONE:
			break;

		case LAYER_TILE:
		{
			TLN_Tilemap tilemap = TLN_LoadTilemap(filename, tmxlayer->name);
			TLN_SetLayerTilemap(layerindex, tilemap);
		}
		break;

		case LAYER_OBJECT:
		{
			TLN_ObjectList objectlist = TLN_LoadObjectList(filename, tmxlayer->name);
			TLN_SetLayerObjects(layerindex, objectlist, NULL);
		}
		break;

		case LAYER_BITMAP:
		{
			TLN_Bitmap bitmap = TLN_LoadBitmap(tmxlayer->image);
			TLN_SetLayerBitmap(layerindex, bitmap);
		}
		break;
		}

		/* direct set of layer properties */
		Layer* layer = GetLayer(layerindex);
		layer->world.xfactor = tmxlayer->parallaxx;
		layer->world.yfactor = tmxlayer->parallaxy;
		layer->world.offsetx = (int)tmxlayer->offsetx;
		layer->world.offsety = (int)tmxlayer->offsety;

		/* opacity selects blend mode */
		if (tmxlayer->visible)
		{
			int opacity = (int)(tmxlayer->opacity * 100);
			if (opacity > 87)
				TLN_SetLayerBlendMode(layerindex, BLEND_NONE, 0);
			else if (opacity > 62)
				TLN_SetLayerBlendMode(layerindex, BLEND_MIX75, 0);
			else if (opacity > 37)
				TLN_SetLayerBlendMode(layerindex, BLEND_MIX50, 0);
			else if (opacity > 12)
				TLN_SetLayerBlendMode(layerindex, BLEND_MIX25, 0);
		}
		else
			TLN_DisableLayer(layerindex);
	}

	/* sets background color if defined */
	if (tmxinfo.bgcolor != 0)
	{
		Color bgcolor;
		bgcolor.value = tmxinfo.bgcolor;
		TLN_SetBGColor(bgcolor.r, bgcolor.g, bgcolor.b);
	}
	else
		TLN_DisableBGColor();
	return true;
}

/*! 
 * \brief Releases world resources loaded with TLN_LoadWorld 
 */
void TLN_ReleaseWorld(void)
{
	int c;

	for (c = 0; c < tmxinfo.num_layers; c += 1)
	{
		TMXLayer* tmxlayer = &tmxinfo.layers[c];
		const int layerindex = tmxinfo.num_layers - c - 1 + first;
		
		Layer* layer = GetLayer(layerindex);
		layer->ok = false;
		switch (tmxlayer->type)
		{
		case LAYER_NONE:
			break;
		
		case LAYER_TILE:
			TLN_DeleteTilemap(layer->tilemap);
			break;

		case LAYER_OBJECT:
			TLN_DeleteObjectList(layer->objects);
			break;

		case LAYER_BITMAP:
			TLN_DeleteBitmap(layer->bitmap);
			break;
		}
	}
}

/*!
 * \brief Sets layer parallax factor to use in conjunction with \ref TLN_SetWorldPosition
 * \param nlayer Layer index [0, num_layers - 1]
 * \param x Horizontal parallax factor
 * \param y Vertical parallax factor
 */
bool TLN_SetLayerParallaxFactor(int nlayer, float x, float y)
{
	Layer *layer;
	if (nlayer >= engine->numlayers)
	{
		TLN_SetLastError(TLN_ERR_IDX_LAYER);
		return false;
	}

	layer = &engine->layers[nlayer];
	layer->world.xfactor = x;
	layer->world.yfactor = y;
	layer->dirty = true;
	TLN_SetLastError(TLN_ERR_OK);
	return true;
}

/*!
 * \brief Sets global world position, moving all layers in sync according to their parallax factor
 * \param x horizontal position in world space
 * \param y vertical position in world space
 */
void TLN_SetWorldPosition(int x, int y)
{
	engine->xworld = x;
	engine->yworld = y;
	engine->dirty = true;
}

/*!
 * \brief Sets the sprite position in world space coordinates
 * \param nsprite Id of the sprite [0, num_sprites - 1]
 * \param x Horizontal world position of pivot (0 = left margin)
 * \param y Vertical world position of pivot (0 = top margin)
 * \sa TLN_SetSpritePivot
 */
bool TLN_SetSpriteWorldPosition(int nsprite, int x, int y)
{
	Sprite *sprite;
	if (nsprite >= engine->numsprites)
	{
		TLN_SetLastError(TLN_ERR_IDX_SPRITE);
		return false;
	}

	sprite = &engine->sprites[nsprite];
	sprite->xworld = x;
	sprite->yworld = y;
	sprite->world_space = true;
	sprite->dirty = true;

	TLN_SetLastError(TLN_ERR_OK);
	return true;
}
