@echo off
set InitDir=%CD%
cd ..
call _paths.bat
set CurDir=%CD%

IF NOT EXIST "%SEVENZIP%\7z.exe" GOTO ErrorNo7z

IF "%DEST_TO_64%"=="TRUE" (
	set BINDIR=bin-w64
	set ARCH_CPU=win64
) ELSE (
	set BINDIR=bin-w32
	set ARCH_CPU=win32
)

set COMPRESS_DLL=%CurDir%\upx.exe -9
set COMPRESS_EXE=%CurDir%\upx.exe -9 --compress-icons=0

call "%SOURCEDIR%\_paths.bat"
set OldPATH=%PATH%
PATH=%QtDir%;%MinGW%;%GitDir%;%SystemRoot%\system32;%SystemRoot%;%PATH%

cd "%SOURCEDIR%\Editor"
"%QtDir%\lrelease" pge_editor.pro
copy languages\*.qm %SOURCEDIR%\%BINDIR%\languages
copy languages\*.png %SOURCEDIR%\%BINDIR%\languages

rem Delete junk translation file causes German Ok/Cancel translations in the dialogs
if exist %SOURCEDIR%\%BINDIR%\languages\qt_en.qm del /Q %SOURCEDIR%\%BINDIR%\languages\qt_en.qm
if exist %SOURCEDIR%\%BINDIR%\translations\qt_en.qm del /Q %SOURCEDIR%\%BINDIR%\translations\qt_en.qm

echo "Preparing Windows deploy..."
cd %SOURCEDIR%\%BINDIR%

set DeployDir=%SOURCEDIR%\%BINDIR%\_win32_deploy
set PgePrjSD=PGE_Project
set TarGzArName=pge-project-dev-%ARCH_CPU%.zip
set DeployFlags=--release --no-opengl-sw --no-system-d3d-compiler

if exist "%DeployDir%\*" del /Q /F /S "%DeployDir%\*"
if not exist "%DeployDir%\*" md "%DeployDir%"
if not exist "%DeployDir%\%PgePrjSD%\*" md "%DeployDir%\%PgePrjSD%"

IF NOT "%DynamicQT%"=="TRUE" GOTO noDynamicQt1
copy "%QtDir%\libstdc++-6.dll" ".\libstdc++-6.dll"
copy "%QtDir%\libstdc++-6.dll" "%DeployDir%\%PgePrjSD%"

%QtDir%\windeployqt --force --no-quick-import %DeployFlags% pge_editor.exe
%QtDir%\windeployqt %DeployFlags% pge_engine.exe
%QtDir%\windeployqt %DeployFlags% pge_calibrator.exe
%QtDir%\windeployqt %DeployFlags% GIFs2PNG.exe
%QtDir%\windeployqt %DeployFlags% PNG2GIFs.exe
%QtDir%\windeployqt %DeployFlags% LazyFixTool.exe
%QtDir%\windeployqt %DeployFlags% pge_manager.exe
%QtDir%\windeployqt %DeployFlags% pge_maintainer.exe
set NEED_COPY_MISSING_DLLS=NO
if "%DEST_TO_64%"=="TRUE" set NEED_COPY_MISSING_DLLS=YES
if "%DEST_TO_32w64%"=="TRUE" set NEED_COPY_MISSING_DLLS=YES

