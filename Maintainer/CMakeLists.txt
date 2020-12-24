cmake_minimum_required (VERSION 3.2)
project(MoondustMaintainer C CXX)

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

include(../_common/translator-qt/translator.cmake)

pge_cxx_standard(14)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

if(NOT USE_SYSTEM_LIBPNG) # Disable native file dialogs when locally-built libPNG was been used to avoid the crash
    add_definitions(-DQT_DONT_USE_NATIVE_FILE_DIALOG)
endif()

# =============== Translations ===============
set(_LANGAUGES_TEMP_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/languages")
set(PGE_LANG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/languages")

file(GLOB PGE_LANG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/languages/*.ts")

set_source_files_properties(${PGE_LANG_SRC_FILES} PROPERTIES OUTPUT_LOCATION ${PGE_LANG_OUTPUT_DIR})
set(PGE_LANG_BIN_FILES)
qt5_add_translation(PGE_LANG_BIN_FILES ${PGE_LANG_SRC_FILES})
add_custom_target(maintainer_translations DEPENDS ${PGE_LANG_BIN_FILES})

if(NOT APPLE AND NOT EMSCRIPTEN)
    install(FILES ${PGE_LANG_BIN_FILES} DESTINATION "${PGE_INSTALL_DIRECTORY}/languages")
endif()
# ===========================================

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Concurrent_EXECUTABLE_COMPILE_FLAGS}")

qt5_wrap_ui(UIS_HDRS
    main_window.ui
    EpisodeCvt/episode_converter.ui
    EpisodeCvt/case_fixer.ui
    Music/audiocvt_sox_gui.ui
    Graphics/gifs2png.ui
    Graphics/png2gifs.ui
)

set(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/_resources/maintainer.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

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
    common_features/app_path.cpp
    files/episode_box.cpp
    common_features/logger.cpp
    EpisodeCvt/episode_converter.cpp
    EpisodeCvt/case_fixer.cpp
    Graphics/gifs2png.cpp
    Graphics/png2gifs.cpp
    ${UIS_HDRS}
    ${RESOURCE_ADDED}
    ${QT_PLUGINS_CPP}
)

if(APPLE)
    add_definitions(-DQ_OS_MACX) # Workaround for MOC
    list(APPEND PGE_MAINTAINER_SRCS
        _resources/maintainer.icns
        ${PGE_LANG_BIN_FILES}
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
) # Needed for Qt to be linked

add_library(PGE_Maintainer_QtLibs INTERFACE)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
target_include_directories(PGE_Maintainer_QtLibs
    INTERFACE
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDES_DIRS}
)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
target_compile_definitions(PGE_Maintainer_QtLibs
    INTERFACE
    ${Qt5Core_DEFINITIONS}
    ${Qt5Gui_DEFINITIONS}
    ${Qt5Widgets_DEFINITIONS}
    ${Qt5Concurrent_DEFINITIONS}
    -DPGE_FILES_QT
)

target_link_libraries(PGE_Maintainer_QtLibs INTERFACE
    ${QT_EXTRA_LIBS_PRE}
    PGE_FileFormatsQt
    ${Qt5Concurrent_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${QT_EXTRA_LIBS}
)
if(QT_PTHREAD)
    target_link_libraries(PGE_Maintainer_QtLibs INTERFACE ${QT_PTHREAD})
endif()


set(PGE_MAINTAINER_LINK_LIBS)
set(PGE_MAINTAINER_INSTALLS)

list(APPEND PGE_MAINTAINER_INSTALLS
    pge_maintainer
)

list(APPEND PGE_MAINTAINER_LINK_LIBS PGE_Maintainer_QtLibs)

if(WIN32)
    list(APPEND PGE_MAINTAINER_LINK_LIBS
        version
    )
else()
    if(UNIX AND NOT APPLE)
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
        OUTPUT_NAME "PGE Maintainer"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_resources/maintainer.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME "pge_maintainer"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ru.wohlsoft.pge-maintainer"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${MAINTAINER_VERSION_STRING}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${MAINTAINER_VERSION_STRING}"
        MACOSX_BUNDLE_ICON_FILE "maintainer.icns"
        CPACK_BUNDLE_NAME "PGE Maintainer"
        MACOSX_BUNDLE_INFO_STRING "Game data maintaining utility"
    )
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/_resources/maintainer.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${PGE_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")

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

    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND PGE_MAINTAINER_LINK_LIBS ${COREFOUNDATION_LIBRARY})
endif()

target_link_libraries(pge_maintainer
    ${PGE_MAINTAINER_LINK_LIBS}
)

pge_set_nopie(pge_maintainer)

install(TARGETS ${PGE_MAINTAINER_INSTALLS}
    RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}"
    BUNDLE DESTINATION "${PGE_INSTALL_DIRECTORY}"
)
