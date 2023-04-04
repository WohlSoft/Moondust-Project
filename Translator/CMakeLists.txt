cmake_minimum_required (VERSION 3.2)
project(MoondustTranslator C CXX)

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
include(../_common/Utf8Main/utf8main.cmake)

pge_cxx_standard(14)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

if(QT_DISABLE_NATIVE_FILE_DIALOG) # Disable native file dialogs when locally-built libPNG was been used to avoid the crash
    message("== Translator: Native file dialogs will be DISABLED")
    add_definitions(-DQT_DONT_USE_NATIVE_FILE_DIALOG)
else()
    message("== Translator: Native file dialogs will be ALLOWED if possible")
endif()

# =============== Translations ===============
set(_LANGAUGES_TEMP_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/languages")
set(PGE_LANG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/languages")

file(GLOB PGE_LANG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/languages/*.ts")

set_source_files_properties(${PGE_LANG_SRC_FILES} PROPERTIES OUTPUT_LOCATION ${PGE_LANG_OUTPUT_DIR})
set(PGE_LANG_BIN_FILES)
qt5_add_translation(PGE_LANG_BIN_FILES ${PGE_LANG_SRC_FILES})
add_custom_target(translator_translations DEPENDS ${PGE_LANG_BIN_FILES})

if(NOT APPLE AND NOT EMSCRIPTEN)
    install(FILES ${PGE_LANG_BIN_FILES} DESTINATION "${PGE_INSTALL_DIRECTORY}/languages")
endif()
# ===========================================

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Concurrent_EXECUTABLE_COMPILE_FLAGS}")

qt5_wrap_ui(UIS_HDRS
    translator_main.ui
)

set(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/_resources/translator.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

if(QT_IMPORT_PLUGINS_MODULE)
    set(QT_PLUGINS_CPP "${CMAKE_BINARY_DIR}/pge_translator_plugins.cpp")
    file(WRITE
            "${QT_PLUGINS_CPP}"
            "${QT_IMPORT_PLUGINS_MODULE}")
endif()

set(PGE_TRANSLATOR_SRCS)

list(APPEND PGE_TRANSLATOR_SRCS
    ${PGETRANSLATOR_SRCS}
    main.cpp
    textdata/textdata.h
    textdata/text_data_processor.h textdata/text_data_processor.cpp
    textdata/files_list_model.h textdata/files_list_model.cpp
    textdata/files_strings.h textdata/files_strings.cpp
    translator_main.h translator_main.cpp
    common_features/app_path.h common_features/app_path.cpp
    common_features/logger.h common_features/logger_sets.h common_features/logger.cpp
    ${UTF8MAIN_SRCS}
    ${UIS_HDRS}
    ${RESOURCE_ADDED}
    ${QT_PLUGINS_CPP}
)

if(APPLE)
    add_definitions(-DQ_OS_MACX) # Workaround for MOC
    set_source_files_properties(${MOONDUST_QT_LANG_BIN_FILES} PROPERTIES GENERATED TRUE) # WORKAROUND
    list(APPEND PGE_TRANSLATOR_SRCS
        _resources/translator.icns
        ${PGE_LANG_BIN_FILES}
        ${MOONDUST_QT_LANG_BIN_FILES}
        ${MOONDUST_QT_LANG_ICON_FILES}
    )
endif()

if(WIN32)
    list(APPEND PGE_TRANSLATOR_SRCS _resources/translator.rc)
endif()

add_executable(pge_translator
    ${PGE_TRANSLATOR_SRCS}
)

add_dependencies(pge_translator
    libpng_Local
    FREETYPE_Local
    PGE_FileFormatsQt
    translator_translations
    moondust_translations
) # Needed for Qt to be linked

add_library(PGE_Translator_QtLibs INTERFACE)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
target_include_directories(PGE_Translator_QtLibs
    INTERFACE
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDES_DIRS}
)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
target_compile_definitions(PGE_Translator_QtLibs
    INTERFACE
    ${Qt5Core_DEFINITIONS}
    ${Qt5Gui_DEFINITIONS}
    ${Qt5Widgets_DEFINITIONS}
    ${Qt5Concurrent_DEFINITIONS}
    -DPGE_FILES_QT
)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
    target_compile_definitions(pge_translator
        PRIVATE
        -DDEBUG_BUILD
    )
endif()

target_link_libraries(PGE_Translator_QtLibs INTERFACE
    ${QT_EXTRA_LIBS_PRE}
    PGE_FileFormatsQt
    ${Qt5Concurrent_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${QT_EXTRA_LIBS}
)
if(QT_PTHREAD)
    target_link_libraries(PGE_Translator_QtLibs INTERFACE ${QT_PTHREAD})
endif()


set(PGE_TRANSLATOR_LINK_LIBS)
set(PGE_TRANSLATOR_INSTALLS)

list(APPEND PGE_TRANSLATOR_INSTALLS
    pge_translator
)

list(APPEND PGE_TRANSLATOR_LINK_LIBS PGE_Translator_QtLibs)

if(WIN32)
    list(APPEND PGE_TRANSLATOR_LINK_LIBS
        version
    )
else()
    if(UNIX AND NOT APPLE)
        list(APPEND PGE_TRANSLATOR_LINK_LIBS dl)
    endif()
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(pge_translator PROPERTIES WIN32_EXECUTABLE ON)
        if(NOT PGE_ENABLE_STATIC_QT)
            list(APPEND PGE_TRANSLATOR_LINK_LIBS "-static")
        endif()
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND PGE_TRANSLATOR_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

if(APPLE)
    set_target_properties(pge_translator PROPERTIES
        OUTPUT_NAME "Moondust Translator"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_resources/translator.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME "pge_translator"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ru.wohlsoft.pge-translator"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${TRANSLATOR_VERSION_STRING}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${TRANSLATOR_VERSION_STRING}"
        MACOSX_BUNDLE_ICON_FILE "translator.icns"
        CPACK_BUNDLE_NAME "Moondust Translator"
        MACOSX_BUNDLE_INFO_STRING "Episode and levels localization tool"
    )
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/_resources/translator.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${PGE_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${MOONDUST_QT_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${MOONDUST_QT_LANG_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")

    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND PGE_TRANSLATOR_LINK_LIBS ${COREFOUNDATION_LIBRARY})
endif()

target_link_libraries(pge_translator
    ${PGE_TRANSLATOR_LINK_LIBS}
)

pge_set_nopie(pge_translator)

install(TARGETS ${PGE_TRANSLATOR_INSTALLS}
    RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}"
    BUNDLE DESTINATION "${PGE_INSTALL_DIRECTORY}"
)
