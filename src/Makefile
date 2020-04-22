CC       = gcc
SOURCES  = $(filter-out Test.c,$(wildcard *.c))
OBJECTS  = $(SOURCES:.c=.o)
INCPATH  = ../include

# Windows specific flags
ifeq ($(OS),Windows_NT)
$(error This Makefile is not intended for Windows! Please use Visual Studio instead)
else
	RM = rm -f
	name := $(shell uname -s)
	arch := $(shell uname -m)
	
	# Linux specific flags (i686, x64 and arm)
	ifeq ($(name),Linux)
		LIBS = -lSDL2 -lc -lz -lpng
		BIN  = libTilengine.so
		LDFLAGS = -shared -s
		LIBPATH = ../lib/linux_$(arch)
		ifeq ($(arch),i686)
			CFLAGS = -m32 -msse2
		endif
		ifeq ($(arch),x86_64)
			CFLAGS = -m64 -msse2
		endif
	endif
	
	# OSX specific flags
	ifeq ($(name),Darwin)
		LIBS = -lpng -lz -framework SDL2 -lc
		BIN  = Tilengine.dylib
		LDFLAGS = -dynamiclib
		LIBPATH = ../lib/darwin_x86_64
	endif
endif

# common flags
CFLAGS += -I$(INCPATH) -std=c99 -O2 -fpic -DLIB_EXPORTS

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJECTS) $(BIN) Test

$(BIN): $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $(BIN) $(LIBS) -fvisibility=hidden
	mkdir -p $(LIBPATH)
	cp $(BIN) $(LIBPATH)

Test: Test.c
	$(RM) Test.o Test
	$(CC) Test.c -o Test -I$(INCPATH) -L$(LIBPATH) -lTilengine -Wl,-rpath,$(LIBPATH)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
