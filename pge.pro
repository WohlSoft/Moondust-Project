include(_common/dest_dir.pri)

TEMPLATE = subdirs

DEFINES += USE_LUA_JIT

android: {
    DEFINES -= USE_LUA_JIT
}

macx:{
    DEFINES -= USE_LUA_JIT
}

useccache: {
    #To speed-up building process http://www.ysbl.york.ac.uk/~lohkamp/speedup_compilation.html
    QMAKE_CC = ccache gcc
    QMAKE_CXX = ccache g++
    message("CHACHE with GCC will be used")
}
#CONFIG+=release CONFIG-=debug QTPLUGIN.platforms=qxcb

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
    error("Sorry, Android platform is not supported for entire project building, but a separated components are may support it");
    #SUBDIRS  -= Engine pcalibrator GIFs2PNG PNG2GIFs LazyFixTool Manager Maintainer MusicPlayer
    #INSTALLS -= configs helps themes calibrator_cnfs
}

DISTFILES += \
    pge_engine.supp

