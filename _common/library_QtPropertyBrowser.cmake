
# Qt Property Browser

if(PGE_ENABLE_QT) # Build if Qt support enabled
    ExternalProject_Add(
            QtPropertyBrowser_Local
            PREFIX ${CMAKE_BINARY_DIR}/external/QtPropertyBrowser
            DOWNLOAD_COMMAND ""
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/QtPropertyBrowser
            CMAKE_ARGS
            "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_DEBUG_POSTFIX=d"
            "-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
            "-DBUILD_EXAMPLES=OFF"
    )
endif()
