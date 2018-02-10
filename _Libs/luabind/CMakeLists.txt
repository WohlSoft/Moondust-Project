cmake_minimum_required (VERSION 2.8.11)
project (LuaBind C)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-D_LIBLUABIND)
if(NOT WIN32)
    add_definitions(-DLUA_USE_LINUX)
endif()

include_directories(. luabind lua)

set(LUA_SRCS)
list(APPEND LUA_SRCS
    lua/lapi.c
    lua/lauxlib.c
    lua/lbaselib.c
    lua/lcode.c
    lua/ldblib.c
    lua/ldebug.c
    lua/ldo.c
    lua/ldump.c
    lua/lfunc.c
    lua/lgc.c
    lua/linit.c
    lua/liolib.c
    lua/llex.c
    lua/lmathlib.c
    lua/lmem.c
    lua/loadlib.c
    lua/lobject.c
    lua/lopcodes.c
    lua/loslib.c
    lua/lparser.c
    lua/lstate.c
    lua/lstring.c
    lua/lstrlib.c
    lua/ltable.c
    lua/ltablib.c
    lua/ltm.c
    lua/lundump.c
    lua/lvm.c
    lua/lzio.c
    lua/print.c
)

if(WIN32)
    list(APPEND LUA_SRCS lua/file_open.c)
endif()

set(LIABIND_SRCS)
list(APPEND LUABIND_SRCS
    src/class.cpp
    src/class_info.cpp
    src/class_registry.cpp
    src/class_rep.cpp
    src/create_class.cpp
    src/error.cpp
    src/exception_handler.cpp
    src/function.cpp
    src/function_introspection.cpp
    src/inheritance.cpp
    src/link_compatibility.cpp
    src/object_rep.cpp
    src/open.cpp
    src/operator.cpp
    src/pcall.cpp
    src/scope.cpp
    src/set_package_preload.cpp
    src/stack_content_by_name.cpp
    src/weak_ref.cpp
    src/wrapper_base.cpp
    src/headertest.cpp
)

add_library(luabind STATIC
    ${LUA_SRCS}
    ${LUABIND_SRCS}
)

install(DIRECTORY
        luabind
        DESTINATION include)

install(FILES
        lua_inclues/lua.hpp
        lua/lua.h
        lua/lualib.h
        lua/luaconf.h
        lua/lauxlib.h
        DESTINATION include/lua_inclues)

INSTALL(TARGETS luabind
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION "include"
)

