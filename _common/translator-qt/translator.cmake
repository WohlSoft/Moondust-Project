# message("Path to Translator is [${CMAKE_CURRENT_LIST_DIR}]")
include_directories(${CMAKE_CURRENT_LIST_DIR})

set(PGETRANSLATOR_SRCS)

list(APPEND PGETRANSLATOR_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/translator.cpp
)

