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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "themes.h"

bool Themes::isLoaded=false;
QString Themes::currentTheme="";

QVector<QIcon > Themes::icons;
QMap<Themes::Icons, QIcon > Themes::icons_map;

QVector<QPixmap > Themes::images;
QMap<Themes::Images, QPixmap > Themes::images_map;

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

    //icons.push_back(QIcon(":/images/"));
    icons.push_back(QIcon(":/lvl16.png"));
    icons_map[level_16] = icons.last();

    icons.push_back(QIcon(":/images/level.png"));
    icons_map[level_24] = icons.last();

    icons.push_back(QIcon("://images/world16.png"));
    icons_map[world_16] = icons.last();

    icons.push_back(QIcon("://images/world.png"));
    icons_map[world_24] = icons.last();

    icons.push_back(QIcon("://images/coin.png"));
    icons_map[npc_16] = icons.last();


    icons.push_back(QIcon("://images/open.png"));
    icons_map[file_open] = icons.last();

    icons.push_back(QIcon("://images/new.png"));
    icons_map[file_new] = icons.last();

    icons.push_back(QIcon("://images/save.png"));
    icons_map[file_save] = icons.last();

    icons.push_back(QIcon("://images/saveas.png"));
    icons_map[file_saveas] = icons.last();

    icons.push_back(QIcon("://reload.png"));
    icons_map[file_reload] = icons.last();

    icons.push_back(QIcon("://images/playmusic.png"));
    icons_map[playmusic] = icons.last();


    icons.push_back(QIcon("://images/grid.png"));
    icons_map[grid_snap] = icons.last();

    icons.push_back(QIcon("://images/animation.png"));
    icons_map[animation] = icons.last();

    icons.push_back(QIcon("://images/search.png"));
    icons_map[search] = icons.last();

    icons.push_back(QIcon("://images/pencil_16x16.png"));
    icons_map[pencil] = icons.last();

    icons.push_back(QIcon("://images/edit_copy.png"));
    icons_map[copy] = icons.last();

    icons.push_back(QIcon("://images/copy_16x16.png"));
    icons_map[copy_16] = icons.last();

    icons.push_back(QIcon("://images/edit_cut.png"));
    icons_map[cut] = icons.last();

    icons.push_back(QIcon("://images/edit_paste.png"));
    icons_map[paste] = icons.last();

    icons.push_back(QIcon("://images/edit_undo.png"));
    icons_map[undo] = icons.last();

    icons.push_back(QIcon("://images/edit_redo.png"));
    icons_map[redo] = icons.last();


    icons.push_back(QIcon("://images/arrow.png"));
    icons_map[selection] = icons.last();

    icons.push_back(QIcon("://select_only.png"));
    icons_map[selection_only] = icons.last();

    icons.push_back(QIcon("://hand.png"));
    icons_map[hand_drag] = icons.last();

    icons.push_back(QIcon("://images/rubber.png"));
    icons_map[erasing] = icons.last();

    icons.push_back(QIcon("://images/resize_accept.png"));
    icons_map[accept] = icons.last();

    icons.push_back(QIcon("://images/resize_cancel.png"));
    icons_map[cancel] = icons.last();


    icons.push_back(QIcon("://images/square_fill.png"));
    icons_map[square_fill] = icons.last();

    icons.push_back(QIcon("://images/line_fill.png"));
    icons_map[line_tool] = icons.last();

    icons.push_back(QIcon("://images/overwrite.png"));
    icons_map[overwrite_mode] = icons.last();

    icons.push_back(QIcon("://images/zoom_reset.png"));
    icons_map[zoom_reset] = icons.last();

    icons.push_back(QIcon("://images/zoom_in.png"));
    icons_map[zoom_in] = icons.last();

    icons.push_back(QIcon("://images/zoom_out.png"));
    icons_map[zoom_out] = icons.last();

    icons.push_back(QIcon("://images/section.png"));
    icons_map[section_settings] = icons.last();

    icons.push_back(QIcon("://images/section16.png"));
    icons_map[section_settings_16] = icons.last();

    icons.push_back(QIcon("://doors.png"));
    icons_map[doors] = icons.last();

    icons.push_back(QIcon("://layers.png"));
    icons_map[layers] = icons.last();

    icons.push_back(QIcon("://images/events.png"));
    icons_map[events] = icons.last();

    icons.push_back(QIcon("://drawWater.png"));
    icons_map[water] = icons.last();

    icons.push_back(QIcon("://drawQuickSand.png"));
    icons_map[quicksand] = icons.last();

    icons.push_back(QIcon("://images/player1_start.png"));
    icons_map[draw_player1] = icons.last();

    icons.push_back(QIcon("://images/player2_start.png"));
    icons_map[draw_player2] = icons.last();



    icons.push_back(QIcon(":/sections/sections/01.png"));
    icons_map[section_1_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/01_n.png"));
    icons_map[section_1_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/02.png"));
    icons_map[section_2_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/02_n.png"));
    icons_map[section_2_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/03.png"));
    icons_map[section_3_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/03_n.png"));
    icons_map[section_3_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/04.png"));
    icons_map[section_4_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/04_n.png"));
    icons_map[section_4_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/05.png"));
    icons_map[section_5_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/05_n.png"));
    icons_map[section_5_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/06.png"));
    icons_map[section_6_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/06_n.png"));
    icons_map[section_6_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/07.png"));
    icons_map[section_7_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/07_n.png"));
    icons_map[section_7_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/08.png"));
    icons_map[section_8_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/08_n.png"));
    icons_map[section_8_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/09.png"));
    icons_map[section_9_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/09_n.png"));
    icons_map[section_9_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/10.png"));
    icons_map[section_10_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/010_n.png"));
    icons_map[section_10_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/11.png"));
    icons_map[section_11_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/011_n.png"));
    icons_map[section_11_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/12.png"));
    icons_map[section_12_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/012_n.png"));
    icons_map[section_12_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/13.png"));
    icons_map[section_13_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/013_n.png"));
    icons_map[section_13_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/14.png"));
    icons_map[section_14_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/014_n.png"));
    icons_map[section_14_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/15.png"));
    icons_map[section_15_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/015_n.png"));
    icons_map[section_15_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/16.png"));
    icons_map[section_16_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/016_n.png"));
    icons_map[section_16_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/17.png"));
    icons_map[section_17_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/017_n.png"));
    icons_map[section_17_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/18.png"));
    icons_map[section_18_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/018_n.png"));
    icons_map[section_18_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/19.png"));
    icons_map[section_19_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/019_n.png"));
    icons_map[section_19_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/20.png"));
    icons_map[section_20_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/020_n.png"));
    icons_map[section_20_default] = icons.last();
    icons.push_back(QIcon(":/sections/sections/21.png"));
    icons_map[section_21_selected] = icons.last();
    icons.push_back(QIcon(":/sections/sections/021_n.png"));
    icons_map[section_21_default] = icons.last();



    icons.push_back(QIcon("://locks/block_op.png"));
    icons_map[blocks_free] = icons.last();

    icons.push_back(QIcon("://locks/block_cl.png"));
    icons_map[blocks_locked] = icons.last();

    icons.push_back(QIcon("://locks/bgo_op.png"));
    icons_map[bgo_free] = icons.last();

    icons.push_back(QIcon("://locks/bgo_cl.png"));
    icons_map[bgo_locked] = icons.last();

    icons.push_back(QIcon("://locks/npc_op.png"));
    icons_map[npc_free] = icons.last();

    icons.push_back(QIcon("://locks/npc_cl.png"));
    icons_map[npc_locked] = icons.last();

    icons.push_back(QIcon("://locks/door_op.png"));
    icons_map[warps_free] = icons.last();

    icons.push_back(QIcon("://locks/npc_cl.png"));
    icons_map[warps_locked] = icons.last();

    icons.push_back(QIcon("://locks/water_op.png"));
    icons_map[physenv_free] = icons.last();

    icons.push_back(QIcon("://locks/water_cl.png"));
    icons_map[physenv_locked] = icons.last();



    icons.push_back(QIcon("://images/lock_tile_op.png"));
    icons_map[tiles_free] = icons.last();

    icons.push_back(QIcon("://images/lock_tile_cl.png"));
    icons_map[tiles_locked] = icons.last();

    icons.push_back(QIcon("://images/lock_scenery_op.png"));
    icons_map[scenery_free] = icons.last();

    icons.push_back(QIcon("://images/lock_scenery_cl.png"));
    icons_map[scenery_locked] = icons.last();

    icons.push_back(QIcon("://images/lock_paths_op.png"));
    icons_map[paths_free] = icons.last();

    icons.push_back(QIcon("://images/lock_paths_cl.png"));
    icons_map[paths_locked] = icons.last();

    icons.push_back(QIcon("://images/lock_levels_op.png"));
    icons_map[levels_free] = icons.last();

    icons.push_back(QIcon("://images/lock_levels_cl.png"));
    icons_map[levels_locked] = icons.last();

    icons.push_back(QIcon("://images/lock_musicbox_op.png"));
    icons_map[musicboxes_free] = icons.last();

    icons.push_back(QIcon("://images/lock_musicbox_cl.png"));
    icons_map[musicboxes_locked] = icons.last();

    //Load default images

    images.push_back(QPixmap("://images/splash_editor.png"));
    images_map[splash] = images.last();

    images.push_back(QPixmap("://cur_pasta.png"));
    images_map[cursor_pasting] = images.last();

    images.push_back(QPixmap("://cur_rubber.png"));
    images_map[cursor_erasing] = images.last();

    images.push_back(QPixmap("://player.png"));
    images_map[player_point] = images.last();

    images.push_back(QPixmap("://player1.png"));
    images_map[player1] = images.last();

    images.push_back(QPixmap("://player2.png"));
    images_map[player2] = images.last();

    isLoaded=true;
}

QIcon Themes::icon(Themes::Icons icn)
{
    if(isLoaded)
        return icons_map[icn];
    else
        return QIcon();
}

QPixmap Themes::icon(Themes::Images img)
{
    if(isLoaded)
        return images_map[img];
    else
        return QPixmap();
}
