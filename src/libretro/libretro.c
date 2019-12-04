// #define LUA_BUILD_AS_DLL

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

static int VIDEO_WIDTH = 480;
static int VIDEO_HEIGHT = 360;

static uint8_t *frame_buf;
static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
static float last_aspect;
static float last_sample_rate;
char retro_base_directory[4096];
char retro_game_path[4096];

/* tilengine specific */
#define TLN_NUM_LAYERS		4
#define TLN_NUM_SPRITES		128
#define TLN_NUM_ANIMATIONS	32

static TLN_Engine engine;
static int frame;
static struct lua_State* L;

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
	info->library_version = "0.1";
	info->need_fullpath = true;
	info->valid_extensions = "ini";
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
	bool no_rom = true;
	enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;

	environ_cb = cb;

	if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
		log_cb = logging.log;
	else
		log_cb = fallback_log;

	static const struct retro_controller_description controllers[] = {
	   { "Nintendo DS", RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0) },
	};

	static const struct retro_controller_info ports[] = {
	   { controllers, 1 },
	   { NULL, 0 },
	};

	cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
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

	/* game logic in lua script */
	lua_getglobal(L, "game_loop");
	lua_pushnumber(L, frame);
	lua_pcall(L, 1, 0, 0);

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
	  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "2" },
	  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "1" },
	  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "3" },
	  { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "4" },
	  { 0 },
	};

	environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

	snprintf(retro_game_path, sizeof(retro_game_path), "%s", info->path);

	check_variables();

	/* lua */
	retval = luaL_loadfile(L, "game.lua");
	printf("%s\n", lua_tostring(L, -1));
	retval = lua_pcall(L, 0, 0, 0);
	printf("%s\n", lua_tostring(L, -1));
	lua_getglobal(L, "game_init");
	lua_getglobal(L, "config");
	VIDEO_WIDTH = getIntField(L, "hres");
	VIDEO_HEIGHT = getIntField(L, "vres");
	numlayers = getIntField(L, "numlayers");
	numsprites = getIntField(L, "numsprites");
	numanimations = getIntField(L, "numanimations");

	/* Tilengine */
	frame_buf = (uint8_t*)malloc(VIDEO_WIDTH * VIDEO_HEIGHT* sizeof(uint32_t));
	engine = TLN_Init(VIDEO_WIDTH, VIDEO_HEIGHT, numlayers, numsprites, numanimations);
	TLN_SetRenderTarget(frame_buf, VIDEO_WIDTH * sizeof(uint32_t));
	frame = 0;

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
