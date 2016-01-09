CC       = gcc
SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:.c=.o)
SHARED   = libTilengine.so.1
CFLAGS   = -msse2 -m32 -std=c99 -O2
TARGETS  = barrel mode7 platformer platformer_test racer scaling shadow shooter tutorial wobble

# OS dependant flags
ifeq ($(OS),Windows_NT)
	LIBS = Tilengine.dll
else
	LIBS = -lTilengine -lm
endif

.PHONY: all all-before all-after clean clean-custom

all: $(TARGETS)

# linux-only target
tilengine: $(SHARED) Tilengine.h
	cp Tilengine.h /usr/local/include
	cp $(SHARED) /usr/local/lib
	ln -sf /usr/local/lib/$(SHARED) /usr/local/lib/libTilengine.so

barrel: Barrel.o Simon.o
	$(CC) Barrel.o Simon.o -o barrel $(LIBS)
	
mode7: Mode7.o Sin.o
	$(CC) Mode7.o Sin.o -o mode7 $(LIBS)
	
platformer: Platformer.o
	$(CC) Platformer.o -o platformer $(LIBS)
	
platformer_test: Platformer_test.o
	$(CC) Platformer_test.o -o platformer_test $(LIBS)
	
racer: Racer.o Tree.o Actor.o
	$(CC) Racer.o Tree.o Actor.o -o racer $(LIBS)
	
scaling: Scaling.o
	$(CC) Scaling.o -o scaling $(LIBS)
	
shadow: Shadow.o
	$(CC) Shadow.o -o shadow $(LIBS)

shooter: Shooter.o Actor.o Boss.o Enemy.o Explosion.o Ship.o Sin.o
	$(CC) Shooter.o Actor.o Boss.o Enemy.o Explosion.o Ship.o Sin.o -o shooter $(LIBS)
	
tutorial: Tutorial.o
	$(CC) Tutorial.o -o tutorial $(LIBS)

wobble: Wobble.o Sin.o
	$(CC) Wobble.o Sin.o -o wobble $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# OS dependant install
install: 
ifeq ($(OS),Windows_NT)
	copy *.exe ..\bin
else
	cp $(TARGETS) ../bin
endif

# OS dependant clean
clean: clean-custom
ifeq ($(OS),Windows_NT)
	del /Q $(OBJECTS)
	del /Q *.exe
else
	rm $(OBJECTS) $(TARGETS)
endif
