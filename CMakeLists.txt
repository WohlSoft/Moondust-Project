cmake_minimum_required (VERSION 2.8.11)
project (Moondust)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource)
if(insource)
    message(FATAL_ERROR "
==== Build in source directory is not allowed! ====
Please run cmake configure from another directory, for example, create a 'build' sub-directory, and run 'cmake ..' from it."
)
endif()

include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX "d")
if(WIN32)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if(POLICY CMP0058) # Dependencies
    cmake_policy(SET CMP0058 NEW)
endif()

set(DEPENDENCIES_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(PGE_INSTALL_DIRECTORY "Moondust" CACHE STRING "Name of directory where all binary and data files are will be installed")

# Destinition bitness of the build
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(PGE_ARCHITECTURE_BITS "32")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PGE_ARCHITECTURE_BITS "64")
else()
    set(PGE_ARCHITECTURE_BITS "unk")
endif()

# Allow install into space-containing paths
set(CMAKE_INSTALL_PREFIX_ORIG ${CMAKE_INSTALL_PREFIX})
string(REPLACE " " "\\ " CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
string(REPLACE "(" "\\(" CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
string(REPLACE ")" "\\)" CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

# Get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message("== Current GIT hash [${GIT_COMMIT_HASH}], branch [${GIT_BRANCH}]")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message("== Using default build configuration which is a Release!")
endif()

include_directories(
    ${DEPENDENCIES_INSTALL_DIR}/include
    ${DEPENDENCIES_INSTALL_DIR}/include/freetype2
    ${DEPENDENCIES_INSTALL_DIR}/include/lua_includes
)
link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

if(APPLE)
    # Prevent "directory not exists" warnings when building XCode as project
    file(MAKE_DIRECTORY ${DEPENDENCIES_INSTALL_DIR}/lib/Debug)
    file(MAKE_DIRECTORY ${DEPENDENCIES_INSTALL_DIR}/lib/Release)
    # Don't store built executables into "Debug" and "Release" folders
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_BUNDLE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_BUNDLE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
endif()

set(PGE_ENABLE_QT_ENABLED_BY_DEFAULT ON)
if(HAIKU OR EMSCRIPTEN OR ANDROID)
    set(PGE_ENABLE_QT_ENABLED_BY_DEFAULT OFF)
    message("== Qt-dependent components are not supported for this platform")
endif()

if(WIN32)
    set(PGE_STATIC_SDLMIXER_PLATFORM_DEFAULT OFF)
else()
    set(PGE_STATIC_SDLMIXER_PLATFORM_DEFAULT ON)
endif()

option(PGE_STATIC_SDLMIXER "Prefer static assembly of SDL Mixer X and libraries" ${PGE_STATIC_SDLMIXER_PLATFORM_DEFAULT})

if(PGE_STATIC_SDLMIXER)
    set(PGE_SHARED_SDLMIXER OFF)
else()
    set(PGE_SHARED_SDLMIXER ON)
endif()

if(PGE_ENABLE_QT_ENABLED_BY_DEFAULT)
    find_package(Qt5 COMPONENTS Core)
    if(Qt5_FOUND)
        option(PGE_ENABLE_QT "Enable build of Qt-dependent components" ${PGE_ENABLE_QT_ENABLED_BY_DEFAULT})
        option(PGE_ENABLE_STATIC_QT "Use static library deployment of Qt (Set this flag when you are using static build of Qt!)" OFF)
        message("== Qt 5 has been found in ${Qt5_DIR}")
        get_filename_component(QT5_BINARY_DIR "${Qt5_DIR}/../../bin" ABSOLUTE)
        find_program(_QT_LRELEASE_PROGRAM lrelease PATHS ${QT5_BINARY_DIR}/bin/)
        if(_QT_LRELEASE_PROGRAM)
            message(STATUS "Found ${_QT_LRELEASE_PROGRAM}, locales will be compiled!")
        else()
            message(WARNING "Unable to find lrelease, locales will NOT be built!")
        endif()
        # Shared deploymend on Windows platforms
        if(WIN32 AND NOT PGE_ENABLE_STATIC_QT)
            include(_common/WinDeployQt.cmake)
        endif()
    else()
        message("== Qt 5 NOT FOUND! Qt-dependent components will be disabled!")
    endif()
endif()

# FreeType to render TTF fonts
ExternalProject_Add(
    FREETYPE_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/FreeType
    URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/freetype-2.7.1.tar.gz
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        -DWITH_ZLIB=OFF -DWITH_BZip2=OFF -DWITH_PNG=OFF -DWITH_HarfBuzz=OFF
)

# SQLite a small database system
ExternalProject_Add(
    SQLite_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/SQLite
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/sqlite3
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

if(WIN32)
    # UNIX to DOS text format converter utility. Is used to deploy text files on Windows.
    ExternalProject_Add(
        Unix2Dos_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/Unix2Dos
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/Unix2Dos
        CMAKE_ARGS
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    )
endif()

include(_common/unix2dos_copy.cmake)


# FreeType is an image management library
ExternalProject_Add(
    FreeImage_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/FreeImage
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/FreeImage
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

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

# LuaBind is a powerful lua binding library for C++
ExternalProject_Add(
    LuaBind_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/luabind
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/luabind
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_DEBUG_POSTFIX=d"
        $<$<BOOL:PGE_USE_LUAJIT>:-DUSE_LUAJIT=ON>
)
if(PGE_USE_LUAJIT)
    add_dependencies(LuaBind_Local LuaJIT_local)
endif()

if(NOT WIN32)
    set(PATCH_CMD "patch")
else()
    # Use one from Git Bash toolchain, otherwise one from MinGW toolchain will crash
    set(PATCH_CMD "C:/Program Files/Git/usr/bin/patch.exe")
endif()

# Simple Direct-Media Layer library, dependency of AudioCodecs and SDL Mixer X
ExternalProject_Add(
    SDL2_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/SDL2
    URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/SDL-default.tar.gz
    PATCH_COMMAND ${PATCH_CMD} -p1 "<" "${CMAKE_SOURCE_DIR}/_Libs/_sources/patches/SDL2_remove_lib_prefix_on_windows.patch"
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        -DSDL_SHARED=${PGE_SHARED_SDLMIXER}
        $<$<BOOL:WIN32>:-DWASAPI=OFF>
        $<$<BOOL:WIN32>:-DCMAKE_SHARED_LIBRARY_PREFIX="">
        $<$<BOOL:LINUX>:-DSNDIO=OFF>
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DEXTRA_CFLAGS=-s USE_PTHREADS=1>
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DPTHREADS=ON>
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DPTHREADS_SEM=ON>
)

# A collection of audio codecs libraries, dependency of SDL Mixer X
ExternalProject_Add(
    AudioCodecs_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/AudioCodecs
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DSDL2_REPO_PATH=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_DEBUG_POSTFIX=d"
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DADLMIDI_USE_DOSBOX_EMULATOR=ON>
    DEPENDS SDL2_Local
)

InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libFLAC/COPYING.Xiph" RENAME "License.FLAC.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libogg/COPYING" RENAME "License.OGG.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libvorbis/COPYING" RENAME "License.Vorbis.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libtimidity/COPYING" RENAME "License.Timidity.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libmad/COPYING" RENAME "License.libMAD.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libid3tag/COPYING" RENAME "License.libID3Tag.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")

# SDL Mixer X - an audio library, fork of SDL Mixer
ExternalProject_Add(
    SDLMixerX_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/SDLMixerX
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/SDL_Mixer_X/
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DAUDIO_CODECS_INSTALL_PATH=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_DEBUG_POSTFIX=d"
        "-DSDL_MIXER_X_SHARED=${PGE_SHARED_SDLMIXER}"
        $<$<BOOL:WIN32>:-DCMAKE_SHARED_LIBRARY_PREFIX="">
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DUSE_FLAC=OFF>
    DEPENDS AudioCodecs_Local SDL2_Local
)

# Build dependent libraries only. Workaround for Ninja
add_custom_target(libs
    DEPENDS
    SDLMixerX_Local
    SDL2_Local
    LuaBind_Local
    FreeImage_Local
    SQLite_Local
    FREETYPE_Local
)

# Append licenses of libraries
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/SDL_Mixer_X/COPYING.txt" RENAME "License.SDL2_mixer_ext.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")

option(PGE_BUILD_ENGINE "Build the PGE Engine component" ON)
if(PGE_BUILD_ENGINE)
    add_subdirectory(Engine)
    # Append changelog file
    InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/changelog.engine.txt" RENAME "changelog.engine.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    # Append license files
    InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/Engine/LICENSE.mit" RENAME "pge_engine.license.mit.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/Engine/LICENSE.gpl3" RENAME "pge_engine.license.gpl3.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/Content/readmes/Engine.Readme.txt" RENAME "pge_engine.readme.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
endif()

if(NOT EMSCRIPTEN AND NOT ANDROID)
    option(PGE_BUILD_GIFS2PNG "Build the GIFs2PNG utility component" ON)
    if(PGE_BUILD_GIFS2PNG)
        add_subdirectory(GIFs2PNG)
        InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/Content/readmes/GIFs2PNG_Readme.txt" RENAME "GIFs2PNG.readme.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    endif()

    option(PGE_BUILD_PNG2GIFS "Build the GIFs2PNG utility component" ON)
    if(PGE_BUILD_PNG2GIFS)
        add_subdirectory(PNG2GIFs)
        InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/Content/readmes/PNG2GIFs_Readme.txt" RENAME "PNG2GIFs.readme.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    endif()

    option(PGE_BUILD_LAZYFIXTOOL "Build the LazyFixTool utility component" ON)
    if(PGE_BUILD_LAZYFIXTOOL)
        add_subdirectory(LazyFixTool)
        InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/Content/readmes/LazyFixTool_Readme.txt" RENAME "LazyFixTool.readme.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    endif()
endif()

if(PGE_ENABLE_QT AND NOT ANDROID)

    set(WINDEPLOY_LIST)

    option(PGE_BUILD_EDITOR "Build the PGE Editor component (Qt5 required)" ON)
    if(PGE_BUILD_EDITOR)
        add_subdirectory(Editor)
        # Append changelog file
        InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/changelog.editor.txt" RENAME "changelog.editor.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
        # Deployment list
        list(APPEND WINDEPLOY_LIST pge_editor)
    endif()

    option(PGE_BUILD_PLAYERCALIBRATOR "Build the Player Calibrator utility component (Qt5 required)" ON)
    if(PGE_BUILD_PLAYERCALIBRATOR)
        add_subdirectory(PlayableCalibrator)
        list(APPEND WINDEPLOY_LIST pge_calibrator)
    endif()

    option(PGE_BUILD_MUSICPLAYER "Build the SDL Mixer X Music Player component (Qt5 required)" ON)
    if(PGE_BUILD_MUSICPLAYER)
        add_subdirectory(MusicPlayer)
        list(APPEND WINDEPLOY_LIST pge_musplay)
    endif()

    option(PGE_BUILD_MAINTAINER "Build the Maintainer utility component (Qt5 required)" ON)
    if(PGE_BUILD_MAINTAINER)
        add_subdirectory(Maintainer)
        list(APPEND WINDEPLOY_LIST pge_maintainer)
    endif()

    # Shared deployment on Windows
    if(WIN32 AND NOT PGE_ENABLE_STATIC_QT)
        string(REPLACE "\\" "/" QT_BINLIB_DIR $ENV{QtDir})
        set(MINGW_BIN_PATH $ENV{MinGW})
        if(NOT MINGW_BIN_PATH)
            set(MINGW_BIN_PATH ${QT_BINLIB_DIR})
        else()
            string(REPLACE "\\" "/" MINGW_BIN_PATH $ENV{MinGW})
        endif()

        add_custom_target(windeploy)
        WinDeployQt(pge_windeploy TARGETS ${WINDEPLOY_LIST} NOSWGL NOANGLE NOD3DCOMPILER)

        set(JUNK_DLLS)
        list(APPEND JUNK_DLLS
            "languages/qt_en.qm"
            "translations/qt_en.qm"
            "imageformats/qjpeg.dll"
            "imageformats/qjpegd.dll"
            "imageformats/qsvg.dll"
            "imageformats/qsvgd.dll"
            "imageformats/qtga.dll"
            "imageformats/qtgad.dll"
            "imageformats/qtiff.dll"
            "imageformats/qtiffd.dll"
            "imageformats/qwbmp.dll"
            "imageformats/qwbmpd.dll"
            "imageformats/qwebp.dll"
            "imageformats/qwebpd.dll"
            "opengl32sw.dll"
            "libEGL.dll"
            "libGLESV2.dll"
            "D3Dcompiler_47.dll"
        )
        add_custom_target(cleanup_files_windeploy DEPENDS pge_windeploy)
        foreach(JunkFile ${JUNK_DLLS})
            add_custom_command(TARGET cleanup_files_windeploy POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E remove
                                "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/${JunkFile}"
            )
        endforeach()

        set(JUNK_DIRS)
        list(APPEND JUNK_DIRS
            "bearer"
            "iconengines"
            "qmltooling"
            "platforminputcontexts"
            "scenegraph"
        )
        add_custom_target(cleanup_dirs_windeploy DEPENDS pge_windeploy)
        foreach(JunkDir ${JUNK_DIRS})
            add_custom_command(TARGET cleanup_dirs_windeploy POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E remove_directory
                                "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/${JunkDir}"
            )
        endforeach()

        set(MINGW_DLLS)
        if(EXISTS "${MINGW_BIN_PATH}/bin/libgcc_s_dw2-1.dll")
            list(APPEND MINGW_DLLS "${MINGW_BIN_PATH}/bin/libgcc_s_dw2-1.dll")
        endif()
        if(EXISTS "${MINGW_BIN_PATH}/bin/libgcc_s_seh-1.dll")
            list(APPEND MINGW_DLLS "${MINGW_BIN_PATH}/bin/libgcc_s_seh-1.dll")
        endif()
        if(EXISTS "${MINGW_BIN_PATH}/bin/libmingwex-0.dll")
            list(APPEND MINGW_DLLS "${MINGW_BIN_PATH}/bin/libmingwex-0.dll")
        endif()
        if(EXISTS "${MINGW_BIN_PATH}/bin/libwinpthread-1.dll")
            list(APPEND MINGW_DLLS "${MINGW_BIN_PATH}/bin/libwinpthread-1.dll")
        endif()
        if(EXISTS "${MINGW_BIN_PATH}/bin/pthreadGC-3.dll")
            list(APPEND MINGW_DLLS "${MINGW_BIN_PATH}/bin/pthreadGC-3.dll")
        endif()
        if(EXISTS "${MINGW_BIN_PATH}/bin/libstdc++-6.dll")
            list(APPEND MINGW_DLLS "${MINGW_BIN_PATH}/bin/libstdc++-6.dll")
        endif()
        install(FILES
            ${MINGW_DLLS}
            DESTINATION "${PGE_INSTALL_DIRECTORY}/"
        )

        add_custom_target(copy_mingw_dlls DEPENDS pge_windeploy)
        foreach(MingwRuntimeDll ${MINGW_DLLS})
            add_custom_command(TARGET copy_mingw_dlls POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E
                               copy ${MingwRuntimeDll} "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}"
            )
        endforeach()
    endif()

    if(WIN32)# Also deploy tools
        add_custom_target(mkdir_tools_windeploy
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/tools"
        )
        add_custom_target(install_tools_windeploy
            WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/tools"
            COMMAND ${CMAKE_COMMAND} -E tar "-xjvf"
                "${CMAKE_SOURCE_DIR}/_Misc/tools/sox-win32.tar.gz" -C "tools"
            DEPENDS mkdir_tools_windeploy
        )
        if(NOT PGE_ENABLE_STATIC_QT)
            add_dependencies(windeploy pge_windeploy cleanup_files_windeploy cleanup_dirs_windeploy)
        endif()
        add_dependencies(windeploy install_tools_windeploy)
    endif()
endif()

# Append Project License file
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/LICENSE" RENAME "LICENSE.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")

# Process UNIX2DOS if needed
InstallTextFiles()

# =============== Deployment stuff ===============
set(PGE_FILE_STANDALONE_EDITOR_MANUAL
    "<html><head><meta http-equiv=\\\"refresh\\\" content=\\\"0; url=http://pgehelp.wohlsoft.ru/manual_editor.html\\\"/></head><body></body></html>")
file(WRITE "${CMAKE_BINARY_DIR}/install_online_help.cmake" "
file(WRITE
        \"${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/help/manual_editor.htm\"
        \"${PGE_FILE_STANDALONE_EDITOR_MANUAL}\")
")

add_custom_target(put_online_help
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX_ORIG}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/help"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/help"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/install_online_help.cmake"
)

if(WIN32)
    set(PGE_FILE_PORTABLE_APP_INI
        "[Main]\r\nforce-portable=true\r\n\r\n")
else()
    set(PGE_FILE_PORTABLE_APP_INI
        "[Main]\nforce-portable=true\n\n")
endif()

file(WRITE "${CMAKE_BINARY_DIR}/install_portable.cmake" "
file(WRITE
        \"${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/pge_editor.ini\"
        \"${PGE_FILE_PORTABLE_APP_INI}\")
file(WRITE
        \"${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/pge_engine.ini\"
        \"${PGE_FILE_PORTABLE_APP_INI}\")
file(WRITE
        \"${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/pge_calibrator.ini\"
        \"${PGE_FILE_PORTABLE_APP_INI}\")
")

add_custom_target(enable_portable
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX_ORIG}"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/install_portable.cmake"
)

if(UNIX AND NOT APPLE) # Deployment for UNIX-like operating system except macOS
    string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LOWER)
    add_custom_target(create_tar
        WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
        COMMAND ${CMAKE_COMMAND} -E tar "-cjvf"
            "pge_project-${CMAKE_SYSTEM_NAME_LOWER}-${PGE_ARCHITECTURE_BITS}.tar.bz2"
            "${CMAKE_INSTALL_PREFIX}/${PGE_INSTALL_DIRECTORY}"
            COMMENT "Packing tarball..."
    )
endif()

if(WIN32) # Deployment for Windows

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(PGE_DLL_SUFFIX "d")
    else()
        set(PGE_DLL_SUFFIX "")
    endif()

    if(PGE_SHARED_SDLMIXER)
        install(FILES
            "${CMAKE_BINARY_DIR}/bin/SDL2${PGE_DLL_SUFFIX}.dll"
            "${CMAKE_BINARY_DIR}/bin/SDL2_mixer_ext${PGE_DLL_SUFFIX}.dll"
            DESTINATION "${PGE_INSTALL_DIRECTORY}/"
        )
    endif()

    find_program(SevenZipProgram 7z
            PATHS
            "$ENV{ProgramFiles}/7-zip/"
            "C:/Program Files/7-zip/"
            "C:/Program Files (x86)/7-zip/"
    )
    if(SevenZipProgram)
        message("Detected 7Zip installed: ${SevenZipProgram}. Deployment is possible!")
        string(REPLACE "\\" "/" QT_BINLIB_DIR $ENV{QtDir})

        set(ZIP_PACK_DIR "${CMAKE_INSTALL_PREFIX_ORIG}/_packed")
        set(ZIP_SRC_DIR "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}")

        add_custom_target(mkdir_packed_create_zip
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ZIP_PACK_DIR}"
        )

        set(PGE_CommonQtFiles
            "${ZIP_SRC_DIR}/Qt5Core${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/Qt5Gui${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/Qt5Network${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/Qt5Widgets${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/imageformats/"
            "${ZIP_SRC_DIR}/platforms/"
            ${MINGW_DLLS}
        )

        set(PGE_CommonFiles
            "${ZIP_SRC_DIR}/*.dll"
            "${ZIP_SRC_DIR}/licenses/"
            "${ZIP_SRC_DIR}/imageformats/"
            "${ZIP_SRC_DIR}/platforms/"
            "${ZIP_SRC_DIR}/help/"
        )

        if(EXISTS "${QT_BINLIB_DIR}/plugins/styles")
            list(APPEND PGE_CommonFiles "${ZIP_SRC_DIR}/styles/")
            list(APPEND PGE_CommonQtFiles "${ZIP_SRC_DIR}/styles/")
        endif()

        set(PGE_MaintainerFiles
            ${PGE_CommonQtFiles}
            "${ZIP_SRC_DIR}/tools"
            "${ZIP_SRC_DIR}/pge_maintainer.exe"
        )

        set(PGE_MusPlayFiles
            ${PGE_CommonQtFiles}
            "${ZIP_SRC_DIR}/SDL2${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/SDL2_mixer_ext${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/pge_musplay.exe"
        )

        set(PGE_CalibratorFiles
            ${PGE_CommonQtFiles}
            "${ZIP_SRC_DIR}/calibrator"
            "${ZIP_SRC_DIR}/pge_calibrator.exe"
        )

        set(PGE_Gifs2PngFiles
            "${ZIP_SRC_DIR}/GIFs2PNG.exe"
            "${ZIP_SRC_DIR}/GIFs2PNG.readme.txt"
        )

        set(PGE_Png2GifsFiles
            "${ZIP_SRC_DIR}/PNG2GIFs.exe"
            "${ZIP_SRC_DIR}/PNG2GIFs.readme.txt"
        )

        set(PGE_LazyFixToolFiles
            "${ZIP_SRC_DIR}/LazyFixTool.exe"
            "${ZIP_SRC_DIR}/LazyFixTool.readme.txt"
        )

        set(PGE_InstallCommonFiles
            ${PGE_CommonFiles}
            "${ZIP_SRC_DIR}/tools"
        )
        set(PGE_InstallEditorFiles
            "${ZIP_SRC_DIR}/changelog.editor.txt"
            "${ZIP_SRC_DIR}/pge_editor.exe"
            "${ZIP_SRC_DIR}/themes"
            "${ZIP_SRC_DIR}/languages"
            -r "-x!engine_*.qm"
        )
        set(PGE_InstallEngineFiles
            "${ZIP_SRC_DIR}/pge_engine.exe"
            "${ZIP_SRC_DIR}/languages"
            "${ZIP_SRC_DIR}/pge_engine.*.txt"
            "${ZIP_SRC_DIR}/changelog.engine.txt"
            -r "-x!*.png" "-x!editor_*.qm" "-x!qt_*.qm"
        )
        set(PGE_InstallToolsFiles
            "${ZIP_SRC_DIR}/GIFs2PNG.exe"
            "${ZIP_SRC_DIR}/GIFs2PNG.readme.txt"
            "${ZIP_SRC_DIR}/PNG2GIFs.exe"
            "${ZIP_SRC_DIR}/PNG2GIFs.readme.txt"
            "${ZIP_SRC_DIR}/LazyFixTool.exe"
            "${ZIP_SRC_DIR}/LazyFixTool.readme.txt"
            "${ZIP_SRC_DIR}/pge_musplay.exe"
            "${ZIP_SRC_DIR}/pge_calibrator.exe"
            "${ZIP_SRC_DIR}/pge_maintainer.exe"
            "${ZIP_SRC_DIR}/calibrator"
            "${ZIP_SRC_DIR}/tools"
        )

        add_custom_target(create_zip_install
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/install-pge-common-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_InstallCommonFiles}
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/install-pge-editor-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_InstallEditorFiles}
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/install-pge-engine-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_InstallEngineFiles}
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/install-pge-tools-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_InstallToolsFiles}
            DEPENDS mkdir_packed_create_zip
            COMMENT "Packing common install files into ZIP archive..."
        )

        add_custom_target(create_zip_tools
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/pge-musplay-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_MusPlayFiles}
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/pge-maintainer-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_MaintainerFiles}
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/pge-calibrator-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_CalibratorFiles}
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/GIFs2PNG-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_Gifs2PngFiles}
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/PNG2GIFs-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_Png2GifsFiles}
            COMMAND ${SevenZipProgram} a -tzip -mx9
                "${ZIP_PACK_DIR}/LazyFixTool-dev-win${PGE_ARCHITECTURE_BITS}.zip"
                ${PGE_LazyFixToolFiles}
            DEPENDS mkdir_packed_create_zip
            COMMENT "Packing tools into ZIP archive..."
        )

        add_custom_target(create_zip
            ${SevenZipProgram}
            a -tzip -mx9
            "${ZIP_PACK_DIR}/pge-project-dev-win${PGE_ARCHITECTURE_BITS}.zip"
            "${ZIP_SRC_DIR}/"
            DEPENDS mkdir_packed_create_zip
            COMMENT "Packing ZIP archive..."
        )
    else()
        message("!! 7Zip not found! Deployment will be inavailable!")
    endif()
endif()
