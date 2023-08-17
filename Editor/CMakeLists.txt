cmake_minimum_required (VERSION 3.5)
project(MoondustEditor C CXX)

if(POLICY CMP0071) # Automoc
    cmake_policy(SET CMP0071 NEW)
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_INSTALL_RPATH ".")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(DEPENDENCIES_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/_includes
    ${DEPENDENCIES_INSTALL_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../_common/ # PGE-FL
)
link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message("== Using default build configuration which is a Release!")
endif()

# Version
include(version.cmake)
# Default GIT version
include(../_common/git_version.cmake)
# Common building properties and optimization flags
include(../_common/build_props.cmake)
# Build properties for executable projects
include(../_common/executable_build_props.cmake)
# Find extra libraries to link when using static Qt (use ${QT_EXTRA_LIBS}!)
include(../_common/static_qt.cmake)

# include shared code modules
include(../_common/ConfigPackManager/PGE_ConfigPackManager.cmake)
include(../_common/DirManager/dirman.cmake)
add_definitions(-DINI_PROCESSING_ALLOW_QT_TYPES)
include(../_common/IniProcessor/IniProcessor.cmake)
include(../_common/tclap/tclap.cmake)
include(../_common/Utils/Utils.cmake)
include(../_common/Utils/UtilsQt.cmake)
include(../_common/Graphics/bitmask2rgba.cmake)
include(../_common/qt-modules/qt_modules.cmake)
include(../_common/qt-modules/compat/pge_qt_compat.cmake)
if(WIN32)
    include(../_common/StackWalker/StackWalker.cmake)
endif()

if(PGE_USE_QT5)
    pge_cxx_standard(14)
else()
    pge_cxx_standard(17)
endif()

set(QT_LIBS_LIST
    ${QT_Qml_LIBRARIES}
    ${QT_Concurrent_LIBRARIES}
    ${QT_Widgets_LIBRARIES}
    ${QT_Network_LIBRARIES}
    ${QT_Gui_LIBRARIES}
    ${QT_Core_LIBRARIES}
)

set(QT_DEFS_LIST
    ${QT_Core_DEFINITIONS}
    ${QT_Gui_DEFINITIONS}
    ${QT_Widgets_DEFINITIONS}
    ${QT_Network_DEFINITIONS}
    ${QT_Concurrent_DEFINITIONS}
    ${QT_Qml_DEFINITIONS}
)

set(QT_INCLUDES_LIST
    ${QT_Core_INCLUDE_DIRS}
    ${QT_Gui_INCLUDE_DIRS}
    ${QT_Widgets_INCLUDE_DIRS}
    ${QT_Network_INCLUDE_DIRS}
    ${QT_Concurrent_INCLUDE_DIRS}
    ${QT_Qml_INCLUDE_DIRS}
)

set(QT_EXECUTABLE_CXX_FLAGS
    ${QT_Core_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Gui_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Widgets_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Network_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Concurrent_EXECUTABLE_COMPILE_FLAGS}
    ${QT_Qml_EXECUTABLE_COMPILE_FLAGS}
)


if(WIN32)
    list(APPEND QT_LIBS_LIST ${QT_WinExtras_LIBRARIES})
    list(APPEND QT_DEFS_LIST ${QT_WinExtras_DEFINITIONS})
    list(APPEND QT_INCLUDES_LIST ${QT_WinExtras_INCLUDE_DIRS})
    list(APPEND QT_EXECUTABLE_CXX_FLAGS ${QT_WinExtras_EXECUTABLE_COMPILE_FLAGS})
endif()

if(QT_DISABLE_NATIVE_FILE_DIALOG) # Disable native file dialogs when locally-built libPNG was been used to avoid the crash
    message("== Editor: Native file dialogs will be DISABLED")
    add_definitions(-DQT_DONT_USE_NATIVE_FILE_DIALOG)
else()
    message("== Editor: Native file dialogs will be ALLOWED if possible")
endif()

# =============== Translations ===============
set(_LANGAUGES_TEMP_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/languages")
set(PGE_LANG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/languages")

FILE(GLOB PGE_LANG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/languages/*.ts")

set_source_files_properties(${PGE_LANG_SRC_FILES} PROPERTIES OUTPUT_LOCATION ${PGE_LANG_OUTPUT_DIR})
set(PGE_LANG_BIN_FILES)
pge_qt_add_translation(PGE_LANG_BIN_FILES ${PGE_LANG_SRC_FILES})
add_custom_target(editor_translations ALL DEPENDS ${PGE_LANG_BIN_FILES})

if(NOT APPLE AND NOT EMSCRIPTEN)
    install(FILES ${PGE_LANG_BIN_FILES} DESTINATION "${PGE_INSTALL_DIRECTORY}/languages")
endif()
# ===========================================

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QT_EXECUTABLE_CXX_FLAGS}")

set(PGE_EDITOR_UI)

list(APPEND PGE_EDITOR_UI
    common_features/crashhandler.ui
    data_configs/config_status/config_status.ui
    data_configs/selection_dialog/config_manager.ui
    dev_console/devconsole.ui
    editing/_dialogs/itemselectdialog.ui
    editing/_dialogs/savingnotificationdialog.ui
    editing/_dialogs/user_data_edit.ui
    editing/_dialogs/wld_setpoint.ui
    editing/edit_level/leveledit.ui
    editing/edit_level/levelprops.ui
    editing/edit_level/lvl_clone_section.ui
    editing/edit_npc/npcedit.ui
    editing/edit_world/world_edit.ui
    editing/_scenes/level/itemmsgbox.ui
    editing/_scenes/level/tonewlayerbox.ui
    main_window/about_dialog/aboutdialog.ui
    main_window/dock/bookmarks_box.ui
    main_window/dock/debugger.ui
    main_window/dock/lvl_events_box.ui
    main_window/dock/lvl_item_properties.ui
    main_window/dock/lvl_item_toolbox.ui
    main_window/dock/lvl_layers_box.ui
    main_window/dock/lvl_sctc_props.ui
    main_window/dock/lvl_search_box.ui
    main_window/dock/lvl_warp_props.ui
    main_window/dock/tileset_item_box.ui
    main_window/dock/variables_box.ui
    main_window/dock/wld_item_props.ui
    main_window/dock/wld_item_toolbox.ui
    main_window/dock/wld_musicbox_itemset.ui
    main_window/dock/wld_search_box.ui
    main_window/dock/wld_settings_box.ui
    main_window/file/lvl_export_image.ui
    main_window/file/wld_export_image.ui
    main_window/greeting_dialog/greeting_dialog.ui
    main_window/plugins/pge_editorplugininfo.ui
    main_window/script/script_editor.ui
    main_window/testing/testing_settings.ui
    main_window/tip_of_day/tip_of_day.ui
    main_window/tools/app_settings.ui
    mainwindow.ui
    main_window/updater/check_updates.ui
    tools/debugger/custom_counter_gui.ui
    tools/external_tools/audiocvt_sox_gui.ui
    tools/math/blocksperseconddialog.ui
    tools/tilesets/tileset_editor.ui
    tools/tilesets/tilesetgroupeditor.ui
    ${MOONDUST_QT_MODULES_UI}
)

set(RESOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/appicon/appicon.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/arrows/arrows.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/contributors/contributors.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/dock/dock.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/dummies/dummies.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/engines/engines.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/files/files.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/fonts/fonts.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/lunalua/lunalua.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/npc/npc.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/sections/sections.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/tips/tips.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/toolbar/toolbar.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/_resources/editor.qrc
)

if(UNIX AND NOT APPLE)
    list(APPEND RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/_resources/files/files-linux.qrc)
endif()

pge_qt_add_resources(RESOURCE_ADDED
    ${RESOURCE}
    ${MOONDUST_QT_MODULES_QRC}
)

if(QT_IMPORT_PLUGINS_MODULE)
    set(QT_PLUGINS_CPP "${CMAKE_BINARY_DIR}/pge_editor_plugins.cpp")
    file(WRITE
            "${QT_PLUGINS_CPP}"
            "${QT_IMPORT_PLUGINS_MODULE}")
endif()

set(PGE_EDITOR_SRCS)

list(APPEND PGE_EDITOR_SRCS
    audio/music_player.cpp
    audio/sdl_music_player.cpp
    ../_common/data_functions/smbx64_validation_messages.cpp
    common_features/animation_timer.cpp
    common_features/app_path.cpp
    common_features/bool_reseter.cpp
    common_features/crashhandler.cpp
    common_features/dir_copy.cpp
    common_features/direction_switch_widget.cpp
    common_features/edit_mode_base.cpp
    common_features/file_keeper.cpp
    common_features/flowlayout.cpp
    common_features/graphics_funcs.cpp
    common_features/graphicsworkspace.cpp
    common_features/grid.cpp
    common_features/installer.cpp
    common_features/image_selector.cpp
    common_features/item_rectangles.cpp
    common_features/items.cpp
    common_features/json_settings_widget.cpp
    common_features/logger_sets.cpp
    common_features/main_window_ptr.cpp
    common_features/npc_animator.cpp
    common_features/proxystyle.cpp
    common_features/resizer/corner_grabber.cpp
    common_features/resizer/item_resizer.cpp
    common_features/safe_msg_box.cpp
    common_features/simple_animator.cpp
    common_features/spash_screen.cpp
    common_features/themes.cpp
    common_features/util.cpp
    common_features/version_cmp.cpp
    data_configs/config_status/config_status.cpp
    data_configs/conf_music.cpp
    data_configs/conf_rotation_tables.cpp
    data_configs/conf_sound.cpp
    data_configs/conf_wld_exit_codes.cpp
    data_configs/conf_wld_level.cpp
    data_configs/conf_wld_path.cpp
    data_configs/conf_wld_scene.cpp
    data_configs/conf_wld_tile.cpp
    data_configs/custom_data.cpp
    data_configs/data_configs.cpp
    data_configs/obj_BG.cpp
    data_configs/obj_bgo.cpp
    data_configs/obj_block.cpp
    data_configs/obj_npc.cpp
    data_configs/obj_player.cpp
    data_configs/obj_tilesets.cpp
    data_configs/selection_dialog/config_manager.cpp
    dev_console/devconsole.cpp
    editing/_components/history/historyelementaddlayer.cpp
    editing/_components/history/historyelementaddwarp.cpp
    editing/_components/history/historyelementchangednewlayer.cpp
    editing/_components/history/historyelementitemsetting.cpp
    editing/_components/history/historyelementlayerchanged.cpp
    editing/_components/history/historyelementmainsetting.cpp
    editing/_components/history/historyelementmergelayer.cpp
    editing/_components/history/historyelementmodification.cpp
    editing/_components/history/historyelementmodifyevent.cpp
    editing/_components/history/historyelementnewlayer.cpp
    editing/_components/history/historyelementplacedoor.cpp
    editing/_components/history/historyelementremovelayerandsave.cpp
    editing/_components/history/historyelementremovelayer.cpp
    editing/_components/history/historyelementremovewarp.cpp
    editing/_components/history/historyelementrenameevent.cpp
    editing/_components/history/historyelementrenamelayer.cpp
    editing/_components/history/historyelementreplaceplayerpoint.cpp
    editing/_components/history/historyelementresizeblock.cpp
    editing/_components/history/historyelementresizesection.cpp
    editing/_components/history/historyelementresizewater.cpp
    editing/_components/history/historyelementsettingsevent.cpp
    editing/_components/history/historyelementsettingssection.cpp
    editing/_components/history/historyelementsettingswarp.cpp
    editing/_components/history/ihistoryelement.cpp
    editing/_components/history/itemsearcher.cpp
    editing/_dialogs/itemselectdialog.cpp
    editing/_dialogs/savingnotificationdialog.cpp
    editing/_dialogs/user_data_edit.cpp
    editing/_dialogs/wld_setpoint.cpp
    editing/edit_base.cpp
    editing/edit_level/level_edit.cpp
    editing/edit_level/levelprops.cpp
    editing/edit_level/lvl_clone_section.cpp
    editing/edit_level/lvl_draw.cpp
    editing/edit_level/lvl_edit_control.cpp
    editing/edit_level/lvl_files_io.cpp
    editing/edit_npc/npc_data_sets.cpp
    editing/edit_npc/npcedit.cpp
    editing/edit_npc/npceditscene.cpp
    editing/edit_npc/npctxt_controls.cpp
    editing/edit_npc/npctxt_files_io.cpp
    editing/edit_world/wld_draw.cpp
    editing/edit_world/wld_edit_control.cpp
    editing/edit_world/wld_files_io.cpp
    editing/edit_world/world_edit.cpp
    editing/_scenes/level/data_manager/lvl_animators.cpp
    editing/_scenes/level/data_manager/lvl_custom_gfx.cpp
    editing/_scenes/level/edit_modes/mode_circle.cpp
    editing/_scenes/level/edit_modes/mode_erase.cpp
    editing/_scenes/level/edit_modes/mode_fill.cpp
    editing/_scenes/level/edit_modes/mode_hand.cpp
    editing/_scenes/level/edit_modes/mode_line.cpp
    editing/_scenes/level/edit_modes/mode_place.cpp
    editing/_scenes/level/edit_modes/mode_resize.cpp
    editing/_scenes/level/edit_modes/mode_select.cpp
    editing/_scenes/level/edit_modes/mode_square.cpp
    editing/_scenes/level/itemmsgbox.cpp
    editing/_scenes/level/items/item_bgo.cpp
    editing/_scenes/level/items/item_block.cpp
    editing/_scenes/level/items/item_door.cpp
    editing/_scenes/level/items/item_npc.cpp
    editing/_scenes/level/items/item_playerpoint.cpp
    editing/_scenes/level/items/item_water.cpp
    editing/_scenes/level/items/lvl_base_item.cpp
    editing/_scenes/level/items/lvl_items_aligning.cpp
    editing/_scenes/level/items/lvl_items_modify.cpp
    editing/_scenes/level/items/lvl_items_place.cpp
    editing/_scenes/level/lvl_clipboard.cpp
    editing/_scenes/level/lvl_collisions.cpp
    editing/_scenes/level/lvl_control.cpp
    editing/_scenes/level/lvl_history_manager.cpp
    editing/_scenes/level/lvl_init_filedata.cpp
    editing/_scenes/level/lvl_item_placing.cpp
    editing/_scenes/level/lvl_scene.cpp
    editing/_scenes/level/lvl_section.cpp
    editing/_scenes/level/lvl_setup.cpp
    editing/_scenes/level/newlayerbox.cpp
    editing/_scenes/level/resizers/lvl_resizer_block.cpp
    editing/_scenes/level/resizers/lvl_resizer_img_shoot.cpp
    editing/_scenes/level/resizers/lvl_resizer_physenv.cpp
    editing/_scenes/level/resizers/lvl_resizers.cpp
    editing/_scenes/level/resizers/lvl_resizer_section.cpp
    editing/_scenes/level/resizers/lvl_resizer_section_evnt.cpp
    editing/_scenes/world/data_manager/wld_animators.cpp
    editing/_scenes/world/data_manager/wld_custom_gfx.cpp
    editing/_scenes/world/edit_modes/wld_mode_circle.cpp
    editing/_scenes/world/edit_modes/wld_mode_erase.cpp
    editing/_scenes/world/edit_modes/wld_mode_fill.cpp
    editing/_scenes/world/edit_modes/wld_mode_hand.cpp
    editing/_scenes/world/edit_modes/wld_mode_line.cpp
    editing/_scenes/world/edit_modes/wld_mode_place.cpp
    editing/_scenes/world/edit_modes/wld_mode_resize.cpp
    editing/_scenes/world/edit_modes/wld_mode_select.cpp
    editing/_scenes/world/edit_modes/wld_mode_setpoint.cpp
    editing/_scenes/world/edit_modes/wld_mode_square.cpp
    editing/_scenes/world/items/item_level.cpp
    editing/_scenes/world/items/item_music.cpp
    editing/_scenes/world/items/item_path.cpp
    editing/_scenes/world/items/item_point.cpp
    editing/_scenes/world/items/item_scene.cpp
    editing/_scenes/world/items/item_tile.cpp
    editing/_scenes/world/items/wld_base_item.cpp
    editing/_scenes/world/items/wld_items_aligning.cpp
    editing/_scenes/world/items/wld_items_modify.cpp
    editing/_scenes/world/items/wld_items_place.cpp
    editing/_scenes/world/resizers/wld_resizer_img_shoot.cpp
    editing/_scenes/world/resizers/wld_resizers.cpp
    editing/_scenes/world/wld_clipboard.cpp
    editing/_scenes/world/wld_collisions.cpp
    editing/_scenes/world/wld_control.cpp
    editing/_scenes/world/wld_history_manager.cpp
    editing/_scenes/world/wld_init_filedata.cpp
    editing/_scenes/world/wld_item_placing.cpp
    editing/_scenes/world/wld_point_selector.cpp
    editing/_scenes/world/wld_scene.cpp
    editing/_scenes/world/wld_setup.cpp
    js_engine/pge_jsengine.cpp
    js_engine/proxies/js_common.cpp
    js_engine/proxies/js_file.cpp
    js_engine/proxies/js_ini.cpp
    js_engine/proxies/js_lua_preproc.cpp
    js_engine/proxies/js_system.cpp
    main.cpp
    main_window/about_dialog/aboutdialog.cpp
    mainwindow.cpp
    main_window/dock/bookmarks_box.cpp
    main_window/dock/debugger.cpp
    main_window/dock/_dock_vizman.cpp
    main_window/dock/itembox_list_model.cpp
    main_window/dock/lvl_events_box.cpp
    main_window/dock/lvl_item_properties.cpp
    main_window/dock/lvl_item_toolbox.cpp
    main_window/dock/lvl_layers_box.cpp
    main_window/dock/lvl_sctc_props.cpp
    main_window/dock/lvl_search_box.cpp
    main_window/dock/lvl_warp_props.cpp
    main_window/dock/mwdock_base.cpp
    main_window/dock/tileset_item_box.cpp
    main_window/dock/variables_box.cpp
    main_window/dock/wld_item_props.cpp
    main_window/dock/wld_item_toolbox.cpp
    main_window/dock/wld_musicbox_itemset.cpp
    main_window/dock/wld_search_box.cpp
    main_window/dock/wld_settings_box.cpp
    main_window/edit/edit_clipboard.cpp
    main_window/edit/edit_history.cpp
    main_window/edit/edit_items.cpp
    main_window/edit/edit_mode.cpp
    main_window/edit/edit_mode_placing.cpp
    main_window/edit/edit_placing_switch.cpp
    main_window/edit/edit_resize.cpp
    main_window/events.cpp
    main_window/file/file_export_image.cpp
    main_window/file/file_new.cpp
    main_window/file/file_open.cpp
    main_window/file/file_recent.cpp
    main_window/file/file_reload.cpp
    main_window/file/file_save.cpp
    main_window/file/lvl_export_image.cpp
    main_window/file/wld_export_image.cpp
    main_window/global_settings.cpp
    main_window/greeting_dialog/greeting_dialog.cpp
    main_window/help.cpp
    main_window/level/level_locks.cpp
    main_window/level/level_props.cpp
    main_window/level/level_sections.cpp
    main_window/level/level_sections_mods.cpp
    main_window/mainw_plugins.cpp
    main_window/mainw_themes.cpp
    main_window/menubar.cpp
    main_window/plugins/pge_editorplugininfo.cpp
    main_window/plugins/pge_editorpluginitem.cpp
    main_window/plugins/pge_editorpluginmanager.cpp
    main_window/script/script_editor.cpp
    main_window/script/script_luafiles.cpp
    main_window/_settings/defaults.cpp
    main_window/_settings/settings_io.cpp
    main_window/sub_windows.cpp
    main_window/testing/testing.cpp
    main_window/testing/testing_settings.cpp
    main_window/tip_of_day/tip_of_day.cpp
    main_window/tools/app_settings.cpp
    main_window/tools/data_configs_mgr.cpp
    main_window/tools/main_clean_npc_gargage.cpp
    main_window/tools/main_tool_cdata_cleaner.cpp
    main_window/tools/main_tool_cdata_import.cpp
    main_window/tools/main_tool_cdata_lazyfix.cpp
    main_window/tools_menu.cpp
    main_window/translator.cpp
    main_window/updater/check_updates.cpp
    main_window/view/view.cpp
    main_window/view/view_zoom.cpp
    main_window/windows_extras.cpp
    main_window/world/world_locks.cpp
    networking/engine_intproc.cpp
    SingleApplication/editor_application.cpp
    SingleApplication/localserver.cpp
    SingleApplication/singleapplication.cpp
    testing/abstract_engine.cpp
    testing/pge_engine.cpp
    testing/luna_tester_engine.cpp
    testing/thextech_engine.cpp
    testing/38a_engine.cpp
    testing/ipc/pge_engine_ipc.cpp
    testing/ipc/luna_capabilities.cpp
    testing/ipc/38A_client.cpp
    tools/async/asyncstarcounter.cpp
    tools/debugger/custom_counter.cpp
    tools/debugger/custom_counter_gui.cpp
    tools/external_tools/audiocvt_sox_gui.cpp
    tools/math/blocksperseconddialog.cpp
    tools/smart_import/smartimporter.cpp
    tools/tilesets/piecesmodel.cpp
    tools/tilesets/tileset_editor.cpp
    tools/tilesets/tileset.cpp
    tools/tilesets/tilesetgroupeditor.cpp
    tools/tilesets/tilesetitembutton.cpp
    ${MOONDUST_QT_MODULES_SRC}
)

file(GLOB MOONDUST_EDITOR_HEADS_SRC
    *.h
    common_features/*.h
    common_features/resizer/*.h
    tools/*.h
    tools/math/*.h
    tools/smart_import/*.h
    tools/debugger/*.h
    tools/tilesets/*.h
    tools/async/*.h
    tools/external_tools/*.h
    data_configs/*.h
    data_configs/config_status/*.h
    data_configs/selection_dialog/*.h
    testing/*.h
    testing/ipc/*.h
    testing/bridges/*.h
    testing/bridges/LunaLoader/*.h
    audio/*.h
    dev_console/*.h
    SingleApplication/*.h
    main_window/*.h
    main_window/view/*.h
    main_window/greeting_dialog/*.h
    main_window/tools/*.h
    main_window/dock/*.h
    main_window/world/*.h
    main_window/testing/*.h
    main_window/file/*.h
    main_window/plugins/*.h
    main_window/level/*.h
    main_window/edit/*.h
    main_window/tip_of_day/*.h
    main_window/about_dialog/*.h
    main_window/script/*.h
    main_window/updater/*.h
    main_window/_settings/*.h
    _includes/*.h
    js_engine/*.h
    js_engine/proxies/*.h
    networking/*.h
    editing/*.h
    editing/edit_world/*.h
    editing/edit_level/*.h
    editing/_components/*.h
    editing/_components/history/*.h
    editing/_components/history/settings/*.h
    editing/_dialogs/*.h
    editing/_dialogs/_WIP/*.h
    editing/_scenes/*.h
    editing/_scenes/common/*.h
    editing/_scenes/world/*.h
    editing/_scenes/world/edit_modes/*.h
    editing/_scenes/world/data_manager/*.h
    editing/_scenes/world/resizers/*.h
    editing/_scenes/world/items/*.h
    editing/_scenes/level/*.h
    editing/_scenes/level/edit_modes/*.h
    editing/_scenes/level/data_manager/*.h
    editing/_scenes/level/resizers/*.h
    editing/_scenes/level/items/*.h
    editing/edit_npc/*.h
)

list(APPEND PGE_EDITOR_SRCS ${MOONDUST_EDITOR_HEADS_SRC})

if(NOT WIN32) # Wine setup dialog
    list(APPEND PGE_EDITOR_SRCS
        testing/wine/wine_setup.cpp
        testing/wine/wine_setup.h
        testing/wine/wine_setup_cfg.h
    )
    list(APPEND PGE_EDITOR_UI testing/wine/wine_setup.ui)
endif()

if(APPLE)
    add_definitions(-DQ_OS_MACX) # Workaround for MOC
    file(GLOB PGE_FILE_ICONS "${CMAKE_CURRENT_SOURCE_DIR}/_resources/files/file_*.icns")
    set_source_files_properties(${MOONDUST_QT_LANG_BIN_FILES} PROPERTIES GENERATED TRUE) # WORKAROUND
    list(APPEND PGE_EDITOR_SRCS
        _resources/appicon/cat_builder.icns
        ${PGE_LANG_BIN_FILES}
        ${MOONDUST_QT_LANG_BIN_FILES}
        ${MOONDUST_QT_LANG_ICON_FILES}
        ${PGE_FILE_ICONS}
    )
endif()

if(WIN32 AND NOT EMSCRIPTEN)
    list(APPEND PGE_EDITOR_SRCS _resources/pge_editor.rc)
endif()

pge_qt_wrap_ui(PGE_EDITOR_UI_HDRS ${PGE_EDITOR_UI})

list(APPEND PGE_EDITOR_SRCS
    ${PGE_EDITOR_UI_HDRS}
    ${RESOURCE_ADDED}
    ${QT_PLUGINS_CPP}
)


if(NOT HAIKU)
    # Generic folder for IPC bridge executables
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/ipc)
        make_directory(${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/ipc)
    endif()


    # 38A IPC bridge
    set(38A_IPC_BRIDGE_BIN "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/ipc/38a_ipc_bridge.exe")

    add_custom_target(38a_ipc_bridge_bin ALL
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/testing/bridges/38a_ipc_bridge.tar.gz"
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/testing/bridges/38a_ipc_bridge.tar.gz
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/ipc"
        COMMENT "Unpacking 38a_ipc_bridge.exe"
        VERBATIM
        BYPRODUCTS "${38A_IPC_BRIDGE_BIN}"
    )

    if(APPLE) # Embed into the bundle
        list(APPEND PGE_EDITOR_SRCS "${38A_IPC_BRIDGE_BIN}")
        set_source_files_properties(${38A_IPC_BRIDGE_BIN} PROPERTIES GENERATED TRUE)
    else()
        install(FILES "${38A_IPC_BRIDGE_BIN}"
            DESTINATION "${PGE_INSTALL_DIRECTORY}/ipc"
        )
    endif()


    # LunaTester IPC bridge
    set(LUNATESTER_BRIDGE_BIN "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/ipc/LunaLoader-exec.exe")

    add_custom_target(lunatester_bridge_bin ALL
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/testing/bridges/LunaLoader-exec.tar.gz"
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/testing/bridges/LunaLoader-exec.tar.gz
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/ipc"
        COMMENT "Unpacking LunaLoader-exec.exe"
        VERBATIM
        BYPRODUCTS "${LUNATESTER_BRIDGE_BIN}"
    )

    if(APPLE) # Embed into the bundle
        list(APPEND PGE_EDITOR_SRCS "${LUNATESTER_BRIDGE_BIN}")
        set_source_files_properties(${LUNATESTER_BRIDGE_BIN} PROPERTIES GENERATED TRUE)
    else()
        install(FILES "${LUNATESTER_BRIDGE_BIN}"
            DESTINATION "${PGE_INSTALL_DIRECTORY}/ipc"
        )
    endif()

endif()

if(NOT WIN32 AND NOT HAIKU)

    set(LUNA_HWND_SHOW_BRIDGE_BIN "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/ipc/luna_hwnd_show.exe")

    # LunaTester Wine helper
    add_custom_target(luna_hwnd_show_bin ALL
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/testing/bridges/luna_hwnd_show.tar.gz"
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/testing/bridges/luna_hwnd_show.tar.gz
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/ipc"
        COMMENT "Unpacking luna_hwnd_show.exe"
        VERBATIM
        BYPRODUCTS "${LUNA_HWND_SHOW_BRIDGE_BIN}"
    )

    if(APPLE) # Embed into the bundle
        list(APPEND PGE_EDITOR_SRCS "${LUNA_HWND_SHOW_BRIDGE_BIN}")
        set_source_files_properties(${LUNA_HWND_SHOW_BRIDGE_BIN} PROPERTIES GENERATED TRUE)
    else()
        install(FILES "${LUNA_HWND_SHOW_BRIDGE_BIN}"
            DESTINATION "${PGE_INSTALL_DIRECTORY}/ipc"
        )
    endif()

endif()

add_executable(pge_editor
    ${PGE_EDITOR_SRCS}
    ${CONFIG_PACK_MANAGER_SRCS}
    ${DIRMANAGER_SRCS}
    ${INIPROCESSOR_SRCS}
    ${STACK_WALKER_SRCS}
    ${UTILS_SRCS}
    ${UTILS_QT_SRCS}
    ${BITMASK2RGBA_SRCS}
)

add_library(PGE_QtLibs INTERFACE)

target_link_libraries(PGE_QtLibs INTERFACE
    ${QT_EXTRA_LIBS_PRE}
    PGE_FileFormatsQt
    ${QT_LIBS_LIST}
    ${QT_EXTRA_LIBS}
)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
target_compile_definitions(PGE_QtLibs
    INTERFACE
    ${QT_DEFS_LIST}
    -DPGE_FILES_QT
)

target_include_directories(PGE_QtLibs
    INTERFACE
    ${QT_INCLUDES_LIST}
)


target_compile_definitions(pge_editor
    PRIVATE
    -DPGE_EDITOR
    -DUSE_SDL_MIXER
)

if(WIN32 AND NOT EMSCRIPTEN)
    target_compile_definitions(pge_editor
        PUBLIC
        -DUNICODE
        -D_UNICODE
        -DNOMINMAX
    )
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
    target_compile_definitions(pge_editor
        PRIVATE
        -DDEBUG_BUILD
    )
endif()

add_dependencies(pge_editor
    SDLMixerX_Local
    SDL2_Local
    FreeImage_Local
    libpng_Local
    FREETYPE_Local
    PGE_FileFormatsQt
    moondust_translations
    editor_translations
)

set(PGE_EDITOR_LINK_LIBS)
set(PGE_EDITOR_INSTALLS)

list(APPEND PGE_EDITOR_INSTALLS
    pge_editor
)

list(APPEND PGE_EDITOR_LINK_LIBS
    ${UTILS_LIBS}
    PGE_QtLibs
    PGE_FreeImage
)

if(PGE_STATIC_SDLMIXER)
    list(APPEND PGE_EDITOR_LINK_LIBS PGE_SDLMixerX_static PGE_SDL2_static)
else()
    list(APPEND PGE_EDITOR_LINK_LIBS PGE_SDLMixerX PGE_SDL2)
endif()

if(NOT APPLE AND NOT MSVC AND NOT PGE_ENABLE_STATIC_QT AND NOT USE_SYSTEM_LIBC)
    set_property(TARGET pge_editor APPEND_STRING PROPERTY LINK_FLAGS " -static-libgcc -static-libstdc++")
endif()

if(WIN32)
    list(APPEND PGE_EDITOR_LINK_LIBS kernel32 psapi) # Needed for LunaTester killer
endif()

if(NOT EMSCRIPTEN AND NOT MSVC)
    if(WIN32)
        set_target_properties(pge_editor PROPERTIES WIN32_EXECUTABLE ON)
        if(NOT PGE_ENABLE_STATIC_QT)
            list(APPEND PGE_EDITOR_LINK_LIBS "-static")
        endif()
    endif()
endif()

if(APPLE)
    set_target_properties(pge_editor PROPERTIES
        OUTPUT_NAME "Moondust Editor"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_resources/pge_editor.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME "pge_editor"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ru.wohlsoft.pge-editor"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${EDITOR_VERSION_STRING}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${EDITOR_VERSION_STRING}"
        MACOSX_BUNDLE_ICON_FILE "cat_builder.icns"
        CPACK_BUNDLE_NAME "Moondust Editor"
        MACOSX_BUNDLE_INFO_STRING "Level and World map Editor for the Moondust Project"
    )
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/_resources/appicon/cat_builder.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${PGE_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${MOONDUST_QT_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${MOONDUST_QT_LANG_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${PGE_FILE_ICONS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties("${38A_IPC_BRIDGE_BIN}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/ipc")
    set_source_files_properties("${LUNA_HWND_SHOW_BRIDGE_BIN}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/ipc")
    set_source_files_properties("${LUNATESTER_BRIDGE_BIN}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/ipc")

    list(APPEND PGE_EDITOR_LINK_LIBS iconv)
endif()

target_link_libraries(pge_editor PUBLIC
    ${PGE_EDITOR_LINK_LIBS}
)

pge_set_nopie(pge_editor)

install(TARGETS ${PGE_EDITOR_INSTALLS}
    RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}/"
    BUNDLE DESTINATION "${PGE_INSTALL_DIRECTORY}/"
)

# Editor themes
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Content/themes"
    DESTINATION "${PGE_INSTALL_DIRECTORY}"
    PATTERN "*.zip" EXCLUDE
    PATTERN "*.xcf" EXCLUDE
    PATTERN "test" EXCLUDE
    PATTERN "pge_default" EXCLUDE
)

# Standalone help
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Content/help"
    DESTINATION "${PGE_INSTALL_DIRECTORY}"
    PATTERN ".git" EXCLUDE
    PATTERN ".pdn" EXCLUDE
    PATTERN ".xcf" EXCLUDE
    PATTERN "generateMap.php" EXCLUDE
    PATTERN "TOC.hhc" EXCLUDE
    PATTERN "Index.hhk" EXCLUDE
)
