IF NOT '%TARGETDIR%'=='' goto good
echo Target dir not set
pause
goto quit

:good

rem ============== COMMON ====================

copy "%SOURCEDIR%\pge.pro" %TARGETDIR%
copy "%SOURCEDIR%\pge_version.h" %TARGETDIR%

rem del /Q /F %TARGETDIR%\_Libs
rem md %TARGETDIR%\_Libs
rem xcopy /Y /E /I "%SOURCEDIR%\_Libs" %TARGETDIR%\_Libs

echo COPY HELP!!!
del /Q /F /S %TARGETDIR%\Content\help
xcopy /Y /E /I %SOURCEDIR%\Content\help %TARGETDIR%\Content\help

echo COPY Calibrator data!!!
xcopy /Y /E /I %SOURCEDIR%\Content\calibrator %TARGETDIR%\Content\calibrator

del /F /Q %TARGETDIR%\_Misc
md %TARGETDIR%\_Misc
xcopy /Y /E /I "%SOURCEDIR%\_Misc" %TARGETDIR%\_Misc

del /F /Q %TARGETDIR%\_Libs\SDL2_mixer_modified
md %TARGETDIR%\_Libs\SDL2_mixer_modified
xcopy /Y /E /I "%SOURCEDIR%\_Libs\SDL2_mixer_modified" "%TARGETDIR%\_Libs\SDL2_mixer_modified"


rem ============== GIFs2PNG ====================


IF NOT EXIST %TARGETDIR%\GIFs2PNG\*.* md %TARGETDIR%\GIFs2PNG
del /Q /S %TARGETDIR%\GIFs2PNG\*.cpp
del /Q /S %TARGETDIR%\GIFs2PNG\*.ui
del /Q /S %TARGETDIR%\GIFs2PNG\*.pro
del /Q /S %TARGETDIR%\GIFs2PNG\*.txt
del /Q /S %TARGETDIR%\GIFs2PNG\*.h

xcopy %SOURCEDIR%\GIFs2PNG %TARGETDIR%\GIFs2PNG /E /D /C /Y
del %TARGETDIR%\GIFs2PNG\MakeFile
del %TARGETDIR%\GIFs2PNG\MakeFile*


rem ============== PNG2GIFs ====================

IF NOT EXIST %TARGETDIR%\PNG2GIFs\*.* md %TARGETDIR%\PNG2GIFs
del /Q /S %TARGETDIR%\PNG2GIFs\*.cpp
del /Q /S %TARGETDIR%\PNG2GIFs\*.ui
del /Q /S %TARGETDIR%\PNG2GIFs\*.pro
del /Q /S %TARGETDIR%\PNG2GIFs\*.txt
del /Q /S %TARGETDIR%\PNG2GIFs\*.h

xcopy %SOURCEDIR%\PNG2GIFs %TARGETDIR%\PNG2GIFs /E /D /C /Y
del %TARGETDIR%\PNG2GIFs\MakeFile
del %TARGETDIR%\PNG2GIFs\MakeFile*



rem ============== LazyFix Tool ====================

IF NOT EXIST %TARGETDIR%\LazyFixTool\*.* md %TARGETDIR%\LazyFixTool
del /Q /S %TARGETDIR%\LazyFixTool\*.cpp
del /Q /S %TARGETDIR%\LazyFixTool\*.ui
del /Q /S %TARGETDIR%\LazyFixTool\*.pro
del /Q /S %TARGETDIR%\LazyFixTool\*.txt
del /Q /S %TARGETDIR%\LazyFixTool\*.h

xcopy %SOURCEDIR%\LazyFixTool %TARGETDIR%\LazyFixTool /E /D /C /Y
del %TARGETDIR%\LazyFixTool\MakeFile
del %TARGETDIR%\LazyFixTool\MakeFile*


rem ============== PlayableCalibrator ====================

