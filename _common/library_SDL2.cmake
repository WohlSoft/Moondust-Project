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

# Simple Direct-Media Layer library, dependency of AudioCodecs and SDL Mixer X
if(ANDROID)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(SDL2_DEBUG_SUFFIX "d")
    else()
        set(SDL2_DEBUG_SUFFIX "")
    endif()
    ExternalProject_Add(SDL2_Local_Build
        PREFIX ${CMAKE_BINARY_DIR}/external/SDL2-NDK
        URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/SDL-default.tar.gz
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
        BUILD_BYPRODUCTS ${DEPENDENCIES_INSTALL_DIR}/lib-ndk-out/libSDL2.so
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
elseif(SDL2_USE_SYSTEM)
    find_package(SDL2 REQUIRED)
    message("== SDL2 will be used from system!")
    add_custom_target(SDL2_Local ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/include/SDL2"
        COMMAND ${CMAKE_COMMAND} -E copy "${SDL2_INCLUDE_DIRS}/*.h" "${CMAKE_BINARY_DIR}/include/SDL2"
    )
elseif(SDL2_VIA_AUTOTOOLS)
    # ============================================================
    # Autotools build of SDL2 on platforms where CMake build is incomplete or not supported
    # ============================================================
    message("== SDL2 will be built through Autotools!")
    ExternalProject_Add(
        SDL2_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/SDL2-AM
        URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/SDL-default.tar.gz
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
    )
else()
    # ============================================================
    # CMake build of SDL2 is a best choice for most of platforms
    # ============================================================
    message("== SDL2 shared: ${PGE_SHARED_SDLMIXER}")
    ExternalProject_Add(
        SDL2_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/SDL2
        URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/SDL-default.tar.gz
        # This issue has been fixed. Kept commented as example for future neccerity to patch futher SDL2 updates
        # PATCH_COMMAND ${PATCH_CMD} -p1 "<" "${CMAKE_SOURCE_DIR}/_Libs/_sources/patches/SDL2_remove_lib_prefix_on_windows.patch"
        CMAKE_ARGS
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
            -DSDL_SHARED=${PGE_SHARED_SDLMIXER}
            # $<$<BOOL:WIN32>:-DWASAPI=OFF>  #For some experiment, enable WASAPI support
            $<$<BOOL:WIN32>:-DCMAKE_SHARED_LIBRARY_PREFIX="">
            $<$<BOOL:LINUX>:-DSNDIO=OFF>
            $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DEXTRA_CFLAGS=-s USE_PTHREADS=1>
            $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DPTHREADS=ON>
            $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DPTHREADS_SEM=ON>
    )
endif()
