# LZMA library

add_library(PGE_liblzma INTERFACE)

set_static_lib(liblzma_A_Lib "${DEPENDENCIES_INSTALL_DIR}" lzma)
set(LZMA_INCLUDE_DIRS "${DEPENDENCIES_INSTALL_DIR}/include")

ExternalProject_Add(
    lzma_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/lzma
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/libxz
    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
        "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        "-DDISABLE_FORCE_DEBUG_POSTFIX=ON"
        "-DCMAKE_DEBUG_POSTFIX=${PGE_LIBS_DEBUG_SUFFIX}"
        "-DBUILD_SHARED_LIBS=OFF"
        "-DBUILD_TESTING=OFF"
        "-DXZ_TOOL_LZMADEC=OFF"
        "-DXZ_TOOL_LZMAINFO=OFF"
        "-DXZ_TOOL_SCRIPTS=OFF"
        "-DXZ_TOOL_SYMLINKS=OFF"
        "-DXZ_TOOL_SYMLINKS_LZMA=OFF"
        "-DXZ_TOOL_XZ=OFF"
        "-DXZ_TOOL_XZDEC=OFF"
        ${ANDROID_CMAKE_FLAGS}
        ${APPLE_CMAKE_FLAGS}
    BUILD_BYPRODUCTS
        "${liblzma_A_Lib}"
)

message("-- liblzma will be built: ${liblzma_A_Lib} --")

target_link_libraries(PGE_liblzma INTERFACE "${liblzma_A_Lib}")
target_include_directories(PGE_liblzma INTERFACE "${LZMA_INCLUDE_DIRS}")
add_dependencies(PGE_liblzma lzma_Local)
