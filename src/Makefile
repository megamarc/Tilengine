CC       = gcc
SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:.c=.o)
CFLAGS   = -msse2 -m32 -std=c99 -O2 -I"../lib"
TARGETS  = barrel colorcycle mode7 platformer platformer_test racer seizetheday scaling shooter tutorial wobble
OUTPUT	 = ../samples

# OS dependant flags
ifeq ($(OS),Windows_NT)
	LIBS = ../lib/win32/Tilengine.lib
else
	LIBS = -L"../lib/linux" -lTilengine -lm
endif

.PHONY: all all-before all-after clean clean-custom

all: $(TARGETS)

barrel: Barrel.o Simon.o
	$(CC) Barrel.o Simon.o -o $(OUTPUT)/barrel $(LIBS)
	
colorcycle: ColorCycle.o
	$(CC) ColorCycle.o -o $(OUTPUT)/colorcycle $(LIBS)
	
mode7: Mode7.o Sin.o
	$(CC) Mode7.o Sin.o -o $(OUTPUT)/mode7 $(LIBS)
	
platformer: Platformer.o
	$(CC) Platformer.o -o $(OUTPUT)/platformer $(LIBS)
	
platformer_test: Platformer_test.o
	$(CC) Platformer_test.o -o $(OUTPUT)/platformer_test $(LIBS)
	
racer: Racer.o Tree.o Actor.o
	$(CC) Racer.o Tree.o Actor.o -o $(OUTPUT)/racer $(LIBS)
	
seizetheday: SeizeTheDay.o
	$(CC) SeizeTheDay.o -o $(OUTPUT)/seizetheday $(LIBS)
	
scaling: Scaling.o
	$(CC) Scaling.o -o $(OUTPUT)/scaling $(LIBS)
	
shooter: Shooter.o Actor.o Boss.o Enemy.o Explosion.o Ship.o Sin.o
	$(CC) Shooter.o Actor.o Boss.o Enemy.o Explosion.o Ship.o Sin.o -o $(OUTPUT)/shooter $(LIBS)
	
tutorial: Tutorial.o
	$(CC) Tutorial.o -o $(OUTPUT)/tutorial $(LIBS)

wobble: Wobble.o Sin.o
	$(CC) Wobble.o Sin.o -o $(OUTPUT)/wobble $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# OS dependant clean
clean: clean-custom
ifeq ($(OS),Windows_NT)
	del /Q $(OBJECTS)
else
	rm $(OBJECTS)
endif
