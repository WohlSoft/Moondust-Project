include_directories(${CMAKE_CURRENT_LIST_DIR})

set(MSG_BOX_PREVIEW_SRC
    ${CMAKE_CURRENT_LIST_DIR}/msg_box_preview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/msg_box_preview.h

    ${CMAKE_CURRENT_LIST_DIR}/msg_macros.cpp
    ${CMAKE_CURRENT_LIST_DIR}/msg_macros.h
)

set(MSG_BOX_PREVIEW_DEFS -DMOONDUST_MSG_MACROS_ERROR_HANDLING)
