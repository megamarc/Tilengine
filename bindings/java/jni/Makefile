CC       = gcc
SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:.c=.o)
RM       = rm

name := $(shell uname -s)
arch := $(shell uname -m)
ifeq ($(name),Linux)
	ifeq ($(arch),i686)
		JAVA_HOME = /usr/lib/jvm/java-1.8.0-openjdk-i386
		INCS = -I"$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/linux"
		CFLAGS = $(INCS) -m32 -msse2 -std=c99 -fpic -O2
	endif
	ifeq ($(arch),x86_64)
		JAVA_HOME = /usr/lib/jvm/java-1.8.0-openjdk-amd64
		INCS = -I"$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/linux"
		CFLAGS = $(INCS) -m64 -msse2 -std=c99 -fpic -O2			
	endif
	ifeq ($(arch),armv7l)
		JAVA_HOME= /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt
		INCS = -I"$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/linux"
		CFLAGS = $(INCS) -std=c99 -fpic -O2
	endif
	LIBS = -lTilengine -lc
	BIN = libTilengineJNI.so
endif
ifeq ($(name),Darwin)
	JAVA_HOME= $(shell /usr/libexec/java_home)
	INCS = -I"$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/darwin"
	CFLAGS = -std=c99 -O2
	LIBS = "/usr/local/lib/Tilengine.dylib" -lc
	BIN = libTilengineJNI.jnilib
endif

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	$(RM) $(OBJECTS) $(BIN)

$(BIN): $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $(BIN) $(LIBS) -s -fvisibility=hidden

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
