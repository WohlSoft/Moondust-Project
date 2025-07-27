QT = core

CONFIG += c++11 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += MOONDUST_UNIT_TEST

INCLUDEPATH += ../../../_common/qt-modules/msg_box_preview/

HEADERS += \
        ../../../_common/qt-modules/msg_box_preview/msg_preprocessor.h

SOURCES += \
        main.cpp \
        ../../../_common/qt-modules/msg_box_preview/msg_preprocessor.cpp
