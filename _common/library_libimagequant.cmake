# Library that implements the lossy PNG compression

add_library(PGE_libimagequant INTERFACE)

set_static_lib(libimagequant_A_Lib "${DEPENDENCIES_INSTALL_DIR}" imagequant_a)

if(WIN32 AND "${TARGET_PROCESSOR}" STREQUAL "i386")
    set(IMAGEQUANT_NO_SSE_OPTION "-DBUILD_WITH_SSE=OFF")
endif()

ExternalProject_Add(
    imagequant_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/libimagequant
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/libimagequant
    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
        "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        "-DDISABLE_FORCE_DEBUG_POSTFIX=ON"
        "-DCMAKE_DEBUG_POSTFIX=${PGE_LIBS_DEBUG_SUFFIX}"
        "-DBUILD_WITH_SSE=OFF"
        ${ANDROID_CMAKE_FLAGS}
        ${APPLE_CMAKE_FLAGS}
        ${IMAGEQUANT_NO_SSE_OPTION}
    BUILD_BYPRODUCTS
        "${libimagequant_A_Lib}"
)

message("-- libimagequant will be built: ${libimagequant_A_Lib} --")

target_link_libraries(PGE_libimagequant INTERFACE "${libimagequant_A_Lib}")
target_include_directories(PGE_libimagequant INTERFACE "${DEPENDENCIES_INSTALL_DIR}")
add_dependencies(PGE_libimagequant imagequant_Local)
