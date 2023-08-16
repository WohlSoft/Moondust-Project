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
    if(NOT MSVC)
        set(PNG_STATICLIB_NAME "png16")
    else()
        set(PNG_STATICLIB_NAME "libpng16_static")
    endif()
    set(libPNG_A_Lib "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${PNG_STATICLIB_NAME}${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(PNG_INCLUDE_DIRS "${DEPENDENCIES_INSTALL_DIR}/include")

    ExternalProject_Add(
        libpng_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/libPNG
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/libpng
        CMAKE_ARGS
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
            "-DDISABLE_FORCE_DEBUG_POSTFIX=ON"
            "-DCMAKE_DEBUG_POSTFIX=${PGE_LIBS_DEBUG_SUFFIX}"
            ${ANDROID_CMAKE_FLAGS}
            ${APPLE_CMAKE_FLAGS}
            "-DPNG_SHARED=OFF"
            "-DPNG_STATIC=ON"
            "-DPNG_BUILD_ZLIB=ON"
            "-DZLIB_INCLUDE_DIR=${DEPENDENCIES_INSTALL_DIR}/include"
            "-DZLIB_LIBRARY=${libZLib_A_Lib}"
            "-DPNG_TESTS=OFF"
        BUILD_BYPRODUCTS
            "${libPNG_A_Lib}"
    )

    message("-- libPNG will be built: ${libPNG_A_Lib} --")

    target_link_libraries(PGE_libPNG INTERFACE "${libPNG_A_Lib}")
endif()
