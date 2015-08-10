@echo off
echo %CD%
xcopy /Y /E /I ..\..\Content\configs\SMBX\*.* .\SMBX
pause
