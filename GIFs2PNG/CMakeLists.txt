cmake_minimum_required (VERSION 3.2)
project(MoondustGIFs2PNG C CXX)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPENDENCIES_INSTALL_DIR}/include
)
link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message("== Using default build configuration which is a Release!")
endif()

set(CMAKE_CXX_STANDARD 14)
if(MSVC)
    if(MSVC_VERSION GREATER_EQUAL "1900")
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
        if (_cpp_latest_flag_supported)
            add_compile_options("/std:c++latest")
        endif()
    endif()
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

set(GIFs2PNG_SRCS)

list(APPEND GIFs2PNG_SRCS
    gifs2png.cpp
    common_features/config_manager.cpp
)

if(WIN32 AND NOT EMSCRIPTEN)
    list(APPEND GIFs2PNG_SRCS
        _resources/gifs2png.rc
    )
endif()

add_executable(GIFs2PNG
    ${GIFs2PNG_SRCS}
    ${DIRMANAGER_SRCS}
    ${FILEMAPPER_SRCS}
    ${INIPROCESSOR_SRCS}
    ${UTF8MAIN_SRCS}
    ${UTILS_SRCS}
)

if(WIN32 AND NOT EMSCRIPTEN)
    target_compile_definitions(GIFs2PNG
        PUBLIC
        -DUNICODE
        -D_UNICODE
        -DNOMINMAX
    )
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
    target_compile_definitions(GIFs2PNG
        PRIVATE
        -DDEBUG_BUILD
    )
endif()

add_dependencies(GIFs2PNG
    FreeImage_Local
)

set(GIFs2PNG_LINK_LIBS)
set(GIFs2PNG_INSTALLS)

list(APPEND GIFs2PNG_LINK_LIBS
    ${UTILS_LIBS}
    PGE_FreeImage
)

list(APPEND GIFs2PNG_INSTALLS
    GIFs2PNG
)

if(NOT APPLE)
    set_target_properties(GIFs2PNG PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(GIFs2PNG PROPERTIES LINK_FLAGS "-mconsole")
        list(APPEND GIFs2PNG_LINK_LIBS "-static")
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND LazyFixTool_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

target_link_libraries(GIFs2PNG
    ${GIFs2PNG_LINK_LIBS}
)

pge_set_nopie(GIFs2PNG)

install(TARGETS ${GIFs2PNG_INSTALLS}
        RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}/"
)
