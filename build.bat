@echo off

SET NoPause=0
SET BuildArgs=
SET MAKE_EXTRA_ARGS=-j 4
SET CMakeIt=1
SET CMakeNinja=0
SET CMakeDeploy=0
SET PortableDeploy=0
SET DebugBuild=0

set OldPATH=%PATH%
set SRCDIR=%CD%

:argsloop
if "%1"=="clean" goto cleanX
if "%1"=="update-submodules"  goto updateSubModules
if "%1"=="repair-submodules"  goto repairSubModules
if "%1"=="nopause"  SET NoPause=1
if "%1"=="noqt" SET BuildArgs=%BuildArgs% -DPGE_ENABLE_QT=OFF
if "%1"=="noeditor" SET BuildArgs=%BuildArgs% -DPGE_BUILD_EDITOR=OFF
if "%1"=="noengine" SET BuildArgs=%BuildArgs% -DPGE_BUILD_ENGINE=OFF
if "%1"=="nocalibrator" SET BuildArgs=%BuildArgs% -DPGE_BUILD_PLAYERCALIBRATOR=OFF
if "%1"=="nogifs2png" SET BuildArgs=%BuildArgs% -DPGE_BUILD_GIFS2PNG=OFF
if "%1"=="nopng2gifs" SET BuildArgs=%BuildArgs% -DPGE_BUILD_PNG2GIFS=OFF
if "%1"=="nolazyfixtool" SET BuildArgs=%BuildArgs% -DPGE_BUILD_LAZYFIXTOOL=OFF
rem if "%1"=="nomanager" SET BuildArgs=%BuildArgs% CONFIG+=nomanager
if "%1"=="nomaintainer" SET BuildArgs=%BuildArgs% -DPGE_BUILD_MAINTAINER=OFF
if "%1"=="nomusicplayer" SET BuildArgs=%BuildArgs% -DPGE_BUILD_MUSICPLAYER=OFF
if "%1"=="cmake-it" SET CMakeIt=1
if "%1"=="qmake-it" (
    echo "QMake support has been removed! Please use CMake build instead!"
    goto error
)
if "%1"=="deploy" SET CMakeDeploy=1
if "%1"=="portable" SET PortableDeploy=1
if "%1"=="ninja" SET CMakeNinja=1
if "%1"=="debug" SET DebugBuild=1
if "%1"=="--help" goto Usage
shift
if NOT "%1"=="" goto argsloop

rem ------------------------------------------------------------
rem ------------------------------------------------------------
rem ------------------------------------------------------------
goto SkipUsage
:Usage
echo.
echo === Build script for PGE Project for Windows ===
echo.
echo SYNTAX:
echo.
echo     build.bat [arg1] [arg2] [arg3] ...
echo.
echo AVAILABLE ARGUMENTS:
echo.
echo --- Actions ---
echo  clean                - Remove all object files and caches to build from scratch
echo  update-submodules    - Pull all submodules up to their latest states
echo  repair-submodules    - Repair invalid or broken submodules
echo  --help               - Print this manual
echo  debug                - Run build in debug configuration
echo  ninja                - Use Ninja build system (CMake only build)
echo  deploy               - Automatically run a deploymed
echo.
echo --- Deployment options ---
echo  portable             - Deploy a portable installation that will don't store
echo                         settings, logs, and game saves in system or home directory.
echo.
echo --- Flags ---
echo  nopause              - Disable pause on script completion
echo.
echo --- Disable building of components ---
echo  noqt                  - Skip building of components are using Qt
echo  noeditor              - Skip building of PGE Editor compoment (Qt5)
echo  noengine              - Skip building of PGE Engine compoment (SDL2)
echo  nocalibrator          - Skip building of Playable Character Calibrator (Qt5)
echo  nomaintainer          - Skip building of PGE Maintainer compoment (Qt5)
echo  nomanager             - Skip building of PGE Manager compoment (Qt5)
echo  nomusicplayer         - Skip building of PGE MusPlay compoment (Qt5)
echo  nogifs2png            - Skip building of GIFs2PNG compoment (STL)
echo  nopng2gifs            - Skip building of PNG2GIFs compoment (STL)
echo  nolazyfixtool         - Skip building of LazyFixTool compoment (STL)
echo.
set NoPause=1
goto quit
:SkipUsage

if %DebugBuild%==0 echo ==RELEASE BUILD!==
if %DebugBuild%==1 echo ==DEBUG BUILD!==

if %DebugBuild%==0 set BUILD_DIR_SUFFUX=-release
if %DebugBuild%==1 set BUILD_DIR_SUFFUX=-debug

if %DebugBuild%==0 set CONFIG_CMAKE=Release
if %DebugBuild%==1 set CONFIG_CMAKE=Debug

rem ------------------------------------------------------------
rem ------------------------------------------------------------
rem ------------------------------------------------------------

IF NOT EXIST _paths.bat echo _paths.bat is not exist! Run "generate_paths.bat" first!
IF NOT EXIST _paths.bat goto error

call _paths.bat
PATH=%QtDir%;%MinGW%;%GitDir%;%CMakeDir%;%SystemRoot%\system32;%SystemRoot%;

rem ------------------------------------------------------------
rem ------------------------------------------------------------
rem ------------------------------------------------------------

set BUILD_DIR=%SRCDIR%/build-pge-cmake%BUILD_DIR_SUFFUX%
set INSTALL_DIR=%SRCDIR%/bin-cmake%BUILD_DIR_SUFFUX%

