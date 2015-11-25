@echo off

SET NoPause=0
SET BuildArgs=

:argsloop
if "%1"=="nopause"  SET NoPause=1
if "%1"=="noengine" SET BuildArgs=%BuildArgs% CONFIG+=noengine
shift
if NOT "%1"=="" goto argsloop

IF NOT EXIST _paths.bat echo _paths.bat is not exist! Run "generate_paths.bat" first!
IF NOT EXIST _paths.bat goto error

call _paths.bat
set OldPATH=%PATH%
PATH=%QtDir%;%MinGW%;%GitDir%;%SystemRoot%\system32;%SystemRoot%;

cd %CD%\Editor
%QtDir%\lrelease.exe *.pro
cd ..

rem build all components
%QtDir%\qmake.exe CONFIG+=release CONFIG-=debug %BuildArgs%
if ERRORLEVEL 1 goto error

%MinGW%\mingw32-make
if ERRORLEVEL 1 goto error

rem copy data and configs into the build directory

%MinGW%\mingw32-make install
if ERRORLEVEL 1 goto error

echo.
echo =========BUILT!!===========
echo.

goto quit
:error
echo.
echo =========ERROR!!===========
echo.
:quit
PATH=%OldPATH%
if not "%1"=="nopause" pause

