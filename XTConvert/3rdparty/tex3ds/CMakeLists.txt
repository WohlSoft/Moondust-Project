cmake_minimum_required(VERSION 3.2...3.5)

if(POLICY CMP0069) # Allow CMAKE_INTERPROCEDURAL_OPTIMIZATION (lto) to be set
    cmake_policy(SET CMP0069 NEW)
endif()

project(libtex3ds VERSION 0.0 LANGUAGES CXX)

set(LIBTEX3DS_SRC
    include/compress.h
    include/encode.h
    include/future.h
    include/quantum.h
    include/rg_etc1.h
    include/subimage.h
    include/swizzle.h
    public/libtex3ds.h
    source/encode.cpp
    source/huff.cpp
    source/libtex3ds.cpp
    source/lzss.cpp
    source/rg_etc1.cpp
    source/rle.cpp
    source/swizzle.cpp
)

add_library(libtex3ds STATIC ${LIBTEX3DS_SRC})

target_include_directories(libtex3ds PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(libtex3ds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)
