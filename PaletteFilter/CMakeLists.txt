cmake_minimum_required (VERSION 3.2)
project(MoondustPaletteFilter C CXX)

set(CMAKE_INSTALL_RPATH ".")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPENDENCIES_INSTALL_DIR}/include
)
link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message("== Using default build configuration which is a Release!")
endif()

# Version
include(version.cmake)
# Default GIT version
include(../_common/git_version.cmake)
# Common building properties and optimization flags
include(../_common/build_props.cmake)
# Build properties for executable projects
include(../_common/executable_build_props.cmake)

include(../_common/tclap/tclap.cmake)
include(../_common/DirManager/dirman.cmake)
include(../_common/Utils/Utils.cmake)
include(../_common/FileMapper/FileMapper.cmake)
include(../_common/Utf8Main/utf8main.cmake)
include(../_common/IniProcessor/IniProcessor.cmake)

pge_cxx_standard(14)

set(PaletteFilter_SRCS)

list(APPEND PaletteFilter_SRCS
    PaletteFilter.cpp
)

if(WIN32 AND NOT EMSCRIPTEN)
    list(APPEND PaletteFilter_SRCS
        _resources/pallete_filter.rc
    )
endif()

add_executable(PaletteFilter
    ${PaletteFilter_SRCS}
    ${DIRMANAGER_SRCS}
    ${FILEMAPPER_SRCS}
    ${INIPROCESSOR_SRCS}
    ${UTF8MAIN_SRCS}
    ${UTILS_SRCS}
)

if(WIN32 AND NOT EMSCRIPTEN)
    target_compile_definitions(PaletteFilter
        PUBLIC
        -DUNICODE
        -D_UNICODE
        -DNOMINMAX
    )
    if(MSVC)
        target_compile_definitions(PaletteFilter PRIVATE -DWIN32_CONSOLE)
    endif()
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
    target_compile_definitions(PaletteFilter
        PRIVATE
        -DDEBUG_BUILD
    )
endif()

add_dependencies(PaletteFilter
    FreeImage_Local
    libpng_Local
)

set(PaletteFilter_LINK_LIBS)
set(PaletteFilter_INSTALLS)

list(APPEND PaletteFilter_LINK_LIBS
    ${UTILS_LIBS}
    PGE_FreeImage
)

list(APPEND PaletteFilter_INSTALLS
    PaletteFilter
)

if(NOT APPLE AND NOT MSVC AND NOT USE_SYSTEM_LIBC)
    set_property(TARGET PaletteFilter APPEND_STRING PROPERTY LINK_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(PaletteFilter PROPERTIES WIN32_EXECUTABLE OFF)
        list(APPEND PaletteFilter_LINK_LIBS "-static")
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND PaletteFilter_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

target_link_libraries(PaletteFilter
    ${PaletteFilter_LINK_LIBS}
)

pge_set_nopie(PaletteFilter)

install(TARGETS ${PaletteFilter_INSTALLS}
        RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}/"
)
