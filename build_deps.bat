@echo off

IF NOT EXIST _paths.bat echo _paths.bat is not exist! Run "generate_paths.bat" first!
IF NOT EXIST _paths.bat goto error

call _paths.bat
set OldPATH=%PATH%
PATH=%QtDir%;%MinGW%;%SystemRoot%\system32;%SystemRoot%;

echo %PATH%

cd _Libs

rem build all components
%QtDir%\qmake.exe CONFIG+=release CONFIG-=debug
if ERRORLEVEL 1 goto error

%MinGW%\mingw32-make
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
:quit
PATH=%OldPATH%
pause
