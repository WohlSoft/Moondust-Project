@echo off
SET wasRemoved=0

call :kfile .
call :kfile ServerLib\ServerAPI
call :kfile ServerLib\ServerAPP
call :kfile _Libs
call :kfile _Libs\SDL2_mixer_modified
call :kfile _Libs\FreeImage
call :kfile _Libs\luabind\_project
call :kfile _Libs\oolua\project
call :kfile Editor
call :kfile Engine
call :kfile GIFs2PNG
call :kfile LazyFixTool
call :kfile PNG2GIFs
call :kfile PlayableCalibrator
call :kfile Manager
call :kfile Maintainer
call :kfile MusicPlayer

if %wasRemoved%==0 echo Everything already clean!

IF NOT '%1'=='nopause' pause
goto ExitX

:kfile
    IF EXIST "%1\MakeFile" del "%1\MakeFile" && SET wasRemoved=1 && echo file %1\MakeFile removed
    IF EXIST "%1\MakeFile*" del "%1\MakeFile*" && SET wasRemoved=1 && echo file %1\MakeFile* removed
    IF EXIST "%1\object_script*" del "%1\object_script*" && SET wasRemoved=1 && echo file %1\object_script* removed
	GOTO :EOF

:ExitX

