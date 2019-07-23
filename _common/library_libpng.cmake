# libPNG is a PNG format library

option(USE_SYSTEM_LIBPNG "Use libPNG and ZLib from the system" OFF)
if(USE_SYSTEM_LIBPNG)
    find_library(LIBPNG_LIBRARY
        NAMES libpng.a png
        PATHS "${CMAKE_BINARY_DIR}/lib/"
    )
endif()

if(USE_SYSTEM_LIBPNG AND LIBPNG_LIBRARY)
    message("-- Found ${LIBPNG_LIBRARY} --")
    set(libPNG_A_Lib ${LIBPNG_LIBRARY})
    find_library(LIBZLIB_LIBRARY
        NAMES libzlib.a libz.a zlib z
        PATHS "${CMAKE_BINARY_DIR}/lib/"
    )
    if(LIBZLIB_LIBRARY)
        message("-- Found ${LIBZLIB_LIBRARY} --")
        set(libZLib_A_Lib ${LIBZLIB_LIBRARY})
    else() # Use that built with separately
        set(libZLib_A_Lib "${CMAKE_BINARY_DIR}/lib/libzlib${PGE_LIBS_DEBUG_SUFFIX}.a")
    endif()
else()
    set(libPNG_A_Lib "${CMAKE_BINARY_DIR}/lib/libpng16${PGE_LIBS_DEBUG_SUFFIX}.a")
    set(libZLib_A_Lib "${CMAKE_BINARY_DIR}/lib/libzlib${PGE_LIBS_DEBUG_SUFFIX}.a")

    set(libpngArchive ${CMAKE_SOURCE_DIR}/_Libs/_sources/libpng-1.6.36.tar.gz)
    file(SHA256 ${libpngArchive} libpngArchive_hash)

    ExternalProject_Add(
        libpng_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/libPNG
        URL ${libpngArchive}
        URL_HASH SHA256=${libpngArchive_hash}
        CMAKE_ARGS
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
            ${ANDROID_CMAKE_FLAGS}
            "-DPNG_SHARED=OFF"
            "-DPNG_STATIC=ON"
            $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
            "-DPNG_TESTS=OFF"
        BUILD_BYPRODUCTS
            "${libPNG_A_Lib}"
    )
endif()
