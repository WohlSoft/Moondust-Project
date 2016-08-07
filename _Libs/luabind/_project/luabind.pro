DEFINES += USE_LUA_JIT

contains(DEFINES, USE_LUA_JIT):{
message("Luabind will be built with LuaJIT!")
} else {
warning("LuaJIT is not connected!!!")
}

LIBS += -lluajit-5.1

INCLUDEPATH += $$PWD/../ $$PWD/../../ $$PWD/../../_builds/$$TARGETOS/include \
               $$PWD/../../_builds/$$TARGETOS/include/luajit-2.0/

include($$PWD/luabind_common.pri)

HEADERS += \
    ../lua_inclues/lua.hpp

