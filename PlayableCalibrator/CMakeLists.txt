cmake_minimum_required (VERSION 3.1.0)
project(MoondustPlayerCalibrator C CXX)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPENDENCIES_INSTALL_DIR}/include
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

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

qt5_wrap_ui(UIS_HDRS
    calibrationmain.ui
    about/about.ui
    animator/animate.ui
    animator/animationedit.ui
    frame_matrix/matrix.ui
    image_calibration/image_calibrator.ui
)

set(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/_resources/images.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

set(PGE_CALIBRATOR_SRCS)

list(APPEND PGE_CALIBRATOR_SRCS
    main.cpp
    calibrationmain.cpp
    frame_matrix/matrix.cpp
    about/about.cpp
    animator/animate.cpp
    animator/animationedit.cpp
    main/files_io.cpp
    main/globals.cpp
    main/graphics.cpp
    main/settings.cpp
    image_calibration/image_calibrator.cpp
    main/mw.cpp
    animator/AnimationScene.cpp
    main/app_path.cpp
    ${UIS_HDRS}
    ${RESOURCE_ADDED}
)

if(APPLE)
    add_definitions(-DQ_OS_MACX) # Workaround for MOC
    list(APPEND PGE_CALIBRATOR_SRCS
        _resources/calibrator.icns
    )
endif()

if(WIN32)
    list(APPEND PGE_CALIBRATOR_SRCS _resources/caltool.rc)
endif()

add_executable(pge_calibrator
    ${PGE_CALIBRATOR_SRCS}
    ${PGE_FILE_LIBRARY_SRCS}
)

target_include_directories(pge_calibrator
    PRIVATE
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
)

add_dependencies(pge_calibrator
    FreeImage_Local
)

set(PGE_CALIBRATOR_LINK_LIBS)
set(PGE_CALIBRATOR_INSTALLS)

list(APPEND PGE_CALIBRATOR_INSTALLS
    pge_calibrator
)

list(APPEND PGE_CALIBRATOR_LINK_LIBS
    ${UTILS_LIBS}
    FreeImageLite$<$<CONFIG:Debug>:d>
)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
target_compile_definitions(pge_calibrator
    PRIVATE
    ${Qt5Widgets_DEFINITIONS}
)

if(WIN32)
    list(APPEND PGE_CALIBRATOR_LINK_LIBS
        version
    )
else()
    if(UNIX AND NOT APPLE)
        list(APPEND PGE_CALIBRATOR_LINK_LIBS dl)
    endif()
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(pge_calibrator PROPERTIES LINK_FLAGS "-mwindows")
        list(APPEND PGE_CALIBRATOR_LINK_LIBS "-static")
    endif()
    list(APPEND PGE_CALIBRATOR_LINK_LIBS pthread)
endif()

if(APPLE)
    set_target_properties(pge_calibrator PROPERTIES
        OUTPUT_NAME "PGE Player Calibrator"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_resources/calibrator.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME "pge_calibrator"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ru.wohlsoft.pge-calibrator"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "2.1.1"
        MACOSX_BUNDLE_LONG_VERSION_STRING "2.1.1"
        MACOSX_BUNDLE_ICON_FILE "calibrator.icns"
        CPACK_BUNDLE_NAME "PGE Player Calibrator"
        MACOSX_BUNDLE_INFO_STRING "Calibrator of playable character hitbox preferences and animations for PGE Project"
    )
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/_resources/calibrator.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND PGE_CALIBRATOR_LINK_LIBS ${COREFOUNDATION_LIBRARY})
endif()

list(APPEND PGE_CALIBRATOR_LINK_LIBS
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${QT_EXTRA_LIBS}
)

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
