
include($$PWD/../../_common/dest_dir.pri)

TEMPLATE = subdirs
CONFIG  -= app_bundle

SUBDIRS += libogg

SUBDIRS += libvorbis
libvorbis.depends = libogg

SUBDIRS += libFLAC
libFLAC.depends = libogg

SUBDIRS += libmad
