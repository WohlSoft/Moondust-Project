call _paths.bat

for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (aboutdialog.o) do if exist "%%~ff" del /q "%%~ff" && echo "%%~ff deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (pge_editor_res.o) do if exist "%%~ff" del /q "%%~ff" && echo "deleted!"

for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (qrc_engine.o) do if exist "%%~ff" del /q "%%~ff" && echo "deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (engine_res.o) do if exist "%%~ff" del /q "%%~ff" && echo "deleted!"

for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (caltool_res.o) do if exist "%%~ff" del /q "%%~ff" && echo "deleted!"
for /R %SOURCEDIR%\bin-w32\_build_x32 %%f in (qrc_images.o) do if exist "%%~ff" del /q "%%~ff" && echo "deleted!"

echo Removed all objects which contains version numbers
pause