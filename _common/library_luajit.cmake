
set(PGE_USE_LUAJIT_ENABLED_BY_DEFAULT ON)
if(ANDROID OR EMSCRIPTEN OR MSVC)
    # Disable LuaJIT for unsupported platforms
    set(PGE_USE_LUAJIT_ENABLED_BY_DEFAULT OFF)
endif()

option(PGE_USE_LUAJIT "Use LuaJIT lua engine" ${PGE_USE_LUAJIT_ENABLED_BY_DEFAULT})

if(PGE_USE_LUAJIT)
    if(WIN32)
        set(MAKECMD "mingw32-make")
    else()
        set(MAKECMD "make")
    endif()
    if(APPLE)
        set(ENV{MACOSX_DEPLOYMENT_TARGET} "${CMAKE_OSX_DEPLOYMENT_TARGET}")
    endif()
    set(LUAJIT_LIBNAME libluajit$<$<CONFIG:Debug>:d>.a)
    string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
    if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
        message("== LuaJIT will be built in debug mode")
        set(BUILD_FLAGS "CCDEBUG=-g -O0")
    endif()

    if(WIN32)
        set(LUAJIT_INSTALL_COMMAND "")
    else()
        set(LUAJIT_INSTALL_COMMAND
            ${MAKECMD}
            install
            "INSTALL_ANAME=${LUAJIT_LIBNAME}"
            "BUILDMODE=static"
            "PREFIX=${CMAKE_BINARY_DIR}"
        )
    endif()

    set(LUAJIT_INSTALL_DIR ${CMAKE_BINARY_DIR})
    set(LUAJIT_LOCAL_SOURCE_DIR ${CMAKE_BINARY_DIR}/external/luajit/LuaJIT-src)

    ExternalProject_Add(
        LuaJIT_local
        PREFIX ${CMAKE_BINARY_DIR}/external/luajit
        URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/luajit.tar.gz
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        SOURCE_DIR ${LUAJIT_LOCAL_SOURCE_DIR}
        BUILD_IN_SOURCE 1
        BUILD_COMMAND
            ${MAKECMD} -s
            "BUILDMODE=static"
            "PREFIX=${CMAKE_BINARY_DIR}"
            "CCWARN+=-Wno-unused-function"
            ${BUILD_FLAGS}
        INSTALL_COMMAND "${LUAJIT_INSTALL_COMMAND}"
    )
    if(WIN32)
        set(LUAJIT_BINARY_FILES "luajit.exe")

        ExternalProject_Add_Step(
            LuaJIT_local CreateLuaJitInstallFolder
            COMMAND ${CMAKE_COMMAND} -E make_directory ${LUAJIT_INSTALL_DIR}/include/luajit-2.1 ${LUAJIT_INSTALL_DIR}/lib ${LUAJIT_INSTALL_DIR}/bin
            COMMENT "Creating install folder"
            DEPENDEES BUILD
        )

        ExternalProject_Add_Step(
            LuaJIT_local InstallLuaJitHeaders
            COMMAND ${CMAKE_COMMAND} -E copy lua.h lua.hpp luaconf.h luajit.h lualib.h lauxlib.h ${LUAJIT_INSTALL_DIR}/include/luajit-2.1
            WORKING_DIRECTORY ${LUAJIT_LOCAL_SOURCE_DIR}/src
            COMMENT "Installing luajit headers"
            DEPENDEES BUILD CreateLuaJitInstallFolder
        )

        ExternalProject_Add_Step(
            LuaJIT_local InstallLuaJitLib
            COMMAND ${CMAKE_COMMAND} -E copy libluajit.a "${LUAJIT_INSTALL_DIR}/lib/${LUAJIT_LIBNAME}"
            WORKING_DIRECTORY ${LUAJIT_LOCAL_SOURCE_DIR}/src
            COMMENT "Installing luajit lib"
            DEPENDEES BUILD CreateLuaJitInstallFolder
        )

        ExternalProject_Add_Step(
            LuaJIT_local InstallLuaJitDLL
            COMMAND ${CMAKE_COMMAND} -E copy ${LUAJIT_BINARY_FILES} ${LUAJIT_INSTALL_DIR}/bin
            WORKING_DIRECTORY ${LUAJIT_LOCAL_SOURCE_DIR}/src
            COMMENT "Installing luajit shared library"
            DEPENDEES BUILD CreateLuaJitInstallFolder
        )
    endif() #NOT WIN32
endif()

