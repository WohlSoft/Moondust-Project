call _paths.bat
call _copy_to_test_dir.bat full

echo.
echo.
echo =======================================================================
echo                          Pack into Laboratory
echo =======================================================================

rem SET packFull=%packFull% "%TESTDIR%\*.dll"
rem SET packFull=%packFull% "%TESTDIR%\platforms"
rem SET packFull=%packFull% "%TESTDIR%\mediaservice"
rem SET packFull=%packFull% "%TESTDIR%\imageformats"
rem SET packFull=%packFull% "%TESTDIR%\fix_music.bat"
SET packFull=
SET packFull=%packFull% "%TESTDIR%\pge_engine.exe"

rem PGE Additional Tools
SET packTools=
SET packTools=%packTools% "%TESTDIR%\GIFs2PNG.exe"
SET packTools=%packTools% "%TESTDIR%\*Readme.txt"
SET packTools=%packTools% "%TESTDIR%\LazyFixTool.exe"
SET packTools=%packTools% "%TESTDIR%\PNG2GIFs.exe"
SET packTools=%packTools% "%TESTDIR%\pge_calibrator.exe"

rem standalone help documents
SET packHelp=
SET packHelp=%packHelp% "%TESTDIR%\help"


SET packConfig=
SET packConfig=%packConfig% "%TESTDIR%\configs\SMBX"

SET packConfigLite=
SET packConfigLite=%packConfigLite% "%TESTDIR%\lab\SMBX"

rem delete undone languages
del "%TESTDIR%\languages\editor_fr.qm"
del "%TESTDIR%\languages\editor_ja.qm"
del "%TESTDIR%\languages\editor_nl.qm"
del "%TESTDIR%\languages\editor_pt.qm"
del "%TESTDIR%\languages\editor_uk.qm"

"%SEVENZIP%\7z" a -tzip %LabDir%\pge_engine_dev.zip %packFull%

echo copy updated archives to Google Drive
copy %LabDir%\pge_engine_dev.zip "%G_DRIVE%\_laboratory\"

rem "%SEVENZIP%\7z" a -tzip %LabDir%\pge_tools_dev.zip %packTools%
rem "%SEVENZIP%\7z" a -tzip %LabDir%\pge_help_standalone.zip %packHelp%
rem "%SEVENZIP%\7z" a -tzip %LabDir%\SMBX_config_pack_full.zip %packConfig%
rem "%SEVENZIP%\7z" a -tzip %LabDir%\SMBX_config_pack_lite.zip %packConfigLite%

IF NOT '%1'=='nopause' pause
