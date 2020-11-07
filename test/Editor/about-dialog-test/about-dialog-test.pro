QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../Editor/main_window/about_dialog/aboutdialog.cpp \
    main.cpp

HEADERS += \
    ../../../Editor/main_window/about_dialog/aboutdialog.h

FORMS += \
    ../../../Editor/main_window/about_dialog/aboutdialog.ui

INCLUDEPATH += /usr/include/x86_64-linux-gnu/SDL2
LIBS += -lSDL2 -lSDL2_mixer_ext

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../../../Editor/_resources/editor.qrc
