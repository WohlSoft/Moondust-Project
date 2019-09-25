# libPNG is a PNG format library

option(USE_SYSTEM_LIBPNG "Use libPNG and ZLib from the system" OFF)

add_library(PGE_libPNG INTERFACE)

if(USE_SYSTEM_LIBPNG)
    add_library(libpng_Local INTERFACE)

    find_package(PNG)
    message("-- Found libPNG: ${PNG_LIBRARIES} --")
    target_link_libraries(PGE_libPNG INTERFACE "${PNG_LIBRARIES}")
    target_include_directories(PGE_libPNG INTERFACE "${PNG_INCLUDE_DIRS}")
    target_compile_definitions(PGE_libPNG INTERFACE "${PNG_DEFINITIONS}")

    set(libPNG_A_Lib "${PNG_LIBRARIES}")

else()
    set(libPNG_A_Lib "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}png16${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")

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
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
            ${ANDROID_CMAKE_FLAGS}
            "-DPNG_SHARED=OFF"
            "-DPNG_STATIC=ON"
            "-DPNG_BUILD_ZLIB=ON"
            "-DZLIB_INCLUDE_DIR=${DEPENDENCIES_INSTALL_DIR}/include"
            "-DZLIB_LIBRARY=${libZLib_A_Lib}"
            $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
            "-DPNG_TESTS=OFF"
        BUILD_BYPRODUCTS
            "${libPNG_A_Lib}"
    )

    message("-- libPNG will be built: ${libPNG_A_Lib} --")

    target_link_libraries(PGE_libPNG INTERFACE "${libPNG_A_Lib}")
endif()
