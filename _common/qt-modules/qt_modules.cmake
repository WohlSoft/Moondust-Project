include_directories(${CMAKE_CURRENT_LIST_DIR})
include(${CMAKE_CURRENT_LIST_DIR}/file_list_browser/file_list_browser.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/custom_music_setup/custom_music_setup.cmake)

set(MOONDUST_QT_MODULES_SRC
    ${FILE_LIST_BROWSER_SRC}
    ${CUSTOM_MUSIC_SETUP_SRC}
)

set(MOONDUST_QT_MODULES_UI
    ${FILE_LIST_BROWSER_UI}
    ${CUSTOM_MUSIC_SETUP_UI}
)

set(MOONDUST_QT_MODULES_QRC
    ${CMAKE_CURRENT_LIST_DIR}/qt_modules.qrc
)
