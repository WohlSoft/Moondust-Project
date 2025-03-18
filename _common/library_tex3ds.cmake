# Library that can pack the 3DS-compatible ROMFS images

add_library(PGE_libtex3ds INTERFACE)

set_static_lib(libtex3ds_A_Lib "${DEPENDENCIES_INSTALL_DIR}" tex3ds)

ExternalProject_Add(
    tex3ds_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/tex3ds
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/tex3ds
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
        "${libtex3ds_A_Lib}"
)

message("-- libtex3ds will be built: ${libtex3ds_A_Lib} --")

target_link_libraries(PGE_libtex3ds INTERFACE "${libtex3ds_A_Lib}")
target_include_directories(PGE_libtex3ds INTERFACE "${DEPENDENCIES_INSTALL_DIR}/include")
add_dependencies(PGE_libtex3ds tex3ds_Local)
