
include(../_common/dest_dir.pri)

TEMPLATE = subdirs
CONFIG -= app_bundle
SUBDIRS = SDL2MixerModded OOLua

#PGEServerAPI.file = $$PWD/../ServerLib/ServerAPI/ServerAPI.pro
SDL2MixerModded.file = SDL2_mixer_modified/SDL2_mixer.pro
OOLua.file = oolua/project/oolua.pro

!android:{
DEFINES+=USE_LUA_JIT
SUBDIRS+=LuaBind
LuaBind.file = luabind/_project/luabind.pro
} else {
SUBDIRS+=LuaBindnoJit
LuaBindnoJit.file = luabind/_project/luabind_nojit.pro
}

deplibs.path = bin
linux-g++: {
deplibs.files += $$PWD/_builds/linux/lib/*.so
}
win32: {
deplibs.files += $$PWD/_builds/win32/bin/*.dll
deplibs.files += $$PWD/_builds/win32/lib/*.dll
}
macx: {
deplibs.files += $$PWD/_builds/macos/lib/*.dylib
deplibs.files += $$PWD/_builds/macos/frameworks/*
}

!macx: {
deplibs.path = $$DESTDIR
} else {
deplibs.path = $$PWD/../bin/_Libs
}

INSTALLS += deplibs

