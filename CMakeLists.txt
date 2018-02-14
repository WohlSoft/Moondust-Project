cmake_minimum_required (VERSION 2.8.11)
project (Moondust)

include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX "d")

set(DEPENDENCIES_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(PGE_INSTALL_DIRECTORY "Moondust")

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

set(PGE_ENABLE_QT_ENABLED_BY_DEFAULT ON)
if(HAIKU OR EMSCRIPTEN OR ANDROID)
    set(PGE_ENABLE_QT_ENABLED_BY_DEFAULT OFF)
    message("== Qt-dependent components are not supported for this platform")
endif()

if(PGE_ENABLE_QT_ENABLED_BY_DEFAULT)
    find_package(Qt5 COMPONENTS Core)
    if(Qt5_FOUND)
        option(PGE_ENABLE_QT "Enable build of Qt-dependent components" ${PGE_ENABLE_QT_ENABLED_BY_DEFAULT})
        message("== Qt 5 has been found in ${Qt5_DIR}")
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
)

# Simple Direct-Media Layer library, dependency of AudioCodecs and SDL Mixer X
ExternalProject_Add(
    SDL2_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/SDL2
    URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/SDL-default.tar.gz
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        -DSDL_SHARED=OFF
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

install(FILES "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libFLAC/COPYING.Xiph" RENAME "License.FLAC.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses/")
install(FILES "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libogg/COPYING" RENAME "License.OGG.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses/")
install(FILES "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libvorbis/COPYING" RENAME "License.Vorbis.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses/")
install(FILES "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libtimidity/COPYING" RENAME "License.Timidity.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses/")
install(FILES "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libmad/COPYING" RENAME "License.libMAD.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses/")
install(FILES "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libid3tag/COPYING" RENAME "License.libID3Tag.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses/")


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
        "-DAUDIO_CODECS_REPO_PATH=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_DEBUG_POSTFIX=d"
        "-DSDL_MIXER_X_SHARED=OFF"
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DUSE_FLAC=OFF>
    DEPENDS AudioCodecs_Local SDL2_Local
)

# Append licenses of libraries
install(FILES "${CMAKE_SOURCE_DIR}/_Libs/SDL_Mixer_X/COPYING.txt" RENAME "License.SDL2_mixer_ext.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses/")

option(PGE_BUILD_ENGINE "Build the PGE Engine component" ON)
if(PGE_BUILD_ENGINE)
    add_subdirectory(Engine)
endif()

if(NOT EMSCRIPTEN AND NOT ANDROID)
    option(PGE_BUILD_GIFS2PNG "Build the GIFs2PNG utility component" ON)
    if(PGE_BUILD_GIFS2PNG)
        add_subdirectory(GIFs2PNG)
    endif()

    option(PGE_BUILD_PNG2GIFS "Build the GIFs2PNG utility component" ON)
    if(PGE_BUILD_PNG2GIFS)
        add_subdirectory(PNG2GIFs)
    endif()

    option(PGE_BUILD_LAZYFIXTOOL "Build the LazyFixTool utility component" ON)
    if(PGE_BUILD_LAZYFIXTOOL)
        add_subdirectory(LazyFixTool)
    endif()
endif()

if(PGE_ENABLE_QT AND NOT ANDROID)
    option(PGE_BUILD_MUSICPLAYER "Build the SDL Mixer X Music Player component (Qt5 required)" ON)
    if(PGE_BUILD_MUSICPLAYER)
        add_subdirectory(MusicPlayer)
    endif()

    option(PGE_BUILD_MAINTAINER "Build the Maintainer utility component (Qt5 required)" ON)
    if(PGE_BUILD_MAINTAINER)
        add_subdirectory(Maintainer)
    endif()
endif()

# Append Project License file
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" RENAME "LICENSE.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/")