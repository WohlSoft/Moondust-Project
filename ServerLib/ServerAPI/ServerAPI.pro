#-------------------------------------------------
#
# Project created by QtCreator 2015-04-12T18:04:18
#
#-------------------------------------------------

QT       -= gui
QT       += network

TARGET = ServerAPI
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++11

SOURCES += \
    pgeconnection.cpp \
    pgeclient.cpp \
    pgeserver.cpp \
    pgeserver_p.cpp \
    packet/packet.cpp \
    packet/predefined/packethandshake.cpp

HEADERS += \
    packet.h \
    pgesocketdefines.h \
    pgeconnection.h \
    pgeclient.h \
    pgeserver.h \
    pgeconnecteduser.h \
    packet/packet.h \
    packet/predefined/packethandshake.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    PacketInfo.txt
