
include($$PWD/../../_common/dest_dir.pri)

TEMPLATE = subdirs
CONFIG  -= app_bundle

SUBDIRS += zlib
SUBDIRS += libogg
SUBDIRS += libvorbis
SUBDIRS += libFLAC
libvorbis.depends = libogg
libFLAC.depends = libogg
SUBDIRS += libmad
SUBDIRS += libid3tag
SUBDIRS += libgme
libgme.depends = zlib
SUBDIRS += libmodplug
SUBDIRS += libADLMIDI
SUBDIRS += libOPNMIDI
SUBDIRS += libtimidity
