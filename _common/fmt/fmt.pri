#
# FMT - is an open-source formatting library for C++. It can be used as a safe alternative to printf or as a fast alternative to IOStreams.
# https://github.com/fmtlib/fmt/
#

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/fmt_format.h \
    $$PWD/fmt_ostream.h \
    $$PWD/fmt_posix.h \
    $$PWD/fmt_printf.h \
    $$PWD/fmt_string.h \
    $$PWD/fmt_time.h

SOURCES += \
    $$PWD/fmt_format.cpp \
    $$PWD/fmt_ostream.cpp \
    $$PWD/fmt_posix.cpp \
    $$PWD/fmt_printf.cpp

