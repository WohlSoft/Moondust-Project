@echo off
call _paths.bat
cd %SOURCEDIR%\bin-w32\_build_x32
echo %CD%
for /r %%F in (*) do if %%~zF==0 del "%%F" && echo deleted %%F
echo.
echo Empty object files was removed!
echo.
IF NOT '%1'=='nopause' pause