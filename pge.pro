include(_common/dest_dir.pri)

TEMPLATE = subdirs
SUBDIRS = DependentLibs \
    Editor \
    Engine \
    pcalibrator \
    GIFs2PNG \
    PNG2GIFs \
    LazyFixTool \
    MusicPlayer \
    Manager \
    Maintainer

DEFINES += USE_LUA_JIT

DependentLibs.file = _Libs/pge_deps.pro

Editor.file	 = Editor/pge_editor.pro
Engine.file	 = Engine/pge_engine.pro
pcalibrator.file = PlayableCalibrator/pge_calibrator.pro
GIFs2PNG.file	 = GIFs2PNG/GIFs2PNG.pro
PNG2GIFs.file	 = PNG2GIFs/png2gifs.pro
LazyFixTool.file = LazyFixTool/LazyFixTool.pro
MusicPlayer.file = MusicPlayer/pge_musplay.pro
Manager.file     = Manager/pge_manager.pro
Maintainer.file  = Maintainer/pge_maintainer.pro

configs.path = $$DESTDIR/configs
configs.files = Content/configs/*
helps.path = $$DESTDIR/help
helps.files = Content/help/*
themes.path = $$DESTDIR/themes
themes.files = Content/themes/*
calibrator_cnfs.path = $$DESTDIR/calibrator
calibrator_cnfs.files  = Content/calibrator/*

INSTALLS += configs helps themes calibrator_cnfs

noeditor: {
    SUBDIRS -= Editor
}
noengine: {
    SUBDIRS -= Engine
}
nocalibrator: {
    SUBDIRS -= pcalibrator
}
nocalibrator: {
    SUBDIRS -= pcalibrator
}
nogifs2png: {
    SUBDIRS -= GIFs2PNG
}
nopng2gifs: {
    SUBDIRS -= PNG2GIFs
}
nolazyfixtool: {
    SUBDIRS -= LazyFixTool
}
nomusicplayer: {
    SUBDIRS -= MusicPlayer
}
nomanager: {
    SUBDIRS -= Manager
}
nomaintainer: {
    SUBDIRS -= Maintainer
}

android:{
DEFINES -= USE_LUA_JIT
SUBDIRS -= Engine pcalibrator GIFs2PNG PNG2GIFs LazyFixTool Manager Maintainer MusicPlayer
INSTALLS -= configs helps themes calibrator_cnfs
}

macx:{
DEFINES -= USE_LUA_JIT
}

#CONFIG+=release CONFIG-=debug QTPLUGIN.platforms=qxcb

DISTFILES += \
    pge_engine.supp

