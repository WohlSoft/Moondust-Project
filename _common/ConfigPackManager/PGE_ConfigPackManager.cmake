# message("Path to ConfigPackManager is [${CMAKE_CURRENT_LIST_DIR}]")
include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

set(CONFIG_PACK_MANAGER_SRCS)

list(APPEND CONFIG_PACK_MANAGER_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/image_size.h
    ${CMAKE_CURRENT_LIST_DIR}/image_size.cpp

    ${CMAKE_CURRENT_LIST_DIR}/base/config_base.h
    ${CMAKE_CURRENT_LIST_DIR}/base/config_base.cpp

    ${CMAKE_CURRENT_LIST_DIR}/level/config_npc.h
    ${CMAKE_CURRENT_LIST_DIR}/level/config_npc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/level/config_block.h
    ${CMAKE_CURRENT_LIST_DIR}/level/config_block.cpp
    ${CMAKE_CURRENT_LIST_DIR}/level/config_bg.h
    ${CMAKE_CURRENT_LIST_DIR}/level/config_bg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/level/config_bgo.h
    ${CMAKE_CURRENT_LIST_DIR}/level/config_bgo.cpp

    ${CMAKE_CURRENT_LIST_DIR}/world/config_wld_generic.h
    ${CMAKE_CURRENT_LIST_DIR}/world/config_wld_generic.cpp
)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set_source_files_properties(${CONFIG_PACK_MANAGER_SRCS} COMPILE_FLAGS -Werror=shadow)
endif()
