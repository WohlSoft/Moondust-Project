
include($$PWD/sqlite3/libsqlite.pri)
include($$PWD/sqlite3/libsqlite_src.pri)

include($$PWD/../_common/strip_garbage.pri)
include($$PWD/../_common/lib_destdir.pri)
TARGET = sqlite3
include($$PWD/../_common/build_props.pri)
DESTDIR = $$PWD/_builds/$$TARGETOS/lib

TRANSLATIONS = ""

LIBS            += -L$$PWD/_builds/$$TARGETOS/lib
INCLUDEPATH     += $$PWD/_builds/$$TARGETOS/include
SQLite3H.path    = $$PWD/_builds/$$TARGETOS/include
SQLite3H.files  += $$PWD/sqlite3/src-amalgamation/sqlite3.h

INSTALLS = SQLite3H

