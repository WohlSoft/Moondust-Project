
# Note: You must also include "library_AudioCodecs.cmake" too!

if(WIN32)
    set(SDL_MixerX_SO_Lib "${CMAKE_BINARY_DIR}/lib/libSDL2_mixer_ext${PGE_LIBS_DEBUG_SUFFIX}.dll.a")
elseif(APPLE)
    set(SDL_MixerX_SO_Lib "${CMAKE_BINARY_DIR}/lib/libSDL2_mixer_ext${PGE_LIBS_DEBUG_SUFFIX}.dylib")
else()
    set(SDL_MixerX_SO_Lib "${CMAKE_BINARY_DIR}/lib/libSDL2_mixer_ext${PGE_LIBS_DEBUG_SUFFIX}.so")
endif()

if(WIN32)
    # list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2_mixer_ext-static${MIX_DEBUG_SUFFIX}.a")
    set(SDL_MixerX_A_Lib "${CMAKE_BINARY_DIR}/lib/libSDL2_mixer_ext-static${PGE_LIBS_DEBUG_SUFFIX}.a")
else()
    set(SDL_MixerX_A_Lib "${CMAKE_BINARY_DIR}/lib/libSDL2_mixer_ext${PGE_LIBS_DEBUG_SUFFIX}.a")
endif()

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
        ${ANDROID_CMAKE_FLAGS}
        $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
        $<$<BOOL:WIN32>:-DCMAKE_SHARED_LIBRARY_PREFIX="">
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DUSE_FLAC=OFF>
    DEPENDS AudioCodecs_Local SDL2_Local
    BUILD_BYPRODUCTS
        "${SDL_MixerX_SO_Lib}"
        "${SDL_MixerX_A_Lib}"
)

add_library(SDLMixerXLibrarySO SHARED IMPORTED GLOBAL)
if(WIN32)
    set_property(TARGET SDLMixerXLibrarySO PROPERTY IMPORTED_IMPLIB "${SDL_MixerX_SO_Lib}")
else()
    set_property(TARGET SDLMixerXLibrarySO PROPERTY IMPORTED_LOCATION "${SDL_MixerX_SO_Lib}")
endif()

add_library(SDLMixerXLibraryA STATIC IMPORTED GLOBAL)
set_property(TARGET SDLMixerXLibraryA PROPERTY
    IMPORTED_LOCATION
    "${SDL_MixerX_A_Lib}"
)

# Append licenses of libraries
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/SDL_Mixer_X/COPYING.txt" RENAME "License.SDL2_mixer_ext.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")