IF "%NEED_COPY_MISSING_DLLS%"=="YES" (
	rem Copy missing DLLS are wasn't copied by windeployqt
    if not exist "platforms\*" md "platforms"
    if not exist "imageformats\*" md "imageformats"
    IF EXIST "%QtDir%\libgcc_s_seh-1.dll"  copy "%QtDir%\libgcc_s_seh-1.dll" .
    IF EXIST "%QtDir%\libmingwex-0.dll"    copy "%QtDir%\libmingwex-0.dll" .
    IF EXIST "%QtDir%\libgcc_s_dw2-1.dll"  copy "%QtDir%\libgcc_s_dw2-1.dll" .
    IF EXIST "%QtDir%\libwinpthread-1.dll" copy "%QtDir%\libwinpthread-1.dll" .
    IF EXIST "%QtDir%\pthreadGC-3.dll"     copy "%QtDir%\pthreadGC-3.dll" .

    IF NOT EXIST platforms\qwindows.dll copy "%QtDir%\..\plugins\platforms\qwindows.dll" platforms\
    IF NOT EXIST imageformats\qgif.dll  copy "%QtDir%\..\plugins\imageformats\qgif.dll" imageformats\
    IF NOT EXIST imageformats\qicns.dll copy "%QtDir%\..\plugins\imageformats\qicns.dll" imageformats\
    IF NOT EXIST imageformats\qico.dll  copy "%QtDir%\..\plugins\imageformats\qico.dll" imageformats\
    IF NOT EXIST imageformats\qsvg.dll  copy "%QtDir%\..\plugins\imageformats\qsvg.dll" imageformats\
)

rem Delete junk translation file causes German Ok/Cancel translations in the dialogs
if exist %SOURCEDIR%\%BINDIR%\languages\qt_en.qm del /Q %SOURCEDIR%\%BINDIR%\languages\qt_en.qm
if exist %SOURCEDIR%\%BINDIR%\translations\qt_en.qm del /Q %SOURCEDIR%\%BINDIR%\translations\qt_en.qm

rem Remove possible temporary files of UPX
if exist %SOURCEDIR%\%BINDIR%\*.upx del /Q "%SOURCEDIR%\%BINDIR%\*.upx"

REM rem %COMPRESS_DLL% D3Dcompiler_*.dll
REM %COMPRESS_DLL% libEGL.dll
REM %COMPRESS_DLL% libGLESV2.dll
REM rem %COMPRESS_DLL% opengl32sw.dll
REM %COMPRESS_DLL% Qt5Concurrent.dll
REM %COMPRESS_DLL% Qt5Core.dll
REM %COMPRESS_DLL% Qt5Gui.dll
REM %COMPRESS_DLL% Qt5Qml.dll
REM %COMPRESS_DLL% Qt5Network.dll
REM %COMPRESS_DLL% Qt5Svg.dll
REM %COMPRESS_DLL% Qt5Widgets.dll
REM %COMPRESS_DLL% Qt5WinExtras.dll
REM %COMPRESS_DLL% Qt5Xml.dll
REM :noDynamicQt1
REM
REM %COMPRESS_DLL% SDL2.dll
REM %COMPRESS_DLL% SDL2_mixer_ext.dll
REM
REM %COMPRESS_DLL% libgcc_s_dw2-1.dll
REM %COMPRESS_DLL% libstdc++-6.dll
REM %COMPRESS_DLL% libwinpthread-1.dll
REM
REM %COMPRESS_EXE% pge_editor.exe
REM %COMPRESS_EXE% PNG2GIFs.exe
REM %COMPRESS_EXE% GIFs2PNG.exe
REM %COMPRESS_EXE% LazyFixTool.exe
REM %COMPRESS_EXE% pge_calibrator.exe
REM %COMPRESS_EXE% pge_engine.exe
REM %COMPRESS_EXE% pge_manager.exe
REM %COMPRESS_EXE% pge_maintainer.exe
REM %COMPRESS_EXE% pge_musplay.exe

copy pge_editor.exe "%DeployDir%\%PgePrjSD%"
copy GIFs2PNG.exe "%DeployDir%\%PgePrjSD%"
copy PNG2GIFs.exe "%DeployDir%\%PgePrjSD%"
copy LazyFixTool.exe "%DeployDir%\%PgePrjSD%"
copy pge_calibrator.exe "%DeployDir%\%PgePrjSD%"
copy pge_engine.exe "%DeployDir%\%PgePrjSD%"
copy pge_maintainer.exe "%DeployDir%\%PgePrjSD%"
copy pge_musplay.exe "%DeployDir%\%PgePrjSD%"

