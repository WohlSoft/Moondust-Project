@echo off
rem IF EXIST _paths.bat goto edit

set QT_VERSION=undefined\mingw53_32
set MINGW_VERSION=undefined
set GIT_PATH=undefined
set CMAKE_PATH=undefined

:detectQt
FOR %%d IN (5.4 5.4.0 5.4.1 5.4.2 5.5 5.5.0 5.5.1 5.6 5.6.0 5.6.1 5.6.1-1 5.7 5.7.1 5.7.2 5.8 5.8.1 5.8.2 5.8.3 5.9 5.9.1 5.9.2 5.9.3 5.10 5.10.1 5.10.2 5.10.3) DO IF EXIST C:\Qt\%%d FOR %%e IN (mingw480_32 mingw491_32 mingw492_32 mingw53_32) DO IF EXIST C:\Qt\%%d\%%e set QT_VERSION=%%d\%%e

:detectMinGW
FOR %%e IN (mingw480_32 mingw491_32 mingw492_32 mingw530_32) DO IF EXIST C:\Qt\Tools\%%e set MINGW_VERSION=%%e

:detectGit
IF EXIST "%ProgramFiles%\Git\cmd" set GIT_PATH=%ProgramFiles%\Git\cmd
IF EXIST "%ProgramFiles(x86)%\Git\cmd" set GIT_PATH=%ProgramFiles(x86)%\Git\cmd
IF EXIST "%APPDATA%\Programs\Git\cmd" set GIT_PATH=%APPDATA%\Programs\Git\cmd
IF EXIST "%LOCALAPPDATA%\Programs\Git\cmd" set GIT_PATH=%LOCALAPPDATA%\Programs\Git\cmd
IF EXIST "%SystemDrive%\Git\cmd" set GIT_PATH=%SystemDrive%\Git\cmd

:detectCMake
IF EXIST "%ProgramFiles%\CMake\bin" set CMAKE_PATH=%ProgramFiles%\CMake\bin
IF EXIST "%ProgramFiles(x86)%\CMake\bin" set CMAKE_PATH=%ProgramFiles(x86)%\CMake\bin
IF EXIST "%APPDATA%\Programs\CMake\bin" set CMAKE_PATH=%APPDATA%\Programs\CMake\bin
IF EXIST "%LOCALAPPDATA%\Programs\CMake\bin" set CMAKE_PATH=%LOCALAPPDATA%\Programs\CMake\bin
IF EXIST "%SystemDrive%\CMake\bin" set CMAKE_PATH=%SystemDrive%\CMake\bin

echo @echo off > _paths.bat
echo rem =============================================================================================== >> _paths.bat
echo rem ==========================PLEASE SET THE QT, MINGW and GIT PATHS=============================== >> _paths.bat
echo rem =============================================================================================== >> _paths.bat
echo. >> _paths.bat
echo rem -----------------Actual path to Qt----------------- >> _paths.bat
echo set QtDir=C:\Qt\%QT_VERSION%\bin>> _paths.bat
echo. >> _paths.bat
echo rem ------------Actual path to MinGW compiler---------- >> _paths.bat
echo set MinGW=C:\Qt\Tools\%MINGW_VERSION%\bin>> _paths.bat
echo rem --Uncomment this if you building for i686 with MinGW-w64-- >> _paths.bat
echo rem set MINGWx32Dest=yes>> _paths.bat
echo rem --Uncomment this if you building for x86_64 with MinGW-w64-- >> _paths.bat
echo rem set MINGWx64Dest=yes>> _paths.bat
echo. >> _paths.bat
echo rem ------------Actual path to git for Windows--------- >> _paths.bat
echo set GitDir=%GIT_PATH%>> _paths.bat
echo. >> _paths.bat
echo rem ------------Actual path to CMake for Windows--------- >> _paths.bat
echo set CMakeDir=%CMAKE_PATH%>> _paths.bat
echo. >> _paths.bat
echo rem ===============================================================================================>> _paths.bat
echo rem ===============================================================================================>> _paths.bat
echo rem ===============================================================================================>> _paths.bat

rem If Qt or MinGW are not been detected
IF "%QT_VERSION%"=="undefined\mingw53_32" goto edit
IF "%MINGW_VERSION%"=="undefined" goto edit
IF "%GIT_PATH%"=="undefined" goto edit
IF "%CMAKE_PATH%"=="undefined" goto edit

echo ===============Detected Qt version %QT_VERSION%====================
C:\Qt\%QT_VERSION%\bin\qmake -v
echo.
echo ============Detected MinGW version %MINGW_VERSION%=================
C:\Qt\Tools\%MINGW_VERSION%\bin\gcc --version

echo =======================Detected GIT================================
"%GIT_PATH%\git.exe" --version
echo ===================================================================

echo =======================Detected CMake================================
"%CMAKE_PATH%\cmake.exe" --version
echo ===================================================================

pause
goto quit
:edit
start notepad _paths.bat
:quit

