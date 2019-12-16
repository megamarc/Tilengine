#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "libretro.h"
#include "Tilengine.h"

#ifdef WIN32
extern SetCurrentDirectoryA(const char* path);
#define chdir SetCurrentDirectoryA
#endif

/* input flags for input_mask, 
   values aligned with RETRO_DEVICE_ID_JOYPAD_n */
enum
{
	INPUT_BUTTON1	= 0x0001,
	INPUT_BUTTON2	= 0x0002,
	INPUT_SELECT	= 0x0004,
	INPUT_START		= 0x0008,
	INPUT_UP		= 0x0010,
	INPUT_DOWN		= 0x0020,
	INPUT_LEFT		= 0x0040,
	INPUT_RIGHT		= 0x0080,
	INPUT_BUTTON3	= 0x0100,
	INPUT_BUTTON4	= 0x0200,
};

static int VIDEO_WIDTH = 480;
static int VIDEO_HEIGHT = 360;

static uint8_t *frame_buf;
static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
static char frame_callback_name[64];
static char raster_callback_name[64];
static char retro_base_directory[4096];

/* tilengine specific */
static TLN_Engine engine;
static int frame;
static struct lua_State* L;
static uint16_t input_mask[2];

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
	(void)level;
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
}

static retro_environment_t environ_cb;

void retro_init(void)
{
	const char *dir = NULL;
	if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
	{
		snprintf(retro_base_directory, sizeof(retro_base_directory), "%s", dir);
	}

	L = luaL_newstate();
	luaL_openlibs(L);
}

void retro_deinit(void)
{
	lua_close(L);
}

unsigned retro_api_version(void)
{
	return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
	log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
	memset(info, 0, sizeof(*info));
	info->library_name = "Tilengine";
	info->library_version = "1.0";
	info->need_fullpath = true;
	info->valid_extensions = "lua";
}

static retro_video_refresh_t video_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_get_system_av_info(struct retro_system_av_info *info)
{
	info->geometry.base_width = VIDEO_WIDTH;
	info->geometry.base_height = VIDEO_HEIGHT;
	info->geometry.max_width = VIDEO_WIDTH;
	info->geometry.max_height = VIDEO_HEIGHT;
	info->geometry.aspect_ratio = 0;	/* auto width/height */
	info->timing.fps = 60;
	info->timing.sample_rate = 22050;
}

static struct retro_rumble_interface rumble;

void retro_set_environment(retro_environment_t cb)
{
	bool no_rom = false;
	enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;

	environ_cb = cb;

	if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
		log_cb = logging.log;
	else
		log_cb = fallback_log;

	cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);
	cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
	(void)cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
	(void)cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
	input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
	input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
	video_cb = cb;
}

void retro_reset(void)
{

}

static void update_input(void)
{
	unsigned port;

	for (port = 0; port < 2; port += 1)
	{
		uint16_t input = 0;

		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
			input |= INPUT_LEFT;
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
			input |= INPUT_RIGHT;
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
			input |= INPUT_UP;
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))
			input |= INPUT_DOWN;
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B))
			input |= INPUT_BUTTON1;
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y))
			input |= INPUT_BUTTON2;
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A))
			input |= INPUT_BUTTON3;
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X))
			input |= INPUT_BUTTON4;
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START))
			input |= INPUT_START;
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT))
			input |= INPUT_SELECT;

		input_mask[port] = input;
	}
}

static void check_variables(void)
{

}

void retro_run(void)
{
	update_input();

	bool updated = false;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
		check_variables();

	TLN_UpdateFrame(frame);
	video_cb(frame_buf, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_WIDTH * sizeof(uint32_t));
	frame += 1;
}

/* returns int field from lua table */
static int getIntField(lua_State* L, const char* key)
{
	int result = 0;

	lua_pushstring(L, key);
	lua_gettable(L, -2);  // get table[key]
	result = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);  // remove number from stack
	return result;
}

bool retro_load_game(const struct retro_game_info *info)
{
	int numlayers, numsprites, numanimations;
	int retval;

	struct retro_input_descriptor desc[] = {
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"Select" },

		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"Select" },

		{ 1, RETRO_DEVICE_NONE, 0, 0,  NULL },
	};

	environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

	if (!info)
		return false;

	check_variables();

	/* load "game.lua" and parse */
	chdir(info->path);
	retval = luaL_loadfile(L, "game.lua");
	retval = lua_pcall(L, 0, 0, 0);
	
	/* get config{} struct from lua */
	lua_getglobal(L, "config");
	VIDEO_WIDTH = getIntField(L, "hres");
	VIDEO_HEIGHT = getIntField(L, "vres");
	numlayers = getIntField(L, "numlayers");
	numsprites = getIntField(L, "numsprites");
	numanimations = getIntField(L, "numanimations");

	/* Setup Tilengine with obtained parameters */
	frame_buf = (uint8_t*)malloc(VIDEO_WIDTH * VIDEO_HEIGHT* sizeof(uint32_t));
	engine = TLN_Init(VIDEO_WIDTH, VIDEO_HEIGHT, numlayers, numsprites, numanimations);
	TLN_SetRenderTarget(frame_buf, VIDEO_WIDTH * sizeof(uint32_t));
	frame = 0;

	/* register default callbacks (can be overriden later) */
	LUA_SetFrameCallback("game_loop");
	LUA_SetRasterCallback(NULL);

	/* call game init */
	lua_getglobal(L, "game_load");
	lua_pcall(L, 0, 0, 0);

	return true;
}

void retro_unload_game(void)
{
	lua_getglobal(L, "game_unload");
	lua_pcall(L, 0, 0, 0);

	TLN_Deinit();
	free(frame_buf);
	frame_buf = NULL;
}

unsigned retro_get_region(void)
{
	return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
	return false;
}

size_t retro_serialize_size(void)
{
	return false;
}

bool retro_serialize(void *data_, size_t size)
{
	return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
	return false;
}

void *retro_get_memory_data(unsigned id)
{
	(void)id;
	return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
	(void)id;
	return 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
	(void)index;
	(void)enabled;
	(void)code;
}

static void frame_callback(int frame)
{
	/* game logic in "game_loop()" lua script */
	lua_getglobal(L, frame_callback_name);
	lua_pushnumber(L, frame);
	lua_pcall(L, 1, 0, 0);
}

static void raster_callback(int line)
{
	/* raster effect in lua script */
	lua_getglobal(L, raster_callback_name);
	lua_pushnumber(L, line);
	lua_pcall(L, 1, 0, 0);
}

/*!
 * Sets the lua function name to be called at the start of each frame, for implementing game logic. Default is "game_loop". Intended to be called from a lua script
 * 
 * \param name name of the lua function, or NULL to disable frame callback
 */
void LUA_SetFrameCallback(const char* name)
{
	if (name)
	{
		strncpy(frame_callback_name, name, sizeof(frame_callback_name));
		TLN_SetFrameCallback(frame_callback);
	}
	else
		TLN_SetFrameCallback(NULL);
}

/*!
 * Sets the lua function name to be called at every scanline for raster effects. Default is none. Intended to be called from a lua script
 * 
 * \param name name of the lua function, or NULL to disable raster callback
 */
void LUA_SetRasterCallback(const char* name)
{
	if (name)
	{
		strncpy(raster_callback_name, name, sizeof(raster_callback_name));
		TLN_SetRasterCallback(raster_callback);
	}
	else
		TLN_SetRasterCallback(NULL);
}

bool LUA_CheckInput(uint8_t port, uint16_t input)
{
	return (input_mask[port] & input) != 0;
}