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


rem ============== GIFs2PNG ====================


IF NOT EXIST %TARGETDIR%\GIFs2PNG\*.* md %TARGETDIR%\GIFs2PNG
del %TARGETDIR%\GIFs2PNG\*.cpp
del %TARGETDIR%\GIFs2PNG\*.h
del %TARGETDIR%\GIFs2PNG\*.pro
del %TARGETDIR%\GIFs2PNG\*.txt
copy "%SOURCEDIR%\GIFs2PNG\*.cpp" %TARGETDIR%\GIFs2PNG
copy "%SOURCEDIR%\GIFs2PNG\*.h" %TARGETDIR%\GIFs2PNG
copy "%SOURCEDIR%\GIFs2PNG\*.pro" %TARGETDIR%\GIFs2PNG
copy "%SOURCEDIR%\GIFs2PNG\*.txt" %TARGETDIR%\GIFs2PNG
del /F /Q %TARGETDIR%\GIFs2PNG\_resources
xcopy /Y /E /I "%SOURCEDIR%\GIFs2PNG\_resources" %TARGETDIR%\GIFs2PNG\_resources


rem ============== PNG2GIFs ====================

IF NOT EXIST %TARGETDIR%\PNG2GIFs\*.* md %TARGETDIR%\PNG2GIFs
del %TARGETDIR%\PNG2GIFs\*.cpp
del %TARGETDIR%\PNG2GIFs\*.pro
del %TARGETDIR%\PNG2GIFs\*.h
del %TARGETDIR%\PNG2GIFs\*.txt
copy "%SOURCEDIR%\PNG2GIFs\*.cpp" %TARGETDIR%\PNG2GIFs
copy "%SOURCEDIR%\PNG2GIFs\*.h" %TARGETDIR%\PNG2GIFs
copy "%SOURCEDIR%\PNG2GIFs\*.pro" %TARGETDIR%\PNG2GIFs
copy "%SOURCEDIR%\PNG2GIFs\*.txt" %TARGETDIR%\PNG2GIFs
del /F /Q %TARGETDIR%\PNG2GIFs\libs
del /F /Q %TARGETDIR%\PNG2GIFs\_resources
xcopy /Y /E /I "%SOURCEDIR%\PNG2GIFs\_resources" %TARGETDIR%\PNG2GIFs\_resources



rem ============== LazyFix Tool ====================

IF NOT EXIST %TARGETDIR%\LazyFixTool\*.* md %TARGETDIR%\LazyFixTool
del %TARGETDIR%\LazyFixTool\*.cpp
del %TARGETDIR%\LazyFixTool\*.pro
del %TARGETDIR%\LazyFixTool\*.h
del %TARGETDIR%\LazyFixTool\*.txt
copy "%SOURCEDIR%\LazyFixTool\*.cpp" %TARGETDIR%\LazyFixTool
copy "%SOURCEDIR%\LazyFixTool\*.h" %TARGETDIR%\LazyFixTool
copy "%SOURCEDIR%\LazyFixTool\*.pro" %TARGETDIR%\LazyFixTool
copy "%SOURCEDIR%\LazyFixTool\*.txt" %TARGETDIR%\LazyFixTool

del /F /Q %TARGETDIR%\LazyFixTool\libs
xcopy /Y /E /I "%SOURCEDIR%\LazyFixTool\libs" %TARGETDIR%\LazyFixTool\libs
del /F /Q %TARGETDIR%\LazyFixTool\_resources
xcopy /Y /E /I "%SOURCEDIR%\LazyFixTool\_resources" %TARGETDIR%\LazyFixTool\_resources


rem ============== PlayableCalibrator ====================

IF NOT EXIST %TARGETDIR%\PlayableCalibrator\*.* md %TARGETDIR%\PlayableCalibrator
del %TARGETDIR%\PlayableCalibrator\*.cpp
del %TARGETDIR%\PlayableCalibrator\*.pro
del %TARGETDIR%\PlayableCalibrator\*.txt
copy "%SOURCEDIR%\PlayableCalibrator\*.cpp" %TARGETDIR%\PlayableCalibrator
copy "%SOURCEDIR%\PlayableCalibrator\*.h" %TARGETDIR%\PlayableCalibrator
copy "%SOURCEDIR%\PlayableCalibrator\*.ui" %TARGETDIR%\PlayableCalibrator
copy "%SOURCEDIR%\PlayableCalibrator\*.pro" %TARGETDIR%\PlayableCalibrator
copy "%SOURCEDIR%\PlayableCalibrator\*.txt" %TARGETDIR%\PlayableCalibrator

