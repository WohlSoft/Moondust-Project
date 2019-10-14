cmake_minimum_required (VERSION 3.2)
project(MoondustLazyFixTool C CXX)

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

set(LazyFixTool_SRCS)

list(APPEND LazyFixTool_SRCS
    LazyFixTool.cpp
)

if(WIN32 AND NOT EMSCRIPTEN)
    list(APPEND LazyFixTool_SRCS
        _resources/lazyfix_tool.rc
    )
endif()

add_executable(LazyFixTool
    ${LazyFixTool_SRCS}
    ${DIRMANAGER_SRCS}
    ${FILEMAPPER_SRCS}
    ${INIPROCESSOR_SRCS}
    ${UTF8MAIN_SRCS}
    ${UTILS_SRCS}
)

if(WIN32 AND NOT EMSCRIPTEN)
    target_compile_definitions(LazyFixTool
        PUBLIC
        -DUNICODE
        -D_UNICODE
        -DNOMINMAX
    )
    if(MSVC)
        target_compile_definitions(LazyFixTool PRIVATE -DWIN32_CONSOLE)
    endif()
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
    target_compile_definitions(LazyFixTool
        PRIVATE
        -DDEBUG_BUILD
    )
endif()

add_dependencies(LazyFixTool
    FreeImage_Local
    libpng_Local
)

set(LazyFixTool_LINK_LIBS)
set(LazyFixTool_INSTALLS)

list(APPEND LazyFixTool_LINK_LIBS
    ${UTILS_LIBS}
    PGE_FreeImage
)

list(APPEND LazyFixTool_INSTALLS
    LazyFixTool
)

if(NOT APPLE AND NOT MSVC AND NOT USE_SYSTEM_LIBC)
    set_property(TARGET LazyFixTool APPEND_STRING PROPERTY LINK_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(LazyFixTool PROPERTIES WIN32_EXECUTABLE OFF)
        list(APPEND LazyFixTool_LINK_LIBS "-static")
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND LazyFixTool_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

target_link_libraries(LazyFixTool
    ${LazyFixTool_LINK_LIBS}
)

pge_set_nopie(LazyFixTool)

install(TARGETS ${LazyFixTool_INSTALLS}
        RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}/"
)
