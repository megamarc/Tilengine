rem Builds samples for Windows x64 using tiny c compiler (https://bellard.org/gcc/)
rem Not cross-platform, just fast builds for my own tests

setlocal
set flags=-I"../include" ../lib/x64/Tilengine.dll ../lib/x64/SDL2.dll

gcc %flags% Barrel.c Simon.c -o barrel.exe
gcc %flags% Mode7.c Sin.c -o mode7.exe
gcc %flags% Platformer.c -o platformer.exe
gcc %flags% Racer.c Tree.c Actor.c -o racer.exe
gcc %flags% Scaling.c -o scaling.exe
gcc %flags% Shadow.c -o shadow.exe
gcc %flags% Shooter.c Actor.c Boss.c Enemy.c Explosion.c Ship.c Sin.c -o shooter.exe
gcc %flags% Tutorial.c -o tutorial.exe
gcc %flags% Wobble.c Sin.c -o wobble.exe
gcc %flags% ColorCycle.c -o colorcycle.exe
gcc %flags% Benchmark.c -o benchmark.exe
gcc %flags% SuperMarioClone.c -o supermarioclone.exe
gcc %flags% TestMouse.c -I"../src/sdl" -o testmouse.exe
gcc %flags% Forest.c -o forest.exe
gcc %flags% QueryLayer.c -o querylayer.exe
