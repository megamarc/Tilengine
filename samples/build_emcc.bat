rem Builds samples for WebAssembly using emcc (https://emscripten.org/)
rem Not cross-platform, just fast builds under Windows

setlocal
set flags=-O1 -I"../include" -L"../lib/emcc" -lz -lpng -lSDL2 -lTilengine
set outdir=wasm

call emcc %flags% --shell-file html/barrel.html Barrel.c -o %outdir%/barrel.html --preload-file assets/sc4
call emcc %flags% --shell-file html/mode7.html Mode7.c -o %outdir%/mode7.html --preload-file assets/smk
call emcc %flags% --shell-file html/platformer.html Platformer.c -o %outdir%/platformer.html --preload-file assets/sonic
call emcc %flags% --shell-file html/racer.html Racer.c Tree.c Actor.c -o %outdir%/racer.html --preload-file assets/racer
call emcc %flags% --shell-file html/scaling.html Scaling.c -o %outdir%/scaling.html --preload-file assets/fox
call emcc %flags% --shell-file html/shadow.html Shadow.c -o %outdir%/shadow.html --preload-file assets/sotb
call emcc %flags% --shell-file html/shooter.html Shooter.c Actor.c Boss.c Enemy.c Explosion.c Ship.c -o %outdir%/shooter.html --preload-file assets/tf4
call emcc %flags% --shell-file html/tutorial.html Tutorial.c -o %outdir%/tutorial.html --preload-file assets/sonic
call emcc %flags% --shell-file html/wobble.html Wobble.c -o %outdir%/wobble.html --preload-file assets/tf3
call emcc %flags% --shell-file html/colorcycle.html ColorCycle.c -o %outdir%/colorcycle.html --preload-file assets/color
call emcc %flags% --shell-file html/forest.html Forest.c -o %outdir%/forest.html --preload-file assets/forest
call emcc %flags% --shell-file html/layerwindow.html LayerWindow.c -o %outdir%/layerwindow.html --preload-file assets/shots
call emcc %flags% --shell-file html/layercircle.html LayerCircle.c -o %outdir%/layercircle.html --preload-file assets/shots

xcopy html\assets\*.* wasm\assets /Y /I
