
include($$PWD/../_common/dest_dir.pri)

TEMPLATE = subdirs
CONFIG  -= app_bundle

# ========= Audio Codecs ==========
SUBDIRS += AudioCodecs

# ========== SDL Mixer X ==========
SUBDIRS += SDL2MixerX
SDL2MixerX.file = SDL_Mixer_X/SDL_Mixer_X.pro
SDL2MixerX.depends = AudioCodecs

# ========== FreeImage (lite) ==========
SUBDIRS += FreeImageLite
FreeImageLite.file = libFreeImageLite.pro

# ========== OO-Lua ==========
# SUBDIRS += OOLua
# OOLua.file = oolua/project/oolua.pro

# ========== Server API library ==========
# SUBDIRS += PGEServerAPI
# PGEServerAPI.file = $$PWD/../ServerLib/ServerAPI/ServerAPI.pro

!android:!macx:{
    # ========== LuaBind (Lua-JIT) ==========
    DEFINES += USE_LUA_JIT
    SUBDIRS += LuaBind
    LuaBind.file = luabind/_project/luabind.pro
} else {
    # ========== LuaBind (PUC-Rio) ==========
    DEFINES -= USE_LUA_JIT
    SUBDIRS += LuaBindnoJit
    LuaBindnoJit.file = luabind/_project/luabind_nojit.pro
}

deplibs.path = bin

linux-g++: {
    deplibs.files += $$PWD/_builds/linux/lib/libSDL2-*.so $$PWD/_builds/linux/lib/libSDL2_mixer_ext.so
    deplibs.extra = cp -a $$PWD/_builds/linux/lib/libSDL2_mixer_ext.so* $$DESTDIR
}

win32: {
    deplibs.files += $$PWD/_builds/win32/bin/*.dll $$PWD/_builds/win32/lib/SDL2_mixer_ext.dll
}

!macx:{
    deplibs.path = $$DESTDIR
    INSTALLS += deplibs
}

