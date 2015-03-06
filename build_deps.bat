@echo off

call _paths.bat

PATH=%QtDir%;%MinGW%;%PATH%

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
pause
