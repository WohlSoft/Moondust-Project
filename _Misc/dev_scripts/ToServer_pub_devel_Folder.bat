call _paths.bat
set TARGETDIR=%PubDir%

CALL ./_copydata.bat

del %TARGETDIR%\Editor\ui_*.h
del %TARGETDIR%\Editor\moc_*.cpp

IF NOT '%1'=='nopause' pause
