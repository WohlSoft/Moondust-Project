# Library that can convert some PCM formats

add_library(PGE_libmmutil INTERFACE)

set_static_lib(libmmutil_A_Lib "${DEPENDENCIES_INSTALL_DIR}" mmutil)

ExternalProject_Add(
    mmutil_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/mmutil
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/mmutil
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
        "${libmmutil_A_Lib}"
)

message("-- libmmutil will be built: ${libmmutil_A_Lib} --")

target_link_libraries(PGE_libmmutil INTERFACE "${libmmutil_A_Lib}")
target_include_directories(PGE_libmmutil INTERFACE "${DEPENDENCIES_INSTALL_DIR}/include")
add_dependencies(PGE_libmmutil mmutil_Local)
