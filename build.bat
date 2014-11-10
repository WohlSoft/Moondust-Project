@echo off
set QtDir= D:\Qt\5.3.1_Static\5.3.2\bin
set MinGW= D:\DevTools\Qt\5.3.0\Tools\mingw482_32\bin

PATH=%QtDir%;%MinGW%;%PATH%

cd %CD%\Editor
%QtDir%\lrelease.exe *.pro
cd ..

rem build all components
%QtDir%\qmake.exe CONFIG+=release CONFIG-=debug
if ERRORLEVEL 1 goto error

%MinGW%\mingw32-make release
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
pause
