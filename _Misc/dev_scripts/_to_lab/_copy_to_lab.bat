@echo off
set InitDir=%CD%
cd ..
call _paths.bat

copy "%SOURCEDIR%\bin-w32\_packed\*.zip" "%LabDir%"

for /F "usebackq tokens=1,2 delims==" %%i in (`wmic os get LocalDateTime /VALUE 2^>NUL`) do if '.%%i.'=='.LocalDateTime.' set ldt=%%j
set ldt=%ldt:~0,4%-%ldt:~4,2%-%ldt:~6,2% %ldt:~8,2%:%ldt:~10,2%:%ldt:~12,2%
echo %ldt% > "%LabDir%\_versions\editor_dev.txt"

cd %InitDir%

pause