@echo off
call _paths.bat
cd %SOURCEDIR%\bin\_build
echo %CD%
for /r %%F in (*) do if %%~zF==0 del "%%F" && echo deleted %%F
echo.
echo Empty object files was removed!
echo.
pause