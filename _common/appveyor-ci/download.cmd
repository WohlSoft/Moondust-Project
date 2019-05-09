@echo off

if NOT EXIST C:\temp-d\NUL md C:\temp-d

rem ============================================================
rem           !!!ALL URLs MOVED INTO appveyor.yml!!!
rem ============================================================

echo "Downloading NinjaBuild..."
"%WGET_BIN%" --quiet %URL_NINJA% -O "%NINJA_ZIP%"
if ERRORLEVEL 1 goto error

echo "Downloading MinGW-w64 (%WINXX_ARCH%)..."
"%WGET_BIN%" --quiet %URL_MINGWw64% -O "%MINGWw64_7z%"
if ERRORLEVEL 1 goto error

echo "Downloading Qt for MinGW-w64 (%WINXX_ARCH%)..."
"%WGET_BIN%" --quiet %URL_QT% -O "%QT_7z%"
if ERRORLEVEL 1 goto error

echo "Installing MinGW-w64 (%WINXX_ARCH%)..."
"%SEVENZIP%" x "%MINGWw64_7z%" -oC:\MinGW-w64
if ERRORLEVEL 1 goto error

echo "Installing Qt (%WINXX_ARCH%)..."
"%SEVENZIP%" x "%QT_7z%" -oC:\Qt
if ERRORLEVEL 1 goto error

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
