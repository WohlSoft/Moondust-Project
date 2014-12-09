call _paths.bat
call _copy_to_test_dir.bat

del /F /Q /S "%TESTDIR%\lab\SMBX\tilesets\"
del /F /Q /S "%TESTDIR%\lab\SMBX\group_tilesets\"

xcopy /Y /E /I "%TESTDIR%\configs\SMBX\tilesets\*.*" "%TESTDIR%\lab\SMBX\tilesets"
xcopy /Y /E /I "%TESTDIR%\configs\SMBX\group_tilesets\*.*" "%TESTDIR%\lab\SMBX\group_tilesets"

echo.
echo.
echo =======================================================================
echo                          Pack into Laboratory
echo =======================================================================

SET EditorDLLs=
SET EditorDLLs=%EditorDLLs% "%TESTDIR%\*.dll"
rem SET EditorDLLs=%EditorDLLs% "%TESTDIR%\platforms"
rem SET EditorDLLs=%EditorDLLs% "%TESTDIR%\mediaservice"
rem SET EditorDLLs=%EditorDLLs% "%TESTDIR%\imageformats"

SET EditorData=
SET EditorData=%EditorData% "%TESTDIR%\data"

SET Editor=
SET Editor=%Editor% "%TESTDIR%\license.txt"
SET Editor=%Editor% "%TESTDIR%\LICENSE.*.txt"
SET Editor=%Editor% "%TESTDIR%\*Readme.txt"
SET Editor=%Editor% "%TESTDIR%\changelog.*.txt"
SET Editor=%Editor% "%TESTDIR%\pge_editor.exe"
rem SET Editor=%Editor% "%TESTDIR%\fix_music.bat"
SET Editor=%Editor% "%TESTDIR%\LazyFixTool.exe"
SET Editor=%Editor% "%TESTDIR%\GIFs2PNG.exe"
SET Editor=%Editor% "%TESTDIR%\PNG2GIFs.exe"
SET Editor=%Editor% "%TESTDIR%\pge_calibrator.exe"
SET Editor=%Editor% "%TESTDIR%\languages"
SET Editor=%Editor% "%TESTDIR%\fonts"
SET Editor=%Editor% "%TESTDIR%\icons"

SET SMBXBaseConfig=
SET SMBXBaseConfig=%SMBXBaseConfig% "%TESTDIR%\lab\SMBX"

SET HelpDocuments=
SET HelpDocuments=%HelpDocuments% "%TESTDIR%\help\*.*"
SET HelpDocuments=%HelpDocuments% "%TESTDIR%\help\customnpcconf"
SET HelpDocuments=%HelpDocuments% "%TESTDIR%\help\customizing"
SET HelpDocuments=%HelpDocuments% "%TESTDIR%\help\editorgui"
SET HelpDocuments=%HelpDocuments% "%TESTDIR%\help\images"
SET HelpDocuments=%HelpDocuments% "%TESTDIR%\help\levelediting"
SET HelpDocuments=%HelpDocuments% "%TESTDIR%\help\screenshots"
SET HelpDocuments=%HelpDocuments% "%TESTDIR%\help\tools"
SET HelpDocuments=%HelpDocuments% "%TESTDIR%\help\worldmapediting"

SET HQMusic="D:\Developer\pge_contant\hq_music\*.*"
SET SynthSound="D:\Developer\pge_contant\synth_sound_pack\*.*"

SET SMBXMusic="D:\Developer\pge_contant\smbx_music\*.*"
SET SMBXSound="D:\Developer\pge_contant\smbx_sound\*.*"

rem delete undone languages
del "%TESTDIR%\languages\editor_fr.qm"
del "%TESTDIR%\languages\editor_ja.qm"
del "%TESTDIR%\languages\editor_nl.qm"
del "%TESTDIR%\languages\editor_pt.qm"
del "%TESTDIR%\languages\editor_uk.qm"

"%SEVENZIP%\7z" a -t7z %LabDir%\install\editor.7z %Editor%
"%SEVENZIP%\7z" a -t7z %LabDir%\install\editor.7z %EditorData%
"%SEVENZIP%\7z" a -t7z %LabDir%\install\smbx_baseconfig.7z %SMBXBaseConfig%
"%SEVENZIP%\7z" a -t7z %LabDir%\install\editor.7z %EditorDLLs%
"%SEVENZIP%\7z" a -t7z %LabDir%\install\help.7z %HelpDocuments%
rem "%SEVENZIP%\7z" a -t7z %LabDir%\install\HQMusic.7z %HQMusic%
rem "%SEVENZIP%\7z" a -t7z %LabDir%\install\SynthSound.7z %SynthSound%
rem "%SEVENZIP%\7z" a -t7z %LabDir%\install\SMBXMusic.7z %SMBXMusic%
rem "%SEVENZIP%\7z" a -t7z %LabDir%\install\SMBXSound.7z %SMBXSound%

IF NOT '%1'=='nopause' pause
start explorer %LabDir%\install\
start notepad %LabDir%\install\files_sizes.txt
