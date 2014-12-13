call _paths.bat
rem call _copy_to_test_dir.bat

echo.
echo.
echo =======================================================================
echo                          Pack into Laboratory
echo =======================================================================

SET GIFs2PNG=
SET GIFs2PNG=%GIFs2PNG% "%TESTDIR%\GIFs2PNG_Readme.txt"
SET GIFs2PNG=%GIFs2PNG% "%TESTDIR%\GIFs2PNG.exe"

SET PNG2GIFs=
SET PNG2GIFs=%PNG2GIFs% "%TESTDIR%\PNG2GIFs_Readme.txt"
SET PNG2GIFs=%PNG2GIFs% "%TESTDIR%\PNG2GIFs.exe"

SET LazyFixTool=
SET LazyFixTool=%PNG2GIFs% "%TESTDIR%\LazyFixTool_Readme.txt"
SET LazyFixTool=%PNG2GIFs% "%TESTDIR%\LazyFixTool.exe"

"%SEVENZIP%\7z" a -tzip %LabDir%\..\_builds\GIFs2PNG\GIFs2PNG_1.0.5_Win32.zip %GIFs2PNG%
"%SEVENZIP%\7z" a -tzip %LabDir%\..\_builds\PNG2GIFs\PNG2GIFs_1.0.0_Win32.zip %PNG2GIFs%
"%SEVENZIP%\7z" a -tzip %LabDir%\..\_builds\LazyFixTool\LazyFixTool_2.0.0_Win32.zip %LazyFixTool%

IF NOT '%1'=='nopause' pause
