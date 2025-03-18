# Library that can convert SPC700 files into Impulse Tracker music files

add_library(PGE_libspc2it INTERFACE)

set_static_lib(libspc2it_A_Lib "${DEPENDENCIES_INSTALL_DIR}" spc2it)

ExternalProject_Add(
    spc2it_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/spc2it
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/spc2it
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
        "${libspc2it_A_Lib}"
)

message("-- libspc2it will be built: ${libspc2it_A_Lib} --")

target_link_libraries(PGE_libspc2it INTERFACE "${libspc2it_A_Lib}")
target_include_directories(PGE_libspc2it INTERFACE "${DEPENDENCIES_INSTALL_DIR}/include")
add_dependencies(PGE_libspc2it spc2it_Local)
