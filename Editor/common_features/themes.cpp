/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

QVector<QIcon > Themes::icons;
QMap<Themes::Icons, QIcon > Themes::icons_map;

QVector<QPixmap > Themes::images;
QMap<Themes::Images, QPixmap > Themes::images_map;

QString Themes::theme_dir="";


Themes::Themes()
{

}

void Themes::init()
{
    if(icons.size()>0)
    {
        icons.clear();
        icons_map.clear();

        images.clear();
        images_map.clear();
    }

    //init default icons
    icons.push_back(QIcon(":/lvl16.png"));
    icons_map[level_16] = icons.last();

    icons.push_back(QIcon(":/images/level.png"));
    icons_map[level_24] = icons.last();

    icons.push_back(QIcon(":/images/world16.png"));
    icons_map[world_16] = icons.last();

    icons.push_back(QIcon(":/images/world.png"));
    icons_map[world_24] = icons.last();

    icons.push_back(QIcon(":/images/coin.png"));
    icons_map[npc_16] = icons.last();


    icons.push_back(QIcon(":/images/open.png"));
    icons_map[file_open] = icons.last();

    icons.push_back(QIcon(":/images/new.png"));
    icons_map[file_new] = icons.last();

    icons.push_back(QIcon(":/images/save.png"));
    icons_map[file_save] = icons.last();

    icons.push_back(QIcon(":/images/saveas.png"));
    icons_map[file_saveas] = icons.last();

    icons.push_back(QIcon(":/reload.png"));
    icons_map[file_reload] = icons.last();


    icons.push_back(QIcon(":/images/bug.png"));
    icons_map[debugger] = icons.last();


    icons.push_back(QIcon(":/images/playmusic.png"));
    icons_map[playmusic] = icons.last();

    icons.push_back(QIcon(":/images/grid.png"));
    icons_map[grid_snap] = icons.last();

    icons.push_back(QIcon(":/images/animation.png"));
    icons_map[animation] = icons.last();

    icons.push_back(QIcon(":/images/search.png"));
    icons_map[search] = icons.last();

    icons.push_back(QIcon(":/images/pencil_16x16.png"));
    icons_map[pencil] = icons.last();

    icons.push_back(QIcon(":/images/edit_copy.png"));
    icons_map[copy] = icons.last();

    icons.push_back(QIcon(":/images/copy_16x16.png"));
    icons_map[copy_16] = icons.last();

    icons.push_back(QIcon(":/images/edit_cut.png"));
    icons_map[cut] = icons.last();

    icons.push_back(QIcon(":/images/edit_paste.png"));
    icons_map[paste] = icons.last();

    icons.push_back(QIcon(":/images/edit_undo.png"));
    icons_map[undo] = icons.last();

    icons.push_back(QIcon(":/images/edit_redo.png"));
    icons_map[redo] = icons.last();


    icons.push_back(QIcon(":/images/arrow.png"));
    icons_map[selection] = icons.last();

    icons.push_back(QIcon(":/select_only.png"));
    icons_map[selection_only] = icons.last();

    icons.push_back(QIcon(":/hand.png"));
    icons_map[hand_drag] = icons.last();

    icons.push_back(QIcon(":/images/rubber.png"));
    icons_map[erasing] = icons.last();

    icons.push_back(QIcon(":/images/resize_accept.png"));
    icons_map[accept] = icons.last();

    icons.push_back(QIcon(":/images/resize_cancel.png"));
    icons_map[cancel] = icons.last();


    icons.push_back(QIcon(":/images/square_fill.png"));
    icons_map[square_fill] = icons.last();

    icons.push_back(QIcon(":/images/line_fill.png"));
    icons_map[line_tool] = icons.last();

    icons.push_back(QIcon(":/images/overwrite.png"));
    icons_map[overwrite_mode] = icons.last();

    icons.push_back(QIcon(":/images/zoom_reset.png"));
    icons_map[zoom_reset] = icons.last();

    icons.push_back(QIcon(":/images/zoom_in.png"));
    icons_map[zoom_in] = icons.last();

    icons.push_back(QIcon(":/images/zoom_out.png"));
    icons_map[zoom_out] = icons.last();

    icons.push_back(QIcon(":/images/section.png"));
    icons_map[section_settings] = icons.last();

    icons.push_back(QIcon(":/images/section16.png"));
    icons_map[section_settings_16] = icons.last();

    icons.push_back(QIcon(":/doors.png"));
    icons_map[doors] = icons.last();

    icons.push_back(QIcon(":/layers.png"));
    icons_map[layers] = icons.last();

    icons.push_back(QIcon(":/images/events.png"));
    icons_map[events] = icons.last();

    icons.push_back(QIcon(":/drawWater.png"));
    icons_map[water] = icons.last();

    icons.push_back(QIcon(":/drawQuickSand.png"));
    icons_map[quicksand] = icons.last();

    icons.push_back(QIcon(":/images/player1_start.png"));
    icons_map[draw_player1] = icons.last();

    icons.push_back(QIcon(":/images/player2_start.png"));
    icons_map[draw_player2] = icons.last();



    images.push_back(QPixmap(":/sections/sections/01.png"));
    images_map[section_1_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/01_n.png"));
    images_map[section_1_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/02.png"));
    images_map[section_2_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/02_n.png"));
    images_map[section_2_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/03.png"));
    images_map[section_3_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/03_n.png"));
    images_map[section_3_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/04.png"));
    images_map[section_4_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/04_n.png"));
    images_map[section_4_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/05.png"));
    images_map[section_5_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/05_n.png"));
    images_map[section_5_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/06.png"));
    images_map[section_6_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/06_n.png"));
    images_map[section_6_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/07.png"));
    images_map[section_7_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/07_n.png"));
    images_map[section_7_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/08.png"));
    images_map[section_8_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/08_n.png"));
    images_map[section_8_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/09.png"));
    images_map[section_9_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/09_n.png"));
    images_map[section_9_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/10.png"));
    images_map[section_10_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/10_n.png"));
    images_map[section_10_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/11.png"));
    images_map[section_11_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/11_n.png"));
    images_map[section_11_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/12.png"));
    images_map[section_12_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/12_n.png"));
    images_map[section_12_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/13.png"));
    images_map[section_13_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/13_n.png"));
    images_map[section_13_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/14.png"));
    images_map[section_14_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/14_n.png"));
    images_map[section_14_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/15.png"));
    images_map[section_15_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/15_n.png"));
    images_map[section_15_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/16.png"));
    images_map[section_16_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/16_n.png"));
    images_map[section_16_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/17.png"));
    images_map[section_17_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/17_n.png"));
    images_map[section_17_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/18.png"));
    images_map[section_18_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/18_n.png"));
    images_map[section_18_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/19.png"));
    images_map[section_19_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/19_n.png"));
    images_map[section_19_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/20.png"));
    images_map[section_20_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/20_n.png"));
    images_map[section_20_default] = images.last();
    images.push_back(QPixmap(":/sections/sections/21.png"));
    images_map[section_21_selected] = images.last();
    images.push_back(QPixmap(":/sections/sections/21_n.png"));
    images_map[section_21_default] = images.last();

    icons.push_back(QIcon(":/images/reset_pos.png"));
    icons_map[section_reset_pos] = icons.last();



    images.push_back(QPixmap(":/locks/block_op.png"));
    images_map[blocks_free] = images.last();

    images.push_back(QPixmap(":/locks/block_cl.png"));
    images_map[blocks_locked] = images.last();

    images.push_back(QPixmap(":/locks/bgo_op.png"));
    images_map[bgo_free] = images.last();

    images.push_back(QPixmap(":/locks/bgo_cl.png"));
    images_map[bgo_locked] = images.last();

    images.push_back(QPixmap(":/locks/npc_op.png"));
    images_map[npc_free] = images.last();

    images.push_back(QPixmap(":/locks/npc_cl.png"));
    images_map[npc_locked] = images.last();

    images.push_back(QPixmap(":/locks/door_op.png"));
    images_map[warps_free] = images.last();

    images.push_back(QPixmap(":/locks/npc_cl.png"));
    images_map[warps_locked] = images.last();

    images.push_back(QPixmap(":/locks/water_op.png"));
    images_map[physenv_free] = images.last();

    images.push_back(QPixmap(":/locks/water_cl.png"));
    images_map[physenv_locked] = images.last();



    images.push_back(QPixmap(":/images/lock_tile_op.png"));
    images_map[tiles_free] = images.last();

    images.push_back(QPixmap(":/images/lock_tile_cl.png"));
    images_map[tiles_locked] = images.last();

    images.push_back(QPixmap(":/images/lock_scenery_op.png"));
    images_map[scenery_free] = images.last();

    images.push_back(QPixmap(":/images/lock_scenery_cl.png"));
    images_map[scenery_locked] = images.last();

    images.push_back(QPixmap(":/images/lock_paths_op.png"));
    images_map[paths_free] = images.last();

    images.push_back(QPixmap(":/images/lock_paths_cl.png"));
    images_map[paths_locked] = images.last();

    images.push_back(QPixmap(":/images/lock_levels_op.png"));
    images_map[levels_free] = images.last();

    images.push_back(QPixmap(":/images/lock_levels_cl.png"));
    images_map[levels_locked] = images.last();

    images.push_back(QPixmap(":/images/lock_musicbox_op.png"));
    images_map[musicboxes_free] = images.last();

    images.push_back(QPixmap(":/images/lock_musicbox_cl.png"));
    images_map[musicboxes_locked] = images.last();

    //Load default images

    images.push_back(QPixmap(":/images/splash_editor.png"));
    images_map[splash] = images.last();

    images.push_back(QPixmap(":/cur_pasta.png"));
    images_map[cursor_pasting] = images.last();

    images.push_back(QPixmap(":/cur_rubber.png"));
    images_map[cursor_erasing] = images.last();

    images.push_back(QPixmap(":/player.png"));
    images_map[player_point] = images.last();

    images.push_back(QPixmap(":/player1.png"));
    images_map[player1] = images.last();

    images.push_back(QPixmap(":/player2.png"));
    images_map[player2] = images.last();

    currentThemeDir = "";
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
    guiset.endGroup();

    guiset.beginGroup("cursors");
        loadImage(guiset,"pasting", cursor_pasting );
        loadImage(guiset,"erasing", cursor_erasing );
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
        loadIcon(guiset,"overwrite-mode", overwrite_mode );
    guiset.endGroup();

    guiset.beginGroup("edit-zoom");
        loadIcon(guiset,"zoom-reset", square_fill );
        loadIcon(guiset,"zoom-in", line_tool );
        loadIcon(guiset,"zoom-out", overwrite_mode );
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

    currentThemeDir=themeDir;
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


QIcon Themes::icon(Themes::Icons icn)
{
    if(isLoaded)
        return icons_map[icn];
    else
        return QIcon();
}

QPixmap Themes::Image(Themes::Images img)
{
    if(isLoaded)
        return images_map[img];
    else
        return QPixmap();
}
