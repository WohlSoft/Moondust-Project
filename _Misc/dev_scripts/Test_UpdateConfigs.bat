call _paths.bat
rem copy "release\pge_editor.exe" "%TESTDIR%"
copy ..\Content\configs\SMBX\*.ini "%TESTDIR%\configs\SMBX"
copy ..\Content\data\*.png "%TESTDIR%\data"
rem cd /d "%TESTDIR%"
rem start .\pge_editor.exe