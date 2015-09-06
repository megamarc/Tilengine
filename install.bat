rem Windows install script
setlocal
set platform=win32
set target=samples
copy lib\%platform%\*.dll %target% /Y
copy samples\%platform%\*.exe %target% /Y
copy Java\%platform%\*.dll %target%
copy Java\class\*.class %target%
copy Python\*.py %target%
cd %target%
