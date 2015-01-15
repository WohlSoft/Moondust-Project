call _paths.bat

upx.exe "%SOURCEDIR%\bin\LazyFixTool.exe"
upx.exe "%SOURCEDIR%\bin\PNG2GIFs.exe"
upx.exe "%SOURCEDIR%\bin\GIFs2PNG.exe"
upx.exe "%SOURCEDIR%\bin\pge_calibrator.exe"
upx.exe "%SOURCEDIR%\bin\pge_editor.exe"
upx.exe "%SOURCEDIR%\bin\pge_engine.exe"
pause
