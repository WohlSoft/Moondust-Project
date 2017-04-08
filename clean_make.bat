@echo off
SET wasRemoved=0

call :kfile %CD%
call :kfile ServerLib\ServerAPI
call :kfile ServerLib\ServerApp
call :kfile _Libs
call :kfile _Libs\AudioCodecs
call :kfile _Libs\AudioCodecs\libmad
call :kfile _Libs\AudioCodecs\libogg
call :kfile _Libs\AudioCodecs\libvorbis
call :kfile _Libs\AudioCodecs\libFLAC
call :kfile _Libs\SDL_Mixer_X
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
GOTO :EOF
goto ExitX

:kfile
    IF EXIST "%1\MakeFile" del "%1\MakeFile" && SET wasRemoved=1 && echo file %1\MakeFile removed
    IF EXIST "%1\MakeFile*" del "%1\MakeFile*" && SET wasRemoved=1 && echo file %1\MakeFile* removed
    IF EXIST "%1\object_script*" del "%1\object_script*" && SET wasRemoved=1 && echo file %1\object_script* removed
	GOTO :EOF
:ExitX
