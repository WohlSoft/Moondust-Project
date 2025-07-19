# LZMA library

add_library(PGE_libgnurx INTERFACE)

set_static_lib(libgnurx_A_Lib "${DEPENDENCIES_INSTALL_DIR}" gnurx)
set(GNURX_INCLUDE_DIRS "${DEPENDENCIES_INSTALL_DIR}/include")

ExternalProject_Add(
    gnurx_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/gnurx
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/libgnurx
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
    BUILD_BYPRODUCTS
        "${libgnurx_A_Lib}"
)

message("-- libgnurx will be built: ${libgnurx_A_Lib} --")

target_link_libraries(PGE_libgnurx INTERFACE "${libgnurx_A_Lib}")
target_include_directories(PGE_libgnurx INTERFACE "${GNURX_INCLUDE_DIRS}")
add_dependencies(PGE_libgnurx gnurx_Local)