call :UpdateFilrsInDirDir "%SOURCEDIR%\PlayableCalibrator\about" %TARGETDIR%\PlayableCalibrator\about
call :UpdateFilrsInDirDir "%SOURCEDIR%\PlayableCalibrator\animator" %TARGETDIR%\PlayableCalibrator\animator
call :UpdateFilrsInDirDir "%SOURCEDIR%\PlayableCalibrator\frame_matrix" %TARGETDIR%\PlayableCalibrator\frame_matrix
call :UpdateFilrsInDirDir "%SOURCEDIR%\PlayableCalibrator\image_calibration" %TARGETDIR%\PlayableCalibrator\image_calibration
call :UpdateFilrsInDirDir "%SOURCEDIR%\PlayableCalibrator\main" %TARGETDIR%\PlayableCalibrator\main
del /F /Q %TARGETDIR%\PlayableCalibrator\_resourses
xcopy /Y /E /I "%SOURCEDIR%\PlayableCalibrator\_resourses" %TARGETDIR%\PlayableCalibrator\_resourses
rem del /F /Q %TARGETDIR%\PlayableCalibrator\libs
rem rem xcopy /Y /E /I "%SOURCEDIR%\PlayableCalibrator\libs" %TARGETDIR%\PlayableCalibrator\libs






rem ============== Engine ====================

IF NOT EXIST %TARGETDIR%\Engine\*.* md %TARGETDIR%\Engine
del %TARGETDIR%\Engine\*.cpp
del %TARGETDIR%\Engine\*.h
del %TARGETDIR%\Engine\*.pro
del %TARGETDIR%\Engine\*.txt
copy "%SOURCEDIR%\Engine\*.cpp" %TARGETDIR%\Engine
copy "%SOURCEDIR%\Engine\*.h" %TARGETDIR%\Engine
copy "%SOURCEDIR%\Engine\*.pro" %TARGETDIR%\Engine
copy "%SOURCEDIR%\Engine\*.txt" %TARGETDIR%\Engine
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\data_configs" %TARGETDIR%\Engine\data_configs
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\graphics" %TARGETDIR%\Engine\graphics
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\physics" %TARGETDIR%\Engine\physics
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\gui" %TARGETDIR%\Engine\gui
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\fontman" %TARGETDIR%\Engine\fontman
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\physics\engine" %TARGETDIR%\Engine\physics\engine
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\scenes" %TARGETDIR%\Engine\scenes
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\scenes\level" %TARGETDIR%\Engine\scenes\level
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\controls" %TARGETDIR%\Engine\controls
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\common_features" %TARGETDIR%\Engine\common_features
call :UpdateFilrsInDirDir "%SOURCEDIR%\Engine\networking" %TARGETDIR%\Engine\networking
del /F /Q %TARGETDIR%\Engine\_resources
xcopy /Y /E /I "%SOURCEDIR%\Engine\_resources" %TARGETDIR%\Engine\_resources
rem call :UpdateFilrsInDirDir "..\Engine\physics" %TARGETDIR%\Engine\physics




rem ============== Editor ====================


del %TARGETDIR%\Editor\*.cpp
del %TARGETDIR%\Editor\*.ui
del %TARGETDIR%\Editor\*.pro
del %TARGETDIR%\Editor\*.h

copy %SOURCEDIR%\Editor\*.cpp %TARGETDIR%\Editor
copy %SOURCEDIR%\Editor\*.h %TARGETDIR%\Editor
copy %SOURCEDIR%\Editor\*.qrc %TARGETDIR%\Editor
copy %SOURCEDIR%\Editor\*.ui %TARGETDIR%\Editor
copy %SOURCEDIR%\Editor\*.pro %TARGETDIR%\Editor

copy %SOURCEDIR%\Editor\changelog.editor.txt %TARGETDIR%\Editor
copy %SOURCEDIR%\Editor\version.txt %TARGETDIR%\Editor

rem del %TARGETDIR%\Editor\ui_*.h

call :ApplyDir audio
call :ApplyDir common_features
call :ApplyDir common_features\resizer
call :ApplyDir data_configs
call :ApplyDir dev_console
call :ApplyDir editing
call :ApplyDir editing\_dialogs
call :ApplyDir editing\_scenes
call :ApplyDir editing\_scenes\level
call :ApplyDir editing\_scenes\level\edit_modes
call :ApplyDir editing\_scenes\world
call :ApplyDir editing\_scenes\world\edit_modes
call :ApplyDir editing\edit_level
call :ApplyDir editing\edit_npc
call :ApplyDir editing\edit_world
call :ApplyDir file_formats
call :ApplyDir main_window
call :ApplyDir main_window\about_dialog
call :ApplyDir main_window\dock
call :ApplyDir main_window\tools
call :ApplyDir networking
call :ApplyDir script
call :ApplyDir script\commands
call :ApplyDir script\gui
call :ApplyDir script\command_compiler
call :ApplyDir SingleApplication
call :ApplyDir tools
call :ApplyDir tools\math
call :ApplyDir tools\external_tools
call :ApplyDir tools\math
call :ApplyDir tools\smart_import
call :ApplyDir tools\tilesets

del /F /Q %TARGETDIR%\Editor\_resources
xcopy /Y /E /I "%SOURCEDIR%\Editor\_resources" %TARGETDIR%\Editor\_resources

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
