
add_library(PGE_ZLib INTERFACE)
add_library(PGE_AudioCodecs INTERFACE)

set(libZLib_A_Lib_buit    "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlib${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")

set_static_lib(libZLib_A_Lib_buit     "${DEPENDENCIES_INSTALL_DIR}" zlib)

if(USE_SYSTEM_LIBPNG)
    find_package(ZLIB REQUIRED)
    message("-- Found ZLib: ${ZLIB_LIBRARIES} --")
    target_link_libraries(PGE_ZLib INTERFACE "${LIBZLIB_LIBRARY}")
    target_include_directories(PGE_ZLib INTERFACE "${ZLIB_INCLUDE_DIRS}")
    target_link_libraries(PGE_libPNG INTERFACE "${LIBZLIB_LIBRARY}")
    set(libZLib_A_Lib "${ZLIB_LIBRARIES}")
else()
    set(libZLib_A_Lib ${libZLib_A_Lib_buit})
    set(ZLIB_INCLUDE_DIRS "${DEPENDENCIES_INSTALL_DIR}/include")
    message("-- ZLib will be built (as a part of AudioCodecs): ${libZLib_A_Lib} --")
    target_link_libraries(PGE_ZLib INTERFACE "${libZLib_A_Lib}")
endif()

if(EMSCRIPTEN)
    set(AUDIOCODECS_EMSCRIPTEN_CMAKE_FLAGS -DADLMIDI_USE_DOSBOX_EMULATOR=ON)
endif()

set(AUDIO_CODECS_BUILD_ARGS)
set(AudioCodecs_Deps)

if(NOT SDL2_USE_SYSTEM)
    list(APPEND AudioCodecs_Deps SDL2_Local)
    list(APPEND AUDIO_CODECS_BUILD_ARGS "-DSDL2_REPO_PATH=${DEPENDENCIES_INSTALL_DIR}")
endif()

if(WIN32 AND "${TARGET_PROCESSOR}" STREQUAL "i386")
    # Disable SIMD on 32-bit Windows architecture to allow running on old computers
    list(APPEND AUDIO_CODECS_BUILD_ARGS
        -DDISABLE_SIMD=ON
    )
endif()

# set_static_lib(AC_FLAC          "${DEPENDENCIES_INSTALL_DIR}" FLAC)
set_static_lib(AC_FLUITLITE     "${DEPENDENCIES_INSTALL_DIR}" fluidlite)
set_static_lib(AC_OPUSFILE      "${DEPENDENCIES_INSTALL_DIR}" opusfile)
set_static_lib(AC_OPUS          "${DEPENDENCIES_INSTALL_DIR}" opus)
# set_static_lib(AC_VORBISFILE    "${DEPENDENCIES_INSTALL_DIR}" vorbisfile)
# set_static_lib(AC_VORBIS        "${DEPENDENCIES_INSTALL_DIR}" vorbis)
set_static_lib(AC_OGG           "${DEPENDENCIES_INSTALL_DIR}" ogg)
set_static_lib(AC_MODPLUG       "${DEPENDENCIES_INSTALL_DIR}" modplug)
set_static_lib(AC_XMP           "${DEPENDENCIES_INSTALL_DIR}" xmp)
set_static_lib(AC_ADLMIDI       "${DEPENDENCIES_INSTALL_DIR}" ADLMIDI)
set_static_lib(AC_OPNMIDI       "${DEPENDENCIES_INSTALL_DIR}" OPNMIDI)
set_static_lib(AC_EDMIDI        "${DEPENDENCIES_INSTALL_DIR}" EDMIDI)
set_static_lib(AC_TIMIDITY      "${DEPENDENCIES_INSTALL_DIR}" timidity_sdl2)
set_static_lib(AC_GME           "${DEPENDENCIES_INSTALL_DIR}" gme)

set(AudioCodecs_Libs
#    "${AC_FLAC}"
    "${AC_FLUITLITE}"
    "${AC_OPUSFILE}"
    "${AC_OPUS}"
#    "${AC_VORBISFILE}"
#    "${AC_VORBIS}"
    "${AC_OGG}"
    "${AC_MODPLUG}"
    "${AC_XMP}"
    "${AC_ADLMIDI}"
    "${AC_OPNMIDI}"
    "${AC_EDMIDI}"
    "${AC_TIMIDITY}"
    "${AC_GME}"
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
        "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
        "-DCMAKE_DEBUG_POSTFIX=${PGE_LIBS_DEBUG_SUFFIX}"
        "-DBUILD_OGG_VORBIS=OFF"
        "-DBUILD_FLAC=OFF"
        "-DBUILD_MPG123=OFF"
        "-DBUILD_WAVPACK=OFF"
        ${AUDIO_CODECS_BUILD_ARGS}
        ${ANDROID_CMAKE_FLAGS}
        ${APPLE_CMAKE_FLAGS}
        ${AUDIOCODECS_EMSCRIPTEN_CMAKE_FLAGS}
    DEPENDS ${AudioCodecs_Deps}
    BUILD_BYPRODUCTS
        ${AudioCodecs_Libs}
        "${libZLib_A_Lib_buit}"
)

target_link_libraries(PGE_AudioCodecs INTERFACE "${AudioCodecs_Libs}" ${libZLib_A_Lib})

#InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libFLAC/COPYING.Xiph" RENAME "License.FLAC.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/FluidLite/LICENSE" RENAME "License.FluidLite.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libopus/COPYING" RENAME "License.Opus.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libopusfile/COPYING" RENAME "License.OpusFile.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
#InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libvorbis/COPYING" RENAME "License.Vorbis.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
#InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libogg/COPYING" RENAME "License.OGG.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
#InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libmad/COPYING" RENAME "License.libMAD.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libmodplug/COPYING" RENAME "License.libModPlug.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libxmp/COPYING.LIB" RENAME "License.libXMP.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libADLMIDI/LICENSE" RENAME "License.libADLMIDI.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libOPNMIDI/LICENSE" RENAME "License.libOPNMIDI.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libEDMIDI/LICENSE.txt" RENAME "License.libEDMIDI.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libtimidity-sdl/COPYING" RENAME "License.Timidity.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
InstallTextFile(FILE "${CMAKE_SOURCE_DIR}/_Libs/AudioCodecs/libgme/license.gpl2.txt" RENAME "License.GME.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}/licenses")
