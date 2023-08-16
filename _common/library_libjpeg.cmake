# libJPEG is a JPEG format library

option(USE_SYSTEM_LIBJPEG "Use libJPEG from the system" OFF)

add_library(PGE_libJPEG INTERFACE)

if(USE_SYSTEM_LIBJPEG)
    add_library(libJPEG_Local INTERFACE)

    find_package(JPEG)
    message("-- Found libJPEG: ${JPEG_LIBRARIES} --")
    target_link_libraries(PGE_libJPEG INTERFACE "${JPEG_LIBRARIES}")
    target_include_directories(PGE_libJPEG INTERFACE "${JPEG_INCLUDE_DIRS}")
    target_compile_definitions(PGE_libJPEG INTERFACE "${JPEG_DEFINITIONS}")

    set(libJPEG_A_Lib "${JPEG_LIBRARIES}")

else()
    if(NOT MSVC)
        set(JPEG_STATICLIB_NAME "jpeg")
    else()
        set(JPEG_STATICLIB_NAME "libjpeg_static")
    endif()
    set(libJPEG_A_Lib "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${JPEG_STATICLIB_NAME}${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(JPEG_INCLUDE_DIRS "${DEPENDENCIES_INSTALL_DIR}/include")

    ExternalProject_Add(
        libJPEG_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/libJPEG
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/libjpeg
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
            "-DJPEG_SHARED=OFF"
            "-DJPEG_STATIC=ON"
            "-DJPEG_BUILD_EXECUTABLES=OFF"
        BUILD_BYPRODUCTS
            "${libJPEG_A_Lib}"
    )

    message("-- libJPEG will be built: ${libJPEG_A_Lib} --")

    target_link_libraries(PGE_libJPEG INTERFACE "${libJPEG_A_Lib}")
endif()
