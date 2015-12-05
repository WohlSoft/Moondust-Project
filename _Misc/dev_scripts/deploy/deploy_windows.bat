@echo off
set InitDir=%CD%
cd ..
call _paths.bat
set CurDir=%CD%

IF NOT EXIST "%SEVENZIP%\7z.exe" GOTO ErrorNo7z

cd "%SOURCEDIR%\Editor"
call "%SOURCEDIR%\_paths.bat"

"%QtDir%\lrelease" pge_editor.pro
copy languages\*.qm %SOURCEDIR%\bin-w32\languages
copy languages\*.png %SOURCEDIR%\bin-w32\languages

echo "Preparing Windows deploy..."
cd %SOURCEDIR%\bin-w32

set DeployDir=%SOURCEDIR%\bin-w32\_win32_deploy
set PgePrjSD=PGE_Project
set TarGzArName=pge-project-dev-win32.zip

if exist "%DeployDir%\*" del /Q /F /S "%DeployDir%\*"
if not exist "%DeployDir%\*" md "%DeployDir%"
if not exist "%DeployDir%\%PgePrjSD%\*" md "%DeployDir%\%PgePrjSD%"

IF NOT "%DynamicQT%"=="TRUE" GOTO noDynamicQt1
copy "%QtDir%\libstdc++-6.dll" ".\libstdc++-6.dll"
copy "%QtDir%\libstdc++-6.dll" "%DeployDir%\%PgePrjSD%"
%QtDir%\windeployqt pge_editor.exe
%QtDir%\windeployqt pge_engine.exe
%QtDir%\windeployqt pge_calibrator.exe
%QtDir%\windeployqt GIFs2PNG.exe
%QtDir%\windeployqt PNG2GIFs.exe
%QtDir%\windeployqt LazyFixTool.exe
%QtDir%\windeployqt pge_manager.exe
%QtDir%\windeployqt pge_maintainer.exe
%CurDir%\upx.exe Qt5Core.dll
%CurDir%\upx.exe Qt5Gui.dll
%CurDir%\upx.exe D3Dcompiler_43.dll
%CurDir%\upx.exe libGLESV2.dll
%CurDir%\upx.exe Qt5Network.dll
%CurDir%\upx.exe Qt5Widgets.dll
%CurDir%\upx.exe libstdc++-6.dll
:noDynamicQt1

%CurDir%\upx.exe --compress-icons=0 pge_editor.exe
%CurDir%\upx.exe --compress-icons=0 PNG2GIFs.exe
%CurDir%\upx.exe --compress-icons=0 GIFs2PNG.exe
%CurDir%\upx.exe --compress-icons=0 LazyFixTool.exe
%CurDir%\upx.exe --compress-icons=0 pge_calibrator.exe
%CurDir%\upx.exe --compress-icons=0 pge_engine.exe
%CurDir%\upx.exe --compress-icons=0 pge_manager.exe
%CurDir%\upx.exe --compress-icons=0 pge_maintainer.exe
%CurDir%\upx.exe --compress-icons=0 pge_musplay.exe

copy pge_editor.exe "%DeployDir%\%PgePrjSD%"
copy GIFs2PNG.exe "%DeployDir%\%PgePrjSD%"
copy PNG2GIFs.exe "%DeployDir%\%PgePrjSD%"
copy LazyFixTool.exe "%DeployDir%\%PgePrjSD%"
copy pge_calibrator.exe "%DeployDir%\%PgePrjSD%"
copy pge_engine.exe "%DeployDir%\%PgePrjSD%"
copy pge_musplay.exe "%DeployDir%\%PgePrjSD%"

copy *.dll "%DeployDir%\%PgePrjSD%"
IF NOT "%DynamicQT%"=="TRUE" GOTO noDynamicQt2
xcopy /Y /E /I .\bearer\*.* "%DeployDir%\%PgePrjSD%\bearer"
xcopy /Y /E /I .\iconengines\*.* "%DeployDir%\%PgePrjSD%\iconengines"
xcopy /Y /E /I .\imageformats\*.* "%DeployDir%\%PgePrjSD%\imageformats"
xcopy /Y /E /I .\platforms\*.* "%DeployDir%\%PgePrjSD%\platforms"
xcopy /Y /E /I .\translations\*.* ".\languages"
:noDynamicQt2
xcopy /Y /E /I .\languages\*.* "%DeployDir%\%PgePrjSD%\languages"
xcopy /Y /E /I .\themes\*.* "%DeployDir%\%PgePrjSD%\themes"
if not exist "%DeployDir%\%PgePrjSD%\help\*.*" md "%DeployDir%\%PgePrjSD%\help"
echo ^<html^>^<head^>^<meta http-equiv="refresh" content="0; url=http://help.engine.wohlnet.ru/manual_editor.html"/^>^</head^>^<body^>^</body^>^</html^> > "%DeployDir%\%PgePrjSD%\help\manual_editor.htm"

