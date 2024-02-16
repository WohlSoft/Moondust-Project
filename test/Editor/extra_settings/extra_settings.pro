QT       += core gui widgets concurrent

INCLUDEPATH += ../../../Editor

CONFIG += c++11

DEFINES += DEBUG_BUILD UNIT_TEST

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    ../../../_common/qt-modules/compat \
    ../../../_common/qt-modules/

SOURCES += \
    ../../../Editor/common_features/json_settings_widget.cpp \
    ../../../_common/qt-modules/custom_music_setup/custom_music_setup.cpp \
    ../../../_common/qt-modules/file_list_browser/bankfilelist.cpp \
    ../../../_common/qt-modules/file_list_browser/file_list_browser.cpp \
    ../../../_common/qt-modules/file_list_browser/file_list_model.cpp \
    ../../../_common/qt-modules/file_list_browser/levelfilelist.cpp \
    ../../../_common/qt-modules/file_list_browser/musicfilelist.cpp \
    main.cpp

HEADERS += \
    ../../../Editor/common_features/json_settings_widget.h \
    ../../../_common/qt-modules/custom_music_setup/custom_music_setup.h \
    ../../../_common/qt-modules/file_list_browser/bankfilelist.h \
    ../../../_common/qt-modules/file_list_browser/file_list_browser.h \
    ../../../_common/qt-modules/file_list_browser/file_list_model.h \
    ../../../_common/qt-modules/file_list_browser/levelfilelist.h \
    ../../../_common/qt-modules/file_list_browser/musicfilelist.h

FORMS += \
    ../../../_common/qt-modules/custom_music_setup/custom_music_setup.ui \
    ../../../_common/qt-modules/file_list_browser/file_list_browser.ui
