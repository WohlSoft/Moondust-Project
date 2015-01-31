@echo off
rem This file copying a config packs from testing directory into Linux server and into bin directory of dev-pack
call _paths.bat
call :CopyConfig PGE Default
call :CopyConfig SMBXInt
call :CopyConfig SMBX
call :CopyConfig Raocow (A2MBXT)
call :CopyConfig SMBX_Redrawn

echo Done!
if not -%1-==-nopause- pause

:CopyConfig
	echo %1...
	if -%2==- set config=%1
	if not -%2==- set config=%1 %2

	if exist "%SOURCEDIR%\bin\configs\%config%\*.*" del /Q /F "%SOURCEDIR%\bin\configs\%config%"  > NUL
	xcopy /Y /E /I "%TESTDIR%\configs\%config%\*.*" "%SOURCEDIR%\bin\configs\%config%"  > NUL
	if exist "%LINUXSERV1%\bin\configs\%config%\*.*" del /Q /F "%LINUXSERV1%\bin\configs\%config%"  > NUL
	xcopy /Y /E /I "%TESTDIR%\configs\%config%\*.*" "%LINUXSERV1%\bin\configs\%config%"  > NUL
	GOTO :EOF
