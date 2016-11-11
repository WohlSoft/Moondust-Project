@echo off

IF NOT EXIST _paths.bat echo _paths.bat is not exist! Run "generate_paths.bat" first!
IF NOT EXIST _paths.bat goto error

SET MAKE_EXTRA_ARGS=-r -j 4

call _paths.bat
set OldPATH=%PATH%
PATH=%QtDir%;%MinGW%;%GitDir%;%SystemRoot%\system32;%SystemRoot%;

echo %PATH%

cd _Libs

rem build all components
%QtDir%\qmake.exe CONFIG+=release CONFIG-=debug DEFINES+=USE_LUA_JIT
if ERRORLEVEL 1 goto error

%MinGW%\mingw32-make %MAKE_EXTRA_ARGS%
if ERRORLEVEL 1 goto error

rem copy data and configs into the build directory

%MinGW%\mingw32-make install
if ERRORLEVEL 1 goto error

cd ..

echo.
echo =========BUILT!!===========
echo.

goto quit
:error
echo.
echo =========ERROR!!===========
echo.
PATH=%OldPATH%
if not "%1"=="nopause" pause
exit /B 1
:quit
PATH=%OldPATH%
if not "%1"=="nopause" pause