IF NOT EXIST %TARGETDIR%\PlayableCalibrator\*.* md %TARGETDIR%\PlayableCalibrator
del /Q /S %TARGETDIR%\PlayableCalibrator\*.cpp
del /Q /S %TARGETDIR%\PlayableCalibrator\*.ui
del /Q /S %TARGETDIR%\PlayableCalibrator\*.pro
del /Q /S %TARGETDIR%\PlayableCalibrator\*.txt
del /Q /S %TARGETDIR%\PlayableCalibrator\*.h

xcopy %SOURCEDIR%\PlayableCalibrator %TARGETDIR%\PlayableCalibrator /E /D /C /Y
del %TARGETDIR%\PlayableCalibrator\MakeFile
del %TARGETDIR%\PlayableCalibrator\MakeFile*

rem ============== Engine ====================

IF NOT EXIST %TARGETDIR%\Engine\*.* md %TARGETDIR%\Engine
del /Q /S %TARGETDIR%\Engine\*.cpp
del /Q /S %TARGETDIR%\Engine\*.ui
del /Q /S %TARGETDIR%\Engine\*.pro
del /Q /S %TARGETDIR%\Engine\*.txt
del /Q /S %TARGETDIR%\Engine\*.h

xcopy %SOURCEDIR%\Engine %TARGETDIR%\Engine /E /D /C /Y
del %TARGETDIR%\Engine\MakeFile
del %TARGETDIR%\Engine\MakeFile*

rem ============== Editor ====================

IF NOT EXIST %TARGETDIR%\Editor\*.* md %TARGETDIR%\Editor
del /Q /S %TARGETDIR%\Editor\*.cpp
del /Q /S %TARGETDIR%\Editor\*.ui
del /Q /S %TARGETDIR%\Editor\*.pro
del /Q /S %TARGETDIR%\Editor\*.h

xcopy %SOURCEDIR%\Editor %TARGETDIR%\Editor /E /D /C /Y
del %TARGETDIR%\Editor\MakeFile
del %TARGETDIR%\Editor\MakeFile*

rem ======= Editor translations ======= 

del /F /Q %TARGETDIR%\Editor\languages

md %TARGETDIR%\Editor\languages
copy %SOURCEDIR%\Editor\languages\*.ts %TARGETDIR%\Editor\languages
copy %SOURCEDIR%\Editor\languages\qt_*.qm %TARGETDIR%\Editor\languages
copy %SOURCEDIR%\Editor\languages\*.png %TARGETDIR%\Editor\languages

md %TARGETDIR%\Content\configs
md %TARGETDIR%\Content\configs\SMBX
md %TARGETDIR%\Content\configs\SMBX\group_tilesets
md %TARGETDIR%\Content\configs\SMBX\tilesets

rem md %TARGETDIR%\Content\data
rem copy /Y %SOURCEDIR%\Content\data\*.png %TARGETDIR%\Content\data

md %TARGETDIR%\Content\help
copy /Y %SOURCEDIR%\Content\configs\SMBX\*.ini %TARGETDIR%\Content\configs\SMBX\
copy /Y %SOURCEDIR%\Content\configs\SMBX\group_tilesets\*.ini %TARGETDIR%\Content\configs\SMBX\group_tilesets\
copy /Y %SOURCEDIR%\Content\configs\SMBX\tilesets\*.ini %TARGETDIR%\Content\configs\SMBX\tilesets\

rem IF EXIST %TARGETDIR%\Content\data\*.gif del /F /Q %TARGETDIR%\Content\data\*.gif
rem copy /Y %SOURCEDIR%\Content\data\*.gif %TARGETDIR%\Content\data


echo Done!!!
goto quit

:ApplyDir
	call :UpdateFilrsInDirDir %SOURCEDIR%\Editor\%1 %TARGETDIR%\Editor\%1
	GOTO :EOF


:UpdateFilrsInDirDir
	IF NOT EXIST %2\* md %2
	del %2\*.h
	del %2\*.cpp
	del %2\*.ui
	copy /Y %1\*.cpp %2
	copy /Y %1\*.ui %2
	copy /Y %1\*.h %2
	GOTO :EOF


:quit
