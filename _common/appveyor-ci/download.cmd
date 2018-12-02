@echo on

if NOT EXIST C:\temp-d\NUL md C:\temp-d

set URL_NINJA=http://wohlsoft.ru/docs/Software/Ninja-Build/ninja-win.zip
set NINJA_ZIP=C:\temp-d\ninja-win.zip

rem ======= Legacy, kept for temporary =======
rem set URL_MINGW64=http://wohlsoft.ru/docs/Software/QtBuilds/MinGW-w64-x86_64-7.1.0-posix-seh-rt_v5-rev0.7z
rem set MINGW64_7z=C:\temp-d\MinGW-w64-x86_64-7.1.0-posix-seh-rt_v5-rev0.7z
rem set URL_QT64=http://wohlsoft.ru/docs/Software/QtBuilds/Qt-5.9.0-x64-shared-mingw-w64-posix-seh-7-1-0.7z
rem set QT64_7z=C:\temp-d\Qt-5.9.0-x64-shared-mingw-w64-posix-seh-7-1-0.7z

rem ============ MinGW-w64 32-bit ============
set URL_MINGW32=http://wohlsoft.ru/docs/Software/MinGW/i686-8.1.0-release-posix-dwarf-rt_v6-rev0.7z
set MINGW32_7z=C:\temp-d\i686-8.1.0-release-posix-dwarf-rt_v6-rev0.7z
set URL_QT32=http://wohlsoft.ru/docs/Software/QtBuilds/Qt-5.6.3-i686-mingw-w64-8.1.0-shared-release.7z
set QT32_7z=C:\temp-d\Qt-5.6.3-i686-mingw-w64-8.1.0-shared-release.7z

rem ============ MinGW-w64 64-bit ============
set URL_MINGW64=http://wohlsoft.ru/docs/Software/MinGW/x86_64-8.1.0-release-posix-seh-rt_v6-rev0.7z
set MINGW64_7z=C:\temp-d\x86_64-8.1.0-release-posix-seh-rt_v6-rev0.7z
set URL_QT64=http://wohlsoft.ru/docs/Software/QtBuilds/Qt-5.11.2-x64-mingw-w64-8.1.0-shared-release.7z
set QT64_7z=C:\temp-d\Qt-5.11.2-x64-mingw-w64-8.1.0-shared-release.7z

echo "Downloading NinjaBuild..."
"%WGET_BIN%" --quiet %URL_NINJA% -O "%NINJA_ZIP%"
if ERRORLEVEL 1 goto error

if [%PLATFORM%]==[Win32] (
    echo "Downloading MinGW-w64 (32-bit)..."
    "%WGET_BIN%" --quiet %URL_MINGW32% -O "%MINGW32_7z%"
    if ERRORLEVEL 1 goto error

    echo "Downloading Qt for MinGW-w64 (32-bit)..."
    "%WGET_BIN%" --quiet %URL_QT32% -O "%QT32_7z%"
    if ERRORLEVEL 1 goto error

    echo "Installing MinGW-w64 (32-bit)..."
    "%SEVENZIP%" x "%MINGW32_7z%" -oC:\MinGW-w64
    if ERRORLEVEL 1 goto error

    echo "Installing Qt32 (32-bit)..."
    "%SEVENZIP%" x "%QT32_7z%" -oC:\Qt
    if ERRORLEVEL 1 goto error
)

if [%PLATFORM%]==[x64] (
    echo "Downloading MinGW-w64 (64-bit)..."
    "%WGET_BIN%" --quiet %URL_MINGW64% -O "%MINGW64_7z%"
    if ERRORLEVEL 1 goto error

    echo "Downloading Qt for MinGW-w64 (64-bit)..."
    "%WGET_BIN%" --quiet %URL_QT64% -O "%QT64_7z%"
    if ERRORLEVEL 1 goto error

    echo "Installing MinGW-w64 (64-bit)..."
    "%SEVENZIP%" x "%MINGW64_7z%" -oC:\MinGW-w64
    if ERRORLEVEL 1 goto error

    echo "Installing Qt64 (64-bit)..."
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
