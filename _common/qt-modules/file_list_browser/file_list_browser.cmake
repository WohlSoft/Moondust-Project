set(FILE_LIST_BROWSER_SRC
    # Main class
    ${CMAKE_CURRENT_LIST_DIR}/file_list_browser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_list_browser.h

    # Level file lists
    ${CMAKE_CURRENT_LIST_DIR}/levelfilelist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/levelfilelist.h

    # Music file lists
    ${CMAKE_CURRENT_LIST_DIR}/musicfilelist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/musicfilelist.h

    # OPL/OPN Bank lists
    ${CMAKE_CURRENT_LIST_DIR}/bankfilelist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bankfilelist.h
)

set(FILE_LIST_BROWSER_UI
    ${CMAKE_CURRENT_LIST_DIR}/file_list_browser.ui
)
