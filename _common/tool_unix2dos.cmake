
if(WIN32)
    # UNIX to DOS text format converter utility. Is used to deploy text files on Windows.
    ExternalProject_Add(
        Unix2Dos_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/Unix2Dos
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/Unix2Dos
        CMAKE_ARGS
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/unix2dos_copy.cmake)
