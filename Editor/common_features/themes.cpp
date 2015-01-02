/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http:/www.gnu.org/licenses/>.
 */

#include "themes.h"

bool Themes::isLoaded=false;
QString Themes::currentThemeDir="";

QMap<Themes::Icons, QIcon > Themes::icons_map;

QMap<Themes::Images, QPixmap > Themes::images_map;

QMap<Themes::Images, int > Themes::int_map;

QMap<Themes::Images, QCursor> Themes::cursor_map;

QString Themes::theme_dir="";


Themes::Themes()
{

}

void Themes::init()
{
    if( icons_map.size()>0 || images_map.size()>0 || int_map.size()>0 || cursor_map.size()>0)
    {
        icons_map.clear();
        images_map.clear();
        int_map.clear();
        cursor_map.clear();
    }

    //init default icons
    icons_map[level_16]     = QIcon(":/lvl16.png");
    icons_map[level_24]     = QIcon(":/images/level.png");
    icons_map[world_16]     = QIcon(":/images/world16.png");
    icons_map[world_24]     = QIcon(":/images/world.png");
    icons_map[npc_16]       = QIcon(":/images/coin.png");

    icons_map[file_open]    = QIcon(":/images/open.png");
    icons_map[file_new]     = QIcon(":/images/new.png");
    icons_map[file_save]    = QIcon(":/images/save.png");
    icons_map[file_saveas]  = QIcon(":/images/saveas.png");
    icons_map[file_reload]  = QIcon(":/reload.png");

    icons_map[debugger]     = QIcon(":/images/bug.png");
    icons_map[tileset_box]  = QIcon(":/images/tileset_box.png");
    icons_map[bookmarks]    = QIcon(":/images/bookmarks.png");

    icons_map[playmusic]    = QIcon(":/images/playmusic.png");
    icons_map[grid_snap]    = QIcon(":/images/grid.png");
    icons_map[animation]    = QIcon(":/images/animation.png");
    icons_map[search]       = QIcon(":/images/search.png");
    icons_map[pencil]       = QIcon(":/images/pencil_16x16.png");

    icons_map[copy]         = QIcon(":/images/edit_copy.png");
    icons_map[copy_16]      = QIcon(":/images/copy_16x16.png");
    icons_map[cut]          = QIcon(":/images/edit_cut.png");
    icons_map[paste]        = QIcon(":/images/edit_paste.png");

    icons_map[undo]         = QIcon(":/images/edit_undo.png");
    icons_map[redo]         = QIcon(":/images/edit_redo.png");

    icons_map[selection]    = QIcon(":/images/arrow.png");
    icons_map[selection_only]= QIcon(":/select_only.png");
    icons_map[hand_drag]    = QIcon(":/hand.png");
    icons_map[erasing]      = QIcon(":/images/rubber.png");
    icons_map[accept]       = QIcon(":/images/resize_accept.png");
    icons_map[cancel]       = QIcon(":/images/resize_cancel.png");

    icons_map[square_fill]  = QIcon(":/images/square_fill.png");
    icons_map[line_tool]    = QIcon(":/images/line_fill.png");
    icons_map[flood_fill]    = QIcon(":/images/flood_fill.png");
    icons_map[overwrite_mode]= QIcon(":/images/overwrite.png");

    icons_map[properties]   = QIcon(":/images/properties.png");

    icons_map[zoom_reset]   = QIcon(":/images/zoom_reset.png");
    icons_map[zoom_in]      = QIcon(":/images/zoom_in.png");
    icons_map[zoom_out]     = QIcon(":/images/zoom_out.png");
    icons_map[section_settings]= QIcon(":/images/section.png");
    icons_map[section_settings_16]= QIcon(":/images/section16.png");
    icons_map[doors]        = QIcon(":/doors.png");
    icons_map[layers]       = QIcon(":/layers.png");
    icons_map[events]       = QIcon(":/images/events.png");
    icons_map[water]        = QIcon(":/drawWater.png");
    icons_map[quicksand]    = QIcon(":/drawQuickSand.png");
    icons_map[draw_player1] = QIcon(":/images/player1_start.png");
    icons_map[draw_player2] = QIcon(":/images/player2_start.png");

    images_map[section_1_selected] = QPixmap(":/sections/sections/01.png");
    images_map[section_1_default]  = QPixmap(":/sections/sections/01_n.png");
    images_map[section_2_selected] = QPixmap(":/sections/sections/02.png");
    images_map[section_2_default]  = QPixmap(":/sections/sections/02_n.png");
    images_map[section_3_selected] = QPixmap(":/sections/sections/03.png");
    images_map[section_3_default]  = QPixmap(":/sections/sections/03_n.png");
    images_map[section_4_selected] = QPixmap(":/sections/sections/04.png");
    images_map[section_4_default]  = QPixmap(":/sections/sections/04_n.png");
    images_map[section_5_selected] = QPixmap(":/sections/sections/05.png");
    images_map[section_5_default]  = QPixmap(":/sections/sections/05_n.png");
    images_map[section_6_selected] = QPixmap(":/sections/sections/06.png");
    images_map[section_6_default]  = QPixmap(":/sections/sections/06_n.png");
    images_map[section_7_selected] = QPixmap(":/sections/sections/07.png");
    images_map[section_7_default]  = QPixmap(":/sections/sections/07_n.png");
    images_map[section_8_selected] = QPixmap(":/sections/sections/08.png");
    images_map[section_8_default]  = QPixmap(":/sections/sections/08_n.png");
    images_map[section_9_selected] = QPixmap(":/sections/sections/09.png");
    images_map[section_9_default]  = QPixmap(":/sections/sections/09_n.png");
    images_map[section_10_selected]= QPixmap(":/sections/sections/10.png");
    images_map[section_10_default] = QPixmap(":/sections/sections/10_n.png");
    images_map[section_11_selected]= QPixmap(":/sections/sections/11.png");
    images_map[section_11_default] = QPixmap(":/sections/sections/11_n.png");
    images_map[section_12_selected]= QPixmap(":/sections/sections/12.png");
    images_map[section_12_default] = QPixmap(":/sections/sections/12_n.png");
    images_map[section_13_selected]= QPixmap(":/sections/sections/13.png");
    images_map[section_13_default] = QPixmap(":/sections/sections/13_n.png");
    images_map[section_14_selected]= QPixmap(":/sections/sections/14.png");
    images_map[section_14_default] = QPixmap(":/sections/sections/14_n.png");
    images_map[section_15_selected]= QPixmap(":/sections/sections/15.png");
    images_map[section_15_default] = QPixmap(":/sections/sections/15_n.png");
    images_map[section_16_selected]= QPixmap(":/sections/sections/16.png");
    images_map[section_16_default] = QPixmap(":/sections/sections/16_n.png");
    images_map[section_17_selected]= QPixmap(":/sections/sections/17.png");
    images_map[section_17_default] = QPixmap(":/sections/sections/17_n.png");
    images_map[section_18_selected]= QPixmap(":/sections/sections/18.png");
    images_map[section_18_default] = QPixmap(":/sections/sections/18_n.png");
    images_map[section_19_selected]= QPixmap(":/sections/sections/19.png");
    images_map[section_19_default] = QPixmap(":/sections/sections/19_n.png");
    images_map[section_20_selected]= QPixmap(":/sections/sections/20.png");
    images_map[section_20_default] = QPixmap(":/sections/sections/20_n.png");
    images_map[section_21_selected]= QPixmap(":/sections/sections/21.png");
    images_map[section_21_default] = QPixmap(":/sections/sections/21_n.png");

    icons_map[section_reset_pos] = QIcon(":/images/reset_pos.png");


    images_map[blocks_free] = QPixmap(":/locks/block_op.png");
    images_map[blocks_locked] = QPixmap(":/locks/block_cl.png");
    images_map[bgo_free] = QPixmap(":/locks/bgo_op.png");
    images_map[bgo_locked] = QPixmap(":/locks/bgo_cl.png");
    images_map[npc_free] = QPixmap(":/locks/npc_op.png");
    images_map[npc_locked] = QPixmap(":/locks/npc_cl.png");
    images_map[warps_free] = QPixmap(":/locks/door_op.png");
    images_map[warps_locked] = QPixmap(":/locks/door_cl.png");
    images_map[physenv_free] = QPixmap(":/locks/water_op.png");
    images_map[physenv_locked] = QPixmap(":/locks/water_cl.png");
    images_map[tiles_free] = QPixmap(":/images/lock_tile_op.png");
    images_map[tiles_locked] = QPixmap(":/images/lock_tile_cl.png");
    images_map[scenery_free] = QPixmap(":/images/lock_scenery_op.png");
    images_map[scenery_locked] = QPixmap(":/images/lock_scenery_cl.png");
    images_map[paths_free] = QPixmap(":/images/lock_paths_op.png");
    images_map[paths_locked] = QPixmap(":/images/lock_paths_cl.png");
    images_map[levels_free] = QPixmap(":/images/lock_levels_op.png");
    images_map[levels_locked] = QPixmap(":/images/lock_levels_cl.png");
    images_map[musicboxes_free] = QPixmap(":/images/lock_musicbox_op.png");
    images_map[musicboxes_locked] = QPixmap(":/images/lock_musicbox_cl.png");

    //Load default images

    images_map[splash] = QPixmap(":/images/splash_editor.png");

    images_map[cursor_pasting] = QPixmap(":/cur_pasta.png");
    int_map[cursor_pasting_x] = 0;
    int_map[cursor_pasting_y] = 0;

    images_map[cursor_erasing] = QPixmap(":/cur_rubber.png");
    int_map[cursor_erasing_x] = 0;
    int_map[cursor_erasing_y] = 0;

    images_map[cursor_flood_fill] = QPixmap(":/images/cur_flood_fill.png");
    int_map[cursor_flood_fill_x] = 2;
    int_map[cursor_flood_fill_y] = 21;

    images_map[player_point] = QPixmap(":/player.png");
    images_map[player1] = QPixmap(":/player1.png");
    images_map[player2] = QPixmap(":/player2.png");


    //dummy images
    images_map[dummy_musicbox] = QPixmap(":/dummies/dummies/music_box.png");
    images_map[dummy_bg] = QPixmap(":/dummies/dummies/dummy-bg.png");
    images_map[dummy_bgo] = QPixmap(":/dummies/dummies/unknown_bgo.png");
    images_map[dummy_block] = QPixmap(":/dummies/dummies/unknown_block.png");
    images_map[dummy_npc] = QPixmap(":/dummies/dummies/unknown_npc.png");
    images_map[dummy_path] = QPixmap(":/dummies/dummies/unknown_path.png");
    images_map[dummy_scenery] = QPixmap(":/dummies/dummies/unknown_scene.png");
    images_map[dummy_tile] = QPixmap(":/dummies/dummies/unknown_tile.png");
    images_map[dummy_wlevel] = QPixmap(":/dummies/dummies/unknown_wlvl.png");

    currentThemeDir = "";
    //initCursors();

    isLoaded=true;
}

