@echo off

SET NoPause=0
SET BuildArgs=
SET MAKE_EXTRA_ARGS=-r -j 4

:argsloop
if "%1"=="clean"  goto cleanX
if "%1"=="repair-submodules"  goto repairSubModules
if "%1"=="nopause"  SET NoPause=1
if "%1"=="noeditor" SET BuildArgs=%BuildArgs% CONFIG+=noeditor
if "%1"=="noengine" SET BuildArgs=%BuildArgs% CONFIG+=noengine
if "%1"=="nocalibrator" SET BuildArgs=%BuildArgs% CONFIG+=nocalibrator
if "%1"=="nogifs2png" SET BuildArgs=%BuildArgs% CONFIG+=nogifs2png
if "%1"=="nopng2gifs" SET BuildArgs=%BuildArgs% CONFIG+=nopng2gifs
if "%1"=="nolazyfixtool" SET BuildArgs=%BuildArgs% CONFIG+=nolazyfixtool
if "%1"=="nomanager" SET BuildArgs=%BuildArgs% CONFIG+=nomanager
if "%1"=="nomaintainer" SET BuildArgs=%BuildArgs% CONFIG+=nomaintainer
if "%1"=="nomusicplayer" SET BuildArgs=%BuildArgs% CONFIG+=nomusicplayer
shift
if NOT "%1"=="" goto argsloop

IF NOT EXIST _paths.bat echo _paths.bat is not exist! Run "generate_paths.bat" first!
IF NOT EXIST _paths.bat goto error

call _paths.bat
set OldPATH=%PATH%
PATH=%QtDir%;%MinGW%;%GitDir%;%SystemRoot%\system32;%SystemRoot%;

IF "%MINGWx64Dest%"=="yes" (
	SET BuildArgs=%BuildArgs% CONFIG+=win64
)

goto run
:cleanX
echo ======== Remove all cached object files and automatically generated Makefiles ========

call ./clean_make.bat nopause

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

echo ==== Clear! ====
exit /B 0
goto quit;

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
goto quit;


:run
cd %CD%\Editor
%QtDir%\lrelease.exe *.pro
cd ..\Engine
%QtDir%\lrelease.exe *.pro
cd ..

rem build all components
%QtDir%\qmake.exe CONFIG+=release CONFIG-=debug %BuildArgs%
if ERRORLEVEL 1 goto error

%MinGW%\mingw32-make %MAKE_EXTRA_ARGS% release
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
PATH=%OldPATH%
if "%NoPause%"=="0" pause
exit /B 1
:quit
PATH=%OldPATH%
if "%NoPause%"=="0" pause
