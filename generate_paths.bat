@echo off
IF EXIST _paths.bat goto edit
echo @echo off > _paths.bat
echo rem =============================================================================================== >> _paths.bat
echo rem ============================PLEASE SET THE QT AND MINGW PATHS================================== >> _paths.bat
echo rem =============================================================================================== >> _paths.bat
echo set QtDir=C:\Qt\5.5\mingw492_32\bin>> _paths.bat
echo set MinGW=C:\Qt\Tools\mingw492_32\bin>> _paths.bat
echo set GitDir=C:\Program Files\Git\cmd>> _paths.bat
echo rem ===============================================================================================>> _paths.bat
echo rem ===============================================================================================>> _paths.bat
echo rem ===============================================================================================>> _paths.bat
:edit
start notepad _paths.bat

