
# Note: You must also include "library_AudioCodecs.cmake" too!

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
        $<$<BOOL:ANDROID>:-DUSE_OPUS=OFF> # TODO: fix the OPUS build and remove this
    DEPENDS AudioCodecs_Local SDL2_Local
)

# Append licenses of libraries
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/SDL_Mixer_X/COPYING.txt" RENAME "License.SDL2_mixer_ext.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")

