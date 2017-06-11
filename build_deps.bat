@echo off

IF NOT EXIST _paths.bat echo _paths.bat is not exist! Run "generate_paths.bat" first!
IF NOT EXIST _paths.bat goto error

SET MAKE_EXTRA_ARGS=-r -s

call _paths.bat
set OldPATH=%PATH%
PATH=%QtDir%;%MinGW%;%GitDir%;%SystemRoot%\system32;%SystemRoot%;

IF "%MINGWx64Dest%"=="yes" (
	SET QMAKE_EXTRA_ARGS=CONFIG+=win64
)

echo %PATH%

echo Clonning missing submodules...
"%GitDir%\git.exe" submodule update --init --recursive
if ERRORLEVEL 1 goto error

cd _Libs

rem build all components
echo Running %QtDir%\qmake.exe...
"%QtDir%\qmake.exe" pge_deps.pro CONFIG+=release CONFIG-=debug DEFINES+=USE_LUA_JIT %QMAKE_EXTRA_ARGS%
if ERRORLEVEL 1 goto error

echo Building...
"%MinGW%\mingw32-make.exe" %MAKE_EXTRA_ARGS% release
if ERRORLEVEL 1 goto error

rem copy data and configs into the build directory

echo Installing...
"%MinGW%\mingw32-make.exe" -s install
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

