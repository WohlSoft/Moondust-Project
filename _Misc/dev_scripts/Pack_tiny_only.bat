call _paths.bat
"%QTPATH%\bin\lrelease.exe" *.pro
copy "release\pge_editor.exe" "%TESTDIR%"
copy "..\GIFs2PNG\release\*.exe" "%TESTDIR%"
copy "..\GIFs2PNG\*.txt" "%TESTDIR%"
copy "..\LazyFixTool\release\*.exe" "%TESTDIR%"
copy "..\LazyFixTool\*.txt" "%TESTDIR%"
copy ..\Content\configs\SMBX\*.ini "%TESTDIR%\configs\SMBX"
copy ..\Content\data\*.gif "%TESTDIR%\data"
copy ..\Content\data\*.png "%TESTDIR%\data"
copy languages\*.qm "%TESTDIR%\languages"

del /Q /F "%TESTDIR%\help"
md "%TESTDIR%\help"
xcopy /Y /E /I ..\Content\help\*.* "%TESTDIR%\help"

echo.
echo.
echo =======================================================================
echo                          Pack into Laboratory
echo =======================================================================

SET packData="%TESTDIR%\*.dll"
SET packData=%packData% "%TESTDIR%\platforms"
SET packData=%packData% "%TESTDIR%\mediaservice"
SET packData=%packData% "%TESTDIR%\imageformats"
SET packData=%packData% "%TESTDIR%\pge_editor.exe"
SET packData=%packData% "%TESTDIR%\LazyFixTool.exe"
SET packData=%packData% "%TESTDIR%\GIFs2PNG.exe"
SET packData=%packData% "%TESTDIR%\languages"
SET packData=%packData% "%TESTDIR%\configs"
SET packData=%packData% "%TESTDIR%\help"
SET packData=%packData% "%TESTDIR%\lab\data"

SET packData3="%TESTDIR%\*.dll"
SET packData3=%packData3% "%TESTDIR%\platforms"
SET packData3=%packData3% "%TESTDIR%\mediaservice"
SET packData3=%packData3% "%TESTDIR%\imageformats"
SET packData3=%packData3% "%TESTDIR%\pge_editor.exe"
SET packData3=%packData3% "%TESTDIR%\LazyFixTool.exe"
SET packData3=%packData3% "%TESTDIR%\GIFs2PNG.exe"
SET packData3=%packData3% "%TESTDIR%\languages"
SET packData3=%packData3% "%TESTDIR%\configs"
SET packData3=%packData3% "%TESTDIR%\help"
SET packData3=%packData3% "%TESTDIR%\data"

SET packData2=%packData2% "%TESTDIR%\pge_editor.exe"
SET packData2=%packData2% "%TESTDIR%\configs"
SET packData2=%packData2% "%TESTDIR%\languages"
SET packData2=%packData2% "%TESTDIR%\help"
SET packData2=%packData2% "%TESTDIR%\GIFs2PNG.exe"
SET packData2=%packData2% "%TESTDIR%\LazyFixTool.exe"

rem delete undone languages
del "%TESTDIR%\languages\editor_fr.qm"
del "%TESTDIR%\languages\editor_it.qm"
del "%TESTDIR%\languages\editor_ja.qm"
del "%TESTDIR%\languages\editor_nl.qm"
del "%TESTDIR%\languages\editor_pt.qm"
del "%TESTDIR%\languages\editor_uk.qm"

"%SEVENZIP%\7z" a -tzip %LabDir%\pge_editor_full_with_music.zip %packData2%
"%SEVENZIP%\7z" a -tzip %LabDir%\pge_editor_lite.zip %packData2%
"%SEVENZIP%\7z" a -tzip %LabDir%\pge_editor_tiny.zip %packData2%

IF NOT '%1'=='nopause' pause
