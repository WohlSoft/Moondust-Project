add_library(PGE_AppPathQt INTERFACE)

target_sources(PGE_AppPathQt
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pge_app_path.h
    ${CMAKE_CURRENT_LIST_DIR}/pge_app_path.cpp
)

target_include_directories(PGE_AppPathQt
    INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}"
)

#set(PGE_APP_PATH_MODULE_NAME "PGE Editor")
#set(PGE_APP_PATH_SETUP_FILE_NAME "pge_editor.ini")
#set(PGE_APP_PATH_VERSION_HEADER "../../Editor/version.h")

target_compile_definitions(PGE_AppPathQt
    INTERFACE
    -DPGE_APP_PATH_MODULE_NAME="${PGE_APP_PATH_MODULE_NAME}"
    -DPGE_APP_PATH_SETUP_FILE_NAME="${PGE_APP_PATH_SETUP_FILE_NAME}"
    -DPGE_APP_PATH_VERSION_HEADER="${PGE_APP_PATH_VERSION_HEADER}"
)

if(PGE_APP_PATH_HAS_THEMES)
    target_compile_definitions(PGE_AppPathQt
        INTERFACE
        -DPGE_APP_PATH_HAS_THEMES
    )
endif()
