@echo off
SET wasRemoved=0

call :kfile MakeFile
call :kfile _Libs\MakeFile
call :kfile _Libs\MakeFile*
call :kfile _Libs\SDL2_mixer_modified\MakeFile
call :kfile _Libs\SDL2_mixer_modified\MakeFile*
call :kfile _Libs\FreeImage\MakeFile
call :kfile _Libs\FreeImage\MakeFile*
call :kfile _Libs\luabind\_project\MakeFile
call :kfile _Libs\luabind\_project\MakeFile*
call :kfile _Libs\oolua\project\MakeFile
call :kfile _Libs\oolua\project\MakeFile*
call :kfile Editor\MakeFile
call :kfile Editor\MakeFile*
call :kfile Engine\MakeFile
call :kfile Engine\MakeFile*
call :kfile GIFs2PNG\MakeFile
call :kfile GIFs2PNG\MakeFile*
call :kfile LazyFixTool\MakeFile
call :kfile LazyFixTool\MakeFile*
call :kfile PNG2GIFs\MakeFile
call :kfile PNG2GIFs\MakeFile*
call :kfile PlayableCalibrator\MakeFile
call :kfile PlayableCalibrator\MakeFile*
call :kfile Manager\MakeFile
call :kfile Manager\MakeFile*
call :kfile Maintainer\MakeFile
call :kfile Maintainer\MakeFile*
call :kfile MusicPlayer\MakeFile
call :kfile MusicPlayer\MakeFile*

if %wasRemoved%==0 echo Everything already clean!

IF NOT '%1'=='nopause' pause
goto ExitX

:kfile
    IF EXIST %1 del %1 && SET wasRemoved=1 && echo file %1 removed
	GOTO :EOF

:ExitX

