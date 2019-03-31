# libPNG is a PNG format library

find_library(LIBPNG_LIBRARY
    NAMES libpng.a libqtlibpng.a png qtlibpng
    PATHS "${CMAKE_BINARY_DIR}/lib/"
)
if(LIBPNG_LIBRARY)
    message("-- Found ${LIBPNG_LIBRARY} --")
    set(libPNG_A_Lib ${LIBPNG_LIBRARY})
    find_library(LIBZLIB_LIBRARY
        NAMES libzlib.a libz.a zlib z
        PATHS "${CMAKE_BINARY_DIR}/lib/"
    )
    message("-- Found ${LIBZLIB_LIBRARY} --")
    set(libZLib_A_Lib ${LIBZLIB_LIBRARY})
else()
    set(libPNG_A_Lib "${CMAKE_BINARY_DIR}/lib/libpng16${PGE_LIBS_DEBUG_SUFFIX}.a")
    set(libZLib_A_Lib "${CMAKE_BINARY_DIR}/lib/libzlib${PGE_LIBS_DEBUG_SUFFIX}.a")

    ExternalProject_Add(
            libpng_Local
            PREFIX ${CMAKE_BINARY_DIR}/external/libPNG
            URL ${CMAKE_SOURCE_DIR}/_Libs/_sources/libpng-1.6.36.tar.gz
            CMAKE_ARGS
                "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
                "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
                "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
                "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
                "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
                "-DPNG_SHARED=OFF"
                "-DPNG_STATIC=ON"
                "-DPNG_TESTS=OFF"
                $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
            BUILD_BYPRODUCTS
                "${libPNG_A_Lib}"
    )
endif()
