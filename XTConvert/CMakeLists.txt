cmake_minimum_required (VERSION 3.5...3.10)
project(libxtconvert CXX C)

if(WIN32 AND "${TARGET_PROCESSOR}" STREQUAL "i386")
    set(BUILD_WITH_SSE OFF CACHE BOOL "" FORCE)
endif()

if(POLICY CMP0071) # Automoc
    cmake_policy(SET CMP0071 NEW)
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_INSTALL_RPATH ".")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPENDENCIES_INSTALL_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../_common/ # PGE-FL
)
link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

# Default GIT version
include(../_common/git_version.cmake)
# Common building properties and optimization flags
include(../_common/build_props.cmake)
# Build properties for executable projects
include(../_common/executable_build_props.cmake)
# Find extra libraries to link when using static Qt (use ${QT_EXTRA_LIBS}!)
include(../_common/static_qt.cmake)

include(../_common/qt-modules/compat/pge_qt_compat.cmake)
include(../_common/Utf8Main/utf8main.cmake)
include(../_common/tclap/tclap.cmake)

if(PGE_USE_QT5)
    pge_cxx_standard(14)
else()
    pge_cxx_standard(17)
endif()

set(QT_LIBS_LIST
    ${QT_Core_LIBRARIES}
)

set(QT_DEFS_LIST
    ${QT_Core_DEFINITIONS}
)

set(QT_INCLUDES_LIST
    ${QT_Core_INCLUDE_DIRS}
)

set(QT_EXECUTABLE_CXX_FLAGS
    ${QT_Core_EXECUTABLE_COMPILE_FLAGS}
)

set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS})
set(ZLIB_LIBRARY ${libZLib_A_Lib})

qt5_add_resources(LIBXTCONVERT_RES "${CMAKE_CURRENT_SOURCE_DIR}/_resources/libxtconvert.qrc")

add_library(libxtconvert
    src/libxtconvert.cpp
    src/graphics_load.cpp src/graphics_load.h
    src/texconv.cpp src/texconv.h
    src/export_tpl.cpp src/export_tpl.h
    src/export_audio.cpp src/export_audio.h
    src/has_no_mask.cpp src/has_no_mask.h
    src/extract_archive.cpp src/extract_archive.h
    src/misc.cpp src/misc.h
    ${LIBXTCONVERT_RES}
    src/json.hpp src/qoa.h
)

add_dependencies(libxtconvert
    PGE_libarchive
    PGE_liblz4pack
    PGE_liblz4
    PGE_lib3dstools
    PGE_libtex3ds
    PGE_libmmutil
    FreeImage_Local
    PGE_libimagequant
    PGE_libspc2it
    PGE_FileFormatsQt
    SDLMixerX_Local
    SDL2_Local
)

add_library(PGE_XTConvert_QtLibs INTERFACE)

target_include_directories(PGE_XTConvert_QtLibs
    INTERFACE
    ${QT_INCLUDES_LIST}
)

target_compile_definitions(PGE_XTConvert_QtLibs
    INTERFACE
    ${QT_DEFS_LIST}
    -DPGE_FILES_QT
)

target_link_libraries(PGE_XTConvert_QtLibs
    INTERFACE
    ${QT_EXTRA_LIBS_PRE}
    PGE_libarchive
    PGE_liblz4pack
    PGE_liblz4
    PGE_lib3dstools
    PGE_libtex3ds
    PGE_libmmutil
    PGE_FreeImage
    PGE_libimagequant
    PGE_libspc2it
    PGE_FileFormatsQt
    ${QT_LIBS_LIST}
    ${QT_EXTRA_LIBS}
)

if(QT_PTHREAD)
    target_link_libraries(PGE_XTConvert_QtLibs PRIVATE ${QT_PTHREAD})
endif()

set(PGE_XTCONVERT_LINK_LIBS)

if(WIN32)
    list(APPEND PGE_XTCONVERT_LINK_LIBS
        version
    )
else()
    if(UNIX AND NOT APPLE AND NOT HAIKU)
        list(APPEND PGE_XTCONVERT_LINK_LIBS dl)
    endif()
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32 AND NOT PGE_ENABLE_STATIC_QT)
        list(APPEND PGE_XTCONVERT_LINK_LIBS "-static")
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND PGE_XTCONVERT_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

if(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND PGE_XTCONVERT_LINK_LIBS ${COREFOUNDATION_LIBRARY})
endif()

target_link_libraries(libxtconvert PRIVATE PGE_XTConvert_QtLibs)

if(PGE_STATIC_SDLMIXER)
    target_link_libraries(libxtconvert PUBLIC PGE_SDLMixerX_static PGE_SDL2_static)
else()
    target_link_libraries(libxtconvert PUBLIC PGE_SDLMixerX PGE_SDL2)
endif()

target_link_libraries(libxtconvert PRIVATE ${PGE_XTCONVERT_LINK_LIBS})

target_include_directories(libxtconvert PUBLIC include)
target_include_directories(libxtconvert PRIVATE src)
set_target_properties(libxtconvert PROPERTIES OUTPUT_NAME "xtconvert")

# XTConvert cli
project(xtconvert_cli CXX)

include_directories(
    ${DEPENDENCIES_INSTALL_DIR}/include
)

if(QT_IMPORT_PLUGINS_MODULE)
    set(QT_PLUGINS_CPP "${CMAKE_BINARY_DIR}/pge_maintainer_plugins.cpp")
    file(WRITE
            "${QT_PLUGINS_CPP}"
            "${QT_IMPORT_PLUGINS_MODULE}")
endif()

# Executables fail to build with Qt 5/6 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QT_EXECUTABLE_CXX_FLAGS}")

add_executable(xtconvert_cli
    src/xtconvert_cli.cpp
    ${UTF8MAIN_SRCS}
    ${QT_PLUGINS_CPP}
)
pge_set_nopie(xtconvert_cli)

set_target_properties(xtconvert_cli PROPERTIES OUTPUT_NAME "xtconvert")
if(WIN32)
    set_target_properties(xtconvert_cli PROPERTIES WIN32_EXECUTABLE OFF)
    if(MSVC)
        target_compile_definitions(xtconvert_cli PRIVATE -DWIN32_CONSOLE)
    endif()
endif()

target_link_libraries(xtconvert_cli PRIVATE libxtconvert PGE_XTConvert_QtLibs ${PGE_XTCONVERT_LINK_LIBS})

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
    target_compile_definitions(libxtconvert PRIVATE -DDEBUG_BUILD)
    target_compile_definitions(xtconvert_cli PRIVATE -DDEBUG_BUILD)
endif()

install(TARGETS xtconvert_cli
    RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}"
)
