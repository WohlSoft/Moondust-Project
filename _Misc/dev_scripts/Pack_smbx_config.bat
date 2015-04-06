call _paths.bat
call _copy_to_test_dir.bat full

echo.
echo.
echo =======================================================================
echo                          Pack into Laboratory
echo =======================================================================

SET packConfig=
SET packConfig=%packConfig% "%TESTDIR%\configs\SMBX"

rem Updating of basic config pack
"%SEVENZIP%\7z" a -tzip %LabDir%\SMBX_config_pack_full.zip %packConfig%

echo done!

IF NOT '%1'=='nopause' pause
