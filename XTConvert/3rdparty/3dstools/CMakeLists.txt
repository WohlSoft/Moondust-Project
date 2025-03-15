cmake_minimum_required(VERSION 3.2...3.5)

if(POLICY CMP0069) # Allow CMAKE_INTERPROCEDURAL_OPTIMIZATION (lto) to be set
    cmake_policy(SET CMP0069 NEW)
endif()

project(libromfs3ds VERSION 0.0 LANGUAGES CXX)

set(LIBROMFS3DS_SRC
    include/libromfs3ds.h
    src/libromfs3ds.cpp
    src/types.h
    src/FileClass.h
    src/romfs.cpp
    src/romfs.h
)

add_library(libromfs3ds STATIC ${LIBROMFS3DS_SRC})

target_include_directories(libromfs3ds PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(libromfs3ds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