copy *.dll "%DeployDir%\%PgePrjSD%"
IF NOT "%DynamicQT%"=="TRUE" GOTO noDynamicQt2
rem xcopy /Y /E /I .\bearer\*.* "%DeployDir%\%PgePrjSD%\bearer"
rem xcopy /Y /E /I .\iconengines\*.* "%DeployDir%\%PgePrjSD%\iconengines"
md "%DeployDir%\%PgePrjSD%\imageformats"
xcopy /Y /E /I .\imageformats\qgif.dll "%DeployDir%\%PgePrjSD%\imageformats"
xcopy /Y /E /I .\imageformats\qico.dll "%DeployDir%\%PgePrjSD%\imageformats"
xcopy /Y /E /I .\platforms\*.* "%DeployDir%\%PgePrjSD%\platforms"
xcopy /Y /E /I .\translations\*.* ".\languages"
:noDynamicQt2
xcopy /Y /E /I .\languages\*.* "%DeployDir%\%PgePrjSD%\languages"
xcopy /Y /E /I .\themes\*.* "%DeployDir%\%PgePrjSD%\themes"
if not exist "%DeployDir%\%PgePrjSD%\help\*.*" md "%DeployDir%\%PgePrjSD%\help"
echo ^<html^>^<head^>^<meta http-equiv="refresh" content="0; url=http://pgehelp.wohlsoft.ru/manual_editor.html"/^>^</head^>^<body^>^</body^>^</html^> > "%DeployDir%\%PgePrjSD%\help\manual_editor.html"

xcopy /Y /E /I .\calibrator "%DeployDir%\%PgePrjSD%\calibrator"
del /Q "%DeployDir%\%PgePrjSD%\calibrator\templates\*.*"
copy "%SOURCEDIR%\Content\readmes\*.txt" "%DeployDir%\%PgePrjSD%"
copy "%SOURCEDIR%\changelog.*.txt" "%DeployDir%\%PgePrjSD%"
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

if not exist "%SOURCEDIR%\%BINDIR%\_packed" md "%SOURCEDIR%\%BINDIR%\_packed"
if exist "%SOURCEDIR%\%BINDIR%\_packed\%TarGzArName%" del "%SOURCEDIR%\%BINDIR%\_packed\%TarGzArName%"

# =========== Packing full archive ==========
echo Packing complete archive...
"%SEVENZIP%\7z" a -tzip -mx9 "%TarGzArName%" "%PgePrjSD%"
move ".\%TarGzArName%" "%SOURCEDIR%\%BINDIR%\_packed\%TarGzArName%"

# ======== Packing thematic archives ========
SET PGEMaintainer=
SET PGEMusPlay=
SET PGECommon=
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\*.dll"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\license.txt"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\GPL*.txt"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\LICENSE.*.txt"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\SDL2_mixer_ext.License.txt"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\help"
IF NOT "%DynamicQT%"=="TRUE" GOTO noDynamicQt3
rem SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\bearer"
rem SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\iconengines"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\imageformats"
SET PGECommon=%PGECommon% "%DeployDir%\%PgePrjSD%\platforms"

IF EXIST "%DeployDir%\%PgePrjSD%\libgcc_s_seh-1.dll" (
	SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\libgcc_s_seh-1.dll"
)
IF EXIST "%DeployDir%\%PgePrjSD%\libmingwex-0.dll" (
	SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\libmingwex-0.dll"
)
IF EXIST "%DeployDir%\%PgePrjSD%\libgcc_s_dw2-1.dll" (
	SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\libgcc_s_dw2-1.dll"
)
SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\libstdc++-6.dll"
IF EXIST "%DeployDir%\%PgePrjSD%\libwinpthread-1.dll" (
	SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\libwinpthread-1.dll"
)
IF EXIST "%DeployDir%\%PgePrjSD%\pthreadGC-3.dll" (
	SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\pthreadGC-3.dll"
)
SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\Qt5Core.dll"
SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\Qt5Gui.dll"
SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\Qt5Network.dll"
SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\Qt5Widgets.dll"
SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\imageformats"
SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\platforms"

