#
# FMT - is an open-source formatting library for C++. It can be used as a safe alternative to printf or as a fast alternative to IOStreams.
# https://github.com/fmtlib/fmt/
#

INCLUDEPATH += $$PWD

# Disable the exceptions (I don't want any extra try/catch blocks, there are ugly!)
# and because most of fetures here are hardcoded, no way user will provide a junk format string!
DEFINES += FMT_NOEXCEPT=

HEADERS += \
    $$PWD/fmt_format.h \
    $$PWD/fmt_ostream.h \
    $$PWD/fmt_posix.h \
    $$PWD/fmt_printf.h \
    $$PWD/fmt_string.h \
    $$PWD/fmt_time.h \
    $$PWD/fmt_qformat.h

SOURCES += \
    $$PWD/fmt_format.cpp \
    $$PWD/fmt_ostream.cpp \
    $$PWD/fmt_posix.cpp \
    $$PWD/fmt_printf.cpp

