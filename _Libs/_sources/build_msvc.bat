@echo off
set InstallTo=%CD%\..\_builds\win32-msvc
echo %InstallTo%

set CURRENT_TARBALL=""
set oldDir=%CD%
set CACHE_DIR="_build_cache_msvc"
set MSBUILD_BIN=C:\Program Files (x86)\MSBuild\14.0\Bin\
set VCENV=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat
rem set VCENV=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat
set MSBUILD_TOOLSET=v140_xp
set MSBUILD_ARCH=Win32
rem set MSBUILD_ARCH=x64
set MSBUILD_TYPE=Release
rem set MSBUILD_TYPE=Debug

call "%VCENV%"

rem 7-zip path is here
call %CD%\..\..\_Misc\dev_scripts\_paths.bat
rem Using GNU-Sed from Git for Windows installation

set USRBIN=C:\Program Files\Git\usr\bin
if not exist "%USRBIN%\sed.exe" set USRBIN=C:\Program Files (x86)\Git\usr\bin
if not exist "%USRBIN%\sed.exe" goto ErrorNoGit

if not exist "%CACHE_DIR%\NUL" md "%CACHE_DIR%"
if not exist "%InstallTo%\NUL" md "%InstallTo%"
if not exist "%InstallTo%\bin\NUL" md "%InstallTo%\bin"
if not exist "%InstallTo%\lib\NUL" md "%InstallTo%\lib"
if not exist "%InstallTo%\include\NUL" md "%InstallTo%\include"
cd "%CACHE_DIR%"

call :BuildLUAJIT
call :BuildSDL
call :BuildFreeType

cd ..

echo.
echo ======================================================
echo.
echo Libraries installed into %InstallTo%
echo.

set CURRENT_TARBALL=One of QMake-built library

goto ExitX
GOTO :EOF

:BuildSDL
	echo =========SDL2===========
	set CURRENT_TARBALL=SDL2
	call :UnArch SDL-default
	cd SDL-default
	"%USRBIN%\sed.exe" -i -- 's/\#define\( \+\)SDL_AUDIO_DRIVER_WASAPI\( \+\)1/\#define SDL_AUDIO_DRIVER_WASAPI 0/' include\SDL_config_windows.h
	"%MSBUILD_BIN%\MSBuild.exe" VisualC\SDL.sln /p:PlatformToolset=%MSBUILD_TOOLSET% /m /nologo /p:Configuration="%MSBUILD_TYPE%" /p:Platfprm="%MSBUILD_ARCH%"
	IF ERRORLEVEL 1 goto BuildError

	rem ==== Installation ====
	copy "VisualC\%MSBUILD_ARCH%\%MSBUILD_TYPE%\SDL2.lib" "%InstallTo%\lib"
	copy "VisualC\%MSBUILD_ARCH%\%MSBUILD_TYPE%\SDL2.dll" "%InstallTo%\bin"
	copy "VisualC\%MSBUILD_ARCH%\%MSBUILD_TYPE%\SDL2.pdb" "%InstallTo%\lib"
	copy "VisualC\%MSBUILD_ARCH%\%MSBUILD_TYPE%\SDL2main.lib" "%InstallTo%\lib"
	xcopy /Y /E /I include "%InstallTo%\include\SDL2"

	cd ..

	GOTO :EOF

:BuildLUAJIT
	echo =========LuaJIT===========
	set CURRENT_TARBALL=LuaJIT
	call :UnArch luajit
	cd luajit\src
	call msvcbuild.bat static
	IF ERRORLEVEL 1 goto BuildError

	rem ==== Installation ====
	copy "lua51.lib" "%InstallTo%\lib\luajit.lib"
	copy "lua51.pdb" "%InstallTo%\lib\luajit.pdb"
	copy "luajit.exe" "%InstallTo%\bin\luajit.exe"
	if not exist "%InstallTo%\include\luajit-2.1\NUL" md "%InstallTo%\include\luajit-2.1"
	copy "lua.h"     "%InstallTo%\include\luajit-2.1"
	copy "lua.hpp"   "%InstallTo%\include\luajit-2.1"
	copy "luaconf.h" "%InstallTo%\include\luajit-2.1"
	copy "luajit.h"  "%InstallTo%\include\luajit-2.1"
	copy "lualib.h"  "%InstallTo%\include\luajit-2.1"
	copy "lauxlib.h" "%InstallTo%\include\luajit-2.1"

	cd ..\..

	GOTO :EOF

:BuildFreeType
	echo =========FreeType===========
	set CURRENT_TARBALL=FreeType
	call :UnArch freetype-2.7.1
	cd freetype-2.7.1
	"%MSBUILD_BIN%\MSBuild.exe" builds\windows\vc2010\freetype.sln /p:PlatformToolset=%MSBUILD_TOOLSET% /m /nologo /p:Configuration="%MSBUILD_TYPE%" /p:Platfprm="%MSBUILD_ARCH%"
	IF ERRORLEVEL 1 goto BuildError

	rem ==== Installation ====
	if "%MSBUILD_TYPE%"=="Debug" (
	copy "objs\vc2010\%MSBUILD_ARCH%\freetype271d.lib" "%InstallTo%\lib\freetype.lib"
	copy "objs\vc2010\%MSBUILD_ARCH%\freetype271d.pdb" "%InstallTo%\lib\freetype.pdb"
	) ELSE (
	copy "objs\vc2010\%MSBUILD_ARCH%\freetype271.lib" "%InstallTo%\lib\freetype.lib"
	copy "objs\vc2010\%MSBUILD_ARCH%\freetype271.pdb" "%InstallTo%\lib\freetype.pdb"
	)
	xcopy /Y /E /I include "%InstallTo%\include"

	cd ..

	GOTO :EOF

:UnArch
	if not exist %1\NUL (
		echo "%SEVENZIP%\7z" e "../%1.tar.*z*" . ...
		"%SEVENZIP%\7z" x "../%1.tar.gz" && "%SEVENZIP%\7z" x "%1.tar"
		IF ERRORLEVEL 1 (
			echo FAILED!
		) ELSE (
			echo OK
		)
	)
	GOTO :EOF

:BuildError
	echo.
	echo ERROR: Failed build of: %CURRENT_TARBALL%
	echo.
	goto ExitX

:ErrorNoGit
	echo.
	echo ERROR: GIT for Windows is not found: %USRBIN%
	echo.
	goto ExitX

:ErrorNo7z
	echo.
	echo ERROR: 7zip not found: %SEVENZIP%
	echo.

:ExitX
	cd %oldDir%
	if not "%1"=="nopause" pause
