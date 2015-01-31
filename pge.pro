TEMPLATE = subdirs
SUBDIRS = SDL2MixerModded Editor Engine pcalibrator GIFs2PNG PNG2GIFs LazyFixTool

SDL2MixerModded.file = _Libs/SDL2_mixer_modified/SDL2_mixer.pro
Editor.file	 = Editor/pge_editor.pro
Engine.file	 = Engine/pge_engine.pro
pcalibrator.file = PlayableCalibrator/pge_calibrator.pro
GIFs2PNG.file	 = GIFs2PNG/GIFs2PNG.pro
PNG2GIFs.file	 = PNG2GIFs/png2gifs.pro
LazyFixTool.file = LazyFixTool/LazyFixTool.pro

configs.path = bin/configs
configs.files = Content/configs/*
helps.path = bin/help
helps.files = Content/help/*
themes.path = bin/themes
themes.files = Content/themes/*
calibrator_cnfs.path = bin/calibrator
calibrator_cnfs.files  = Content/calibrator/*

deplibs.path = bin
linux-g++: {
deplibs.files += _Libs/_builds/linux/lib/*.so*
deplibs.files += _Libs/_builds/sdl2_mixer_mod/*.so*
}
win32: {
deplibs.files += _Libs/_builds/win32/bin/*.dll
deplibs.files += _Libs/_builds/sdl2_mixer_mod/*.dll
}

INSTALLS += configs helps themes calibrator_cnfs deplibs