///
/// \brief Themes::availableThemes
/// \return list of available themes in the /themes directory
///
QStringList Themes::availableThemes()
{
    QString themesPath(ApplicationPath+"/themes/");
    QDir themesDir(themesPath);
    QStringList allThemes = themesDir.entryList(QDir::AllDirs);

    QStringList available_themes;

    foreach(QString c, allThemes)
    {
        QString theme_dir = themesPath+c+"/";
        QString themeName;

        QString gui_ini = theme_dir + "theme.ini";

        if(!QFileInfo(gui_ini).exists()) continue; //Skip if it is not a config
        QSettings guiset(gui_ini, QSettings::IniFormat);
        guiset.setIniCodec("UTF-8");

        guiset.beginGroup("main");
            themeName = guiset.value("theme-name", QDir(theme_dir).dirName()).toString().remove('|');
        guiset.endGroup();

        available_themes << QDir(theme_dir).dirName() + "|" + themeName;
    }

    available_themes.sort(Qt::CaseInsensitive);
    available_themes.push_front("|Default");

    return available_themes;
}

QString Themes::currentTheme()
{
    return currentThemeDir;
}

void Themes::loadTheme(QString themeDir)
{
    init();

    QString themesPath(ApplicationPath+"/themes/");
    theme_dir = themesPath+themeDir+"/";


    QString gui_ini = theme_dir + "theme.ini";
    if(!QFileInfo(gui_ini).exists()) return;

    QSettings guiset(gui_ini, QSettings::IniFormat);
    guiset.setIniCodec("UTF-8");

    guiset.beginGroup("main");
        loadImage(guiset,"default-splash", splash );
    guiset.endGroup();

    guiset.beginGroup("file-icons");
        loadIcon(guiset,"level-16", level_16 );
        loadIcon(guiset,"level-24", level_24 );
        loadIcon(guiset,"world-16", world_16 );
        loadIcon(guiset,"world-24", world_24 );
        loadIcon(guiset,"npc-16", npc_16 );
    guiset.endGroup();

    guiset.beginGroup("file-io");
        loadIcon(guiset,"file-open", file_open );
        loadIcon(guiset,"file-new", file_new );
        loadIcon(guiset,"file-save", file_save );
        loadIcon(guiset,"file-save-as", file_saveas );
        loadIcon(guiset,"file-reload", file_reload );
    guiset.endGroup();

    guiset.beginGroup("icons");
        loadIcon(guiset,"debugger", debugger );
        loadIcon(guiset,"bookmarks", bookmarks );
        loadIcon(guiset,"tileset-box", tileset_box );
    guiset.endGroup();

    guiset.beginGroup("cursors");
        loadImage(guiset,"normal", cursor_normal );
        loadInteger(guiset,"normal-x", cursor_normal_x );
        loadInteger(guiset,"normal-y", cursor_normal_y );

        loadImage(guiset,"pasting", cursor_pasting );
        loadInteger(guiset,"pasting-x", cursor_pasting_x );
        loadInteger(guiset,"pasting-y", cursor_pasting_y );

        loadImage(guiset,"erasing", cursor_erasing );
        loadInteger(guiset,"erasing-x", cursor_erasing_x );
        loadInteger(guiset,"erasing-y", cursor_erasing_y );

        loadImage(guiset,"resizing", cursor_resizing );
        loadInteger(guiset,"resizing-x", cursor_resizing_x );
        loadInteger(guiset,"resizing-y", cursor_resizing_y );

        loadImage(guiset,"placing", cursor_placing );
        loadInteger(guiset,"placing-x", cursor_placing_x );
        loadInteger(guiset,"placing-y", cursor_placing_y );

        loadImage(guiset,"square-fill", cursor_square_fill );
        loadInteger(guiset,"square-fill-x", cursor_square_fill_x );
        loadInteger(guiset,"square-fill-y", cursor_square_fill_y );

        loadImage(guiset,"line-fill", cursor_line_fill );
        loadInteger(guiset,"line-fill-x", cursor_line_fill_x );
        loadInteger(guiset,"line-fill-y", cursor_line_fill_y );

        loadImage(guiset,"flood-fill", cursor_flood_fill );
        loadInteger(guiset,"flood-fill-x", cursor_flood_fill_x );
        loadInteger(guiset,"flood-fill-y", cursor_flood_fill_y );
    guiset.endGroup();

    guiset.beginGroup("edit-common");
        loadIcon(guiset,"playmusic", playmusic );
        loadIcon(guiset,"grid-snap", grid_snap );
        loadIcon(guiset,"animation", animation );
        loadIcon(guiset,"search", search );
        loadIcon(guiset,"pencil", pencil );
    guiset.endGroup();

    guiset.beginGroup("edit-clipboard");
        loadIcon(guiset,"copy", copy );
        loadIcon(guiset,"copy-16", copy_16 );
        loadIcon(guiset,"cut", cut );
        loadIcon(guiset,"paste", paste );
    guiset.endGroup();

    guiset.beginGroup("edit-history");
        loadIcon(guiset,"undo", undo );
        loadIcon(guiset,"redo", redo );
    guiset.endGroup();

    guiset.beginGroup("edit-mode");
        loadIcon(guiset,"selection", selection );
        loadIcon(guiset,"selection-only", selection_only );
        loadIcon(guiset,"hand-drag", hand_drag );
        loadIcon(guiset,"erasing", erasing );
    guiset.endGroup();

    guiset.beginGroup("edit-resizing");
        loadIcon(guiset,"accept", accept );
        loadIcon(guiset,"cancel", cancel );
    guiset.endGroup();

    guiset.beginGroup("edit-place");
        loadIcon(guiset,"square-fill", square_fill );
        loadIcon(guiset,"line-tool", line_tool );
        loadIcon(guiset,"flood-fill", flood_fill );
        loadIcon(guiset,"overwrite-mode", overwrite_mode );
        loadIcon(guiset,"properties", properties );
    guiset.endGroup();

    guiset.beginGroup("edit-zoom");
        loadIcon(guiset,"zoom-reset", zoom_reset );
        loadIcon(guiset,"zoom-in", zoom_in );
        loadIcon(guiset,"zoom-out", zoom_out );
    guiset.endGroup();

    guiset.beginGroup("level");
        loadIcon(guiset,"section-settings", section_settings );
        loadIcon(guiset,"section-settings-16", section_settings_16 );
        loadIcon(guiset,"doors", doors );
        loadIcon(guiset,"layers", layers );
        loadIcon(guiset,"events", events );
        loadIcon(guiset,"water", water );
        loadIcon(guiset,"quicksand", quicksand );
        loadIcon(guiset,"player1", draw_player1 );
        loadIcon(guiset,"player2", draw_player2 );
    guiset.endGroup();

    guiset.beginGroup("level-items");
        loadImage(guiset,"player-point", player_point );
        loadImage(guiset,"player1", player1 );
        loadImage(guiset,"player2", player2 );
    guiset.endGroup();

    guiset.beginGroup("section-numbers");
        loadImage(guiset,"section-1-selected", section_1_selected );
        loadImage(guiset,"section-1-default", section_1_default );
        loadImage(guiset,"section-2-selected", section_2_selected );
        loadImage(guiset,"section-2-default", section_2_default );
        loadImage(guiset,"section-3-selected", section_3_selected );
        loadImage(guiset,"section-3-default", section_3_default );
        loadImage(guiset,"section-4-selected", section_4_selected );
        loadImage(guiset,"section-4-default", section_4_default );
        loadImage(guiset,"section-5-selected", section_5_selected );
        loadImage(guiset,"section-5-default", section_5_default );
        loadImage(guiset,"section-6-selected", section_6_selected );
        loadImage(guiset,"section-6-default", section_6_default );
        loadImage(guiset,"section-7-selected", section_7_selected );
        loadImage(guiset,"section-7-default", section_7_default );
        loadImage(guiset,"section-8-selected", section_8_selected );
        loadImage(guiset,"section-8-default", section_8_default );
        loadImage(guiset,"section-9-selected", section_9_selected );
        loadImage(guiset,"section-9-default", section_9_default );
        loadImage(guiset,"section-10-selected", section_10_selected );
        loadImage(guiset,"section-10-default", section_10_default );
        loadImage(guiset,"section-11-selected", section_11_selected );
        loadImage(guiset,"section-11-default", section_11_default );
        loadImage(guiset,"section-12-selected", section_12_selected );
        loadImage(guiset,"section-12-default", section_12_default );
        loadImage(guiset,"section-13-selected", section_13_selected );
        loadImage(guiset,"section-13-default", section_13_default );
        loadImage(guiset,"section-14-selected", section_14_selected );
        loadImage(guiset,"section-14-default", section_14_default );
        loadImage(guiset,"section-15-selected", section_15_selected );
        loadImage(guiset,"section-15-default", section_15_default );
        loadImage(guiset,"section-16-selected", section_16_selected );
        loadImage(guiset,"section-16-default", section_16_default );
        loadImage(guiset,"section-17-selected", section_17_selected );
        loadImage(guiset,"section-17-default", section_17_default );
        loadImage(guiset,"section-18-selected", section_18_selected );
        loadImage(guiset,"section-18-default", section_18_default );
        loadImage(guiset,"section-19-selected", section_19_selected );
        loadImage(guiset,"section-19-default", section_19_default );
        loadImage(guiset,"section-20-selected", section_20_selected );
        loadImage(guiset,"section-20-default", section_20_default );
        loadImage(guiset,"section-21-selected", section_21_selected );
        loadImage(guiset,"section-21-default", section_21_default );
        loadIcon(guiset,"section-reset-pos", section_reset_pos );
    guiset.endGroup();

    guiset.beginGroup("locks-level");
        loadImage(guiset,"blocks-free", blocks_free );
        loadImage(guiset,"blocks-locked", blocks_locked );
        loadImage(guiset,"bgo-free", bgo_free );
        loadImage(guiset,"bgo-locked", bgo_locked );
        loadImage(guiset,"npc-free", npc_free );
        loadImage(guiset,"npc-locked", npc_locked );
        loadImage(guiset,"warps-free", warps_free );
        loadImage(guiset,"warps-locked", warps_locked );
        loadImage(guiset,"physenv-free", physenv_free );
        loadImage(guiset,"physenv-locked", physenv_locked );
    guiset.endGroup();

    guiset.beginGroup("locks-world");
        loadImage(guiset,"tiles-free", tiles_free );
        loadImage(guiset,"tiles-locked", tiles_locked );
        loadImage(guiset,"scenery-free", scenery_free );
        loadImage(guiset,"scenery-locked", scenery_locked );
        loadImage(guiset,"paths-free", paths_free );
        loadImage(guiset,"paths-locked", paths_locked );
        loadImage(guiset,"levels-free", levels_free );
        loadImage(guiset,"levels-locked", levels_locked );
        loadImage(guiset,"musicboxes-free", musicboxes_free );
        loadImage(guiset,"musicboxes-locked", musicboxes_locked );
    guiset.endGroup();

    guiset.beginGroup("items");
        loadImage(guiset,"level-background", dummy_bg );
        loadImage(guiset,"level-bgo", dummy_bgo );
        loadImage(guiset,"level-block", dummy_block );
        loadImage(guiset,"level-npc", dummy_npc );
        loadImage(guiset,"world-path", dummy_path );
        loadImage(guiset,"world-scenery", dummy_scenery );
        loadImage(guiset,"world-tile", dummy_tile );
        loadImage(guiset,"world-level", dummy_wlevel );
        loadImage(guiset,"world-musicbox", dummy_musicbox );
    guiset.endGroup();

    currentThemeDir=themeDir;

    initCursors();
}


