project(libMoondustEpisodeBox CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_INSTALL_RPATH ".")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPENDENCIES_INSTALL_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ # PGE-FL
)

link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

set(LIBRARY_PROJECT 1)

# Common building properties and optimization flags
include(../../build_props.cmake)

if(PGE_USE_QT5)
    pge_cxx_standard(14)
else()
    pge_cxx_standard(17)
endif()

set(QT_DEFS_LIST ${QT_Core_DEFINITIONS})
set(QT_INCLUDES_LIST ${QT_Core_INCLUDE_DIRS})

add_library(episode_box STATIC
    episode_box.h
    episode_box.cpp
)

add_dependencies(episode_box pgefl_qt)

target_compile_definitions(episode_box PRIVATE ${QT_DEFS_LIST})
target_compile_definitions(episode_box PUBLIC -DPGE_FILES_QT)
target_include_directories(episode_box PRIVATE ${QT_INCLUDES_LIST})
target_include_directories(episode_box PUBLIC ${CMAKE_CURRENT_LIST_DIR})
