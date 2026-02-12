cmake_minimum_required (VERSION 3.5...4.0)
project(MoondustTranslator C CXX)

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

set(PGE_APP_PATH_MODULE_NAME "Moondust Calibrator")
set(PGE_APP_PATH_SETUP_FILE_NAME "pge_translator.ini")
set(PGE_APP_PATH_VERSION_HEADER "../../Translator/version.h")
set(PGE_APP_PATH_HAS_THEMES FALSE)
set(PGE_APP_PATH_TARGET_SUFFIX Translator)
include(../_common/qt-modules/app_path_qt/app_path.cmake)

include(../_common/translator-qt/translator.cmake)
include(../_common/qt-modules/compat/pge_qt_compat.cmake)
include(../_common/Utf8Main/utf8main.cmake)
include(../_common/qt-modules/msg_box_preview/msg_box_preview.cmake)
include(../_common/qt-modules/syntax_highlight/syntax_highlight.cmake)

if(PGE_USE_QT5)
    pge_cxx_standard(14)
else()
    pge_cxx_standard(17)
endif()

if(QT_DISABLE_NATIVE_FILE_DIALOG) # Disable native file dialogs when locally-built libPNG was been used to avoid the crash
    message("== Translator: Native file dialogs will be DISABLED")
    add_definitions(-DQT_DONT_USE_NATIVE_FILE_DIALOG)
else()
    message("== Translator: Native file dialogs will be ALLOWED if possible")
endif()

