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

SOURCES += \
    ../../../Editor/common_features/json_settings_widget.cpp \
    ../../../Editor/editing/_dialogs/file_list_browser.cpp \
    ../../../Editor/editing/_dialogs/levelfilelist.cpp \
    ../../../Editor/editing/_dialogs/musicfilelist.cpp \
    main.cpp

HEADERS += \
    ../../../Editor/common_features/json_settings_widget.h \ \
    ../../../Editor/editing/_dialogs/file_list_browser.h \
    ../../../Editor/editing/_dialogs/levelfilelist.h \
    ../../../Editor/editing/_dialogs/musicfilelist.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    ../../../Editor/editing/_dialogs/file_list_browser.ui
