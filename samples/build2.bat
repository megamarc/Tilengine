rem Builds samples for Windows x64 using tiny c compiler (https://bellard.org/gcc/)
rem Not cross-platform, just fast builds for my own tests

setlocal
set flags=-I"../include" ../lib/x64/Tilengine.dll

gcc %flags% Barrel.c Simon.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o barrel.exe
gcc %flags% Mode7.c Sin.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o mode7.exe
gcc %flags% Platformer.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o platformer.exe
gcc %flags% Racer.c Tree.c Actor.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o racer.exe
gcc %flags% Scaling.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o scaling.exe
gcc %flags% Shadow.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o shadow.exe
gcc %flags% Shooter.c Actor.c Boss.c Enemy.c Explosion.c Ship.c Sin.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o shooter.exe
gcc %flags% Tutorial.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o tutorial.exe
gcc %flags% Wobble.c Sin.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o wobble.exe
gcc %flags% ColorCycle.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o colorcycle.exe
gcc %flags% Benchmark.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o benchmark.exe
gcc %flags% SuperMarioClone.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o supermarioclone.exe
gcc %flags% TestMouse.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o testmouse.exe
gcc %flags% Forest.c -I"../src/sdl" -lmingw32 -lSDL2main -lSDL2 -lwinmm -o forest.exe
