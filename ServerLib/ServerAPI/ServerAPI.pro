#-------------------------------------------------
#
# Project created by QtCreator 2015-04-12T18:04:18
#
#-------------------------------------------------

QT       += gui
QT       += widgets
QT       += network

TARGET = ServerAPI
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++11

DEFINES += LIBPGEServerAPI

DESTDIR = $$PWD/../../_Libs/_builds/commonlibs

include(../../_common/build_props.pri)

SOURCES += \
    packetV2/packets/packetcursor.cpp

HEADERS += \
    packetV2/pgepacketregister.h \
    packetV2/pgepacketgloabls.h \
    packetV2/packets/packet.h \
    packetV2/packets/packetcursor.h

OTHER_FILES += \
    PacketInfo.txt


