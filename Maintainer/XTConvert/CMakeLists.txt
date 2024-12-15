cmake_minimum_required (VERSION 3.5)
project(libxtconvert CXX)

add_subdirectory(3rdparty/mbediso EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/3dstools EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/tex3ds EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/libimagequant EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/spc2it EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/mmutil EXCLUDE_FROM_ALL)
include(cmake/build_libarchive.cmake)
find_package(Qt5 REQUIRED COMPONENTS Core)

qt5_add_resources(LIBXTCONVERT_RES "${CMAKE_CURRENT_SOURCE_DIR}/_resources/libxtconvert.qrc")

add_library(libxtconvert
    libxtconvert.cpp
    graphics_load.cpp
    src/texconv.cpp
    src/export_tpl.cpp
    src/export_audio.cpp
    src/has_no_mask.cpp
    src/extract_archive.cpp
    ${LIBXTCONVERT_RES}
)

add_dependencies(libxtconvert archive_static lz4_pack_static libromfs3ds libtex3ds libmmutil FreeImage_Local PGE_FileFormatsQt libpng_Local SDLMixerX_Local SDL2_Local)

target_link_libraries(libxtconvert PRIVATE Qt5::Core)
target_link_libraries(libxtconvert PUBLIC archive_static lz4_pack_static libromfs3ds libtex3ds libmmutil PGE_FreeImage imagequant_a libspc2it PGE_FileFormatsQt)

if(PGE_STATIC_SDLMIXER)
    target_link_libraries(libxtconvert PUBLIC PGE_SDLMixerX_static PGE_SDL2_static)
else()
    target_link_libraries(libxtconvert PUBLIC PGE_SDLMixerX PGE_SDL2)
endif()

target_include_directories(libxtconvert PUBLIC include)
target_include_directories(libxtconvert PRIVATE src)

project(xtconvert CXX)

include(../../_common/tclap/tclap.cmake)

include_directories(
    ${DEPENDENCIES_INSTALL_DIR}/include
)

add_executable(xtconvert_cli xtconvert_cli.cpp)
set_target_properties(xtconvert_cli PROPERTIES OUTPUT_NAME "xtconvert")
target_link_libraries(xtconvert_cli PRIVATE Qt5::Core)
target_link_libraries(xtconvert_cli PUBLIC libxtconvert)
