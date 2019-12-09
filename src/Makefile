CC       = gcc
SOURCES  = $(filter-out Test.c,$(wildcard *.c))
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
		LIBS = -lc -lz -lpng
		BIN  = $(TARGET).so
		LDFLAGS = -shared -s
		ifeq ($(arch),i686)
			CFLAGS = -m32 -msse2
			LIBPATH = ../lib/linux_i686
		endif
		ifeq ($(arch),x86_64)
			CFLAGS = -m64 -msse2
			LIBPATH = ../lib/linux_x86_64
		endif
		ifeq ($(arch),armv7l)
			LIBPATH = ../lib/linux_arm
		endif
	endif
	
	# OSX specific flags
	ifeq ($(name),Darwin)
		LIBS = -lpng -lz -lc
		BIN  = $(TARGET).dylib
		LDFLAGS = -dynamiclib
		LIBPATH = ../lib/darwin_x86_64
	endif
endif

# common flags
CFLAGS += -I$(INCPATH) -std=c99 -O2 -fpic -DLIB_EXPORTS

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJECTS) $(BIN)

$(BIN): $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $(BIN) $(LIBS) -fvisibility=hidden
	mkdir -p $(LIBPATH)
	cp $(BIN) $(LIBPATH)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
