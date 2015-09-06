@echo off
del /Q visualc\*.ncb
del /Q visualc\*.user
rd visualc\Debug /s /q
rd visualc\Release /s /q

del /Q samples\*.exe
del /Q samples\*.class
del /Q samples\*.py
del /Q samples\*.dll
del /Q samples\*.ilk
del /Q samples\*.pdb
del /Q samples\*.lib
del /Q samples\*.exp
