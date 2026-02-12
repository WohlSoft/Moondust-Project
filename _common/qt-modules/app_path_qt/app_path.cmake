# ---- Example values to define prior include ----
#set(PGE_APP_PATH_MODULE_NAME "PGE Editor")
#set(PGE_APP_PATH_SETUP_FILE_NAME "pge_editor.ini")
#set(PGE_APP_PATH_VERSION_HEADER "../../Editor/version.h")
#set(PGE_APP_PATH_HAS_THEMES TRUE)
#set(PGE_APP_PATH_TARGET_SUFFIX Editor)


if(NOT PGE_APP_PATH_TARGET_SUFFIX)
    message(FATAL_ERROR "Target name for app path module PGE_APP_PATH_TARGET_SUFFIX= is undefined!")
endif()

add_library(PGE_AppPath${PGE_APP_PATH_TARGET_SUFFIX}Qt INTERFACE)

target_sources(PGE_AppPath${PGE_APP_PATH_TARGET_SUFFIX}Qt
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pge_app_path.h
    ${CMAKE_CURRENT_LIST_DIR}/pge_app_path.cpp
)

target_include_directories(PGE_AppPath${PGE_APP_PATH_TARGET_SUFFIX}Qt
    INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}"
)

target_compile_definitions(PGE_AppPath${PGE_APP_PATH_TARGET_SUFFIX}Qt
    INTERFACE
    -DPGE_APP_PATH_MODULE_NAME="${PGE_APP_PATH_MODULE_NAME}"
    -DPGE_APP_PATH_SETUP_FILE_NAME="${PGE_APP_PATH_SETUP_FILE_NAME}"
    -DPGE_APP_PATH_VERSION_HEADER="${PGE_APP_PATH_VERSION_HEADER}"
)

if(PGE_APP_PATH_HAS_THEMES)
    target_compile_definitions(PGE_AppPath${PGE_APP_PATH_TARGET_SUFFIX}Qt
        INTERFACE
        -DPGE_APP_PATH_HAS_THEMES
    )
endif()
