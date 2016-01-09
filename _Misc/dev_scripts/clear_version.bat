call _paths.bat

for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (aboutdialog.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (moc_aboutdialog.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (pge_editor_res.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (check_updates.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (moc_check_updates.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"

for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (qrc_engine.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (engine_res.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (main.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"

for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (caltool_res.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (qrc_images.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"

for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (qrc_png2gifs.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (png2gifs.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (qrc_gifs2png.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (gifs2png.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (qrc_lazyfix_tool.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (LazyFixTool.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"

echo Removed all objects which contains version numbers
IF NOT '%1'=='nopause' pause

