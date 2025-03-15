cmake_minimum_required (VERSION 3.5)
project(libxtconvert CXX)

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
include(../../_common/git_version.cmake)
# Common building properties and optimization flags
include(../../_common/build_props.cmake)
# Build properties for executable projects
include(../../_common/executable_build_props.cmake)
# Find extra libraries to link when using static Qt (use ${QT_EXTRA_LIBS}!)
include(../../_common/static_qt.cmake)

include(../../_common/qt-modules/compat/pge_qt_compat.cmake)
include(../../_common/Utf8Main/utf8main.cmake)

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

add_subdirectory(3rdparty/mbediso EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/3dstools EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/tex3ds EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/libimagequant EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/spc2it EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/mmutil EXCLUDE_FROM_ALL)

set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS})
set(ZLIB_LIBRARY ${libZLib_A_Lib})

include(cmake/build_libarchive.cmake)

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

add_dependencies(archive_static PGE_AudioCodecs)
add_dependencies(libxtconvert archive_static lz4_pack_static libromfs3ds libtex3ds libmmutil FreeImage_Local PGE_FileFormatsQt SDLMixerX_Local SDL2_Local)

target_include_directories(libxtconvert PRIVATE ${QT_INCLUDES_LIST})
target_compile_definitions(libxtconvert PRIVATE ${QT_DEFS_LIST})
target_link_libraries(libxtconvert PRIVATE ${QT_EXTRA_LIBS_PRE})
target_link_libraries(libxtconvert PUBLIC archive_static lz4_pack_static libromfs3ds libtex3ds libmmutil PGE_FreeImage imagequant_a libspc2it PGE_FileFormatsQt)
target_link_libraries(libxtconvert PRIVATE ${QT_LIBS_LIST} ${QT_EXTRA_LIBS})

if(PGE_STATIC_SDLMIXER)
    target_link_libraries(libxtconvert PUBLIC PGE_SDLMixerX_static PGE_SDL2_static)
else()
    target_link_libraries(libxtconvert PUBLIC PGE_SDLMixerX PGE_SDL2)
endif()

target_include_directories(libxtconvert PUBLIC include)
target_include_directories(libxtconvert PRIVATE src)
set_target_properties(libxtconvert PROPERTIES OUTPUT_NAME "xtconvert")

# XTConvert cli
project(xtconvert_cli CXX)

include(../../_common/tclap/tclap.cmake)

include_directories(
    ${DEPENDENCIES_INSTALL_DIR}/include
)

# Executables fail to build with Qt 5/6 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QT_EXECUTABLE_CXX_FLAGS}")

add_executable(xtconvert_cli src/xtconvert_cli.cpp)
pge_set_nopie(xtconvert_cli)

set_target_properties(xtconvert_cli PROPERTIES OUTPUT_NAME "xtconvert")
target_include_directories(xtconvert_cli PRIVATE ${QT_INCLUDES_LIST})
target_compile_definitions(xtconvert_cli PRIVATE ${QT_DEFS_LIST})
target_link_libraries(xtconvert_cli PRIVATE ${QT_EXTRA_LIBS_PRE})
target_link_libraries(xtconvert_cli PUBLIC libxtconvert)
target_link_libraries(xtconvert_cli PRIVATE ${QT_LIBS_LIST} ${QT_EXTRA_LIBS})
