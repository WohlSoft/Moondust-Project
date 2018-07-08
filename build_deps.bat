@echo off

SET DebugArgs=0
SET NoPause=0
SET PGE_DEPS_BuildLibs=0
SET PGE_DEPS_BuildLibsOnly=0

if "%1"=="debugscript" SET DebugArgs=1

SET MAKE_EXTRA_ARGS=-r -s

if "x%DebugArgs%"=="x1" echo Begin arguments parsing...
:argsloop
if "x%DebugArgs%"=="x1" echo Pass argument %1...
if "x%DebugArgs%"=="x1" echo Arg...
if "%1"=="debugscript"      SET DebugArgs=1
if "x%DebugArgs%"=="x1" echo Arg...
if "%1"=="build-libs"       SET PGE_DEPS_BuildLibs=1
if "x%DebugArgs%"=="x1" echo Arg...
rem Workaround for weird AppVeyor's error
if "%1"=="buildlibs"        SET PGE_DEPS_BuildLibs=1
if "x%DebugArgs%"=="x1" echo Arg...
if "%1"=="build-libs-only"  SET PGE_DEPS_BuildLibs=1
if "x%DebugArgs%"=="x1" echo Arg...
if "%1"=="build-libs-only"  SET PGE_DEPS_BuildLibsOnly=1
if "x%DebugArgs%"=="x1" echo Arg...
if "%1"=="nopause"          SET NoPause=1
if "x%DebugArgs%"=="x1" echo Help arg...
if "%1"=="--help" goto Usage
if "x%DebugArgs%"=="x1" echo Shift...
shift
if "x%DebugArgs%"=="x1" echo Loop...
if NOT "%1"=="" goto argsloop

goto SkipUsage
:Usage
echo.
echo === PGE Project Dependency builder for Windows ===
echo.
echo SYNTAX:
echo.
echo     build_deps.bat [arg1] [arg2] [arg3] ...
echo.
echo AVAILABLE ARGUMENTS:
echo.
echo --- Actions ---
echo  --help              - Print this manual
echo.
echo --- Flags ---
echo  build-libs          - Rebuild LuaJIT, SDL2, FreeType and GLEW from scratch
echo  build-libs-only     - Don't build QMake based libraries
echo  nopause             - Disable pause on script completion
echo.
set NoPause=1
set OldPATH=%PATH%
goto quit
:SkipUsage

if "x%DebugArgs%"=="x1" echo Checking for _paths.bat existing...
IF NOT EXIST _paths.bat echo _paths.bat is not exist! Run "generate_paths.bat" first!
IF NOT EXIST _paths.bat goto error

if "x%DebugArgs%"=="x1" echo Call "_paths.bat"...
call _paths.bat
set OldPATH=%PATH%

if "x%DebugArgs%"=="x1" echo Change PATH...
PATH=%QtDir%;%MinGW%;%GitDir%;%SystemRoot%\system32;%SystemRoot%

IF "%MINGWx64Dest%"=="yes" (
	if "x%DebugArgs%"=="x1" echo Set 64-bit build...
    SET QMAKE_EXTRA_ARGS=CONFIG+=win64
)
IF "%MINGWx32Dest%"=="yes" (
	if "x%DebugArgs%"=="x1" echo Set 32-bit MinGW-w64 build...
    SET QMAKE_EXTRA_ARGS=CONFIG+=win32-mingw-w64
)

echo %PATH%

echo Clonning missing submodules...
"%GitDir%\git.exe" submodule update --init --recursive
if ERRORLEVEL 1 goto error

if NOT "%PGE_DEPS_BuildLibs%"=="1" goto skipBuildLibs
rem Build dependent libraries
set TempOldPATH=%PATH%
cd _Libs\_sources
set PATH=C:\MinGW\msys\1.0\bin;%SystemRoot%\system32;%SystemRoot%

set MSYSTEM=MINGW32

if not "x%PROCESSOR_ARCHITECTURE%" == "xAMD64" goto _NotX64
set OldCOMSPEC=%COMSPEC%
set COMSPEC=%WINDIR%\SysWOW64\cmd.exe
:_NotX64

if NOT "%MINGWx64Dest%"=="yes" if NOT "%MINGWx32Dest%"=="yes" set PATH=C:\MinGW\bin;%PATH%
if NOT "%MINGWx64Dest%"=="yes" if NOT "%MINGWx32Dest%"=="yes" set BahsCmd="echo $PWD; ./build_mingw.sh"
if "%MINGWx64Dest%"=="yes" set PATH=%MinGW%;%PATH%
if "%MINGWx64Dest%"=="yes" set BahsCmd="./build_mingw-w64.sh"
if "%MINGWx32Dest%"=="yes" set PATH=%MinGW%;%PATH%
if "%MINGWx32Dest%"=="yes" set BahsCmd="./build_mingw-w64-32.sh"

echo %PATH%
bash -i -c %BahsCmd%
if ERRORLEVEL 1 (
    set COMSPEC=%OldCOMSPEC%
    cd ..\..
    goto error
)
cd ..\..
set COMSPEC=%OldCOMSPEC%
set PATH=%TempOldPATH%
set MSYSTEM=
echo %PATH%
if "%PGE_DEPS_BuildLibsOnly%"=="1" goto buildCompleted
:skipBuildLibs

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

:buildCompleted

echo.
echo =========BUILT!!===========
echo.

goto quit
:error
echo.
echo =========ERROR!!===========
echo.
PATH=%OldPATH%
if "%NoPause%"=="0" pause
exit /B 1
:quit
PATH=%OldPATH%
if "%NoPause%"=="0" pause

