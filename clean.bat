@echo off
del /Q visualc\*.ncb
del /Q visualc\*.user
del /Q src_lib\*.o
del /Q jni\*.o
rd visualc\Debug /s /q
rd visualc\Release /s /q
rd src_lib\Debug /s /q
rd src_lib\Release /s /q
rd jni/Debug /s /q
rd jni/Release /s /q

del /Q samples\*.exe
del /Q samples\*.class
del /Q samples\*.py*
del /Q samples\*.dll
del /Q samples\*.ilk
del /Q samples\*.pdb
del /Q samples\*.lib
del /Q samples\*.exp
