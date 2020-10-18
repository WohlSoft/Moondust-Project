# message("Path to UtilsSDL is [${CMAKE_CURRENT_LIST_DIR}]")
include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

set(UTILS_QT_SRCS)

list(APPEND UTILS_QT_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/dir_list_ci_qt.cpp
)

