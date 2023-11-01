QT       += core gui widgets
CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../../../_common/qt-modules/font_setup_widget/

SOURCES += \
    ../../../_common/qt-modules/font_setup_widget/font_setup_widget.cpp \
    main.cpp \
    fontsetuptest.cpp

HEADERS += \
    ../../../_common/qt-modules/font_setup_widget/font_setup_widget.h \
    fontsetuptest.h

FORMS += \
    fontsetuptest.ui
