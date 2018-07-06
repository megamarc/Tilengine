CC       = gcc
SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:.c=.o)

# Windows specific flags
ifeq ($(OS),Windows_NT)
$(error This Makefile si not intended for Windows! Please use Visual Studio instead)
else
	RM = rm -f
	name := $(shell uname -s)
	arch := $(shell uname -m)
	
	# Linux specific flags (i686, x64 and arm)
	ifeq ($(name),Linux)
		LIBS = -lSDL2 -lc -lz -lpng
		BIN  = libTilengine.so
		LDFLAGS = -shared
		ifeq ($(arch),i686)
			CFLAGS = -m32 -msse2
		endif
		ifeq ($(arch),x86_64)
			CFLAGS = -m64 -msse2
		endif
	endif
	
	# OSX specific flags
	ifeq ($(name),Darwin)
		LIBS = -lpng -framework SDL2 -lc
		BIN  = Tilengine.dylib
		LDFLAGS = -dynamiclib
	endif
endif

# common flags
CFLAGS += -I"../include" -std=c99 -O2 -fpic -DLIB_EXPORTS

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJECTS) $(BIN) Test

$(BIN): $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $(BIN) $(LIBS) -s -fvisibility=hidden
	mkdir ../lib -p
	cp $(BIN) ../lib

Test: Test.c
	$(RM) Test.o Test
	$(CC) Test.c -o Test -I"../include" -L"../lib" -lTilengine -Wl,-rpath,"../lib"

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
