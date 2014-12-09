@echo off
:retry
call _paths.bat

"%QTPATH%\bin\lrelease.exe" %SOURCEDIR%\Editor\pge_editor.pro

copy %SOURCEDIR%\Editor\languages\*.qm "%TESTDIR%\languages"
copy %SOURCEDIR%\Editor\languages\*.png "%TESTDIR%\languages"

copy "%SOURCEDIR%\bin\*.exe" "%TESTDIR%"
rem if errorlevel 1 goto wrong

copy "%SOURCEDIR%\Editor\changelog.editor.txt" "%TESTDIR%"

copy %SOURCEDIR%\Content\configs\SMBX\*.txt "%TESTDIR%\configs\SMBX"
copy %SOURCEDIR%\Content\configs\SMBX\*.txt "%TESTDIR%\lab\SMBX"

copy %SOURCEDIR%\Content\configs\SMBX\*.ini "%TESTDIR%\configs\SMBX"
copy %SOURCEDIR%\Content\configs\SMBX\*.ini "%TESTDIR%\lab\SMBX"

copy %SOURCEDIR%\Content\calibrator\spriteconf\*.ini "%TESTDIR%\calibrator\spriteconf"

if '%1'=='full' del /Q /F /S "%TESTDIR%\configs\SMBX\data\graphics"
if '%1'=='full' md "%TESTDIR%\configs\SMBX\data\graphics"
if '%1'=='full' xcopy /Y /E /I %SOURCEDIR%\bin\SMBX_Data\graphics\*.* "%TESTDIR%\configs\SMBX\data\graphics"

if '%1'=='full' del /Q /F /S "%TESTDIR%\lab\SMBX\data\graphics"
if '%1'=='full' md "%TESTDIR%\lab\SMBX\data\graphics"
if '%1'=='full' xcopy /Y /E /I %SOURCEDIR%\bin\SMBX_Data\data\graphics\*.* "%TESTDIR%\lab\SMBX\data\graphics"

rem del /Q /F "%TESTDIR%\data\*.*"
rem copy %SOURCEDIR%\Content\data\*.png "%TESTDIR%\data"

if '%1'=='full' del /Q /F /S "%TESTDIR%\help"
if '%1'=='full' md "%TESTDIR%\help"
if '%1'=='full' xcopy /Y /E /I %SOURCEDIR%\Content\help\*.* "%TESTDIR%\help"

cd /d "%TESTDIR%"
rem start .\pge_editor.exe
goto quit
:wrong
rem Here is cyrilic codepage #866
echo ੯, ⮠, 㡥 !
rem taskkill /F /IM pge_editor.exe /T
goto retry
:quit