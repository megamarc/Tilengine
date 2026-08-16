CC       = gcc
SOURCES  = $(filter-out Test.c,$(wildcard *.c)) libretro/libretro.c
OBJECTS  = $(SOURCES:.c=.o)
INCPATH  = ../include
TARGET   = tilengine_libretro
LUAJIT_A = lua/libluajit.a
LUAJIT_SRC ?= $(abspath ../.luajit-src)
LUAJIT_URL = https://codeload.github.com/LuaJIT/LuaJIT/tar.gz/refs/heads/v2.1
LUAJIT_CFLAGS = -fPIC

# Windows specific flags
ifeq ($(OS),Windows_NT)
$(error This Makefile is not intended for Windows! Please use Visual Studio instead)
else
	RM = rm -f
	name := $(shell uname -s)
	arch := $(shell uname -m)

	# Linux specific flags (i686, x64 and arm)
	ifeq ($(name),Linux)
		LIBS = -lc -lz -lpng -lm -ldl
		BIN  = $(TARGET).so
		LDFLAGS = -shared -s
		LIBPATH = ../lib/linux_$(arch)
		LUAJIT_LDFLAGS = -Wl,--whole-archive $(LUAJIT_A) -Wl,--no-whole-archive
		ifeq ($(arch),i686)
			CFLAGS = -m32 -msse2
			LUAJIT_CFLAGS += -m32 -msse2
		endif
		ifeq ($(arch),x86_64)
			CFLAGS = -m64 -msse2
			LUAJIT_CFLAGS += -m64 -msse2
		endif
		ifeq ($(arch),armv7l)
			LIBPATH = ../lib/linux_arm
		endif
	endif

	# macOS specific flags (x86_64 and arm64)
	ifeq ($(name),Darwin)
		BIN  = $(TARGET).dylib
		LDFLAGS = -dynamiclib -arch $(arch)
		LIBPATH = ../lib/darwin_$(arch)
		CFLAGS = -arch $(arch)
		LUAJIT_CFLAGS += -arch $(arch)
		LUAJIT_LDFLAGS = -Wl,-force_load,$(LUAJIT_A)
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
	endif
endif

# common flags
CFLAGS += -I$(INCPATH) -Ilua -Ilibretro -std=c99 -O2 -fpic -DLIB_EXPORTS
CFLAGS += -Wno-unused-result -Wno-format-truncation

.PHONY: all all-before all-after clean clean-custom build-luajit

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJECTS) $(BIN)

# Static LuaJIT with -fPIC so it can be linked into the libretro shared object.
# Shared libluajit (.so/.dylib) is never used.
$(LUAJIT_A):
	$(MAKE) build-luajit

build-luajit:
	@if [ ! -f "$(LUAJIT_SRC)/src/Makefile" ]; then \
		echo "Downloading LuaJIT into $(LUAJIT_SRC)"; \
		mkdir -p "$(LUAJIT_SRC)"; \
		curl -fsSL "$(LUAJIT_URL)" | tar xz -C "$(LUAJIT_SRC)" --strip-components=1; \
	fi
	$(MAKE) -C "$(LUAJIT_SRC)/src" BUILDMODE=static TARGET_CFLAGS="$(LUAJIT_CFLAGS)" MACOSX_DEPLOYMENT_TARGET="$${MACOSX_DEPLOYMENT_TARGET:-11.0}"
	cp -f "$(LUAJIT_SRC)/src/libluajit.a" "$(LUAJIT_A)"

$(BIN): $(OBJECTS) $(LUAJIT_A)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LUAJIT_LDFLAGS) -o $(BIN) $(LIBS) -fvisibility=hidden
	mkdir -p $(LIBPATH)
	cp $(BIN) $(LIBPATH)
	cp $(BIN) ../lua_game/
	cd ../lua_game && ln -sf $(BIN) lib$(BIN)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
