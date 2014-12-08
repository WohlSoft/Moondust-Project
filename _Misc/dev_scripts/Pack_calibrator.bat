call _paths.bat
rem call _copy_to_test_dir.bat

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
rem SET packFull=%packFull% "%TESTDIR%\calibrator"
SET packFull=%packFull% "%TESTDIR%\pge_calibrator.exe"
SET packFull=%packFull% "%TESTDIR%\calibrator"

del /F /Q /S "%TESTDIR%\calibrator\templates\*.*"
"%SEVENZIP%\7z" a -tzip V:\engine.wohlnet.ru\docs\docs\_builds\PGE_Calibrator\pge_calibrator.zip %packFull%

IF NOT '%1'=='nopause' pause
