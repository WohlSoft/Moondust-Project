QT += core
QT -= gui

TARGET = H_Generators
CONFIG += console
CONFIG -= app_bundle

include(../../_common/dest_dir.pri)
include(../../_common/build_props.pri)

TEMPLATE = app

SOURCES += main.cpp

