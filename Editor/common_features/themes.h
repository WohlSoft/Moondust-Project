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

#ifndef THEMES_H
#define THEMES_H

#include <QString>
#include <QIcon>
#include <QCursor>
#include <QMap>
#include <QDir>
#include <QVector>
#include <QImage>
#include <QSettings>
#include "app_path.h"


class Themes
{
public:
    Themes();

    enum Icons
    {
        level_16=0,
        level_24,
        world_16,
        world_24,
        npc_16,

        file_open,
        file_new,
        file_save,
        file_saveas,
        file_reload,

        debugger,

        playmusic,
        grid_snap,
        animation,
        search,
        pencil,

        copy,
        copy_16,
        cut,
        paste,

        undo,
        redo,

        selection,
        selection_only,
        hand_drag,
        erasing,

        accept,
        cancel,

        square_fill,
        line_tool,
        flood_fill,
        overwrite_mode,

        properties,

        zoom_reset,
        zoom_in,
        zoom_out,

        section_settings,
        section_settings_16,
        doors,
        layers,
        events,
        water,
        quicksand,
        draw_player1,
        draw_player2,

        section_reset_pos
    };

    enum Images
    {
        splash=0,
        cursor_normal,
        cursor_normal_x,
        cursor_normal_y,

        cursor_pasting,
        cursor_pasting_x,
        cursor_pasting_y,

        cursor_erasing,
        cursor_erasing_x,
        cursor_erasing_y,

        cursor_resizing,
        cursor_resizing_x,
        cursor_resizing_y,

        cursor_placing,
        cursor_placing_x,
        cursor_placing_y,

        cursor_square_fill,
        cursor_square_fill_x,
        cursor_square_fill_y,

        cursor_line_fill,
        cursor_line_fill_x,
        cursor_line_fill_y,

        cursor_flood_fill,
        cursor_flood_fill_x,
        cursor_flood_fill_y,

        player_point,
        player1,
        player2,

        section_1_selected,
        section_1_default,
        section_2_selected,
        section_2_default,
        section_3_selected,
        section_3_default,
        section_4_selected,
        section_4_default,
        section_5_selected,
        section_5_default,
        section_6_selected,
        section_6_default,
        section_7_selected,
        section_7_default,
        section_8_selected,
        section_8_default,
        section_9_selected,
        section_9_default,
        section_10_selected,
        section_10_default,
        section_11_selected,
        section_11_default,
        section_12_selected,
        section_12_default,
        section_13_selected,
        section_13_default,
        section_14_selected,
        section_14_default,
        section_15_selected,
        section_15_default,
        section_16_selected,
        section_16_default,
        section_17_selected,
        section_17_default,
        section_18_selected,
        section_18_default,
        section_19_selected,
        section_19_default,
        section_20_selected,
        section_20_default,
        section_21_selected,
        section_21_default,

        blocks_free,
        blocks_locked,
        bgo_free,
        bgo_locked,
        npc_free,
        npc_locked,
        warps_free,
        warps_locked,
        physenv_free,
        physenv_locked,

        tiles_free,
        tiles_locked,
        scenery_free,
        scenery_locked,
        paths_free,
        paths_locked,
        levels_free,
        levels_locked,
        musicboxes_free,
        musicboxes_locked
    };

    static void init();
    static QStringList availableThemes();
    static QString currentTheme();
    static void loadTheme(QString themeDir);

    static QIcon icon(Icons icn);
    static QPixmap Image(Images img);
    static int Integer(Images intval);
    static QCursor Cursor(Images intval);

private:
    static bool isLoaded;
    static QString currentThemeDir;

    static QMap<Icons, QIcon > icons_map;
    static QMap<Images, QPixmap > images_map;
    static QMap<Images, int> int_map;
    static QMap<Images, QCursor> cursor_map;

    static void loadIcon(QSettings &s, QString value, Icons icn);
    static void loadImage(QSettings &s, QString value, Images img);
    static void loadInteger(QSettings &s, QString value, Images intVal);
    static void initCursors();
    static QString theme_dir;
};

#endif // THEMES_H
