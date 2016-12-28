@echo off

setlocal
set copycmd=/y
set platform=armeabi-v7a
set basepath=deploy\android
set srcpath=src_samples
set srclibpath=src_lib
set libpath=lib
set binpath=bin
set bindpath=bindings
set javapath=%bindpath%\java

rem create folders
rem md %basepath%
md %basepath%\%srclibpath%
md %basepath%\%libpath%\%platform%
md %basepath%\%javapath%\bin
md %basepath%\%javapath%\src

rem copy public sources
copy %srclibpath%\Base64.c %basepath%\%srclibpath%
copy %srclibpath%\Load*.* %basepath%\%srclibpath%
copy %srclibpath%\simplexml.* %basepath%\%srclibpath%
copy %srclibpath%\Window.c %basepath%\%srclibpath%

rem copy library
copy %libpath%\*.* %basepath%\%libpath%
copy %libpath%\%platform%\*.* %basepath%\%libpath%\%platform%

rem copy java files
copy %javapath%\bin\*.* %basepath%\%javapath%\bin
copy %javapath%\bin\%platform%\*.* %basepath%\%javapath%\bin
copy %javapath%\src_jni\*.c %basepath%\%javapath%\src
copy %javapath%\src_java\*.java %basepath%\%javapath%\src

cd %basepath%
"C:\Program Files\7-Zip\7z" a -r tilengine_android.zip * *.*
move tilengine_android.zip ..
endlocal

:end