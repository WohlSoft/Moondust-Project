set(SDL2_VIA_AUTOTOOLS 0)
set(SDL2_USE_SYSTEM 0)

if(HAIKU)
    set(SDL2_USE_SYSTEM 1)
endif()

# Not needed for now
#if(NOT WIN32)
#    set(PATCH_CMD "patch")
#else()
#    # Use one from Git Bash toolchain, otherwise one from MinGW toolchain will crash
#    set(PATCH_CMD "C:/Program Files/Git/usr/bin/patch.exe")
#endif()

add_library(PGE_SDL2        INTERFACE)
add_library(PGE_SDL2_static INTERFACE)

option(USE_SYSTEM_SDL2 "Use SDL2 from the system" OFF)

if(USE_SYSTEM_SDL2)
    set(SDL2_USE_SYSTEM 1)
endif()

if(WIN32)
    set(SDL2_SOURCE_TARBALL "${CMAKE_SOURCE_DIR}/_Libs/_sources/SDL-default.zip")
else()
    set(SDL2_SOURCE_TARBALL "${CMAKE_SOURCE_DIR}/_Libs/_sources/SDL-default.tar.gz")
endif()

file(SHA256 ${SDL2_SOURCE_TARBALL} SDL2_SOURCE_TARBALL_HASH)

set(SDL2_main_Lib "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2main${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")

if(WIN32)
    set(SDL2_SO_Lib "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2${PGE_LIBS_DEBUG_SUFFIX}.dll.a")