IF EXIST "%DeployDir%\%PgePrjSD%\libgcc_s_seh-1.dll" (
	SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\libgcc_s_seh-1.dll"
)
IF EXIST "%DeployDir%\%PgePrjSD%\libmingwex-0.dll" (
	SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\libmingwex-0.dll"
)
IF EXIST "%DeployDir%\%PgePrjSD%\libgcc_s_dw2-1.dll" (
	SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\libgcc_s_dw2-1.dll"
)
IF EXIST "%DeployDir%\%PgePrjSD%\libwinpthread-1.dll" (
	SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\libwinpthread-1.dll"
)
IF EXIST "%DeployDir%\%PgePrjSD%\pthreadGC-3.dll" (
	SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\pthreadGC-3.dll"
)
SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\libstdc++-6.dll"
SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\Qt5Core.dll"
SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\Qt5Gui.dll"
SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\Qt5Network.dll"
SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\Qt5Widgets.dll"
SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\imageformats"
SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\platforms"
:noDynamicQt3

SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\SDL2.dll"
SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\SDL2_mixer_ext.dll"
SET PGEMusPlay=%PGEMusPlay% "%DeployDir%\%PgePrjSD%\pge_musplay.exe"

SET PGEMaintainer=%PGEMaintainer% "%DeployDir%\%PgePrjSD%\pge_maintainer.exe"

SET PGEEditor=
SET PGEEditor=%PGEEditor% "%DeployDir%\%PgePrjSD%\changelog.editor.txt"
SET PGEEditor=%PGEEditor% "%DeployDir%\%PgePrjSD%\pge_editor.exe"
SET PGEEditor=%PGEEditor% "%DeployDir%\%PgePrjSD%\themes"
SET PGEEditor=%PGEEditor% "%DeployDir%\%PgePrjSD%\languages"
SET PGEEditor=%PGEEditor% -r -x!engine_*.qm

SET PGEEngine=
SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\pge_engine.exe"
SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\SDL2.dll"
SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\SDL2_mixer_ext.dll"
SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\Engine.Readme.txt"
SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\changelog.engine.txt"
SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\languages"
IF EXIST "%DeployDir%\%PgePrjSD%\libgcc_s_seh-1.dll"  SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\libgcc_s_seh-1.dll"
IF EXIST "%DeployDir%\%PgePrjSD%\libmingwex-0.dll" 	  SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\libmingwex-0.dll"
IF EXIST "%DeployDir%\%PgePrjSD%\libgcc_s_dw2-1.dll"  SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\libgcc_s_dw2-1.dll"
IF EXIST "%DeployDir%\%PgePrjSD%\libwinpthread-1.dll" SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\libwinpthread-1.dll"
IF EXIST "%DeployDir%\%PgePrjSD%\pthreadGC-3.dll"     SET PGEEngine=%PGEEngine% "%DeployDir%\%PgePrjSD%\pthreadGC-3.dll"
SET PGEEngine=%PGEEngine% -r -x!*.png -x!editor_*.qm -x!qt_*.qm

SET PGETools=
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\GIFs2PNG.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\*Readme.txt"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\LazyFixTool.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\PNG2GIFs.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\pge_calibrator.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\pge_maintainer.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\pge_musplay.exe"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\calibrator"
SET PGETools=%PGETools% "%DeployDir%\%PgePrjSD%\tools"

echo Packing of Online-Install packages data
"%SEVENZIP%\7z" a -tzip -mx9 "install-pge-common-dev-%ARCH_CPU%.zip" %PGECommon%
"%SEVENZIP%\7z" a -tzip -mx9 "install-pge-editor-dev-%ARCH_CPU%.zip" %PGEEditor%
"%SEVENZIP%\7z" a -tzip -mx9 "install-pge-engine-dev-%ARCH_CPU%.zip" %PGEEngine%
"%SEVENZIP%\7z" a -tzip -mx9 "install-pge-tools-dev-%ARCH_CPU%.zip" %PGETools%
"%SEVENZIP%\7z" a -tzip -mx9 "pge-musplay-dev-%ARCH_CPU%.zip" %PGEMusPlay%
"%SEVENZIP%\7z" a -tzip -mx9 "pge-maintainer-dev-%ARCH_CPU%.zip" %PGEMaintainer%
move ".\*.zip" "%SOURCEDIR%\%BINDIR%\_packed"

PATH=%OldPATH%

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
