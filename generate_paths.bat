@echo off
IF EXIST _paths.bat goto edit
echo @echo off > _paths.bat
echo rem =============================================================================================== >> _paths.bat
echo rem ============================PLEASE SET THE QT AND MINGW PATHS================================== >> _paths.bat
echo rem =============================================================================================== >> _paths.bat
echo set QtDir=D:\Qt\5.3.1_Static\5.3.2\bin>> _paths.bat
echo set MinGW=D:\DevTools\Qt\5.3.0\Tools\mingw482_32\bin>> _paths.bat
echo rem ===============================================================================================>> _paths.bat
echo rem ===============================================================================================>> _paths.bat
echo rem ===============================================================================================>> _paths.bat
:edit
start notepad _paths.bat

