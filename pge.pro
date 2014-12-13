TEMPLATE = subdirs
SUBDIRS = Editor Engine pcalibrator GIFs2PNG PNG2GIFs LazyFixTool

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
calibrator_cnfs.path = bin/calibrator
calibrator_cnfs.files  = Content/calibrator/*

INSTALLS += configs helps calibrator_cnfs
