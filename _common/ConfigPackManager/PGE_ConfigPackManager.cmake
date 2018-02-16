# message("Path to ConfigPackManager is [${CMAKE_CURRENT_LIST_DIR}]")
include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

set(CONFIG_PACK_MANAGER_SRCS)

list(APPEND CONFIG_PACK_MANAGER_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/image_size.cpp
    ${CMAKE_CURRENT_LIST_DIR}/level/config_npc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/level/config_block.cpp
    ${CMAKE_CURRENT_LIST_DIR}/level/config_bg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/level/config_bgo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/world/config_wld_generic.cpp
)

