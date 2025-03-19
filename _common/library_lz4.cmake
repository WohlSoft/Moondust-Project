# LZ4 library that implements an extremely fast compression algorithm

add_library(PGE_liblz4 INTERFACE)

set_static_lib(liblz4_A_Lib "${DEPENDENCIES_INSTALL_DIR}" lz4)
set(LZ4_INCLUDE_DIRS "${DEPENDENCIES_INSTALL_DIR}/include")

ExternalProject_Add(
    lz4_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/lz4
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/lz4/build/cmake
    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
        "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        "-DDISABLE_FORCE_DEBUG_POSTFIX=ON"
        "-DCMAKE_DEBUG_POSTFIX=${PGE_LIBS_DEBUG_SUFFIX}"
        "-DLZ4_BUNDLED_MODE=OFF"
        "-DBUILD_SHARED_LIBS=OFF"
        ${ANDROID_CMAKE_FLAGS}
        ${APPLE_CMAKE_FLAGS}
    BUILD_BYPRODUCTS
        "${liblz4_A_Lib}"
)

message("-- liblz4 will be built: ${liblz4_A_Lib} --")

target_link_libraries(PGE_liblz4 INTERFACE "${liblz4_A_Lib}")
target_include_directories(PGE_liblz4 INTERFACE "${LZ4_INCLUDE_DIRS}")
add_dependencies(PGE_liblz4 lz4_Local lz4_extra_heads)
