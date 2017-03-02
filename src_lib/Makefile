ifeq ($(OS),Windows_NT)
	include Makefile_mingw
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        include Makefile_linux
    endif
    ifeq ($(UNAME_S),Darwin)
        include Makefile_osx
    endif
endif
