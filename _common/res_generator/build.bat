@echo off
call ..\..\_paths.bat
PATH=%MinGW%;%PATH%

md bin
g++ -std=c++11 -c -I../IniProcessor/ -I../Utf8Main/ res_generator.cpp -o bin/res_generator.o -Wall
g++ -std=c++11 -c -I../IniProcessor/ ../IniProcessor/ini_processing.cpp -o bin/ini_processing.o -Wall
g++ -std=c++11 -c -I../Utf8Main/ ../Utf8Main/utf8main_win32.cpp -o bin/utf8main_win32.o -Wall

g++ bin/ini_processing.o bin/res_generator.o bin/utf8main_win32.o -o bin/res_generator

rem # Build resources!
bin\res_generator.exe --config resource.ini "../../Engine/_resources/"

rd /S /Q bin
