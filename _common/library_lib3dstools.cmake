# Library that can pack the 3DS-compatible ROMFS images

add_library(PGE_lib3dstools INTERFACE)

set_static_lib(lib3dstools_A_Lib "${DEPENDENCIES_INSTALL_DIR}" romfs3ds)

ExternalProject_Add(
    lib3dstools_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/3dstools
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/3dstools
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
        "${lib3dstools_A_Lib}"
)

message("-- lib3dstools will be built: ${lib3dstools_A_Lib} --")

target_link_libraries(PGE_lib3dstools INTERFACE "${lib3dstools_A_Lib}")
target_include_directories(PGE_lib3dstools INTERFACE "${DEPENDENCIES_INSTALL_DIR}/include")
