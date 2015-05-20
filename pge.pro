TEMPLATE = subdirs
SUBDIRS = DependentLibs \
    Editor \
    Engine \
    pcalibrator \
    GIFs2PNG \
    PNG2GIFs \
    LazyFixTool \
    Manager

DependentLibs.file = _Libs/pge_deps.pro

Editor.file	 = Editor/pge_editor.pro
Engine.file	 = Engine/pge_engine.pro
pcalibrator.file = PlayableCalibrator/pge_calibrator.pro
GIFs2PNG.file	 = GIFs2PNG/GIFs2PNG.pro
PNG2GIFs.file	 = PNG2GIFs/png2gifs.pro
LazyFixTool.file = LazyFixTool/LazyFixTool.pro
Manager.file     = Manager/pge_manager.pro

configs.path = bin/configs
configs.files = Content/configs/*
helps.path = bin/help
helps.files = Content/help/*
themes.path = bin/themes
themes.files = Content/themes/*
calibrator_cnfs.path = bin/calibrator
calibrator_cnfs.files  = Content/calibrator/*

INSTALLS += configs helps themes calibrator_cnfs

android:{
SUBDIRS -= Engine pcalibrator GIFs2PNG PNG2GIFs LazyFixTool Manager DependentLibs
INSTALLS -= configs helps themes calibrator_cnfs
}

#CONFIG+=release CONFIG-=debug QTPLUGIN.platforms=qxcb
