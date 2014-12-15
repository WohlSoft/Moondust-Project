@echo off
call _paths.bat
copy "%SOURCEDIR%\Documents\SMBX64 standard\*.*" "%LabDir%\..\_SMBX64"
copy "%SOURCEDIR%\Documents\SMBX64 standard\File format info\*.*" "%LabDir%\..\_SMBX64\file_formats"
copy "%SOURCEDIR%\Documents\SMBX64 standard\Object info\*.*" "%LabDir%\..\_SMBX64\Object info"
copy "%SOURCEDIR%\Documents\SMBX64 standard\Object lists\*.*" "%LabDir%\..\_SMBX64\Objects"
copy "%SOURCEDIR%\Documents\SMBX64 standard\Images\*.*" "%LabDir%\..\_SMBX64\Images"

copy "%SOURCEDIR%\Documents\PGE-1\File format info\*.*" "%LabDir%\..\_PGE\File format info"

pause