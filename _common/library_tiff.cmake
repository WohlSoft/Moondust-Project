# Build of the minimal libTIFF library as a workaround

add_library(PGE_libtiff INTERFACE)

set_static_lib(libtiff_A_Lib "${DEPENDENCIES_INSTALL_DIR}" tiff)
set(TIFF_INCLUDE_DIRS "${DEPENDENCIES_INSTALL_DIR}/include")

ExternalProject_Add(
    tiff_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/libtiff
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/libtiff
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
        "-Djbig=OFF"
        "-Djpeg=OFF"
        "-Djpeg12=OFF"
        "-Dlzma=OFF"
        "-Dsphinx=OFF"
        "-Dsphinx-linkcheck=OFF"
        "-Dtiff-tools=OFF"
        "-Dwebp=OFF"
        "-Dwin32-io=OFF"
        "-Dzlib=OFF"
        "-Dzstd=OFF"
        ${ANDROID_CMAKE_FLAGS}
        ${APPLE_CMAKE_FLAGS}
    BUILD_BYPRODUCTS
        "${libtiff_A_Lib}"
)

message("-- libtiff will be built: ${libtiff_A_Lib} --")

target_link_libraries(PGE_libtiff INTERFACE "${libtiff_A_Lib}")
target_include_directories(PGE_libtiff INTERFACE "${TIFF_INCLUDE_DIRS}")
add_dependencies(PGE_libtiff tiff_Local)
