import qbs
import qbs.FileInfo
import qbs.Process

Product
{
    type: "application"
    Depends { name: "cpp" }
    Depends {
        name: "Qt";
        submodules: ["gui", "widgets", "network", "concurrent", "qml"]
    }
    Depends {
        name: "Qt";
        submodules: ["winextras"]
        condition: qbs.targetOS.contains("windows")
    }
    // Operating system identify
    Properties{
        condition: qbs.targetOS.contains("osx")
        property string destOS: "macos"
        property string binDir: "bin"
        property string languageDir: binDir+"/languages"
    }
    Properties{
        condition: qbs.targetOS.contains("linux")
        property string destOS: "linux"
        property string binDir: "bin"
        property string languageDir: binDir+"/languages"
    }
    Properties{
        condition: qbs.targetOS.contains("windows")
        property string destOS: "win32"
        property string binDir: "bin-w32"
        property string languageDir: binDir+"/languages"
    }

    // Application name
    Properties {
        condition: qbs.targetOS.contains("osx")
        name: "PGE Editor"
    }
    Properties {
        condition: !qbs.targetOS.contains("osx")
        name: "pge_editor"
    }

    property string  pgeRoot: { // Because impossible just type "path+/../" (in result path+/dotdot/), need do this
        var newPath = path;
        newPath = newPath.substring(0, newPath.lastIndexOf("/"));
        return newPath;
    }

    property string GIT_VERSION: {
        var proc = new Process();
        proc.exec("git", ["--git-dir", pgeRoot+"/.git", "--work-tree", pgeRoot+"/", "describe", "--always"]);
        var out = proc.readStdOut();
        return out.trim();
    }

    //Common linking properties
    Properties {
        condition: true
        cpp.staticLibraries: ['freeimagelite']
        cpp.cxxLanguageVersion: "c++11"
        cpp.defines: ['PGE_EDITOR', 'USE_SDL_MIXER', 'PGE_FILES_QT', 'GIT_VERSION="' + GIT_VERSION + '"']
        cpp.includePaths: [
            path,
            pgeRoot + "/_Libs/_builds/" + destOS +"/include",
            pgeRoot + "/_Libs/SDL2_mixer_modified",
            path + "/_includes",
            pgeRoot + "/_Libs",
            pgeRoot + "/_common",
        ]
        cpp.libraryPaths: [ pgeRoot + "/_Libs/_builds/" + destOS + "/lib" ]
        destinationDirectory: {
            var newPath = pgeRoot;
            newPath += "/" + binDir;
            console.info("-> PGE Editor: " + qbs.buildVariant +
                         " for " + destOS + "-"+ qbs.architecture +
                         ", GIT version " + GIT_VERSION +
                         ", target path: " + newPath
                         );
            return newPath;
        }
    }

    // Build variant
    Properties {
        condition: qbs.buildVariant == "debug"
        cpp.debugInformation: true
        cpp.optimization: "none"
    }
    Properties {
        condition: qbs.buildVariant == "release"
        cpp.debugInformation: false
        cpp.optimization: "fast"
    }

    //Link libraries
    Properties {
        condition: qbs.targetOS.contains("linux")
        cpp.dynamicLibraries: ['SDL2', 'SDL2_mixer_ext']
    }
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.dynamicLibraries: ['SDL2', 'SDL2_mixer_ext', 'SDL2main', 'version', 'dbghelp', 'winmm']
        //cpp.staticLibraries: ['pthread']
        //cpp.cppflags: ['-static-libgcc', '-static-libstdc++']
    }
    Properties {
        condition: qbs.targetOS.contains("osx")
        cpp.includePaths: [pgeRoot + '/_Libs/_builds/macos/frameworks/SDL2.framework/Headers']
        cpp.frameworkPaths: [ pgeRoot + '/_Libs/_builds/macos/frameworks' ]
        cpp.frameworks: [ 'SDL2' ]
        cpp.dynamicLibraries: ['SDL2_mixer_ext']
    }

    //Install properties
    consoleApplication: false
    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: false
        //qbs.installPrefix: path
        //qbs.installRoot: "/"
        //qbs.installDir: "/bin"
    }

    Group {
        name: "Config Pack Manager"
        files:[
            "../_common/ConfigPackManager/image_size.cpp",
            "../_common/ConfigPackManager/image_size.h",
            "../_common/ConfigPackManager/level/config_bgo.cpp",
            "../_common/ConfigPackManager/level/config_bgo.h",
            "../_common/ConfigPackManager/level/config_block.cpp",
            "../_common/ConfigPackManager/level/config_block.h",
            "../_common/ConfigPackManager/level/config_npc.cpp",
            "../_common/ConfigPackManager/level/config_npc.h",
            "../_common/ConfigPackManager/world/config_wld_generic.cpp",
            "../_common/ConfigPackManager/world/config_wld_generic.h",
        ]
    }

    Group {
        name: "PGE File Library"
        files: [
            "../_common/PGE_File_Formats/CSVReader.h",
            "../_common/PGE_File_Formats/CSVReaderPGE.h",
            "../_common/PGE_File_Formats/CSVUtils.h",
            "../_common/PGE_File_Formats/ConvertUTF.c",
            "../_common/PGE_File_Formats/ConvertUTF.h",
            "../_common/PGE_File_Formats/charsetconvert.h",
            "../_common/PGE_File_Formats/file_formats.cpp",
            "../_common/PGE_File_Formats/file_formats.h",
            "../_common/PGE_File_Formats/file_rw_lvl.cpp",
            "../_common/PGE_File_Formats/file_rw_lvl_38a.cpp",
            "../_common/PGE_File_Formats/file_rw_lvlx.cpp",
            "../_common/PGE_File_Formats/file_rw_meta.cpp",
            "../_common/PGE_File_Formats/file_rw_npc_txt.cpp",
            "../_common/PGE_File_Formats/file_rw_sav.cpp",
            "../_common/PGE_File_Formats/file_rw_savx.cpp",
            "../_common/PGE_File_Formats/file_rw_smbx64_cnf.cpp",
            "../_common/PGE_File_Formats/file_rw_wld.cpp",
            "../_common/PGE_File_Formats/file_rw_wldx.cpp",
            "../_common/PGE_File_Formats/file_rwopen.cpp",
            "../_common/PGE_File_Formats/file_strlist.cpp",
            "../_common/PGE_File_Formats/file_strlist.h",
            "../_common/PGE_File_Formats/lvl_filedata.cpp",
            "../_common/PGE_File_Formats/lvl_filedata.h",
            "../_common/PGE_File_Formats/meta_filedata.h",
            "../_common/PGE_File_Formats/npc_filedata.cpp",
            "../_common/PGE_File_Formats/npc_filedata.h",
            "../_common/PGE_File_Formats/pge_file_lib_globs.cpp",
            "../_common/PGE_File_Formats/pge_file_lib_globs.h",
            "../_common/PGE_File_Formats/pge_file_lib_sys.h",
            "../_common/PGE_File_Formats/pge_x.cpp",
            "../_common/PGE_File_Formats/pge_x.h",
            "../_common/PGE_File_Formats/pge_x_macro.h",
            "../_common/PGE_File_Formats/save_filedata.cpp",
            "../_common/PGE_File_Formats/save_filedata.h",
            "../_common/PGE_File_Formats/smbx64.cpp",
            "../_common/PGE_File_Formats/smbx64.h",
            "../_common/PGE_File_Formats/smbx64_cnf_filedata.cpp",
            "../_common/PGE_File_Formats/smbx64_cnf_filedata.h",
            "../_common/PGE_File_Formats/smbx64_macro.h",
            "../_common/PGE_File_Formats/wld_filedata.cpp",
            "../_common/PGE_File_Formats/wld_filedata.h",
        ]
    }

    Group
    {
        name: "SMBX64 Validation messages"
        files: [
            "../_common/data_functions/smbx64_validation_messages.cpp",
            "../_common/data_functions/smbx64_validation_messages.h",
        ]
    }

    Group {
        name: "EasyBMP"
        files: [
            "../_Libs/EasyBMP/EasyBMP.cpp",
            "../_Libs/EasyBMP/EasyBMP.h",
            "../_Libs/EasyBMP/EasyBMP_BMP.h",
            "../_Libs/EasyBMP/EasyBMP_DataStructures.h",
            "../_Libs/EasyBMP/EasyBMP_VariousBMPutilities.h",
        ]
    }

    Group {
        name: "Gif Library"
        files: [
            "../_Libs/giflib/dgif_lib.c",
            "../_Libs/giflib/egif_lib.c",
            "../_Libs/giflib/gif_err.c",
            "../_Libs/giflib/gif_font.c",
            "../_Libs/giflib/gif_hash.c",
            "../_Libs/giflib/gif_hash.h",
            "../_Libs/giflib/gif_lib.h",
            "../_Libs/giflib/gif_lib_private.h",
            "../_Libs/giflib/gifalloc.c",
            "../_Libs/giflib/quantize.c",
        ]
    }

    Group {
        condition: qbs.targetOS.contains("windows")
        name: "RC-File for Windows"
        files: [
            "_resources/pge_editor.rc",
        ]
    }

    files: [
        "SingleApplication/editor_application.cpp",
        "SingleApplication/editor_application.h",
        "SingleApplication/localserver.cpp",
        "SingleApplication/localserver.h",
        "SingleApplication/semaphore_winapi.h",
        "SingleApplication/singleapplication.cpp",
        "SingleApplication/singleapplication.h",
        "_resources/editor.qrc",
        "audio/music_player.cpp",
        "audio/music_player.h",
        "audio/sdl_music_player.cpp",
        "audio/sdl_music_player.h",
        "common_features/RTree.h",
        "common_features/animation_timer.cpp",
        "common_features/animation_timer.h",
        "common_features/app_path.cpp",
        "common_features/app_path.h",
        "common_features/bool_reseter.cpp",
        "common_features/bool_reseter.h",
        "common_features/crashhandler.cpp",
        "common_features/crashhandler.h",
        "common_features/crashhandler.ui",
        "common_features/data_array.h",
        "common_features/dir_copy.cpp",
        "common_features/dir_copy.h",
        "common_features/edit_mode_base.cpp",
        "common_features/edit_mode_base.h",
        "common_features/file_mapper.cpp",
        "common_features/file_mapper.h",
        "common_features/flowlayout.cpp",
        "common_features/flowlayout.h",
        "common_features/graphics_funcs.cpp",
        "common_features/graphics_funcs.h",
        "common_features/graphicsworkspace.cpp",
        "common_features/graphicsworkspace.h",
        "common_features/grid.cpp",
        "common_features/grid.h",
        "common_features/installer.cpp",
        "common_features/installer.h",
        "common_features/item_rectangles.cpp",
        "common_features/item_rectangles.h",
        "common_features/items.cpp",
        "common_features/items.h",
        "common_features/logger.cpp",
        "common_features/logger.h",
        "common_features/logger_sets.h",
        "common_features/main_window_ptr.cpp",
        "common_features/main_window_ptr.h",
        "common_features/npc_animator.cpp",
        "common_features/npc_animator.h",
        "common_features/number_limiter.h",
        "common_features/proxystyle.cpp",
        "common_features/proxystyle.h",
        "common_features/resizer/corner_grabber.cpp",
        "common_features/resizer/corner_grabber.h",
        "common_features/resizer/item_resizer.cpp",
        "common_features/resizer/item_resizer.h",
        "common_features/safe_msg_box.cpp",
        "common_features/safe_msg_box.h",
        "common_features/simple_animator.cpp",
        "common_features/simple_animator.h",
        "common_features/spash_screen.cpp",
        "common_features/spash_screen.h",
        "common_features/themes.cpp",
        "common_features/themes.h",
        "common_features/timecounter.cpp",
        "common_features/timecounter.h",
        "common_features/util.cpp",
        "common_features/util.h",
        "common_features/version_cmp.cpp",
        "common_features/version_cmp.h",
        "data_configs/conf_music.cpp",
        "data_configs/conf_rotation_tables.cpp",
        "data_configs/conf_sound.cpp",
        "data_configs/conf_wld_level.cpp",
        "data_configs/conf_wld_path.cpp",
        "data_configs/conf_wld_scene.cpp",
        "data_configs/conf_wld_tile.cpp",
        "data_configs/config_status/config_status.cpp",
        "data_configs/config_status/config_status.h",
        "data_configs/config_status/config_status.ui",
        "data_configs/custom_data.cpp",
        "data_configs/custom_data.h",
        "data_configs/data_configs.cpp",
        "data_configs/data_configs.h",
        "data_configs/obj_BG.cpp",
        "data_configs/obj_BG.h",
        "data_configs/obj_bgo.cpp",
        "data_configs/obj_bgo.h",
        "data_configs/obj_block.cpp",
        "data_configs/obj_block.h",
        "data_configs/obj_npc.cpp",
        "data_configs/obj_npc.h",
        "data_configs/obj_player.cpp",
        "data_configs/obj_player.h",
        "data_configs/obj_rotation_table.h",
        "data_configs/obj_tilesets.cpp",
        "data_configs/obj_tilesets.h",
        "data_configs/obj_wld_items.h",
        "data_configs/selection_dialog/config_manager.cpp",
        "data_configs/selection_dialog/config_manager.h",
        "data_configs/selection_dialog/config_manager.ui",
        "defines.h",
        "dev_console/devconsole.cpp",
        "dev_console/devconsole.h",
        "dev_console/devconsole.ui",
        "editing/_components/history/historyelementaddlayer.cpp",
        "editing/_components/history/historyelementaddlayer.h",
        "editing/_components/history/historyelementaddwarp.cpp",
        "editing/_components/history/historyelementaddwarp.h",
        "editing/_components/history/historyelementchangednewlayer.cpp",
        "editing/_components/history/historyelementchangednewlayer.h",
        "editing/_components/history/historyelementitemsetting.cpp",
        "editing/_components/history/historyelementitemsetting.h",
        "editing/_components/history/historyelementlayerchanged.cpp",
        "editing/_components/history/historyelementlayerchanged.h",
        "editing/_components/history/historyelementmainsetting.cpp",
        "editing/_components/history/historyelementmainsetting.h",
        "editing/_components/history/historyelementmergelayer.cpp",
        "editing/_components/history/historyelementmergelayer.h",
        "editing/_components/history/historyelementmodification.cpp",
        "editing/_components/history/historyelementmodification.h",
        "editing/_components/history/historyelementmodifyevent.cpp",
        "editing/_components/history/historyelementmodifyevent.h",
        "editing/_components/history/historyelementnewlayer.cpp",
        "editing/_components/history/historyelementnewlayer.h",
        "editing/_components/history/historyelementplacedoor.cpp",
        "editing/_components/history/historyelementplacedoor.h",
        "editing/_components/history/historyelementremovelayer.cpp",
        "editing/_components/history/historyelementremovelayer.h",
        "editing/_components/history/historyelementremovelayerandsave.cpp",
        "editing/_components/history/historyelementremovelayerandsave.h",
        "editing/_components/history/historyelementremovewarp.cpp",
        "editing/_components/history/historyelementremovewarp.h",
        "editing/_components/history/historyelementrenameevent.cpp",
        "editing/_components/history/historyelementrenameevent.h",
        "editing/_components/history/historyelementrenamelayer.cpp",
        "editing/_components/history/historyelementrenamelayer.h",
        "editing/_components/history/historyelementreplaceplayerpoint.cpp",
        "editing/_components/history/historyelementreplaceplayerpoint.h",
        "editing/_components/history/historyelementresizeblock.cpp",
        "editing/_components/history/historyelementresizeblock.h",
        "editing/_components/history/historyelementresizesection.cpp",
        "editing/_components/history/historyelementresizesection.h",
        "editing/_components/history/historyelementresizewater.cpp",
        "editing/_components/history/historyelementresizewater.h",
        "editing/_components/history/historyelementsettingsevent.cpp",
        "editing/_components/history/historyelementsettingsevent.h",
        "editing/_components/history/historyelementsettingssection.cpp",
        "editing/_components/history/historyelementsettingssection.h",
        "editing/_components/history/historyelementsettingswarp.cpp",
        "editing/_components/history/historyelementsettingswarp.h",
        "editing/_components/history/ihistoryelement.cpp",
        "editing/_components/history/ihistoryelement.h",
        "editing/_components/history/itemsearcher.cpp",
        "editing/_components/history/itemsearcher.h",
        "editing/_dialogs/itemselectdialog.cpp",
        "editing/_dialogs/itemselectdialog.h",
        "editing/_dialogs/itemselectdialog.ui",
        "editing/_dialogs/levelfilelist.cpp",
        "editing/_dialogs/levelfilelist.h",
        "editing/_dialogs/levelfilelist.ui",
        "editing/_dialogs/musicfilelist.cpp",
        "editing/_dialogs/musicfilelist.h",
        "editing/_dialogs/musicfilelist.ui",
        "editing/_dialogs/savingnotificationdialog.cpp",
        "editing/_dialogs/savingnotificationdialog.h",
        "editing/_dialogs/savingnotificationdialog.ui",
        "editing/_dialogs/wld_setpoint.cpp",
        "editing/_dialogs/wld_setpoint.h",
        "editing/_dialogs/wld_setpoint.ui",
        "editing/_scenes/level/data_manager/lvl_animators.cpp",
        "editing/_scenes/level/data_manager/lvl_custom_gfx.cpp",
        "editing/_scenes/level/edit_modes/mode_circle.cpp",
        "editing/_scenes/level/edit_modes/mode_circle.h",
        "editing/_scenes/level/edit_modes/mode_erase.cpp",
        "editing/_scenes/level/edit_modes/mode_erase.h",
        "editing/_scenes/level/edit_modes/mode_fill.cpp",
        "editing/_scenes/level/edit_modes/mode_fill.h",
        "editing/_scenes/level/edit_modes/mode_hand.cpp",
        "editing/_scenes/level/edit_modes/mode_hand.h",
        "editing/_scenes/level/edit_modes/mode_line.cpp",
        "editing/_scenes/level/edit_modes/mode_line.h",
        "editing/_scenes/level/edit_modes/mode_place.cpp",
        "editing/_scenes/level/edit_modes/mode_place.h",
        "editing/_scenes/level/edit_modes/mode_resize.cpp",
        "editing/_scenes/level/edit_modes/mode_resize.h",
        "editing/_scenes/level/edit_modes/mode_select.cpp",
        "editing/_scenes/level/edit_modes/mode_select.h",
        "editing/_scenes/level/edit_modes/mode_square.cpp",
        "editing/_scenes/level/edit_modes/mode_square.h",
        "editing/_scenes/level/itemmsgbox.cpp",
        "editing/_scenes/level/itemmsgbox.h",
        "editing/_scenes/level/itemmsgbox.ui",
        "editing/_scenes/level/items/item_bgo.cpp",
        "editing/_scenes/level/items/item_bgo.h",
        "editing/_scenes/level/items/item_block.cpp",
        "editing/_scenes/level/items/item_block.h",
        "editing/_scenes/level/items/item_door.cpp",
        "editing/_scenes/level/items/item_door.h",
        "editing/_scenes/level/items/item_npc.cpp",
        "editing/_scenes/level/items/item_npc.h",
        "editing/_scenes/level/items/item_playerpoint.cpp",
        "editing/_scenes/level/items/item_playerpoint.h",
        "editing/_scenes/level/items/item_water.cpp",
        "editing/_scenes/level/items/item_water.h",
        "editing/_scenes/level/items/lvl_base_item.cpp",
        "editing/_scenes/level/items/lvl_base_item.h",
        "editing/_scenes/level/items/lvl_items_aligning.cpp",
        "editing/_scenes/level/items/lvl_items_modify.cpp",
        "editing/_scenes/level/items/lvl_items_place.cpp",
        "editing/_scenes/level/lvl_clipboard.cpp",
        "editing/_scenes/level/lvl_collisions.cpp",
        "editing/_scenes/level/lvl_control.cpp",
        "editing/_scenes/level/lvl_history_manager.cpp",
        "editing/_scenes/level/lvl_history_manager.h",
        "editing/_scenes/level/lvl_init_filedata.cpp",
        "editing/_scenes/level/lvl_item_placing.cpp",
        "editing/_scenes/level/lvl_item_placing.h",
        "editing/_scenes/level/lvl_scene.cpp",
        "editing/_scenes/level/lvl_scene.h",
        "editing/_scenes/level/lvl_section.cpp",
        "editing/_scenes/level/lvl_setup.cpp",
        "editing/_scenes/level/newlayerbox.cpp",
        "editing/_scenes/level/newlayerbox.h",
        "editing/_scenes/level/resizers/lvl_resizer_block.cpp",
        "editing/_scenes/level/resizers/lvl_resizer_img_shoot.cpp",
        "editing/_scenes/level/resizers/lvl_resizer_physenv.cpp",
        "editing/_scenes/level/resizers/lvl_resizer_section.cpp",
        "editing/_scenes/level/resizers/lvl_resizer_section_evnt.cpp",
        "editing/_scenes/level/resizers/lvl_resizers.cpp",
        "editing/_scenes/level/tonewlayerbox.ui",
        "editing/_scenes/world/data_manager/wld_animators.cpp",
        "editing/_scenes/world/data_manager/wld_custom_gfx.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_circle.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_circle.h",
        "editing/_scenes/world/edit_modes/wld_mode_erase.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_erase.h",
        "editing/_scenes/world/edit_modes/wld_mode_fill.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_fill.h",
        "editing/_scenes/world/edit_modes/wld_mode_hand.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_hand.h",
        "editing/_scenes/world/edit_modes/wld_mode_line.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_line.h",
        "editing/_scenes/world/edit_modes/wld_mode_place.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_place.h",
        "editing/_scenes/world/edit_modes/wld_mode_resize.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_resize.h",
        "editing/_scenes/world/edit_modes/wld_mode_select.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_select.h",
        "editing/_scenes/world/edit_modes/wld_mode_setpoint.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_setpoint.h",
        "editing/_scenes/world/edit_modes/wld_mode_square.cpp",
        "editing/_scenes/world/edit_modes/wld_mode_square.h",
        "editing/_scenes/world/items/item_level.cpp",
        "editing/_scenes/world/items/item_level.h",
        "editing/_scenes/world/items/item_music.cpp",
        "editing/_scenes/world/items/item_music.h",
        "editing/_scenes/world/items/item_path.cpp",
        "editing/_scenes/world/items/item_path.h",
        "editing/_scenes/world/items/item_point.cpp",
        "editing/_scenes/world/items/item_point.h",
        "editing/_scenes/world/items/item_scene.cpp",
        "editing/_scenes/world/items/item_scene.h",
        "editing/_scenes/world/items/item_tile.cpp",
        "editing/_scenes/world/items/item_tile.h",
        "editing/_scenes/world/items/wld_base_item.cpp",
        "editing/_scenes/world/items/wld_base_item.h",
        "editing/_scenes/world/items/wld_items_aligning.cpp",
        "editing/_scenes/world/items/wld_items_modify.cpp",
        "editing/_scenes/world/items/wld_items_place.cpp",
        "editing/_scenes/world/resizers/wld_resizer_img_shoot.cpp",
        "editing/_scenes/world/resizers/wld_resizers.cpp",
        "editing/_scenes/world/wld_clipboard.cpp",
        "editing/_scenes/world/wld_collisions.cpp",
        "editing/_scenes/world/wld_control.cpp",
        "editing/_scenes/world/wld_history_manager.cpp",
        "editing/_scenes/world/wld_history_manager.h",
        "editing/_scenes/world/wld_init_filedata.cpp",
        "editing/_scenes/world/wld_item_placing.cpp",
        "editing/_scenes/world/wld_item_placing.h",
        "editing/_scenes/world/wld_point_selector.cpp",
        "editing/_scenes/world/wld_point_selector.h",
        "editing/_scenes/world/wld_scene.cpp",
        "editing/_scenes/world/wld_scene.h",
        "editing/_scenes/world/wld_setup.cpp",
        "editing/edit_level/level_edit.cpp",
        "editing/edit_level/level_edit.h",
        "editing/edit_level/leveledit.ui",
        "editing/edit_level/levelprops.cpp",
        "editing/edit_level/levelprops.h",
        "editing/edit_level/levelprops.ui",
        "editing/edit_level/lvl_clone_section.cpp",
        "editing/edit_level/lvl_clone_section.h",
        "editing/edit_level/lvl_clone_section.ui",
        "editing/edit_level/lvl_draw.cpp",
        "editing/edit_level/lvl_edit_control.cpp",
        "editing/edit_level/lvl_files_io.cpp",
        "editing/edit_npc/npc_data_sets.cpp",
        "editing/edit_npc/npcedit.cpp",
        "editing/edit_npc/npcedit.h",
        "editing/edit_npc/npcedit.ui",
        "editing/edit_npc/npceditscene.cpp",
        "editing/edit_npc/npceditscene.h",
        "editing/edit_npc/npctxt_controls.cpp",
        "editing/edit_npc/npctxt_files_io.cpp",
        "editing/edit_world/wld_draw.cpp",
        "editing/edit_world/wld_edit_control.cpp",
        "editing/edit_world/wld_files_io.cpp",
        "editing/edit_world/world_edit.cpp",
        "editing/edit_world/world_edit.h",
        "editing/edit_world/world_edit.ui",
        "js_engine/pge_jsengine.cpp",
        "js_engine/pge_jsengine.h",
        "js_engine/proxies/js_common.cpp",
        "js_engine/proxies/js_common.h",
        "js_engine/proxies/js_file.cpp",
        "js_engine/proxies/js_file.h",
        "js_engine/proxies/js_ini.cpp",
        "js_engine/proxies/js_ini.h",
        "js_engine/proxies/js_lua_preproc.cpp",
        "js_engine/proxies/js_lua_preproc.h",
        "js_engine/proxies/js_utils.h",
        "main.cpp",
        "main_window/_settings/defaults.cpp",
        "main_window/_settings/settings_io.cpp",
        "main_window/about_dialog/aboutdialog.cpp",
        "main_window/about_dialog/aboutdialog.h",
        "main_window/about_dialog/aboutdialog.ui",
        "main_window/dock/_dock_vizman.cpp",
        "main_window/dock/_dock_vizman.h",
        "main_window/dock/bookmarks_box.cpp",
        "main_window/dock/bookmarks_box.h",
        "main_window/dock/bookmarks_box.ui",
        "main_window/dock/debugger.cpp",
        "main_window/dock/debugger.h",
        "main_window/dock/debugger.ui",
        "main_window/dock/lvl_events_box.cpp",
        "main_window/dock/lvl_events_box.h",
        "main_window/dock/lvl_events_box.ui",
        "main_window/dock/lvl_item_properties.cpp",
        "main_window/dock/lvl_item_properties.h",
        "main_window/dock/lvl_item_properties.ui",
        "main_window/dock/lvl_item_toolbox.cpp",
        "main_window/dock/lvl_item_toolbox.h",
        "main_window/dock/lvl_item_toolbox.ui",
        "main_window/dock/lvl_layers_box.cpp",
        "main_window/dock/lvl_layers_box.h",
        "main_window/dock/lvl_layers_box.ui",
        "main_window/dock/lvl_sctc_props.cpp",
        "main_window/dock/lvl_sctc_props.h",
        "main_window/dock/lvl_sctc_props.ui",
        "main_window/dock/lvl_search_box.cpp",
        "main_window/dock/lvl_search_box.h",
        "main_window/dock/lvl_search_box.ui",
        "main_window/dock/lvl_warp_props.cpp",
        "main_window/dock/lvl_warp_props.h",
        "main_window/dock/lvl_warp_props.ui",
        "main_window/dock/mwdock_base.cpp",
        "main_window/dock/mwdock_base.h",
        "main_window/dock/tileset_item_box.cpp",
        "main_window/dock/tileset_item_box.h",
        "main_window/dock/tileset_item_box.ui",
        "main_window/dock/toolboxes.h",
        "main_window/dock/toolboxes_protos.h",
        "main_window/dock/variables_box.cpp",
        "main_window/dock/variables_box.h",
        "main_window/dock/variables_box.ui",
        "main_window/dock/wld_item_props.cpp",
        "main_window/dock/wld_item_props.h",
        "main_window/dock/wld_item_props.ui",
        "main_window/dock/wld_item_toolbox.cpp",
        "main_window/dock/wld_item_toolbox.h",
        "main_window/dock/wld_item_toolbox.ui",
        "main_window/dock/wld_search_box.cpp",
        "main_window/dock/wld_search_box.h",
        "main_window/dock/wld_search_box.ui",
        "main_window/dock/wld_settings_box.cpp",
        "main_window/dock/wld_settings_box.h",
        "main_window/dock/wld_settings_box.ui",
        "main_window/edit/edit_clipboard.cpp",
        "main_window/edit/edit_history.cpp",
        "main_window/edit/edit_items.cpp",
        "main_window/edit/edit_mode.cpp",
        "main_window/edit/edit_mode_placing.cpp",
        "main_window/edit/edit_placing_switch.cpp",
        "main_window/edit/edit_resize.cpp",
        "main_window/events.cpp",
        "main_window/file/file_export_image.cpp",
        "main_window/file/file_new.cpp",
        "main_window/file/file_open.cpp",
        "main_window/file/file_recent.cpp",
        "main_window/file/file_reload.cpp",
        "main_window/file/file_save.cpp",
        "main_window/file/lvl_export_image.cpp",
        "main_window/file/lvl_export_image.h",
        "main_window/file/lvl_export_image.ui",
        "main_window/file/wld_export_image.cpp",
        "main_window/file/wld_export_image.h",
        "main_window/file/wld_export_image.ui",
        "main_window/global_settings.cpp",
        "main_window/global_settings.h",
        "main_window/greeting_dialog/greeting_dialog.cpp",
        "main_window/greeting_dialog/greeting_dialog.h",
        "main_window/greeting_dialog/greeting_dialog.ui",
        "main_window/help.cpp",
        "main_window/level/level_locks.cpp",
        "main_window/level/level_props.cpp",
        "main_window/level/level_sections.cpp",
        "main_window/level/level_sections_mods.cpp",
        "main_window/mainw_plugins.cpp",
        "main_window/mainw_themes.cpp",
        "main_window/menubar.cpp",
        "main_window/plugins/pge_editorplugininfo.cpp",
        "main_window/plugins/pge_editorplugininfo.h",
        "main_window/plugins/pge_editorplugininfo.ui",
        "main_window/plugins/pge_editorpluginitem.cpp",
        "main_window/plugins/pge_editorpluginitem.h",
        "main_window/plugins/pge_editorpluginmanager.cpp",
        "main_window/plugins/pge_editorpluginmanager.h",
        "main_window/script/script_editor.cpp",
        "main_window/script/script_editor.h",
        "main_window/script/script_editor.ui",
        "main_window/script/script_luafiles.cpp",
        "main_window/sub_windows.cpp",
        "main_window/testing/luna_tester.cpp",
        "main_window/testing/luna_tester.h",
        "main_window/testing/testing.cpp",
        "main_window/testing/testing_settings.cpp",
        "main_window/testing/testing_settings.h",
        "main_window/testing/testing_settings.ui",
        "main_window/tip_of_day/tip_of_day.cpp",
        "main_window/tip_of_day/tip_of_day.h",
        "main_window/tip_of_day/tip_of_day.ui",
        "main_window/tools/app_settings.cpp",
        "main_window/tools/app_settings.h",
        "main_window/tools/app_settings.ui",
        "main_window/tools/data_configs_mgr.cpp",
        "main_window/tools/main_clean_npc_gargage.cpp",
        "main_window/tools/main_tool_cdata_cleaner.cpp",
        "main_window/tools/main_tool_cdata_import.cpp",
        "main_window/tools/main_tool_cdata_lazyfix.cpp",
        "main_window/tools_menu.cpp",
        "main_window/translator.cpp",
        "main_window/updater/check_updates.cpp",
        "main_window/updater/check_updates.h",
        "main_window/updater/check_updates.ui",
        "main_window/updater/updater_links.h",
        "main_window/view/view.cpp",
        "main_window/view/view_zoom.cpp",
        "main_window/windows_extras.cpp",
        "main_window/world/world_locks.cpp",
        "mainwindow.cpp",
        "mainwindow.h",
        "mainwindow.ui",
        "networking/engine_intproc.cpp",
        "networking/engine_intproc.h",
        "tools/async/asyncstarcounter.cpp",
        "tools/async/asyncstarcounter.h",
        "tools/debugger/custom_counter.cpp",
        "tools/debugger/custom_counter.h",
        "tools/debugger/custom_counter_gui.cpp",
        "tools/debugger/custom_counter_gui.h",
        "tools/debugger/custom_counter_gui.ui",
        "tools/external_tools/audiocvt_sox_gui.cpp",
        "tools/external_tools/audiocvt_sox_gui.h",
        "tools/external_tools/audiocvt_sox_gui.ui",
        "tools/external_tools/gifs2png_gui.cpp",
        "tools/external_tools/gifs2png_gui.h",
        "tools/external_tools/gifs2png_gui.ui",
        "tools/external_tools/lazyfixtool_gui.cpp",
        "tools/external_tools/lazyfixtool_gui.h",
        "tools/external_tools/lazyfixtool_gui.ui",
        "tools/external_tools/png2gifs_gui.cpp",
        "tools/external_tools/png2gifs_gui.h",
        "tools/external_tools/png2gifs_gui.ui",
        "tools/math/blocksperseconddialog.cpp",
        "tools/math/blocksperseconddialog.h",
        "tools/math/blocksperseconddialog.ui",
        "tools/smart_import/smartimporter.cpp",
        "tools/smart_import/smartimporter.h",
        "tools/tilesets/piecesmodel.cpp",
        "tools/tilesets/piecesmodel.h",
        "tools/tilesets/tileset.cpp",
        "tools/tilesets/tileset.h",
        "tools/tilesets/tilesetconfiguredialog.cpp",
        "tools/tilesets/tilesetconfiguredialog.h",
        "tools/tilesets/tilesetconfiguredialog.ui",
        "tools/tilesets/tilesetgroupeditor.cpp",
        "tools/tilesets/tilesetgroupeditor.h",
        "tools/tilesets/tilesetgroupeditor.ui",
        "tools/tilesets/tilesetitembutton.cpp",
        "tools/tilesets/tilesetitembutton.h",
        "version.h",
    ]
}
