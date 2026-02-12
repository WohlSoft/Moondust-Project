cmake_minimum_required (VERSION 3.5...3.10)
project(SDLMixerMusPlayer C CXX)

if(POLICY CMP0071) # Automoc
    cmake_policy(SET CMP0071 NEW)
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPENDENCIES_INSTALL_DIR}/include
    ${DEPENDENCIES_INSTALL_DIR}/include/SDL2
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

if(PGE_USE_QT5)
    pge_cxx_standard(14)
else()
    pge_cxx_standard(17)
endif()

# WORKAROUND: Avoid ImpLib file creation on a non-library project
if(CYGWIN OR MINGW)
    # Prune --out-implib from executables. It doesn't make sense even
    # with --export-all-symbols.
    string(REGEX REPLACE "-Wl,--out-implib,[^ ]+ " " "
        CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE}")
    string(REGEX REPLACE "-Wl,--out-implib,[^ ]+ " " "
        CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}")
endif()

set(QT_LIBS_LIST
    ${QT_Network_LIBRARIES}
    ${QT_Widgets_LIBRARIES}
    ${QT_Gui_LIBRARIES}
    ${QT_Core_LIBRARIES}
)

set(QT_DEFS_LIST
    ${QT_Core_DEFINITIONS}
    ${QT_Gui_DEFINITIONS}
    ${QT_Widgets_DEFINITIONS}
    ${QT_Network_DEFINITIONS}
)

set(QT_INCLUDES_LIST
    ${QT_Core_INCLUDE_DIRS}
    ${QT_Gui_INCLUDE_DIRS}
    ${QT_Widgets_INCLUDE_DIRS}
    ${QT_Network_INCLUDE_DIRS}
)

set(QT_EXECUTABLE_CXX_FLAGS
    ${QT_Core_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Gui_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Widgets_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Network_EXECUTABLE_COMPILE_FLAGS}
)

if(QT_DISABLE_NATIVE_FILE_DIALOG) # Disable native file dialogs when locally-built libPNG was been used to avoid the crash
    message("== Music Player: Native file dialogs will be DISABLED")
    add_definitions(-DQT_DONT_USE_NATIVE_FILE_DIALOG)
else()
    message("== Music Player: Native file dialogs will be ALLOWED if possible")
endif()

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QT_EXECUTABLE_CXX_FLAGS}")

set(SDLMixer_MusPlay_SRC)
include(sources.cmake)

pge_qt_add_resources(RESOURCE_ADDED ${SDLMixer_MusPlay_RESOURCE})
pge_qt_wrap_ui(UIS_HDRS ${SDLMixer_MusPlay_UI})

if(QT_IMPORT_PLUGINS_MODULE)
    set(QT_PLUGINS_CPP "${CMAKE_BINARY_DIR}/pge_musplay_plugins.cpp")
    file(WRITE
            "${QT_PLUGINS_CPP}"
            "${QT_IMPORT_PLUGINS_MODULE}")
endif()

list(APPEND SDLMixer_MusPlay_SRC
    ${UIS_HDRS}
    ${RESOURCE_ADDED}
    ${QT_PLUGINS_CPP}
)

add_executable(pge_musplay
    ${SDLMixer_MusPlay_SRC}
)

add_dependencies(pge_musplay
    SDLMixerX_Local
    SDL2_Local
    libpng_Local
    FREETYPE_Local
)

add_library(PGE_MusPlay_QtLibs INTERFACE)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
target_include_directories(PGE_MusPlay_QtLibs
    INTERFACE
    ${QT_INCLUDES_LIST}
    ${USE_SDL_MIXER_X_PREFIX}/include
)
if(QT_PTHREAD)
    target_link_libraries(PGE_MusPlay_QtLibs INTERFACE ${QT_PTHREAD})
endif()

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
target_compile_definitions(PGE_MusPlay_QtLibs
    INTERFACE
    ${QT_DEFS_LIST}
)

target_link_libraries(PGE_MusPlay_QtLibs
    INTERFACE
    ${QT_EXTRA_LIBS_PRE}
    ${QT_LIBS_LIST}
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

if(NOT APPLE AND NOT HAIKU AND NOT MSVC AND NOT PGE_ENABLE_STATIC_QT AND NOT USE_SYSTEM_LIBC)
    set_property(TARGET pge_musplay APPEND_STRING PROPERTY LINK_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(pge_musplay PROPERTIES WIN32_EXECUTABLE ON)
        if(NOT PGE_ENABLE_STATIC_QT)
            list(APPEND PGE_MUSPLAY_LINK_LIBS "-static")
        endif()
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND PGE_MUSPLAY_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

if(APPLE)
    set_target_properties(pge_musplay PROPERTIES
        OUTPUT_NAME "Moondust Music Player"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_resources/musplay.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME "pge_musplay"
        MACOSX_BUNDLE_BUNDLE_NAME "Moondust Music Player"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ru.wohlsoft.pge-musplay"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${MUSPLAY_VERSION_1}.${MUSPLAY_VERSION_2}.${MUSPLAY_VERSION_3}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${MUSPLAY_VERSION_1}.${MUSPLAY_VERSION_2}.${MUSPLAY_VERSION_3}"
        MACOSX_BUNDLE_ICON_FILE "cat_musplay.icns"
        CPACK_BUNDLE_NAME "Moondust Music Player"
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

if(NOT PGE_INSTALL_ALLOW_PIE_EXECUTABLES)
    pge_set_nopie(pge_musplay)
endif()

if(PGE_INSTALL_UNIX_DESKTOP AND PGE_INSTALL_UNIX_TREE)
    # application launcher
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/share/applications")
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/share/applications")
    endif()

    set(PGE_COMPONENT_EXECUTABLE_NAME "pge_musplay")
    set(DESKTOP_EXEC ${PGE_COMPONENT_EXECUTABLE_NAME})
    set(DESKTOP_ICON ${PGE_COMPONENT_EXECUTABLE_NAME})
    set(DESKTOP_WMCLASS ${PGE_COMPONENT_EXECUTABLE_NAME})

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/_resources/icon.desktop.in"
                   "${DEPENDENCIES_INSTALL_DIR}/share/applications/${PGE_COMPONENT_EXECUTABLE_NAME}.desktop")
    install(FILES "${DEPENDENCIES_INSTALL_DIR}/share/applications/${PGE_COMPONENT_EXECUTABLE_NAME}.desktop"
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
            COMPONENT "MoondustMusicPlayer")

    # application icons
    set(PGE_DESKTOP_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/_resources/cat_musplay")

    foreach(FOR_ICON_SIZE 16;32;48;256)
        install(FILES "${PGE_DESKTOP_ICON_PATH}/cat_musplay_${FOR_ICON_SIZE}x${FOR_ICON_SIZE}.png"
                DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${FOR_ICON_SIZE}x${FOR_ICON_SIZE}/apps/"
                RENAME "${PGE_COMPONENT_EXECUTABLE_NAME}.png"
                COMPONENT "MoondustMusicPlayer")
    endforeach()
endif()

install(TARGETS ${PGE_MUSPLAY_INSTALLS}
    RUNTIME DESTINATION "${PGE_INSTALL_BIN}"
    BUNDLE DESTINATION "${PGE_INSTALL_BIN}"
    COMPONENT "MoondustMusicPlayer"
)
