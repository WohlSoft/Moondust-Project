@echo off

SET SEVENZIP=C:\Program Files\7-Zip
IF NOT EXIST "%SEVENZIP%\7z.exe" SET SEVENZIP=C:\Program Files (x86)\7-Zip
IF NOT EXIST "%SEVENZIP%\7z.exe" SET SEVENZIP=%ProgramFiles%\7-Zip
