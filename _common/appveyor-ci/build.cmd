@echo on

rem build_deps.bat buildlibs nopause
rem build.bat nopause # noengine nocalibrator nogifs2png nopng2gifs nolazyfixtool nomanager nomaintainer nomusicplayer
rem cd _Misc\dev_scripts\deploy\
rem if [%PLATFORM%]==[Win32] deploy_windows_dynamic.bat nopause
rem if [%PLATFORM%]==[Win32] deploy_windows_dynamic_32_w64.bat nopause
rem if [%PLATFORM%]==[x64] deploy_windows_dynamic_x64.bat nopause
rem cd ..\..\..\

echo == Contents of _paths.bat: ==
type _paths.bat
echo =============================
echo QtDir=%QtDir%
"%QtDir%\qmake" --version
echo MinGW=%MinGW%
"%MinGW%\gcc" --version
echo GitDir=%GitDir%
"%GitDir%\git" --version
echo CMakeDir=%CMakeDir%
"%CMakeDir%\cmake" --version
echo =============================

build.bat nopause cmake-it ninja deploy
