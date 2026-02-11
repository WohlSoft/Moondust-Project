cmake_minimum_required (VERSION 3.5...3.10)
project(MoondustMaintainer C CXX)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/../_common/ # PGE-FL
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

set(PGE_APP_PATH_MODULE_NAME "PGE Maintainer")
set(PGE_APP_PATH_SETUP_FILE_NAME "pge_maintainer.ini")
set(PGE_APP_PATH_VERSION_HEADER "../../Maintainer/version.h")
set(PGE_APP_PATH_HAS_THEMES FALSE)
set(PGE_APP_PATH_TARGET_SUFFIX Maintainer)
include(../_common/qt-modules/app_path_qt/app_path.cmake)

include(../_common/translator-qt/translator.cmake)
include(../_common/qt-modules/compat/pge_qt_compat.cmake)
include(../_common/Utf8Main/utf8main.cmake)

if(PGE_USE_QT5)
    pge_cxx_standard(14)
else()
    pge_cxx_standard(17)
endif()

set(QT_LIBS_LIST
    ${QT_Concurrent_LIBRARIES}
    ${QT_Widgets_LIBRARIES}
    ${QT_Gui_LIBRARIES}
    ${QT_Core_LIBRARIES}
)

set(QT_DEFS_LIST
    ${QT_Core_DEFINITIONS}
    ${QT_Gui_DEFINITIONS}
    ${QT_Widgets_DEFINITIONS}
    ${QT_Concurrent_DEFINITIONS}
)

set(QT_INCLUDES_LIST
    ${QT_Core_INCLUDE_DIRS}
    ${QT_Gui_INCLUDE_DIRS}
    ${QT_Widgets_INCLUDE_DIRS}
    ${QT_Concurrent_INCLUDE_DIRS}
)

set(QT_EXECUTABLE_CXX_FLAGS
    ${QT_Core_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Gui_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Widgets_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Concurrent_EXECUTABLE_COMPILE_FLAGS}
)

if(QT_DISABLE_NATIVE_FILE_DIALOG) # Disable native file dialogs when locally-built libPNG was been used to avoid the crash
    message("== Maintainer: Native file dialogs will be DISABLED")
    add_definitions(-DQT_DONT_USE_NATIVE_FILE_DIALOG)
else()
    message("== Maintainer: Native file dialogs will be ALLOWED if possible")
endif()

