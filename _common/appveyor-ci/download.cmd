@echo on

if NOT EXIST C:\temp-d\NUL md C:\temp-d

set URL_NINJA=http://wohlsoft.ru/docs/Software/Ninja-Build/ninja-win.zip
set NINJA_ZIP=C:\temp-d\ninja-win.zip

set URL_MINGW64=http://wohlsoft.ru/docs/Software/QtBuilds/MinGW-w64-x86_64-7.1.0-posix-seh-rt_v5-rev0.7z
set MINGW64_7z=C:\temp-d\MinGW-w64-x86_64-7.1.0-posix-seh-rt_v5-rev0.7z

set URL_QT64=http://wohlsoft.ru/docs/Software/QtBuilds/Qt-5.9.0-x64-shared-mingw-w64-posix-seh-7-1-0.7z
set QT64_7z=C:\temp-d\Qt-5.9.0-x64-shared-mingw-w64-posix-seh-7-1-0.7z

echo "Downloading NinjaBuild..."
"%WGET_BIN%" --quiet %URL_NINJA% -O "%NINJA_ZIP%"
if ERRORLEVEL 1 goto error

rem TODO: Provide 32-bit MinGW-w64 alternative 32-bit builds chain
rem       to prevent any possible missings of toolchains in a case of AppVeyor images
if [%PLATFORM%]==[x64] (
    echo "Downloading MinGW-w64..."
    "%WGET_BIN%" --quiet %URL_MINGW64% -O "%MINGW64_7z%"
    if ERRORLEVEL 1 goto error

    echo "Downloading Qt for MinGW-w64..."
    "%WGET_BIN%" --quiet %URL_QT64% -O "%QT64_7z%"
    if ERRORLEVEL 1 goto error

    echo "Installing MinGW-w64..."
    "%SEVENZIP%" x "%MINGW64_7z%" -oC:\MinGW-w64
    if ERRORLEVEL 1 goto error

    echo "Installing Qt64..."
    "%SEVENZIP%" x "%QT64_7z%" -oC:\Qt
    if ERRORLEVEL 1 goto error
)

echo "Installing NinjaBuild..."
"%SEVENZIP%" x "%NINJA_ZIP%" -o"%COMPILER%"
if ERRORLEVEL 1 goto error

goto quit

:error
echo.
echo =========ERROR!!===========
echo.
exit /B 1

:quit
exit /B 0
