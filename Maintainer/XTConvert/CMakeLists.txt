cmake_minimum_required (VERSION 3.5)
project(libxtconvert CXX)

add_subdirectory(3rdparty/mbediso EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/3dstools EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/tex3ds EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/libimagequant EXCLUDE_FROM_ALL)
include(cmake/build_libarchive.cmake)
find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(libxtconvert libxtconvert.cpp graphics_load.cpp src/texconv.cpp)

add_dependencies(libxtconvert archive_static lz4_pack_static libromfs3ds libtex3ds FreeImage_Local libpng_Local)

target_link_libraries(libxtconvert PRIVATE Qt6::Core)
target_link_libraries(libxtconvert PUBLIC archive_static lz4_pack_static libromfs3ds libtex3ds PGE_FreeImage imagequant_a)

target_include_directories(libxtconvert PUBLIC include)
target_include_directories(libxtconvert PRIVATE src)

project(xtconvert CXX)

add_executable(xtconvert_cli xtconvert_cli.cpp)
target_link_libraries(xtconvert_cli PRIVATE Qt6::Core)
target_link_libraries(xtconvert_cli PUBLIC libxtconvert)
