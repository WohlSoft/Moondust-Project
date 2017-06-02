@echo off
set CurDirXXXX=%CD%
call clear_version.bat nopause
call clearEmpties.bat nopause

cd %CurDirXXXX%\..\..\

call clean_make.bat nopause
call build.bat nopause

cd %CurDirXXXX%\deploy\
call deploy_windows_dynamic.bat nopause

cd %CurDirXXXX%
echo ==========Everything has been completed!===========
IF NOT '%1'=='nopause' pause

