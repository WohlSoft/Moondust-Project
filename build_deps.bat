@echo off

IF NOT EXIST _paths.bat echo _paths.bat is not exist! Run "generate_paths.bat" first!
IF NOT EXIST _paths.bat goto error

SET NoPause=0
SET BuildLibs=0
SET BuildLibsOnly=0
SET MAKE_EXTRA_ARGS=-r -s

:argsloop
if "%1"=="build-libs"       SET BuildLibs=1
if "%1"=="build-libs-only"  SET BuildLibs=1
if "%1"=="build-libs-only"  SET BuildLibsOnly=1
if "%1"=="nopause"          SET NoPause=1
if "%1"=="--help" (
    echo.
    echo PGE Project Dependency builder for Windows
    echo.
    echo Usage:
    echo.
    echo     build_deps.bat [arg1] [arg2] [arg3] ...
    echo.
    echo Arguments:
    echo.
    echo build-libs           - Rebuild LuaJIT, SDL2, FreeType and GLEW from scratch
    echo build-libs-only      - Don't build QMake based libraries
    echo nopause              - Disable pause on script completion
    echo --help               - Print this manual
    echo.
    set NoPause=1
    set OldPATH=%PATH%
    goto quit
)
shift
if NOT "%1"=="" goto argsloop

call _paths.bat
set OldPATH=%PATH%
PATH=%QtDir%;%MinGW%;%GitDir%;%SystemRoot%\system32;%SystemRoot%

IF "%MINGWx64Dest%"=="yes" (
    SET QMAKE_EXTRA_ARGS=CONFIG+=win64
)

echo %PATH%

echo Clonning missing submodules...
"%GitDir%\git.exe" submodule update --init --recursive
if ERRORLEVEL 1 goto error

if NOT "%BuildLibs%"=="1" goto skipBuildLibs
rem Build dependent libraries
set TempOldPATH=%PATH%
cd _Libs\_sources
set PATH=C:\MinGW\msys\1.0\bin;%SystemRoot%\system32;%SystemRoot%

set MSYSTEM=MINGW32

if not "x%PROCESSOR_ARCHITECTURE%" == "xAMD64" goto _NotX64
set OldCOMSPEC=%COMSPEC%
set COMSPEC=%WINDIR%\SysWOW64\cmd.exe
:_NotX64

if "%MINGWx64Dest%"=="yes" set PATH=C:\MinGW-w64\x86_64-7.1.0-posix-seh-rt_v5-rev0\mingw64\bin;%PATH%
if "%MINGWx64Dest%"=="yes" set BahsCmd="./build_mingw-w64.sh"
if NOT "%MINGWx64Dest%"=="yes" set PATH=C:\MinGW\bin;%PATH%
if NOT "%MINGWx64Dest%"=="yes" set BahsCmd="echo $PWD; ./build_mingw.sh"

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
if "%BuildLibsOnly%"=="1" goto buildCompleted
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