void Themes::loadIcon(QSettings &s, QString value, Themes::Icons icn)
{
    if(!s.value(value, "").toString().isEmpty())
    {
        QIcon   tmpIcn;
        tmpIcn = QIcon(theme_dir + s.value(value, "").toString());
        if(!tmpIcn.isNull())
            icons_map[icn] = tmpIcn;
    }

}

void Themes::loadImage(QSettings &s, QString value, Themes::Images img)
{
    if(!s.value(value, "").toString().isEmpty())
    {
        QPixmap tmpImg;
        tmpImg = QPixmap(theme_dir + s.value(value, "").toString());
        if(!tmpImg.isNull())
            images_map[img] = tmpImg;
    }
}

void Themes::loadInteger(QSettings &s, QString value, Themes::Images intVal)
{
    if(!s.value(value, "").toString().isEmpty())
    {
        int tmpVal;
        tmpVal = s.value(value, "-1").toInt();
        if(tmpVal>=0)
            int_map[intVal] = tmpVal;
    }
}

void Themes::initCursors()
{
    cursor_map[cursor_pasting] = QCursor(images_map[cursor_pasting],
                                         int_map[cursor_pasting_x],
                                         int_map[cursor_pasting_y]
                                         );

    cursor_map[cursor_erasing] = QCursor(images_map[cursor_erasing],
                                         int_map[cursor_erasing_x],
                                         int_map[cursor_erasing_y]
                                         );

    cursor_map[cursor_flood_fill] = QCursor(images_map[cursor_flood_fill],
                                         int_map[cursor_flood_fill_x],
                                         int_map[cursor_flood_fill_y]
                                         );

    if(images_map.contains(cursor_normal))
        cursor_map[cursor_normal] = QCursor(images_map[cursor_normal],
                                            int_map[cursor_normal_x],
                                            int_map[cursor_normal_y]
                                            );
    else
        cursor_map[cursor_normal] = QCursor(Qt::ArrowCursor);


    if(images_map.contains(cursor_resizing))
        cursor_map[cursor_resizing] = QCursor(images_map[cursor_resizing],
                                            int_map[cursor_resizing_x],
                                            int_map[cursor_resizing_y]
                                            );
    else
        cursor_map[cursor_resizing] = QCursor(Qt::CrossCursor);

    if(images_map.contains(cursor_placing))
        cursor_map[cursor_placing] = QCursor(images_map[cursor_placing],
                                            int_map[cursor_placing_x],
                                            int_map[cursor_placing_y]
                                            );
    else
        cursor_map[cursor_placing] = QCursor(Qt::CrossCursor);


    if(images_map.contains(cursor_square_fill))
        cursor_map[cursor_square_fill] = QCursor(images_map[cursor_square_fill],
                                            int_map[cursor_square_fill_x],
                                            int_map[cursor_square_fill_y]
                                            );
    else
        cursor_map[cursor_square_fill] = QCursor(Qt::CrossCursor);

    if(images_map.contains(cursor_line_fill))
        cursor_map[cursor_line_fill] = QCursor(images_map[cursor_line_fill],
                                            int_map[cursor_line_fill_x],
                                            int_map[cursor_line_fill_y]
                                            );
    else
        cursor_map[cursor_line_fill] = QCursor(Qt::CrossCursor);
}


QIcon Themes::icon(Themes::Icons icn)
{
    if(isLoaded && icons_map.contains(icn))
        return icons_map[icn];
    else
        return QIcon();
}

QPixmap Themes::Image(Themes::Images img)
{
    if(isLoaded && images_map.contains(img))
        return images_map[img];
    else
        return QPixmap();
}

int Themes::Integer(Themes::Images intval)
{
    if(isLoaded && int_map.contains(intval))
        return int_map[intval];
    else
        return 0;
}

QCursor Themes::Cursor(Themes::Images intval)
{
    if(isLoaded && cursor_map.contains(intval))
        return cursor_map[intval];
    else
        return QCursor(Qt::ArrowCursor);
}
