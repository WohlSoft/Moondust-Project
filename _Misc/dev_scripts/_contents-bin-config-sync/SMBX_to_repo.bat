@echo off
echo %CD%
copy .\SMBX\*.* ..\..\Content\configs\SMBX\
xcopy /Y /E /I .\SMBX\characters ..\..\Content\configs\SMBX\characters
xcopy /Y /E /I .\SMBX\fonts ..\..\Content\configs\SMBX\fonts
xcopy /Y /E /I .\SMBX\group_tilesets ..\..\Content\configs\SMBX\group_tilesets
xcopy /Y /E /I .\SMBX\script ..\..\Content\configs\SMBX\script
xcopy /Y /E /I .\SMBX\tilesets ..\..\Content\configs\SMBX\tilesets
del /F /Q ..\..\Content\configs\SMBX\ReadME.txt
del /F /Q ..\..\Content\configs\SMBX\tilesets\_backup
rd ..\..\Content\configs\SMBX\tilesets\_backup
del /F /Q ..\..\Content\configs\SMBX\tilesets\xxx
rd ..\..\Content\configs\SMBX\tilesets\xxx
del /F /Q ..\..\Content\configs\SMBX\group_tilesets\grps
rd ..\..\Content\configs\SMBX\group_tilesets\grps
pause