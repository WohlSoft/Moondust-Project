call _paths.bat

upx.exe --compress-icons=0 "%SOURCEDIR%\bin\LazyFixTool.exe"
upx.exe --compress-icons=0 "%SOURCEDIR%\bin\PNG2GIFs.exe"
upx.exe --compress-icons=0 "%SOURCEDIR%\bin\GIFs2PNG.exe"
upx.exe --compress-icons=0 "%SOURCEDIR%\bin\pge_calibrator.exe"
upx.exe --compress-icons=0 "%SOURCEDIR%\bin\pge_editor.exe"
upx.exe --compress-icons=0 "%SOURCEDIR%\bin\pge_manager.exe"
upx.exe --compress-icons=0 "%SOURCEDIR%\bin\pge_engine.exe"
pause
