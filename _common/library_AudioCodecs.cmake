
set(libZLib_AC_A_Lib "${CMAKE_BINARY_DIR}/lib/libzlib${PGE_LIBS_DEBUG_SUFFIX}.a")

if(NOT DEFINED libZLib_A_Lib)
    set(libZLib_A_Lib "${libZLib_AC_A_Lib}")
endif()

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
        $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
        "-DCMAKE_DEBUG_POSTFIX=d"
        ${ANDROID_CMAKE_FLAGS}
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DADLMIDI_USE_DOSBOX_EMULATOR=ON>
    DEPENDS SDL2_Local
    BUILD_BYPRODUCTS
        "${libZLib_AC_A_Lib}"
)

InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libFLAC/COPYING.Xiph" RENAME "License.FLAC.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libogg/COPYING" RENAME "License.OGG.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libvorbis/COPYING" RENAME "License.Vorbis.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libtimidity-sdl/COPYING" RENAME "License.Timidity.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libmad/COPYING" RENAME "License.libMAD.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libid3tag-sdl/COPYING" RENAME "License.libID3Tag.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")

