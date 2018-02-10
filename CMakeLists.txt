cmake_minimum_required (VERSION 2.8.11)
project (Moondust)

include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX "d")

set(DEPENDENCIES_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DGIT_VERSION="CMakeUnknown")

include_directories(
    ${DEPENDENCIES_INSTALL_DIR}/include
    ${DEPENDENCIES_INSTALL_DIR}/include/freetype2
    ${DEPENDENCIES_INSTALL_DIR}/include/lua_includes
)
link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

# FreeType to render TTF fonts
ExternalProject_Add(
    FREETYPE_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/FreeType
    URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/freetype-2.7.1.tar.gz
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" -DWITH_ZLIB=OFF -DWITH_BZip2=OFF -DWITH_PNG=OFF -DWITH_HarfBuzz=OFF
)

# SQLite a small database system
ExternalProject_Add(
    SQLite_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/SQLite
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/sqlite3
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

# FreeType is an image management library
ExternalProject_Add(
    FreeImage_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/FreeImage
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/FreeImage
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

# LuaBind is a powerful lua binding library for C++
ExternalProject_Add(
    LuaBind_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/luabind
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/luabind
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" "-DCMAKE_DEBUG_POSTFIX=d"
)

# Simple Direct-Media Layer library, dependency of AudioCodecs and SDL Mixer X
ExternalProject_Add(
    SDL2_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/SDL2
    URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/SDL-default.tar.gz
    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        -DSNDIO=OFF
        -DSDL_SHARED=OFF
)

# A collection of audio codecs libraries, dependency of SDL Mixer X
ExternalProject_Add(
    AudioCodecs_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/AudioCodecs
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" "-DSDL2_REPO_PATH=${DEPENDENCIES_INSTALL_DIR}" "-DCMAKE_DEBUG_POSTFIX=d"
    DEPENDS SDL2_Local
)

# SDL Mixer X - an audio library, fork of SDL Mixer
ExternalProject_Add(
    SDLMixerX_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/SDLMixerX
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/SDL_Mixer_X/
    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DAUDIO_CODECS_REPO_PATH=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_DEBUG_POSTFIX=d"
        "-DSDL_MIXER_X_SHARED=OFF"
    DEPENDS AudioCodecs_Local SDL2_Local
)

add_subdirectory(Engine)