# =============== Translations ===============
set(_LANGAUGES_TEMP_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/languages")
set(PGE_LANG_OUTPUT_DIR "${PGE_DEBUG_DATA}/languages")

file(GLOB PGE_LANG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/languages/*.ts")

set_source_files_properties(${PGE_LANG_SRC_FILES} PROPERTIES OUTPUT_LOCATION ${PGE_LANG_OUTPUT_DIR})
set(PGE_LANG_BIN_FILES)
pge_qt_add_translation(PGE_LANG_BIN_FILES ${PGE_LANG_SRC_FILES})
add_custom_target(translator_translations DEPENDS ${PGE_LANG_BIN_FILES})

if(NOT APPLE AND NOT EMSCRIPTEN)
    install(FILES ${PGE_LANG_BIN_FILES} DESTINATION "${PGE_INSTALL_DATA}/languages" COMPONENT "MoondustTranslator")
endif()
# ===========================================

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QT_Widgets_EXECUTABLE_COMPILE_FLAGS} ${QT_Concurrent_EXECUTABLE_COMPILE_FLAGS}")

pge_qt_wrap_ui(UIS_HDRS
    translator_main.ui
    translate_field.ui
    dialogue_item.ui
    about/about.ui
    lang_select/lang_select.ui
)

set(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/_resources/translator.qrc)
pge_qt_add_resources(RESOURCE_ADDED ${RESOURCE})

if(QT_IMPORT_PLUGINS_MODULE)
    set(QT_PLUGINS_CPP "${CMAKE_BINARY_DIR}/pge_translator_plugins.cpp")
    file(WRITE
            "${QT_PLUGINS_CPP}"
            "${QT_IMPORT_PLUGINS_MODULE}")
endif()

set(PGE_TRANSLATOR_SRCS)

list(APPEND PGE_TRANSLATOR_SRCS
    ${PGETRANSLATOR_SRCS}
    ${MSG_BOX_PREVIEW_SRC}
    ${SYNTAX_HIGHLIGHT_SRC}
    main.cpp
    textdata/textdata.h
    textdata/text_types.h
    textdata/text_data_processor.h textdata/text_data_processor.cpp
    textdata/files_list_model.h textdata/files_list_model.cpp
    textdata/dialogues_list_model.h textdata/dialogues_list_model.cpp
    textdata/files_strings.h textdata/files_strings.cpp
    textdata/langs_list_model.h textdata/langs_list_model.cpp
    textdata/delegates/checkbox_delegate.cpp textdata/delegates/checkbox_delegate.h
    about/about.h about/about.cpp
    lang_select/lang_select.h lang_select/lang_select.cpp
    translator_main.h translator_main.cpp
    translate_field.h translate_field.cpp
    dialogue_item.h dialogue_item.cpp
    common_features/logger.h common_features/logger_sets.h common_features/logger.cpp
    common_features/growing_text_edit.h common_features/growing_text_edit.cpp
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
    ${QT_Core_INCLUDE_DIRS}
    ${QT_Gui_INCLUDE_DIRS}
    ${QT_Widgets_INCLUDE_DIRS}
    ${QT_Concurrent_INCLUDE_DIRS}
)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
target_compile_definitions(PGE_Translator_QtLibs
    INTERFACE
    ${QT_Core_DEFINITIONS}
    ${QT_Gui_DEFINITIONS}
    ${QT_Widgets_DEFINITIONS}
    ${QT_Concurrent_DEFINITIONS}
    -DPGE_FILES_QT
)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
    target_compile_definitions(pge_translator
        PRIVATE
        -DDEBUG_BUILD
    )
endif()

target_compile_definitions(pge_translator PRIVATE ${MSG_BOX_PREVIEW_DEFS})

target_link_libraries(PGE_Translator_QtLibs INTERFACE
    ${QT_EXTRA_LIBS_PRE}
    PGE_FileFormatsQt
    ${QT_Concurrent_LIBRARIES}
    ${QT_Widgets_LIBRARIES}
    ${QT_Gui_LIBRARIES}
    ${QT_Core_LIBRARIES}
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

list(APPEND PGE_TRANSLATOR_LINK_LIBS PGE_AppPathTranslatorQt PGE_Translator_QtLibs)

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
        MACOSX_BUNDLE_BUNDLE_NAME "Moondust Translator"
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

if(ENABLE_ADDRESS_SANITIZER)
    target_compile_options(pge_translator PRIVATE -fsanitize=address)
    target_link_options(pge_translator PRIVATE -fsanitize=address)
endif()

if(NOT PGE_INSTALL_ALLOW_PIE_EXECUTABLES)
    pge_set_nopie(pge_translator)
endif()

if(PGE_INSTALL_UNIX_DESKTOP AND PGE_INSTALL_UNIX_TREE)
    # application launcher
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/share/applications")
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/share/applications")
    endif()

    set(PGE_COMPONENT_EXECUTABLE_NAME "pge_translator")
    set(DESKTOP_EXEC ${PGE_COMPONENT_EXECUTABLE_NAME})
    set(DESKTOP_ICON ${PGE_COMPONENT_EXECUTABLE_NAME})
    set(DESKTOP_WMCLASS ${PGE_COMPONENT_EXECUTABLE_NAME})

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/_resources/icon.desktop.in"
                   "${DEPENDENCIES_INSTALL_DIR}/share/applications/${PGE_COMPONENT_EXECUTABLE_NAME}.desktop")
    install(FILES "${DEPENDENCIES_INSTALL_DIR}/share/applications/${PGE_COMPONENT_EXECUTABLE_NAME}.desktop"
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
            COMPONENT "MoondustTranslator")

    # application icons
    set(PGE_DESKTOP_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/_resources/icon")

    foreach(FOR_ICON_SIZE 16;32;48;256)
        install(FILES "${PGE_DESKTOP_ICON_PATH}/translator${FOR_ICON_SIZE}.png"
                DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${FOR_ICON_SIZE}x${FOR_ICON_SIZE}/apps/"
                RENAME "${PGE_COMPONENT_EXECUTABLE_NAME}.png"
                COMPONENT "MoondustTranslator")
    endforeach()
endif()

install(TARGETS ${PGE_TRANSLATOR_INSTALLS}
    RUNTIME DESTINATION "${PGE_INSTALL_BIN}"
    BUNDLE DESTINATION "${PGE_INSTALL_BIN}"
    COMPONENT "MoondustTranslator"
)