# =============== Translations ===============
set(_LANGAUGES_TEMP_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/languages")
set(PGE_LANG_OUTPUT_DIR "${PGE_DEBUG_DATA}/languages")

file(GLOB PGE_LANG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/languages/*.ts")

set_source_files_properties(${PGE_LANG_SRC_FILES} PROPERTIES OUTPUT_LOCATION ${PGE_LANG_OUTPUT_DIR})
set(PGE_LANG_BIN_FILES)
pge_qt_add_translation(PGE_LANG_BIN_FILES ${PGE_LANG_SRC_FILES})
add_custom_target(maintainer_translations DEPENDS ${PGE_LANG_BIN_FILES})

if(NOT APPLE AND NOT EMSCRIPTEN)
    install(FILES ${PGE_LANG_BIN_FILES} DESTINATION "${PGE_INSTALL_DATA}/languages")
endif()
# ===========================================

# Executables fail to build with Qt 5/6 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QT_EXECUTABLE_CXX_FLAGS}")

pge_qt_wrap_ui(UIS_HDRS
    main_window.ui
    EpisodeCvt/episode_converter.ui
    EpisodeCvt/case_fixer.ui
    Music/audiocvt_sox_gui.ui
    Graphics/gifs2png.ui
    Graphics/png2gifs.ui
    Graphics/lazyfix.ui
    Graphics/palette_filter.ui
)

set(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/_resources/maintainer.qrc)
pge_qt_add_resources(RESOURCE_ADDED ${RESOURCE})

if(QT_IMPORT_PLUGINS_MODULE)
    set(QT_PLUGINS_CPP "${CMAKE_BINARY_DIR}/pge_maintainer_plugins.cpp")
    file(WRITE
            "${QT_PLUGINS_CPP}"
            "${QT_IMPORT_PLUGINS_MODULE}")
endif()

set(PGE_MAINTAINER_SRCS)

list(APPEND PGE_MAINTAINER_SRCS
    ${PGETRANSLATOR_SRCS}
    main.cpp
    main_window.cpp
    Music/audiocvt_sox_gui.cpp
    files/episode_box.cpp
    common_features/logger.cpp
    EpisodeCvt/episode_converter.cpp
    EpisodeCvt/case_fixer.cpp
    Graphics/gifs2png.cpp
    Graphics/png2gifs.cpp
    Graphics/lazyfix.cpp
    Graphics/palette_filter.cpp
    ${UTF8MAIN_SRCS}
    ${UIS_HDRS}
    ${RESOURCE_ADDED}
    ${QT_PLUGINS_CPP}
)

if(APPLE)
    add_definitions(-DQ_OS_MACX) # Workaround for MOC
    set_source_files_properties(${MOONDUST_QT_LANG_BIN_FILES} PROPERTIES GENERATED TRUE) # WORKAROUND
    list(APPEND PGE_MAINTAINER_SRCS
        _resources/maintainer.icns
        ${PGE_LANG_BIN_FILES}
        ${MOONDUST_QT_LANG_BIN_FILES}
        ${MOONDUST_QT_LANG_ICON_FILES}
    )
endif()

if(WIN32)
    list(APPEND PGE_MAINTAINER_SRCS _resources/maintainer.rc)
endif()

add_executable(pge_maintainer
    ${PGE_MAINTAINER_SRCS}
)

add_dependencies(pge_maintainer
    libpng_Local
    FREETYPE_Local
    PGE_FileFormatsQt
    maintainer_translations
    moondust_translations
) # Needed for Qt to be linked

add_library(PGE_Maintainer_QtLibs INTERFACE)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
target_include_directories(PGE_Maintainer_QtLibs
    INTERFACE
    ${QT_INCLUDES_LIST}
)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
target_compile_definitions(PGE_Maintainer_QtLibs
    INTERFACE
    ${QT_DEFS_LIST}
    -DPGE_FILES_QT
)

target_link_libraries(PGE_Maintainer_QtLibs INTERFACE
    ${QT_EXTRA_LIBS_PRE}
    PGE_FileFormatsQt
    ${QT_LIBS_LIST}
    ${QT_EXTRA_LIBS}
)
if(QT_PTHREAD)
    target_link_libraries(PGE_Maintainer_QtLibs INTERFACE ${QT_PTHREAD})
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
    target_compile_definitions(pge_maintainer
        PRIVATE
        -DDEBUG_BUILD
    )
endif()


set(PGE_MAINTAINER_LINK_LIBS)
set(PGE_MAINTAINER_INSTALLS)

list(APPEND PGE_MAINTAINER_INSTALLS
    pge_maintainer
)

list(APPEND PGE_MAINTAINER_LINK_LIBS PGE_AppPathMaintainerQt PGE_Maintainer_QtLibs)

if(WIN32)
    list(APPEND PGE_MAINTAINER_LINK_LIBS
        version
    )
else()
    if(UNIX AND NOT APPLE AND NOT HAIKU)
        list(APPEND PGE_MAINTAINER_LINK_LIBS dl)
    endif()
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(pge_maintainer PROPERTIES WIN32_EXECUTABLE ON)
        if(NOT PGE_ENABLE_STATIC_QT)
            list(APPEND PGE_MAINTAINER_LINK_LIBS "-static")
        endif()
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND PGE_MAINTAINER_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

if(APPLE)
    set_target_properties(pge_maintainer PROPERTIES
        OUTPUT_NAME "Moondust Maintainer"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_resources/maintainer.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME "pge_maintainer"
        MACOSX_BUNDLE_BUNDLE_NAME "Moondust Maintainer"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ru.wohlsoft.pge-maintainer"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${MAINTAINER_VERSION_STRING}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${MAINTAINER_VERSION_STRING}"
        MACOSX_BUNDLE_ICON_FILE "maintainer.icns"
        CPACK_BUNDLE_NAME "Moondust Maintainer"
        MACOSX_BUNDLE_INFO_STRING "Game data maintaining utility"
    )
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/_resources/maintainer.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${PGE_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${MOONDUST_QT_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${MOONDUST_QT_LANG_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")

    if(TARGET GIFs2PNG)
        add_dependencies(pge_maintainer GIFs2PNG)
        set(GIFs2PNG_BINARY "${CMAKE_BINARY_DIR}/bin/GIFs2PNG")
        target_sources(pge_maintainer PRIVATE ${GIFs2PNG_BINARY})
        set_source_files_properties(${GIFs2PNG_BINARY} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS" GENERATED TRUE)
    endif()

    if(TARGET PNG2GIFs)
        add_dependencies(pge_maintainer PNG2GIFs)
        set(PNG2GIFs_BINARY "${CMAKE_BINARY_DIR}/bin/PNG2GIFs")
        target_sources(pge_maintainer PRIVATE ${PNG2GIFs_BINARY})
        set_source_files_properties(${PNG2GIFs_BINARY} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS" GENERATED TRUE)
    endif()

    if(TARGET LazyFixTool)
        add_dependencies(pge_maintainer LazyFixTool)
        set(LAZYFIXTOOL_BINARY "${CMAKE_BINARY_DIR}/bin/LazyFixTool")
        target_sources(pge_maintainer PRIVATE ${LAZYFIXTOOL_BINARY})
        set_source_files_properties(${LAZYFIXTOOL_BINARY} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS" GENERATED TRUE)
    endif()

    if(TARGET PaletteFilter)
        add_dependencies(pge_maintainer PaletteFilter)
        set(PALETTEFILTER_BINARY "${CMAKE_BINARY_DIR}/bin/PaletteFilter")
        target_sources(pge_maintainer PRIVATE ${PALETTEFILTER_BINARY})
        set_source_files_properties(${PALETTEFILTER_BINARY} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS" GENERATED TRUE)
    endif()

    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND PGE_MAINTAINER_LINK_LIBS ${COREFOUNDATION_LIBRARY})
endif()

target_link_libraries(pge_maintainer
    ${PGE_MAINTAINER_LINK_LIBS}
)

pge_set_nopie(pge_maintainer)

if(PGE_INSTALL_UNIX_DESKTOP AND PGE_INSTALL_UNIX_TREE)
    # application launcher
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/share/applications")
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/share/applications")
    endif()

    set(PGE_COMPONENT_EXECUTABLE_NAME "pge_maintainer")
    set(DESKTOP_EXEC ${PGE_COMPONENT_EXECUTABLE_NAME})
    set(DESKTOP_ICON ${PGE_COMPONENT_EXECUTABLE_NAME})
    set(DESKTOP_WMCLASS ${PGE_COMPONENT_EXECUTABLE_NAME})

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/_resources/icon.desktop.in"
                   "${DEPENDENCIES_INSTALL_DIR}/share/applications/${PGE_COMPONENT_EXECUTABLE_NAME}.desktop")
    install(FILES "${DEPENDENCIES_INSTALL_DIR}/share/applications/${PGE_COMPONENT_EXECUTABLE_NAME}.desktop"
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

    # application icons
    set(PGE_DESKTOP_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/_resources/icon")

    install(FILES "${PGE_DESKTOP_ICON_PATH}/maintainer16.png"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps/" RENAME "${PGE_COMPONENT_EXECUTABLE_NAME}.png")
    install(FILES "${PGE_DESKTOP_ICON_PATH}/maintainer32.png"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps/" RENAME "${PGE_COMPONENT_EXECUTABLE_NAME}.png")
    install(FILES "${PGE_DESKTOP_ICON_PATH}/maintainer48.png"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps/" RENAME "${PGE_COMPONENT_EXECUTABLE_NAME}.png")
    install(FILES "${PGE_DESKTOP_ICON_PATH}/maintainer256.png"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps/" RENAME "${PGE_COMPONENT_EXECUTABLE_NAME}.png")
endif()

install(TARGETS ${PGE_MAINTAINER_INSTALLS}
    RUNTIME DESTINATION "${PGE_INSTALL_BIN}"
    BUNDLE DESTINATION "${PGE_INSTALL_BIN}"
)
