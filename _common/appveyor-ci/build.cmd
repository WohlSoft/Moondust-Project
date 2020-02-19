@echo off

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

build.bat nopause ninja deploy portable
