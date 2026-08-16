CC       = gcc
SOURCES  = $(filter-out Test.c,$(wildcard *.c)) libretro/libretro.c
OBJECTS  = $(SOURCES:.c=.o)
INCPATH  = ../include
TARGET   = tilengine_libretro

# Windows specific flags
ifeq ($(OS),Windows_NT)
$(error This Makefile is not intended for Windows! Please use Visual Studio instead)
else
	RM = rm -f
	name := $(shell uname -s)
	arch := $(shell uname -m)

	# Linux specific flags (i686, x64 and arm)
	ifeq ($(name),Linux)
		LIBS = -lc -lz -lpng -llua5.1 -lm -ldl
		BIN  = $(TARGET).so
		LDFLAGS = -shared -s -Wl,-rpath,'$$ORIGIN'
		LIBPATH = ../lib/linux_$(arch)
		ifeq ($(arch),i686)
			CFLAGS = -m32 -msse2
		endif
		ifeq ($(arch),x86_64)
			CFLAGS = -m64 -msse2
		endif
		ifeq ($(arch),armv7l)
			LIBPATH = ../lib/linux_arm
		endif

		# LuaJIT is required because game.lua uses ffi.
		#
		# Prefer a local LuaJIT build, then system LuaJIT.
		ifneq ($(wildcard lua/libluajit-5.1.so*),)
			LIBS = -Llua -lluajit-5.1
		else ifneq ($(wildcard lua/libluajit.a),)
			LIBS = lua/libluajit.a
		else ifneq ($(wildcard /usr/lib/libluajit-5.1.so*),)
			CFLAGS += -I/usr/include/luajit-2.1
			LIBS = -lluajit-5.1
		else
			$(error LuaJIT not found. Install luajit or build it in lua/)
		endif

		LIBS += -lc -lz -lpng -lm -ldl
	endif

	# macOS specific flags (x86_64 and arm64)
	ifeq ($(name),Darwin)
		BIN  = $(TARGET).dylib
		LDFLAGS = -dynamiclib -arch $(arch)
		LIBPATH = ../lib/darwin_$(arch)
		CFLAGS = -arch $(arch)
		ifeq ($(arch),arm64)
			BREW_PREFIX = /opt/homebrew
		endif
		ifeq ($(arch),x86_64)
			BREW_PREFIX = /usr/local
		endif
		ifneq ($(wildcard $(BREW_PREFIX)/include),)
			CFLAGS += -I$(BREW_PREFIX)/include
			LIBS = -L$(BREW_PREFIX)/lib -lpng -lz -lc -lm
		else
			LIBS = -lpng -lz -lc -lm
		endif
		# LuaJIT (required for ffi in game.lua), then Lua 5.1
		ifneq ($(wildcard $(BREW_PREFIX)/opt/luajit/lib/libluajit-5.1.dylib),)
			LIBS += -L$(BREW_PREFIX)/opt/luajit/lib -lluajit-5.1
		else ifneq ($(wildcard lua/libluajit.a),)
			LIBS += lua/libluajit.a
		else ifneq ($(wildcard $(BREW_PREFIX)/opt/lua@5.1/lib/liblua.5.1.dylib),)
			LIBS += -L$(BREW_PREFIX)/opt/lua@5.1/lib -llua.5.1
		else ifneq ($(wildcard lua/liblua.a),)
			LIBS += lua/liblua.a
		else
			LIBS += -llua5.1
		endif
	endif
endif

# common flags
CFLAGS += -I$(INCPATH) -Ilua -Ilibretro -std=c99 -O2 -fpic -DLIB_EXPORTS
CFLAGS += -Wno-unused-result -Wno-format-truncation

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJECTS) $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(BIN) $(LIBS) -fvisibility=hidden
	mkdir -p $(LIBPATH)
	cp $(BIN) $(LIBPATH)
	cp $(BIN) ../lua_game/
	cd ../lua_game && ln -sf $(BIN) lib$(BIN)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
