set(PGE_ENABLE_QT_ENABLED_BY_DEFAULT ON)

if(EMSCRIPTEN OR ANDROID)
    set(PGE_ENABLE_QT_ENABLED_BY_DEFAULT OFF)
    message("== Qt-dependent components are not supported for this platform")
    include(_common/find_linguist.cmake)
endif()

if(PGE_ENABLE_QT_ENABLED_BY_DEFAULT)
    # FIXME: Set to OFF when Qt 6 support will be ready
    option(PGE_FORCE_QT5 "Find Qt5 instead of Qt6. Support of Qt 6 is EXPERIMENTAL and NOT ready." ON)

    if(PGE_FORCE_QT5)
        find_package(Qt5 COMPONENTS Core QUIET) # Check is Qt available
        set(QT_ANY_DIR ${Qt5_DIR})
        set(PGE_USE_QT5 TRUE)
    else()
        find_package(Qt6 COMPONENTS Core QUIET) # Check is Qt available
        set(QT_ANY_DIR ${Qt6_DIR})

        if(NOT Qt6_FOUND)
            find_package(Qt5 COMPONENTS Core QUIET) # Check is Qt available
            set(QT_ANY_DIR ${Qt5_DIR})
            set(PGE_USE_QT5 TRUE)
            set(PGEFL_PREFER_QT5 TRUE) # For PGE-FL to don't let it find Qt 6
        else()
            set(PGE_USE_QT6 TRUE)
        endif()
    endif()

    if(Qt5_FOUND OR Qt6_FOUND)
        set(QT_ANY_FOUND TRUE)
    endif()

    if(QT_ANY_FOUND)
        option(PGE_ENABLE_QT "Enable build of Qt-dependent components" ${PGE_ENABLE_QT_ENABLED_BY_DEFAULT})
        option(PGE_ENABLE_STATIC_QT "Use static library deployment of Qt (Set this flag when you are using static build of Qt!)" OFF)
        message("== Qt has been found in ${QT_ANY_DIR}")
        if(Qt6_FOUND)
            get_filename_component(QT6_BINARY_DIRS "${QT_ANY_DIR}/../../bin" ABSOLUTE)
            if(UNIX AND NOT APPLE)
                list(APPEND QT6_BINARY_DIRS "/usr/lib/qt6/bin")
            endif()
        else()
            get_filename_component(QT5_BINARY_DIRS "${QT_ANY_DIR}/../../bin" ABSOLUTE)
            if(UNIX AND NOT APPLE)
                list(APPEND QT5_BINARY_DIRS "/usr/lib/qt5/bin")
            endif()
        endif()


        # Tools

        if(Qt6_FOUND)
            find_program(_QT_LRELEASE_PROGRAM NAMES lrelease lrelease-qt6 PATHS ${QT6_BINARY_DIRS})
        else()
            find_program(_QT_LRELEASE_PROGRAM NAMES lrelease lrelease-qt5 PATHS ${QT5_BINARY_DIRS})
        endif()

        if(_QT_LRELEASE_PROGRAM)
            message(STATUS "Found ${_QT_LRELEASE_PROGRAM}, locales will be compiled!")
        else()
            message(WARNING "Unable to find lrelease, locales will NOT be built!")
        endif()


        if(Qt6_FOUND)
            find_program(_QT_LUPDATE_PROGRAM NAMES lupdate lupdate-qt6 PATHS ${QT6_BINARY_DIRS})
        else()
            find_program(_QT_LUPDATE_PROGRAM NAMES lupdate lupdate-qt5 PATHS ${QT5_BINARY_DIRS})
        endif()

        if(_QT_LUPDATE_PROGRAM)
            message(STATUS "Found ${_QT_LUPDATE_PROGRAM}, locales can be refreshed!")
        else()
            message(WARNING "Unable to find lupdate, locales can't be refreshed!")
        endif()

        # Libraries
        macro(pge_qt_find_and_define_lib QT_COMPONENT_NAME)
            if(NOT PGE_FORCE_QT5 OR Qt6_FOUND)
                set(XXX_QT_VER Qt6)
            else()
                set(XXX_QT_VER Qt5)
            endif()

            message("-- Finding Qt component: ${XXX_QT_VER}${QT_COMPONENT_NAME}")

            find_package(${XXX_QT_VER}${QT_COMPONENT_NAME} REQUIRED)
            set(QT_${QT_COMPONENT_NAME}_LIBRARIES ${${XXX_QT_VER}${QT_COMPONENT_NAME}_LIBRARIES})
            set(QT_${QT_COMPONENT_NAME}_DEFINITIONS ${${XXX_QT_VER}${QT_COMPONENT_NAME}_DEFINITIONS})
            set(QT_${QT_COMPONENT_NAME}_INCLUDE_DIRS ${${XXX_QT_VER}${QT_COMPONENT_NAME}_INCLUDE_DIRS})
            set(QT_${QT_COMPONENT_NAME}_EXECUTABLE_COMPILE_FLAGS ${${XXX_QT_VER}${QT_COMPONENT_NAME}_EXECUTABLE_COMPILE_FLAGS})
            unset(XXX_QT_VER)
        endmacro()

        pge_qt_find_and_define_lib(Core)
        pge_qt_find_and_define_lib(Gui)
        pge_qt_find_and_define_lib(Widgets)
        pge_qt_find_and_define_lib(Network)
        pge_qt_find_and_define_lib(Concurrent)
        pge_qt_find_and_define_lib(Qml)
        pge_qt_find_and_define_lib(LinguistTools)
        if(WIN32)
            pge_qt_find_and_define_lib(WinExtras)
        endif()
    else()
        message("== Qt 5/6 NOT FOUND! Qt-dependent components will be disabled!")
    endif()
else()
    set(PGE_ENABLE_QT OFF)
endif()

macro(pge_qt_wrap_ui)
    if(Qt6_FOUND)
        qt6_wrap_ui(${ARGN})
    else()
        qt5_wrap_ui(${ARGN})
    endif()
endmacro()

macro(pge_qt_add_resources)
    if(Qt6_FOUND)
        qt6_add_resources(${ARGN})
    else()
        qt5_add_resources(${ARGN})
    endif()
endmacro()

macro(pge_qt_add_translation)
    if(Qt6_FOUND)
        qt6_add_translation(${ARGN})
    else()
        qt5_add_translation(${ARGN})
    endif()
endmacro()