else()
    set(SDL2_SO_Lib "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

if(WIN32)
    # list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2_mixer_ext-static${MIX_DEBUG_SUFFIX}.a")
    set(SDL2_A_Lib "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2-static${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
    set(SDL2_A_Lib "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

# Simple Direct-Media Layer library, dependency of AudioCodecs and SDL Mixer X
if(SDL2_USE_SYSTEM)
    add_library(SDL2_Local INTERFACE)

    message("== SDL2 will be used from system!")

    find_package(SDL2 REQUIRED)
    message("-- Found SDL2: ${SDL2_LIBRARIES} ${SDL2_INCLUDE_DIRS} --")

    if(TARGET SDL2::SDL2)
        target_link_libraries(PGE_SDL2 INTERFACE SDL2::SDL2)
        target_link_libraries(PGE_SDL2_static INTERFACE SDL2::SDL2)
    else()
        string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
        target_include_directories(PGE_SDL2 INTERFACE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(PGE_SDL2 INTERFACE ${SDL2_LIBRARIES})
        target_include_directories(PGE_SDL2_static INTERFACE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(PGE_SDL2_static INTERFACE ${SDL2_LIBRARIES})
    endif()

    set(SDL2_SO_Lib ${SDL2_LIBRARIES})
    set(SDL2_A_Lib ${SDL2_LIBRARIES})
    set(SDL2_main_Lib ${SDL2MAIN_LIBRARY})

elseif(ANDROID)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(SDL2_DEBUG_SUFFIX "d")
    else()
        set(SDL2_DEBUG_SUFFIX "")
    endif()
    ExternalProject_Add(SDL2_Local_Build
        PREFIX ${CMAKE_BINARY_DIR}/external/SDL2-NDK
        URL ${SDL2_SOURCE_TARBALL}
        URL_HASH SHA256=${SDL2_SOURCE_TARBALL_HASH}
        CONFIGURE_COMMAND ""
        INSTALL_COMMAND ""
        BUILD_COMMAND ${ANDROID_NDK}/ndk-build -C ${CMAKE_BINARY_DIR}/external/SDL2-NDK/src/SDL2_Local_Build SDL2 SDL2_main hidapi
        NDK_PROJECT_PATH=null
        APP_BUILD_SCRIPT=${CMAKE_BINARY_DIR}/external/SDL2-NDK/src/SDL2_Local_Build/Android.mk
        # NDK_OUT=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../..
        NDK_OUT=${DEPENDENCIES_INSTALL_DIR}/lib-ndk-out/
        # NDK_LIBS_OUT=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/..
        NDK_LIBS_OUT=${DEPENDENCIES_INSTALL_DIR}/lib-ndk-libs-out/
        APP_ABI=${ANDROID_ABI}
        NDK_ALL_ABIS=${ANDROID_ABI}
        APP_PLATFORM=${ANDROID_PLATFORM}
        BUILD_BYPRODUCTS
            ${DEPENDENCIES_INSTALL_DIR}/lib-ndk-out/libSDL2.so
            "${CMAKE_BINARY_DIR}/lib/libSDL2.so"
            "${CMAKE_BINARY_DIR}/lib/libhidapi.so"
    )
    add_custom_target(SDL2_Local ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/include/SDL2"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/external/SDL2-NDK/src/SDL2_Local_Build/include/*.h" "${CMAKE_BINARY_DIR}/include/SDL2"
        # COMMAND ${CMAKE_COMMAND} -E copy "${DEPENDENCIES_INSTALL_DIR}/lib-ndk-out/local/${ANDROID_ABI}/libSDL2.a" "${CMAKE_BINARY_DIR}/lib/libSDL2${SDL2_DEBUG_SUFFIX}.a"
        COMMAND ${CMAKE_COMMAND} -E copy "${DEPENDENCIES_INSTALL_DIR}/lib-ndk-out/local/${ANDROID_ABI}/libSDL2main.a" "${CMAKE_BINARY_DIR}/lib/libSDL2main${SDL2_DEBUG_SUFFIX}.a"
        COMMAND ${CMAKE_COMMAND} -E copy "${DEPENDENCIES_INSTALL_DIR}/lib-ndk-out/local/${ANDROID_ABI}/libstdc++.a" "${CMAKE_BINARY_DIR}/lib/libstdc++.a"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/Engine/android-project/moondust/jniLibs/${ANDROID_ABI}"
        COMMAND ${CMAKE_COMMAND} -E copy "${DEPENDENCIES_INSTALL_DIR}/lib-ndk-out/local/${ANDROID_ABI}/*.so" "${CMAKE_BINARY_DIR}/lib"
        COMMAND ${CMAKE_COMMAND} -E copy "${DEPENDENCIES_INSTALL_DIR}/lib-ndk-out/local/${ANDROID_ABI}/*.so" "${CMAKE_SOURCE_DIR}/Engine/android-project/moondust/jniLibs/${ANDROID_ABI}"
        DEPENDS SDL2_Local_Build
    )
    add_library(SDL2LibrarySO SHARED IMPORTED GLOBAL)
    set_property(TARGET SDL2LibrarySO PROPERTY
        IMPORTED_LOCATION
        "${CMAKE_BINARY_DIR}/lib/libSDL2.so"
    )
    add_library(HIDAPILibrary SHARED IMPORTED GLOBAL)
    set_property(TARGET HIDAPILibrary PROPERTY
        IMPORTED_LOCATION
        "${CMAKE_BINARY_DIR}/lib/libhidapi.so"
    )
    target_link_libraries(PGE_SDL2 INTERFACE SDL2LibrarySO HIDAPILibrary)
    target_link_libraries(PGE_SDL2_static INTERFACE SDL2LibrarySO HIDAPILibrary)

elseif(SDL2_VIA_AUTOTOOLS)
    # ============================================================
    # Autotools build of SDL2 on platforms where CMake build is incomplete or not supported
    # ============================================================
    message("== SDL2 will be built through Autotools!")
    ExternalProject_Add(
        SDL2_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/SDL2-AM
        URL ${SDL2_SOURCE_TARBALL}
        URL_HASH SHA256=${SDL2_SOURCE_TARBALL_HASH}
        BUILD_IN_SOURCE 1
        PATCH_COMMAND sed -i "s/-version-info [^ ]\\+/-avoid-version /g" "Makefile.in"
              COMMAND sed -i "s/libSDL2-2\\.0\\.so\\.0/libSDL2\\.so/g" "SDL2.spec.in"
        CONFIGURE_COMMAND "./configure"
            "--prefix=${CMAKE_BINARY_DIR}"
            "--includedir=${CMAKE_BINARY_DIR}/include"
            "--libdir=${CMAKE_BINARY_DIR}/lib"
            $<$<BOOL:WIN32>:--disable-wasapi>
            $<$<BOOL:LINUX>:--disable-sndio>
        BUILD_COMMAND make -j 4
        INSTALL_DIR make install
            COMMAND ln -s "libSDL2.a" "${CMAKE_BINARY_DIR}/lib/libSDL2d.a"
            COMMAND ln -s "libSDL2.so" "${CMAKE_BINARY_DIR}/lib/libSDL2d.so"
        BUILD_BYPRODUCTS
            "${SDL2_SO_Lib}"
            "${SDL2_A_Lib}"
            "${SDL2_main_Lib}"
    )
    target_link_libraries(PGE_SDL2 INTERFACE "${SDL2_SO_Lib}")
    target_link_libraries(PGE_SDL2_static INTERFACE "${SDL2_A_Lib}")
    if((WIN32 OR HAIKU) AND NOT EMSCRIPTEN)
        target_link_libraries(PGE_SDL2 INTERFACE ${SDL2_main_Lib})
        target_link_libraries(PGE_SDL2_static INTERFACE  ${SDL2_main_Lib})
    endif()

else()
    # ============================================================
    # CMake build of SDL2 is a best choice for most of platforms
    # ============================================================
    message("== SDL2 shared: ${PGE_SHARED_SDLMIXER}")
    ExternalProject_Add(
        SDL2_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/SDL2
        URL ${SDL2_SOURCE_TARBALL}
        URL_HASH SHA256=${SDL2_SOURCE_TARBALL_HASH}
        # This issue has been fixed. Kept commented as example for future neccerity to patch futher SDL2 updates
        # PATCH_COMMAND ${PATCH_CMD} -p1 "<" "${CMAKE_SOURCE_DIR}/_Libs/_sources/patches/SDL2_remove_lib_prefix_on_windows.patch"
        CMAKE_ARGS
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
            $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
            -DSDL_SHARED=${PGE_SHARED_SDLMIXER}
            -DLIBC=ON
            # $<$<BOOL:WIN32>:-DWASAPI=OFF>  #For some experiment, enable WASAPI support
            $<$<BOOL:WIN32>:-DCMAKE_SHARED_LIBRARY_PREFIX="">
            $<$<BOOL:LINUX>:-DSNDIO=OFF>
            $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DEXTRA_CFLAGS=-s USE_PTHREADS=1>
            $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DPTHREADS=ON>
            $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DPTHREADS_SEM=ON>
        BUILD_BYPRODUCTS
            "${SDL2_SO_Lib}"
            "${SDL2_A_Lib}"
            "${SDL2_main_Lib}"
    )

    target_link_libraries(PGE_SDL2 INTERFACE "${SDL2_SO_Lib}")
    target_link_libraries(PGE_SDL2_static INTERFACE "${SDL2_A_Lib}")
    if((WIN32 OR HAIKU) AND NOT EMSCRIPTEN)
        target_link_libraries(PGE_SDL2 INTERFACE ${SDL2_main_Lib})
        target_link_libraries(PGE_SDL2_static INTERFACE  ${SDL2_main_Lib})
    endif()
endif()

set(SDL2_DEPENDENT_LIBS)
set(SDL2_SO_DEPENDENT_LIBS)

if(WIN32 AND NOT EMSCRIPTEN)
    list(APPEND SDL2_DEPENDENT_LIBS
        version opengl32 dbghelp advapi32 kernel32 winmm imm32 setupapi
    )
    list(APPEND SDL2_SO_DEPENDENT_LIBS
        version opengl32 dbghelp advapi32 kernel32 winmm imm32 setupapi
    )
endif()

if(NOT WIN32 AND NOT EMSCRIPTEN AND NOT APPLE AND NOT ANDROID)
    find_library(_LIB_GL GL)
    if(_LIB_GL)
        list(APPEND SDL2_DEPENDENT_LIBS ${_LIB_GL})
        list(APPEND SDL2_SO_DEPENDENT_LIBS ${_LIB_GL})
    endif()

    find_library(_lib_dl dl)
    if(_lib_dl)
        list(APPEND SDL2_DEPENDENT_LIBS ${_lib_dl})
        list(APPEND SDL2_SO_DEPENDENT_LIBS ${_lib_dl})
    endif()
endif()

if(ANDROID)
    list(APPEND SDL2_DEPENDENT_LIBS
        GLESv1_CM GLESv2 OpenSLES log dl hidapi android
    )
endif()

if(HAIKU)
    list(APPEND SDL2_DEPENDENT_LIBS
        be device game media
    )
endif()

if(APPLE)
    find_library(COREAUDIO_LIBRARY CoreAudio)
    list(APPEND SDL2_DEPENDENT_LIBS ${COREAUDIO_LIBRARY})
    find_library(COREVIDEO_LIBRARY CoreVideo)
    list(APPEND SDL2_DEPENDENT_LIBS ${COREVIDEO_LIBRARY})
    find_library(IOKIT_LIBRARY IOKit)
    list(APPEND SDL2_DEPENDENT_LIBS ${IOKIT_LIBRARY})
    find_library(CARBON_LIBRARY Carbon)
    list(APPEND SDL2_DEPENDENT_LIBS ${CARBON_LIBRARY})
    find_library(COCOA_LIBRARY Cocoa)
    list(APPEND SDL2_DEPENDENT_LIBS ${COCOA_LIBRARY})
    find_library(FORCEFEEDBAK_LIBRARY ForceFeedback)
    list(APPEND SDL2_DEPENDENT_LIBS ${FORCEFEEDBAK_LIBRARY})
    find_library(METAL_LIBRARY Metal)
    list(APPEND SDL2_DEPENDENT_LIBS ${METAL_LIBRARY})
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND SDL2_DEPENDENT_LIBS ${COREFOUNDATION_LIBRARY})
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
    list(APPEND SDL2_DEPENDENT_LIBS ${AUDIOTOOLBOX_LIBRARY})
    find_library(AUDIOUNIT_LIBRARY AudioUnit)
    list(APPEND SDL2_DEPENDENT_LIBS ${AUDIOUNIT_LIBRARY})
    find_library(OPENGL_LIBRARY OpenGL)
    list(APPEND SDL2_DEPENDENT_LIBS ${OPENGL_LIBRARY})
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    find_library(_lib_pthread pthread)
    if(_lib_pthread)
        list(APPEND SDL2_DEPENDENT_LIBS ${_lib_pthread})
        list(APPEND SDL2_SO_DEPENDENT_LIBS ${_lib_pthread})
    endif()
endif()

if(NOT USE_SYSTEM_SDL2 AND PGE_SHARED_SDLMIXER AND NOT WIN32)
    install(FILES ${SDL2_SO_Lib} DESTINATION ${PGE_INSTALL_DIRECTORY})
endif()

target_link_libraries(PGE_SDL2 INTERFACE ${SDL2_SO_DEPENDENT_LIBS})
target_link_libraries(PGE_SDL2_static INTERFACE ${SDL2_DEPENDENT_LIBS})
