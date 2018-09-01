set(SDL2_VIA_AUTOTOOLS 0)

if(HAIKU OR ANDROID)
    set(SDL2_VIA_AUTOTOOLS 1)
endif()

# Not needed for now
#if(NOT WIN32)
#    set(PATCH_CMD "patch")
#else()
#    # Use one from Git Bash toolchain, otherwise one from MinGW toolchain will crash
#    set(PATCH_CMD "C:/Program Files/Git/usr/bin/patch.exe")
#endif()

# Simple Direct-Media Layer library, dependency of AudioCodecs and SDL Mixer X
if(SDL2_VIA_AUTOTOOLS)
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
            "-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
            -DSDL_SHARED=${PGE_SHARED_SDLMIXER}
            $<$<BOOL:WIN32>:-DWASAPI=OFF>
            $<$<BOOL:WIN32>:-DCMAKE_SHARED_LIBRARY_PREFIX="">
            $<$<BOOL:LINUX>:-DSNDIO=OFF>
            $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DEXTRA_CFLAGS=-s USE_PTHREADS=1>
            $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DPTHREADS=ON>
            $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Emscripten>:-DPTHREADS_SEM=ON>
    )
endif()
