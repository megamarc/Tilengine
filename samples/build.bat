rem Builds samples for Windows x64 using tiny c compiler (https://bellard.org/tcc/)
rem Not cross-platform, just fast builds for my own tests

setlocal
set flags=-I"../include" ../lib/x64/Tilengine.dll

tcc %flags% Barrel.c Simon.c -o barrel.exe
tcc %flags% Mode7.c Sin.c -o mode7.exe
tcc %flags% Platformer.c -o platformer.exe
tcc %flags% Racer.c Tree.c Actor.c -o racer.exe
tcc %flags% Scaling.c -o scaling.exe
tcc %flags% Shadow.c -o shadow.exe
tcc %flags% Shooter.c Actor.c Boss.c Enemy.c Explosion.c Ship.c Sin.c -o shooter.exe
tcc %flags% Tutorial.c -o tutorial.exe
tcc %flags% Wobble.c Sin.c -o wobble.exe
tcc %flags% ColorCycle.c -o colorcycle.exe
tcc %flags% Benchmark.c -o benchmark.exe
tcc %flags% SuperMarioClone.c -o supermarioclone.exe
tcc %flags% TestMouse.c -I"../src/sdl" -o testmouse.exe
tcc %flags% Forest.c -o forest.exe
tcc %flags% QueryLayer.c -o querylayer.exe
