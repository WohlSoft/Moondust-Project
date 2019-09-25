cmake_minimum_required (VERSION 3.2)
project(SDLMixerMusPlayer C CXX)

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
    ${DEPENDENCIES_INSTALL_DIR}/include/freetype2
    ${DEPENDENCIES_INSTALL_DIR}/include/lua_includes
)
link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

# Version
include(version.cmake)
# Default GIT version
include(../_common/git_version.cmake)
# Common building properties and optimization flags
include(../_common/build_props.cmake)
# Build properties for executable projects
include(../_common/executable_build_props.cmake)
# Find extra libraries to link when using static Qt (use ${QT_EXTRA_LIBS}!)
include(../_common/static_qt.cmake)

# WORKAROUND: Avoid ImpLib file creation on a non-library project
if(CYGWIN OR MINGW)
    # Prune --out-implib from executables. It doesn't make sense even
    # with --export-all-symbols.
    string(REGEX REPLACE "-Wl,--out-implib,[^ ]+ " " "
        CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE}")
    string(REGEX REPLACE "-Wl,--out-implib,[^ ]+ " " "
        CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}")
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Network_EXECUTABLE_COMPILE_FLAGS}")

qt5_wrap_ui(UIS_HDRS
    MainWindow/mainwindow.ui
    MainWindow/sfx_tester.ui
    MainWindow/setup_midi.ui
    AssocFiles/assoc_files.ui
)

set(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/_resources/musicplayer.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

if(QT_IMPORT_PLUGINS_MODULE)
    set(QT_PLUGINS_CPP "${CMAKE_BINARY_DIR}/pge_musplay_plugins.cpp")
    file(WRITE
            "${QT_PLUGINS_CPP}"
            "${QT_IMPORT_PLUGINS_MODULE}")
endif()

set(SDLMixer_MusPlay_SRC)

list(APPEND SDLMixer_MusPlay_SRC
    AssocFiles/assoc_files.cpp
    Effects/reverb.cpp
    MainWindow/label_marquee.cpp
    MainWindow/musplayer_base.cpp
    MainWindow/musplayer_qt.cpp
    MainWindow/seek_bar.cpp
    MainWindow/setup_midi.cpp
    MainWindow/sfx_tester.cpp
    Player/mus_player.cpp
    SingleApplication/localserver.cpp
    SingleApplication/pge_application.cpp
    SingleApplication/singleapplication.cpp
    main.cpp
    wave_writer.c
    ${UIS_HDRS}
    ${RESOURCE_ADDED}
    ${QT_PLUGINS_CPP}
)

if(APPLE)
    add_definitions(-DQ_OS_MACX) # Workaround for MOC
    set(PGE_FILE_ICONS "${CMAKE_CURRENT_SOURCE_DIR}/_resources/file_musplay.icns")
    list(APPEND SDLMixer_MusPlay_SRC
        _resources/cat_musplay.icns
        ${PGE_FILE_ICONS}
    )
endif()

if(WIN32)
    list(APPEND SDLMixer_MusPlay_SRC _resources/musicplayer.rc)
endif()


add_executable(pge_musplay
    ${SDLMixer_MusPlay_SRC}
)

add_dependencies(pge_musplay
    SDLMixerX_Local
    SDL2_Local
)

add_library(PGE_MusPlay_QtLibs INTERFACE)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
target_include_directories(PGE_MusPlay_QtLibs
    INTERFACE
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${USE_SDL_MIXER_X_PREFIX}/include
)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
target_compile_definitions(PGE_MusPlay_QtLibs
    INTERFACE
    ${Qt5Core_DEFINITIONS}
    ${Qt5Gui_DEFINITIONS}
    ${Qt5Widgets_DEFINITIONS}
    ${Qt5Network_DEFINITIONS}
)

target_link_libraries(PGE_MusPlay_QtLibs
    INTERFACE
    ${QT_EXTRA_LIBS_PRE}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${QT_EXTRA_LIBS}
)


set(PGE_MUSPLAY_LINK_LIBS)
set(PGE_MUSPLAY_INSTALLS)

list(APPEND PGE_MUSPLAY_INSTALLS
    pge_musplay
)

list(APPEND PGE_MUSPLAY_LINK_LIBS PGE_MusPlay_QtLibs)

option(USE_OFFICIAL_SDL_MIXER "Compile player in compatibility with official SDL Mixer variant" OFF)

if(USE_OFFICIAL_SDL_MIXER)
    set(SDL_MIXER_X_LIB SDL2_mixer)
else()
    target_compile_definitions(pge_musplay
        PRIVATE
        -DUSE_SDL_MIXER_X
    )

    if(PGE_STATIC_SDLMIXER)
        list(APPEND PGE_MUSPLAY_LINK_LIBS PGE_SDLMixerX_static PGE_SDL2_static)
    else()
        list(APPEND PGE_MUSPLAY_LINK_LIBS PGE_SDLMixerX PGE_SDL2)
    endif()
endif()

if(NOT APPLE AND NOT PGE_ENABLE_STATIC_QT)
    set_target_properties(pge_musplay PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(pge_musplay PROPERTIES LINK_FLAGS "-mwindows")
        if(NOT PGE_ENABLE_STATIC_QT)
            list(APPEND PGE_MUSPLAY_LINK_LIBS "-static")
        endif()
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND LazyFixTool_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

if(APPLE)
    set_target_properties(pge_musplay PROPERTIES
        OUTPUT_NAME "PGE Music Player"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_resources/musplay.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME "pge_musplay"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ru.wohlsoft.pge-musplay"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.3.0"
        MACOSX_BUNDLE_LONG_VERSION_STRING "1.3.0"
        MACOSX_BUNDLE_ICON_FILE "cat_musplay.icns"
        CPACK_BUNDLE_NAME "PGE Music Player"
        MACOSX_BUNDLE_INFO_STRING "A simple looped music player working on SDL Mixer X"
    )
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/_resources/cat_musplay.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${PGE_FILE_ICONS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND PGE_MUSPLAY_LINK_LIBS ${COREFOUNDATION_LIBRARY})

    list(APPEND PGE_MUSPLAY_LINK_LIBS iconv)
endif()

target_link_libraries(pge_musplay
    ${PGE_MUSPLAY_LINK_LIBS}
)

pge_set_nopie(pge_musplay)

install(TARGETS ${PGE_MUSPLAY_INSTALLS}
    RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}"
    BUNDLE DESTINATION "${PGE_INSTALL_DIRECTORY}"
)
