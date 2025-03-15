cmake_minimum_required(VERSION 3.5)

set(VERSION 2.18.0-moondust)

project(imagequant C)

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL ARM64)
  option(BUILD_WITH_SSE "Use SSE" OFF)
else()
  option(BUILD_WITH_SSE "Use SSE" ON)
endif()

find_package(OpenMP)
if(OPENMP_FOUND)
  option(BUILD_WITH_OPENMP "Use OpenMP" OFF)
else()
  set(BUILD_WITH_OPENMP OFF)
endif()

add_library(imagequant_a STATIC
  libimagequant.c
  blur.c
  mediancut.c
  mempool.c
  nearest.c
  pam.c
  kmeans.c
  remap.c
)

if(BUILD_WITH_SSE)
  target_compile_definitions(imagequant_a PRIVATE -DUSE_SSE=1)
endif()

if(BUILD_WITH_OPENMP)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  target_link_libraries(imagequant_a PRIVATE ${OpenMP_C_LIBRARIES})
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  target_compile_options(imagequant_a PRIVATE "-std=c99")
endif()

target_include_directories(imagequant_a PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(imagequant_a PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
