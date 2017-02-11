message("Luabind will be built with original PUC-RIO LUA 5.1!")

INCLUDEPATH += $$PWD/../ $$PWD/../lua/

include($$PWD/luabind_common.pri)

win32: include($$PWD/wfopen.pri)

SOURCES +=  \
    ../lua/lapi.c \
    ../lua/lauxlib.c \
    ../lua/lbaselib.c \
    ../lua/lcode.c \
    ../lua/ldblib.c \
    ../lua/ldebug.c \
    ../lua/ldo.c \
    ../lua/ldump.c \
    ../lua/lfunc.c \
    ../lua/lgc.c \
    ../lua/linit.c \
    ../lua/liolib.c \
    ../lua/llex.c \
    ../lua/lmathlib.c \
    ../lua/lmem.c \
    ../lua/loadlib.c \
    ../lua/lobject.c \
    ../lua/lopcodes.c \
    ../lua/loslib.c \
    ../lua/lparser.c \
    ../lua/lstate.c \
    ../lua/lstring.c \
    ../lua/lstrlib.c \
    ../lua/ltable.c \
    ../lua/ltablib.c \
    ../lua/ltm.c \
    ../lua/lundump.c \
    ../lua/lvm.c \
    ../lua/lzio.c \
    ../lua/print.c

HEADERS += ../lua/lapi.h \
    ../lua/lauxlib.h \
    ../lua/lcode.h \
    ../lua/ldebug.h \
    ../lua/ldo.h \
    ../lua/lfunc.h \
    ../lua/lgc.h \
    ../lua/llex.h \
    ../lua/llimits.h \
    ../lua/lmem.h \
    ../lua/lobject.h \
    ../lua/lopcodes.h \
    ../lua/lparser.h \
    ../lua/lstate.h \
    ../lua/lstring.h \
    ../lua/ltable.h \
    ../lua/ltm.h \
    ../lua/lua.h \
    ../lua/luaconf.h \
    ../lua/lualib.h \
    ../lua/lundump.h \
    ../lua/lvm.h \
    ../lua/lzio.h \
    ../lua_inclues/lua.hpp \
    ../lua/file_open.h
