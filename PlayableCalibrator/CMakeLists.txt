cmake_minimum_required (VERSION 3.5...3.10)
project(MoondustPlayerCalibrator C CXX)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/../_common/ # translator-qt
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
include(../_common/qt-modules/compat/pge_qt_compat.cmake)

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
    message("== Calibrator: Native file dialogs will be DISABLED")
    add_definitions(-DQT_DONT_USE_NATIVE_FILE_DIALOG)
else()
    message("== Calibrator: Native file dialogs will be ALLOWED if possible")
endif()

# =============== Translations ===============
set(_LANGAUGES_TEMP_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/languages")
set(PGE_LANG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/languages")

file(GLOB PGE_LANG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/languages/*.ts")

set_source_files_properties(${PGE_LANG_SRC_FILES} PROPERTIES OUTPUT_LOCATION ${PGE_LANG_OUTPUT_DIR})
set(PGE_LANG_BIN_FILES)
pge_qt_add_translation(PGE_LANG_BIN_FILES ${PGE_LANG_SRC_FILES})
add_custom_target(calibrator_translations DEPENDS ${PGE_LANG_BIN_FILES})

if(NOT APPLE AND NOT EMSCRIPTEN)
    install(FILES ${PGE_LANG_BIN_FILES} DESTINATION "${PGE_INSTALL_DIRECTORY}/languages")
endif()
# ===========================================

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QT_EXECUTABLE_CXX_FLAGS}")

pge_qt_wrap_ui(UIS_HDRS
    calibration_main.ui
    about/about.ui
    animator/animator.ui
    animator/animation_edit.ui
    frame_matrix/matrix.ui
    image_calibration/image_calibrator.ui
)

set(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/_resources/images.qrc)
pge_qt_add_resources(RESOURCE_ADDED ${RESOURCE})

if(QT_IMPORT_PLUGINS_MODULE)
    set(QT_PLUGINS_CPP "${CMAKE_BINARY_DIR}/pge_calibrator_plugins.cpp")
    file(WRITE
            "${QT_PLUGINS_CPP}"
            "${QT_IMPORT_PLUGINS_MODULE}")
endif()

set(PGE_CALIBRATOR_SRCS)

list(APPEND PGE_CALIBRATOR_SRCS
    ${PGETRANSLATOR_SRCS}
    main.cpp
    calibration_main.cpp
    frame_matrix/matrix.cpp
    frame_matrix/matrix_widget.cpp
    about/about.cpp
    animator/animator.cpp
    animator/animation_edit.cpp
    image_calibration/image_calibrator.cpp
    main/mouse_scene.cpp
    main/calibration_file.cpp
    main/frame_tune_scene.cpp
    main/files_io.cpp
    main/graphics.cpp
    main/settings.cpp
    main/app_path.cpp
    ${UIS_HDRS}
    ${RESOURCE_ADDED}
    ${QT_PLUGINS_CPP}
)

if(APPLE)
    add_definitions(-DQ_OS_MACX) # Workaround for MOC
    set_source_files_properties(${MOONDUST_QT_LANG_BIN_FILES} PROPERTIES GENERATED TRUE) # WORKAROUND
    list(APPEND PGE_CALIBRATOR_SRCS
        _resources/calibrator.icns
        ${PGE_LANG_BIN_FILES}
        ${MOONDUST_QT_LANG_BIN_FILES}
        ${MOONDUST_QT_LANG_ICON_FILES}
    )
endif()

if(WIN32)
    list(APPEND PGE_CALIBRATOR_SRCS _resources/caltool.rc)
endif()

add_executable(pge_calibrator
    ${PGE_CALIBRATOR_SRCS}
    ${PGE_FILE_LIBRARY_SRCS}
)

add_library(PGE_Calibrator_QtLibs INTERFACE)

target_include_directories(PGE_Calibrator_QtLibs
    INTERFACE
    ${QT_INCLUDES_LIST}
)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
target_compile_definitions(PGE_Calibrator_QtLibs
    INTERFACE
    ${QT_DEFS_LIST}
)

target_link_libraries(PGE_Calibrator_QtLibs
    INTERFACE
    ${QT_EXTRA_LIBS_PRE}
    ${QT_LIBS_LIST}
    ${QT_EXTRA_LIBS}
)
if(QT_PTHREAD)
    target_link_libraries(PGE_Calibrator_QtLibs INTERFACE ${QT_PTHREAD})
endif()

add_dependencies(pge_calibrator
    FreeImage_Local
    libpng_Local
    FREETYPE_Local
    calibrator_translations
    moondust_translations
)

set(PGE_CALIBRATOR_LINK_LIBS)
set(PGE_CALIBRATOR_INSTALLS)

list(APPEND PGE_CALIBRATOR_INSTALLS
    pge_calibrator
)

list(APPEND PGE_CALIBRATOR_LINK_LIBS
    ${UTILS_LIBS}
    PGE_Calibrator_QtLibs
    PGE_FreeImage
)

pge_set_nopie(pge_calibrator)

if(WIN32)
    list(APPEND PGE_CALIBRATOR_LINK_LIBS
        version
    )
else()
    if(UNIX AND NOT APPLE)
        find_library(_lib_dl dl)
        if(_lib_dl)
            list(APPEND PGE_CALIBRATOR_LINK_LIBS ${_lib_dl})
        endif()
    endif()
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(pge_calibrator PROPERTIES WIN32_EXECUTABLE ON)
        if(NOT PGE_ENABLE_STATIC_QT)
            list(APPEND PGE_CALIBRATOR_LINK_LIBS "-static")
        endif()
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND PGE_CALIBRATOR_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

if(APPLE)
    set_target_properties(pge_calibrator PROPERTIES
        OUTPUT_NAME "Moondust Character Calibrator"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_resources/calibrator.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME "pge_calibrator"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ru.wohlsoft.pge-calibrator"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${CALIBRATOR_VERSION_STRING}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${CALIBRATOR_VERSION_STRING}"
        MACOSX_BUNDLE_ICON_FILE "calibrator.icns"
        CPACK_BUNDLE_NAME "Moondust Character Calibrator"
        MACOSX_BUNDLE_INFO_STRING "Calibrator of playable character hitbox preferences and animations for Moondust Project"
    )
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/_resources/calibrator.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${PGE_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${MOONDUST_QT_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${MOONDUST_QT_LANG_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")

    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND PGE_CALIBRATOR_LINK_LIBS ${COREFOUNDATION_LIBRARY})
endif()

target_link_libraries(pge_calibrator
    ${PGE_CALIBRATOR_LINK_LIBS}
)

install(TARGETS ${PGE_CALIBRATOR_INSTALLS}
    RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}"
    BUNDLE DESTINATION "${PGE_INSTALL_DIRECTORY}"
)

# Calibrator's default properties
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Content/calibrator" DESTINATION "${CMAKE_BINARY_DIR}/bin")

# Calibrator's stuff
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Content/calibrator"
    DESTINATION "${PGE_INSTALL_DIRECTORY}"
)
