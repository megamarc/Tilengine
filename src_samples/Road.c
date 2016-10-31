#include "Tilengine.h"

#define WIDTH	400
#define HEIGHT	240

#define Y_BASE	96
#define ROAD_H	(HEIGHT - Y_BASE)

/* linear interpolation */
#define lerp(x,x0,x1,fx0,fx1) (fx0 + ((fx1 - fx0)*(x - x0)/(x1 - x0)))

/* maps */
enum
{
	MAP_SKY,
	MAP_TREES,
	MAP_ROAD1,
	MAP_ROAD2,
	MAX_MAP
};

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

typedef struct
{
	TLN_Tileset tileset;
	TLN_Tilemap tilemap;
}
Map;

static Map maps[MAX_MAP];

/* road */
float s0;		/* scale at horizon */
float s1;		/* scale at bottom */
int   sf;		/* scale factor */
struct
{
	float s;	/* scale */
	int	  z;	/* z depth */
	int   t;	/* texture line */
}
road[ROAD_H];

static int curve[ROAD_H];

/* helper for loading a related tileset + tilemap */
static void LoadMap (int index, char* name)
{
	char filename[64];
	Map* map = &maps[index];

	/* load tileset */
	sprintf (filename, "%s.tsx", name);
	map->tileset = TLN_LoadTileset (filename);

	/* load tilemap */
	sprintf (filename, "%s.tmx", name);
	map->tilemap = TLN_LoadTilemap (filename, "Layer 1");
}

/* helper for freeing a tileset + tilemap */ 
static void FreeMap (int index)
{
	Map* map = &maps[index];
	
	TLN_DeleteTileset (map->tileset);
	TLN_DeleteTilemap (map->tilemap);
}

static void SetMap (int index, int nlayer)
{
	TLN_SetLayer (nlayer, maps[index].tileset, maps[index].tilemap); 
}

static void raster_callback (int line);

static int frame;

/* entry point */
int main (int argc, char* argv[])
{
	int y;

	/* setup engine */
	TLN_Init (WIDTH, HEIGHT, MAX_LAYER,0,0);
	TLN_SetBGColor (0,128,238);
	TLN_CreateWindow ("overlay.bmp", false);

	/* load layer (tileset + tilemap) */
	LoadMap (MAP_SKY,   "NamcoHorizon");
	LoadMap (MAP_TREES, "NamcoTrees");
	LoadMap (MAP_ROAD1, "NamcoRoad1");
	LoadMap (MAP_ROAD2, "NamcoRoad2");

	TLN_SetRasterCallback (raster_callback);

	/* init road */
	s0 = 0.03f;
	s1 = 1.0f;
	sf = (int)(s1/s0);
	for (y=0; y<ROAD_H; y++)
	{
		road[y].s = lerp (y, 0,ROAD_H, s0,s1);
		//road[y].s = ((y*49)/143) + 1;
		road[y].z = (int)500/road[y].s;
		//road[y].z = 500*50/road[y].s;
		road[y].t = lerp(road[y].z, 1,sf, 384,0)/50;
		
		curve[ROAD_H - y - 1] = (y*(y+1)/2)/64;
	}

	/* main loop */
	while (TLN_ProcessWindow ())
	{
		/* render to the window */
		TLN_DrawFrame (frame);
		frame++;
	}

	/* release resources */
	FreeMap (MAP_SKY);
	FreeMap (MAP_TREES);
	FreeMap (MAP_ROAD1);
	FreeMap (MAP_ROAD2);
	TLN_DeleteWindow ();
	TLN_Deinit ();

	return 0;
}

static void raster_callback (int line)
{
	if (line==0)
	{
		SetMap (MAP_SKY, LAYER_BACKGROUND);
		TLN_SetLayerPosition (LAYER_BACKGROUND, frame/2, 48);
		TLN_DisableLayer (LAYER_FOREGROUND);
	}

	if (line==48)
	{
		SetMap (MAP_TREES, LAYER_FOREGROUND);
		TLN_SetLayerPosition (LAYER_FOREGROUND, frame,0);
		TLN_ResetLayerMode (LAYER_FOREGROUND);
	}

	if (line==Y_BASE)
	{
		SetMap (MAP_ROAD2, LAYER_FOREGROUND);
	}

	if (line >= Y_BASE)
	{
		int nscan = line - Y_BASE;
		float sc = road[nscan].s;
		int zw = road[nscan].z;
		int n = (ROAD_H - nscan);
		int s = (n*(n+1)/2)/128;
		int dx = (((WIDTH - (384 - 0)*sc) / 2) + s) / sc;
		int y = (ROAD_H*500)/zw;

		//printf ("%d: z=%d s=%d y=%d\n", nscan, zw, s, y);
		
		TLN_SetLayerPosition (LAYER_FOREGROUND, -dx,road[nscan].t - line - frame);
		TLN_SetLayerScaling (LAYER_FOREGROUND, sc, 1.0f);
	}
}