xcopy /Y /E /I .\calibrator "%DeployDir%\%PgePrjSD%\calibrator"
del /Q "%DeployDir%\%PgePrjSD%\calibrator\templates\*.*"
copy "%SOURCEDIR%\Content\readmes\*.txt" "%DeployDir%\%PgePrjSD%"
copy "%SOURCEDIR%\Editor\changelog.editor.txt" "%DeployDir%\%PgePrjSD%"
copy "%SOURCEDIR%\Engine\changelog.engine.txt" "%DeployDir%\%PgePrjSD%"
copy "%SOURCEDIR%\LICENSE" "%DeployDir%\%PgePrjSD%\license.txt"
del "%DeployDir%\%PgePrjSD%\themes\*.zip"
del "%DeployDir%\%PgePrjSD%\themes\pge_default\*.zip"
"%SEVENZIP%\7z" x "%CurDir%\..\tools\sox-win32.zip" -o"%DeployDir%\%PgePrjSD%\tools" * -r

rem Creating empty directories for a user stuff
md "%DeployDir%\%PgePrjSD%\configs"
md "%DeployDir%\%PgePrjSD%\worlds"
md "%DeployDir%\%PgePrjSD%\screenshots"
md "%DeployDir%\%PgePrjSD%\logs"

echo Creating INI-files with portable config
echo [Main]> "%DeployDir%\%PgePrjSD%\pge_editor.ini"
echo force-portable=true>> "%DeployDir%\%PgePrjSD%\pge_editor.ini"
echo [Main]> "%DeployDir%\%PgePrjSD%\pge_engine.ini"
echo force-portable=true>> "%DeployDir%\%PgePrjSD%\pge_engine.ini"
echo [Main]> "%DeployDir%\%PgePrjSD%\pge_calibrator.ini"
echo force-portable=true>> "%DeployDir%\%PgePrjSD%\pge_calibrator.ini"

cd "%DeployDir%"

if exist ".\%TarGzArName%" del ".\%TarGzArName%"

if not exist "%SOURCEDIR%\bin-w32\_packed" md "%SOURCEDIR%\bin-w32\_packed"
if exist "%SOURCEDIR%\bin-w32\_packed\%TarGzArName%" del "%SOURCEDIR%\bin-w32\_packed\%TarGzArName%"

echo Packing complete archive...
"%SEVENZIP%\7z" a -tzip "%TarGzArName%" "%PgePrjSD%"
move ".\%TarGzArName%" "%SOURCEDIR%\bin-w32\_packed\%TarGzArName%"

SET PGECommon=
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\*.dll"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\license.txt"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\LICENSE.*.txt"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\languages"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\help"
IF NOT "%DynamicQT%"=="TRUE" GOTO noDynamicQt3
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\bearer"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\iconengines"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\imageformats"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\platforms"
:noDynamicQt3

SET PGEEditor=
SET PGEEditor=%PGEEditor% "%DeployDir%\%PgePrjSD%\changelog.editor.txt"
SET PGEEditor=%PGEEditor% "%DeployDir%\%PgePrjSD%\pge_editor.exe"
SET PGEEditor=%PGEEditor% "%DeployDir%\%PgePrjSD%\themes"
SET PGEEditor=%PGEEditor% "%DeployDir%\%PgePrjSD%\tools"

SET PGEEngine=
SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\pge_engine.exe"
SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\Engine.Readme.txt"

SET PGETools=
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\GIFs2PNG.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\*Readme.txt"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\LazyFixTool.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\PNG2GIFs.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\pge_calibrator.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\pge_musplay.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\calibrator"

echo Packing of editor data
"%SEVENZIP%\7z" a -tzip "install-pge-common-dev-win32.zip" %PGECommon%
"%SEVENZIP%\7z" a -tzip "install-pge-editor-dev-win32.zip" %PGEEditor%
"%SEVENZIP%\7z" a -tzip "install-pge-engine-dev-win32.zip" %PGEEngine%
"%SEVENZIP%\7z" a -tzip "install-pge-tools-dev-win32.zip" %PGETools%
move ".\*.zip" "%SOURCEDIR%\bin-w32\_packed"

echo.
echo "All done!"
echo.
cd %InitDir%
goto exitFrom
:ErrorNo7z
echo.
echo ERROR: 7zip not found: %SEVENZIP%
echo.
:exitFrom
if not "%1"=="nopause" pause