if NOT EXIST "%BUILD_DIR%\NUL" md "%BUILD_DIR%"

cd "%BUILD_DIR%"

if %CMakeNinja%==1 SET CMAKE_GENERATOR=Ninja
if %CMakeNinja%==0 SET CMAKE_GENERATOR=MinGW Makefiles

@echo on
cmake -G "%CMAKE_GENERATOR%" -DCMAKE_PREFIX_PATH="%QtDir%/../" -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE% -DPGE_INSTALL_DIRECTORY="PGE_Project" %BuildArgs% "%SRCDIR%"
@echo off
if ERRORLEVEL 1 goto error

cmake --build . --target all -- %MAKE_EXTRA_ARGS%
if ERRORLEVEL 1 goto error

cmake --build . --target install -- %MAKE_EXTRA_ARGS%
if ERRORLEVEL 1 goto error

cmake --build . --target windeploy -- %MAKE_EXTRA_ARGS%
if ERRORLEVEL 1 goto error

if %CMakeDeploy%==0 goto cMakeSkipDeploy
    rem === Small tweaking inside of the repository ===
    if %PortableDeploy%==1 (
        cmake --build . --target enable_portable
        if ERRORLEVEL 1 goto error
    )

    cmake --build . --target put_online_help
    if ERRORLEVEL 1 goto error

    rem === Packing ZIP archives to publish them later ===
    cmake --build . --target create_zip -- %MAKE_EXTRA_ARGS%
    if ERRORLEVEL 1 goto error

    cmake --build . --target create_zip_tools -- %MAKE_EXTRA_ARGS%
    if ERRORLEVEL 1 goto error

    cmake --build . --target create_zip_install -- %MAKE_EXTRA_ARGS%
    if ERRORLEVEL 1 goto error
:cMakeSkipDeploy

cd "%SRCDIR%"

echo.
echo =========BUILT!!===========
echo.
exit /B 0
goto quit

:cleanX
echo ======== Remove all cached object files and automatically generated Makefiles ========

IF EXIST .\bin-w32\_build_x32\NUL (
                    echo removing bin-w32/_build_x32 ...
                    rmdir /s /q bin-w32\_build_x32
)

IF EXIST .\bin-w64\_build_x32\NUL (
                    echo removing bin-w64/_build_x32 ...
                    rmdir /s /q bin-w64\_build_x32
)

IF EXIST .\bin-w32\_build_x64\NUL (
                    echo removing bin-w32/_build_x64 ...
                    rmdir /s /q bin-w32\_build_x64
)

IF EXIST .\_Libs\_sources\_build_cache\NUL (
    echo 'removing Dependencies build cache ...'
    rmdir /s /q _Libs\_sources\_build_cache
)

IF EXIST .\_Libs\_sources\_build_cache-w64\NUL (
    echo 'removing Dependencies build cache for 64-bit builds ...'
    rmdir /s /q _Libs\_sources\_build_cache-w64
)

IF EXIST .\_Libs\_sources\_build_cache_msvc\NUL (
    echo 'removing Dependencies build cache for MSVC ...'
    rmdir /s /q _Libs\_sources\_build_cache_msvc
)

IF EXIST .\build-pge-cmake-release\NUL (
                    echo removing build-pge-cmake-release ...
                    rmdir /s /q build-pge-cmake-release
)

IF EXIST .\build-pge-cmake-debug\NUL (
                    echo removing build-pge-cmake-debug ...
                    rmdir /s /q build-pge-cmake-debug
)

echo ==== Clear! ====
exit /B 0
goto quit


rem ------------------------------------------------------------
rem ------------------------------------------------------------
rem ------------------------------------------------------------

:updateSubModules
git submodule foreach git checkout master
git submodule foreach git pull origin master
exit /B 0
goto quit


rem ------------------------------------------------------------
rem ------------------------------------------------------------
rem ------------------------------------------------------------

:repairSubModules
rem !!FIXME!! Implement parsing of submodules list and fill this array automatically
rem NOTE: Don't use "git submodule foreach" because broken submodule will not shown in it's list!
set SUBMODULES=_Libs\FreeImage
set SUBMODULES=%SUBMODULES% _Libs\QtPropertyBrowser
set SUBMODULES=%SUBMODULES% _Libs\sqlite3
set SUBMODULES=%SUBMODULES% _Libs\AudioCodecs
set SUBMODULES=%SUBMODULES% _Libs\SDL_Mixer_X
set SUBMODULES=%SUBMODULES% _common\PGE_File_Formats
set SUBMODULES=%SUBMODULES% _common\PgeGameSave\submodule
set SUBMODULES=%SUBMODULES% Content\help
rem \===============================================================================
for %%s in (%SUBMODULES%) do (
echo Remove folder %%s ...
if exist %%s\nul rd /Q /S %%s
)
echo Fetching new submodules...
git submodule init
git submodule update
echo.
git submodule foreach git checkout master
git submodule foreach git pull origin master
echo.
echo ==== Fixed! ====
exit /B 0
goto quit

rem ------------------------------------------------------------
rem ------------------------------------------------------------
rem ------------------------------------------------------------

:error
echo.
echo =========ERROR!!===========
echo.
PATH=%OldPATH%
cd "%SRCDIR%"
if "%NoPause%"=="0" pause
exit /B 1

:quit
PATH=%OldPATH%
if "%NoPause%"=="0" pause
