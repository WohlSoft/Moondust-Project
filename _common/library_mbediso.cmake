# libmbediso is a compact ISO library made by ds-sloth

add_library(PGE_libmbediso INTERFACE)
add_library(PGE_liblz4 INTERFACE)

set_static_lib(libmbediso_A_Lib "${DEPENDENCIES_INSTALL_DIR}" mbediso)
set_static_lib(liblz4_pack_static_A_Lib "${DEPENDENCIES_INSTALL_DIR}" lz4_pack_static)
set_static_lib(liblz4_A_Lib "${DEPENDENCIES_INSTALL_DIR}" lz4)
set(MBEDISO_INCLUDE_DIRS "${DEPENDENCIES_INSTALL_DIR}/include")
set(LZ3_INCLUDE_DIRS "${DEPENDENCIES_INSTALL_DIR}/include/mbediso/lz4")

ExternalProject_Add(
    libmbediso_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/mbediso
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/mbediso
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
        "${libmbediso_A_Lib}"
        "${liblz4_pack_static_A_Lib}"
        "${liblz4_A_Lib}"
)

message("-- libmbediso will be built: ${libmbediso_A_Lib} --")

target_link_libraries(PGE_libmbediso INTERFACE "${libmbediso_A_Lib}")
target_include_directories(PGE_libmbediso INTERFACE "${MBEDISO_INCLUDE_DIRS}")

target_link_libraries(PGE_liblz4 INTERFACE "${liblz4_pack_static_A_Lib}" "${liblz4_A_Lib}")
target_include_directories(PGE_liblz4 INTERFACE "${LZ3_INCLUDE_DIRS}")
add_dependencies(PGE_liblz4 libmbediso_Local)
