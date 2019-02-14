# FIXME: Remove this ugly workaround when implement proper recursive search per Qt directory
set(POSSIBLE_QT_PATHS
        "~/Qt/5.12.3/gcc_64/bin"
        "~/Qt/5.12.2/gcc_64/bin"
        "~/Qt/5.12.1/gcc_64/bin"
        "~/Qt/5.12.0/gcc_64/bin"
        "~/Qt/5.12/gcc_64/bin"
        "/opt/Qt/5.12.3/gcc_64/bin"
        "/opt/Qt/5.12.2/gcc_64/bin"
        "/opt/Qt/5.12.1/gcc_64/bin"
        "/opt/Qt/5.12.0/gcc_64/bin"
        "/opt/Qt/5.12/gcc_64/bin"
        "/usr/local/bin/"
        "/usr/bin/"
        "C:/Qt/5.12.3/mingw730_64/bin"
        "C:/Qt/5.12.2/mingw730_64/bin"
        "C:/Qt/5.12.1/mingw730_64/bin"
        "C:/Qt/5.12.0/mingw730_64/bin"
        "C:/Qt/5.12/mingw730_64/bin"
        "C:/Qt/5.11.3/mingw530_32/bin"
        "C:/Qt/5.11.2/mingw530_32/bin"
        "C:/Qt/5.11.1/mingw530_32/bin"
        "C:/Qt/5.11.0/mingw530_32/bin"
        "C:/Qt/5.11/mingw530_32/bin"
)

find_program(_QT_LRELEASE_PROGRAM lrelease NO_DEFAULT_PATH PATHS ${POSSIBLE_QT_PATHS})
if(_QT_LRELEASE_PROGRAM)
    message(STATUS "Found ${_QT_LRELEASE_PROGRAM}, locales will be compiled!")
else()
    message(WARNING "Unable to find lrelease, locales will NOT be built!")
endif()

find_program(_QT_LUPDATE_PROGRAM lupdate NO_DEFAULT_PATH PATHS ${POSSIBLE_QT_PATHS})
if(_QT_LUPDATE_PROGRAM)
    message(STATUS "Found ${_QT_LUPDATE_PROGRAM}, locales can be refreshed!")
else()
    message(WARNING "Unable to find lupdate, locales can't be refreshed!")
endif()
