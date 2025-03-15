cmake_minimum_required(VERSION 3.2...3.5)

include(TestBigEndian)

if(POLICY CMP0069) # Allow CMAKE_INTERPROCEDURAL_OPTIMIZATION (lto) to be set
    cmake_policy(SET CMP0069 NEW)
endif()

project(mbediso VERSION 0.0 LANGUAGES C)

set(MBEDISO_SRC
    src/internal/directory.c
    src/internal/fs.c
    src/internal/io.c
    src/internal/read.c
    src/internal/string_diff.c
    src/internal/util.c
    src/internal/lz4_header.c
    src/public/file.c
    src/public/dir.c
    src/public/fs.c
)

add_subdirectory(lz4/build/cmake EXCLUDE_FROM_ALL)

add_library(mbediso STATIC ${MBEDISO_SRC})

test_big_endian(MBEDISO_BIG_ENDIAN)
if(MBEDISO_BIG_ENDIAN)
    target_compile_definitions(mbediso PRIVATE -DMBEDISO_BIG_ENDIAN=1)
else()
    target_compile_definitions(mbediso PRIVATE -DMBEDISO_BIG_ENDIAN=0)
endif()

target_link_libraries(mbediso PRIVATE lz4_static)

target_include_directories(mbediso PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(mbediso PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS mbediso
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib"
        INCLUDES DESTINATION "include")

if(NOT MSVC)
    target_compile_options(mbediso PRIVATE -Wall -Wextra -pedantic)
endif()


# example utility to allow user to read from an ISO
add_executable(read examples/main.c)
target_link_libraries(read PRIVATE mbediso)


# lz4 pack command line util
project(lz4_pack VERSION 0.0 LANGUAGES CXX)

add_library(lz4_pack_static util/lz4_pack/lz4_pack.cpp)
target_include_directories(lz4_pack_static PUBLIC util/lz4_pack/include)
target_link_libraries(lz4_pack_static PUBLIC lz4_static)

add_executable(lz4_pack_cli util/lz4_pack/main.cpp)
target_link_libraries(lz4_pack_cli PRIVATE lz4_pack_static)
