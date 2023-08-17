/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QApplication>
#include "themes.h"

bool Themes::isLoaded = false;
QString Themes::currentThemeDir = "";
QString Themes::styleSheet = "";

Themes::Palettes Themes::currentPalette = Themes::Palettes::Default;
QPalette Themes::currentPaletteD;
bool Themes::currentPaletteIsDark = false;

QMap<Themes::Icons, QIcon > Themes::icons_map;
QMap<Themes::Icons, QIcon > Themes::icons_map_dark;

QMap<Themes::Images, QPixmap > Themes::images_map;
QMap<Themes::Images, QPixmap > Themes::images_map_dark;

QMap<Themes::Images, int > Themes::int_map;
QMap<Themes::Images, QCursor> Themes::cursor_map;

QString Themes::theme_dir = QString();


Themes::Themes()
{}

void Themes::init()
{
    icons_map.clear();
    images_map.clear();
    int_map.clear();
    cursor_map.clear();
    images_map_dark.clear();
    icons_map_dark.clear();

    //init default icons
    icons_map[level_16]     = QIcon(":/toolbar/dock/level16.png");
    icons_map[level_24]     = QIcon(":/toolbar/dock/level.png");
    icons_map[world_16]     = QIcon(":/toolbar/dock/world16.png");
    icons_map[world_24]     = QIcon(":/toolbar/dock/world.png");
    icons_map[npc_16]       = QIcon(":/toolbar/dock/coin.png");

    icons_map[level_item_browser] = QIcon(":/toolbar/dock/item_browser_level.png");
    icons_map[world_item_browser] = QIcon(":/toolbar/dock/item_browser_world.png");
    icons_map[world_music_boxes] = QIcon(":/toolbar/dock/musicboxes.png");

    icons_map[file_open]    = QIcon(":/toolbar/file/open.png");
    icons_map[file_new]     = QIcon(":/toolbar/file/new.png");
    icons_map[file_save]    = QIcon(":/toolbar/file/save.png");
    icons_map[file_saveas]  = QIcon(":/toolbar/file/save_as.png");
    icons_map[file_reload]  = QIcon(":/toolbar/file/reload.png");

    icons_map[debugger]     = QIcon(":/toolbar/dock/bug.png");
    icons_map[tileset_box]  = QIcon(":/toolbar/dock/tileset_box.png");
    icons_map[bookmarks]    = QIcon(":/toolbar/dock/bookmarks.png");

    icons_map[playmusic]    = QIcon(":/toolbar/playmusic.png");
    icons_map_dark[playmusic] = QIcon(":/toolbar/playmusic_dark.png");

    icons_map[grid_snap]    = QIcon(":/toolbar/edit/grid_snap.png");
    icons_map_dark[grid_snap] = QIcon(":/toolbar/edit/grid_snap_dark.png");

    icons_map[show_grid]    = QIcon(":/toolbar/view/show_grid.png");
    icons_map_dark[show_grid] = QIcon(":/toolbar/view/show_grid_dark.png");
    icons_map[camera_grid]  = QIcon(":/toolbar/view/camera_grid.png");
    icons_map_dark[camera_grid] = QIcon(":/toolbar/view/camera_grid_dark.png");
    icons_map[animation]    = QIcon(":/toolbar/view/animation.png");

    icons_map[search]       = QIcon(":/toolbar/dock/search.png");
    icons_map[pencil]       = QIcon(":/dock/pencil_16x16.png");

    icons_map[copy]         = QIcon(":/toolbar/edit/copy.png");
    icons_map[copy_16]      = QIcon(":/dock/copy_16x16.png");
    icons_map[cut]          = QIcon(":/toolbar/edit/cut.png");
    icons_map[paste]        = QIcon(":/toolbar/edit/paste.png");

    icons_map[undo]         = QIcon(":/toolbar/edit/undo.png");
    icons_map[redo]         = QIcon(":/toolbar/edit/redo.png");

    icons_map[selection]    = QIcon(":/toolbar/modes/select.png");
    icons_map[selection_only] = QIcon(":/toolbar/modes/select_only.png");
    icons_map[hand_drag]    = QIcon(":/toolbar/modes/drag.png");
    icons_map[erasing]      = QIcon(":/toolbar/modes/erase.png");
    icons_map[accept]       = QIcon(":/toolbar/modes/accept.png");
    icons_map[cancel]       = QIcon(":/toolbar/modes/cancel.png");

    icons_map[rect_fill]  = QIcon(":/toolbar/place/fill_rect.png");
    icons_map[circle_fill]  = QIcon(":/toolbar/place/fill_circle.png");
    icons_map[line_tool]    = QIcon(":/toolbar/place/fill_line.png");
    icons_map[flood_fill]    = QIcon(":/toolbar/place/fill_flood.png");
    icons_map[overwrite_mode] = QIcon(":/toolbar/place/overwrite.png");

    icons_map[properties]   = QIcon(":/toolbar/dock/properties.png");

    icons_map[zoom_reset]   = QIcon(":/toolbar/view/zoom_reset.png");
    icons_map[zoom_in]      = QIcon(":/toolbar/view/zoom_in.png");
    icons_map[zoom_out]     = QIcon(":/toolbar/view/zoom_out.png");

    icons_map[section_settings] = QIcon(":/toolbar/dock/section.png");
    icons_map[section_settings_16] = QIcon(":/toolbar/dock/section16.png");
    icons_map[doors]        = QIcon(":/toolbar/dock/doors.png");
    icons_map[layers]       = QIcon(":/toolbar/dock/layers.png");
    icons_map[events]       = QIcon(":/toolbar/dock/events.png");

    icons_map[water]        = QIcon(":/toolbar/modes/draw_water.png");
    icons_map[quicksand]    = QIcon(":/toolbar/modes/draw_sand.png");
    icons_map[draw_player1] = QIcon(":/toolbar/modes/player1_start.png");
    icons_map[draw_player2] = QIcon(":/toolbar/modes/player2_start.png");

    images_map[section_0_selected] = QPixmap(":/sections/00.png");
    images_map[section_0_default]  = QPixmap(":/sections/00_n.png");
    images_map_dark[section_0_default]  = QPixmap(":/sections/00_dark.png");
    images_map[section_1_selected] = QPixmap(":/sections/01.png");
    images_map[section_1_default]  = QPixmap(":/sections/01_n.png");
    images_map_dark[section_1_default]  = QPixmap(":/sections/01_dark.png");
    images_map[section_2_selected] = QPixmap(":/sections/02.png");
    images_map[section_2_default]  = QPixmap(":/sections/02_n.png");
    images_map_dark[section_2_default]  = QPixmap(":/sections/02_dark.png");
    images_map[section_3_selected] = QPixmap(":/sections/03.png");
    images_map[section_3_default]  = QPixmap(":/sections/03_n.png");
    images_map_dark[section_3_default]  = QPixmap(":/sections/03_dark.png");
    images_map[section_4_selected] = QPixmap(":/sections/04.png");
    images_map[section_4_default]  = QPixmap(":/sections/04_n.png");
    images_map_dark[section_4_default]  = QPixmap(":/sections/04_dark.png");
    images_map[section_5_selected] = QPixmap(":/sections/05.png");
    images_map[section_5_default]  = QPixmap(":/sections/05_n.png");
    images_map_dark[section_5_default]  = QPixmap(":/sections/05_dark.png");
    images_map[section_6_selected] = QPixmap(":/sections/06.png");
    images_map[section_6_default]  = QPixmap(":/sections/06_n.png");
    images_map_dark[section_6_default]  = QPixmap(":/sections/06_dark.png");
    images_map[section_7_selected] = QPixmap(":/sections/07.png");
    images_map[section_7_default]  = QPixmap(":/sections/07_n.png");
    images_map_dark[section_7_default]  = QPixmap(":/sections/07_dark.png");
    images_map[section_8_selected] = QPixmap(":/sections/08.png");
    images_map[section_8_default]  = QPixmap(":/sections/08_n.png");
    images_map_dark[section_8_default]  = QPixmap(":/sections/08_dark.png");
    images_map[section_9_selected] = QPixmap(":/sections/09.png");
    images_map[section_9_default]  = QPixmap(":/sections/09_n.png");
    images_map_dark[section_9_default]  = QPixmap(":/sections/09_dark.png");
    images_map[section_10_selected] = QPixmap(":/sections/10.png");
    images_map[section_10_default] = QPixmap(":/sections/10_n.png");
    images_map_dark[section_10_default]  = QPixmap(":/sections/10_dark.png");
    images_map[section_11_selected] = QPixmap(":/sections/11.png");
    images_map[section_11_default] = QPixmap(":/sections/11_n.png");
    images_map_dark[section_11_default]  = QPixmap(":/sections/11_dark.png");
    images_map[section_12_selected] = QPixmap(":/sections/12.png");
    images_map[section_12_default] = QPixmap(":/sections/12_n.png");
    images_map_dark[section_12_default]  = QPixmap(":/sections/12_dark.png");
    images_map[section_13_selected] = QPixmap(":/sections/13.png");
    images_map[section_13_default] = QPixmap(":/sections/13_n.png");
    images_map_dark[section_13_default]  = QPixmap(":/sections/13_dark.png");
    images_map[section_14_selected] = QPixmap(":/sections/14.png");
    images_map[section_14_default] = QPixmap(":/sections/14_n.png");
    images_map_dark[section_14_default]  = QPixmap(":/sections/14_dark.png");
    images_map[section_15_selected] = QPixmap(":/sections/15.png");
    images_map[section_15_default] = QPixmap(":/sections/15_n.png");
    images_map_dark[section_15_default]  = QPixmap(":/sections/15_dark.png");
    images_map[section_16_selected] = QPixmap(":/sections/16.png");
    images_map[section_16_default] = QPixmap(":/sections/16_n.png");
    images_map_dark[section_16_default]  = QPixmap(":/sections/16_dark.png");
    images_map[section_17_selected] = QPixmap(":/sections/17.png");
    images_map[section_17_default] = QPixmap(":/sections/17_n.png");
    images_map_dark[section_17_default]  = QPixmap(":/sections/17_dark.png");
    images_map[section_18_selected] = QPixmap(":/sections/18.png");
    images_map[section_18_default] = QPixmap(":/sections/18_n.png");
    images_map_dark[section_18_default]  = QPixmap(":/sections/18_dark.png");
    images_map[section_19_selected] = QPixmap(":/sections/19.png");
    images_map[section_19_default] = QPixmap(":/sections/19_n.png");
    images_map_dark[section_19_default]  = QPixmap(":/sections/19_dark.png");
    images_map[section_20_selected] = QPixmap(":/sections/20.png");
    images_map[section_20_default] = QPixmap(":/sections/20_n.png");
    images_map_dark[section_20_default]  = QPixmap(":/sections/20_dark.png");
    //images_map[section_21_selected] = QPixmap(":/sections/21.png");
    //images_map[section_21_default] = QPixmap(":/sections/21_n.png");

    icons_map[section_goto_left_bottom] = QIcon(":/toolbar/goto/left_bottom.png");
    icons_map[section_goto_left_top] = QIcon(":/toolbar/goto/left_top.png");
    icons_map[section_goto_top_right] = QIcon(":/toolbar/goto/top_right.png");
    icons_map[section_goto_right_bottom] = QIcon(":/toolbar/goto/right_bottom.png");
    icons_map_dark[section_goto_left_bottom] = QIcon(":/toolbar/goto/left_bottom_dark.png");
    icons_map_dark[section_goto_left_top] = QIcon(":/toolbar/goto/left_top_dark.png");
    icons_map_dark[section_goto_top_right] = QIcon(":/toolbar/goto/top_right_dark.png");
    icons_map_dark[section_goto_right_bottom] = QIcon(":/toolbar/goto/right_bottom_dark.png");

    images_map[blocks_free] = QPixmap(":/toolbar/locks/block_op.png");
    images_map[blocks_locked] = QPixmap(":/toolbar/locks/block_cl.png");
    images_map[bgo_free] = QPixmap(":/toolbar/locks/bgo_op.png");
    images_map[bgo_locked] = QPixmap(":/toolbar/locks/bgo_cl.png");
    images_map[npc_free] = QPixmap(":/toolbar/locks/npc_op.png");
    images_map[npc_locked] = QPixmap(":/toolbar/locks/npc_cl.png");
    images_map[warps_free] = QPixmap(":/toolbar/locks/door_op.png");
    images_map[warps_locked] = QPixmap(":/toolbar/locks/door_cl.png");
    images_map[physenv_free] = QPixmap(":/toolbar/locks/water_op.png");
    images_map[physenv_locked] = QPixmap(":/toolbar/locks/water_cl.png");
    images_map[tiles_free] = QPixmap(":/toolbar/locks/tile_op.png");
    images_map[tiles_locked] = QPixmap(":/toolbar/locks/tile_cl.png");
    images_map[scenery_free] = QPixmap(":/toolbar/locks/scenery_op.png");
    images_map[scenery_locked] = QPixmap(":/toolbar/locks/scenery_cl.png");
    images_map[paths_free] = QPixmap(":/toolbar/locks/paths_op.png");
    images_map[paths_locked] = QPixmap(":/toolbar/locks/paths_cl.png");
    images_map[levels_free] = QPixmap(":/toolbar/locks/levels_op.png");
    images_map[levels_locked] = QPixmap(":/toolbar/locks/levels_cl.png");
    images_map[musicboxes_free] = QPixmap(":/toolbar/locks/musicbox_op.png");
    images_map[musicboxes_locked] = QPixmap(":/toolbar/locks/musicbox_cl.png");

    //Load default images

    images_map[splash] = QPixmap(":/images/splash_editor.png");

    images_map[cursor_pasting] = QPixmap(":/cursors/pasta.png");
    int_map[cursor_pasting_x] = 0;
    int_map[cursor_pasting_y] = 0;

    images_map[cursor_erasing] = QPixmap(":/cursors/rubber.png");
    int_map[cursor_erasing_x] = 0;
    int_map[cursor_erasing_y] = 0;

    images_map[cursor_flood_fill] = QPixmap(":/cursors/flood_fill.png");
    int_map[cursor_flood_fill_x] = 2;
    int_map[cursor_flood_fill_y] = 21;

    images_map[player_point] = QPixmap(":/players/point.png");
    images_map[player1] = QPixmap(":/players/point1.png");
    images_map[player2] = QPixmap(":/players/point2.png");


    //dummy images
    images_map[dummy_musicbox] = QPixmap(":/dummies/music_box.png");
    images_map[dummy_bg] = QPixmap(":/dummies/dummy-bg.png");
    images_map[dummy_bgo] = QPixmap(":/dummies/unknown_bgo.png");
    images_map[dummy_block] = QPixmap(":/dummies/unknown_block.png");
    images_map[dummy_npc] = QPixmap(":/dummies/unknown_npc.png");
    images_map[dummy_path] = QPixmap(":/dummies/unknown_path.png");
    images_map[dummy_scenery] = QPixmap(":/dummies/unknown_scene.png");
    images_map[dummy_terrain] = QPixmap(":/dummies/unknown_tile.png");
    images_map[dummy_wlevel] = QPixmap(":/dummies/unknown_wlvl.png");

    currentThemeDir.clear();
    styleSheet.clear();
    //initCursors();

    isLoaded = true;
}

///
/// \brief Themes::availableThemes
/// \return list of available themes in the /themes directory
///
QStringList Themes::availableThemes()
{
    QString themesPath(ApplicationPath + "/themes/");
    QDir themesDir(themesPath);
    QStringList allThemes = themesDir.entryList(QDir::AllDirs);

    QStringList available_themes;

    for(QString &c : allThemes)
    {
        QString theme_dir = themesPath + c + "/";
        QString themeName;

        QString gui_ini = theme_dir + "theme.ini";

        if(!QFileInfo(gui_ini).exists()) continue; //Skip if it is not a config
        QSettings guiset(gui_ini, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        guiset.setIniCodec("UTF-8");
#endif

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

void Themes::loadTheme(const QString &themeDir)
{
    init();

    QString themesPath(ApplicationPath + "/themes/");
    theme_dir = themesPath + themeDir + "/";

    QString gui_ini = theme_dir + "theme.ini";
    if(!QFileInfo(gui_ini).exists()) return;

    QSettings guiset(gui_ini, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    guiset.setIniCodec("UTF-8");
#endif

    guiset.beginGroup("main");
    loadImage(guiset, "default-splash", splash);
    loadStyleSheet(guiset, "style-sheet");
    guiset.endGroup();

    guiset.beginGroup("file-icons");
    loadIcon(guiset, "level-16", level_16);
    loadIcon(guiset, "level-24", level_24);
    loadIcon(guiset, "world-16", world_16);
    loadIcon(guiset, "world-24", world_24);
    loadIcon(guiset, "npc-16", npc_16);
    guiset.endGroup();

    guiset.beginGroup("file-io");
    loadIcon(guiset, "file-open", file_open);
    loadIcon(guiset, "file-new", file_new);
    loadIcon(guiset, "file-save", file_save);
    loadIcon(guiset, "file-save-as", file_saveas);
    loadIcon(guiset, "file-reload", file_reload);
    guiset.endGroup();

    guiset.beginGroup("icons");
    loadIcon(guiset, "debugger", debugger);
    loadIcon(guiset, "bookmarks", bookmarks);
    loadIcon(guiset, "tileset-box", tileset_box);
    guiset.endGroup();

    guiset.beginGroup("cursors");
    loadImage(guiset, "normal", cursor_normal);
    loadInteger(guiset, "normal-x", cursor_normal_x);
    loadInteger(guiset, "normal-y", cursor_normal_y);

    loadImage(guiset, "pasting", cursor_pasting);
    loadInteger(guiset, "pasting-x", cursor_pasting_x);
    loadInteger(guiset, "pasting-y", cursor_pasting_y);

    loadImage(guiset, "erasing", cursor_erasing);
    loadInteger(guiset, "erasing-x", cursor_erasing_x);
    loadInteger(guiset, "erasing-y", cursor_erasing_y);

    loadImage(guiset, "resizing", cursor_resizing);
    loadInteger(guiset, "resizing-x", cursor_resizing_x);
    loadInteger(guiset, "resizing-y", cursor_resizing_y);

    loadImage(guiset, "placing", cursor_placing);
    loadInteger(guiset, "placing-x", cursor_placing_x);
    loadInteger(guiset, "placing-y", cursor_placing_y);

    loadImage(guiset, "square-fill", cursor_square_fill);
    loadInteger(guiset, "square-fill-x", cursor_square_fill_x);
    loadInteger(guiset, "square-fill-y", cursor_square_fill_y);

    loadImage(guiset, "line-fill", cursor_line_fill);
    loadInteger(guiset, "line-fill-x", cursor_line_fill_x);
    loadInteger(guiset, "line-fill-y", cursor_line_fill_y);

    loadImage(guiset, "flood-fill", cursor_flood_fill);
    loadInteger(guiset, "flood-fill-x", cursor_flood_fill_x);
    loadInteger(guiset, "flood-fill-y", cursor_flood_fill_y);
    guiset.endGroup();

    guiset.beginGroup("edit-common");
    loadIcon(guiset, "playmusic", playmusic);
    loadIcon(guiset, "grid-snap", grid_snap);
    loadIcon(guiset, "show-grid", show_grid);
    loadIcon(guiset, "camera-grid", camera_grid);
    loadIcon(guiset, "animation", animation);
    loadIcon(guiset, "search", search);
    loadIcon(guiset, "pencil", pencil);
    guiset.endGroup();

    guiset.beginGroup("edit-clipboard");
    loadIcon(guiset, "copy", copy);
    loadIcon(guiset, "copy-16", copy_16);
    loadIcon(guiset, "cut", cut);
    loadIcon(guiset, "paste", paste);
    guiset.endGroup();

    guiset.beginGroup("edit-history");
    loadIcon(guiset, "undo", undo);
    loadIcon(guiset, "redo", redo);
    guiset.endGroup();

    guiset.beginGroup("edit-mode");
    loadIcon(guiset, "selection", selection);
    loadIcon(guiset, "selection-only", selection_only);
    loadIcon(guiset, "hand-drag", hand_drag);
    loadIcon(guiset, "erasing", erasing);
    guiset.endGroup();

    guiset.beginGroup("edit-resizing");
    loadIcon(guiset, "accept", accept);
    loadIcon(guiset, "cancel", cancel);
    guiset.endGroup();

    guiset.beginGroup("edit-place");
    loadIcon(guiset, "square-fill", rect_fill);
    loadIcon(guiset, "circle-fill", circle_fill);
    loadIcon(guiset, "line-tool", line_tool);
    loadIcon(guiset, "flood-fill", flood_fill);
    loadIcon(guiset, "overwrite-mode", overwrite_mode);
    loadIcon(guiset, "properties", properties);
    guiset.endGroup();

    guiset.beginGroup("edit-zoom");
    loadIcon(guiset, "zoom-reset", zoom_reset);
    loadIcon(guiset, "zoom-in", zoom_in);
    loadIcon(guiset, "zoom-out", zoom_out);
    guiset.endGroup();

    guiset.beginGroup("level");
    loadIcon(guiset, "item-browser", level_item_browser);
    loadIcon(guiset, "section-settings", section_settings);
    loadIcon(guiset, "section-settings-16", section_settings_16);
    loadIcon(guiset, "doors", doors);
    loadIcon(guiset, "layers", layers);
    loadIcon(guiset, "events", events);
    loadIcon(guiset, "water", water);
    loadIcon(guiset, "quicksand", quicksand);
    loadIcon(guiset, "player1", draw_player1);
    loadIcon(guiset, "player2", draw_player2);
    guiset.endGroup();

    guiset.beginGroup("world");
    loadIcon(guiset, "item-browser", world_item_browser);
    loadIcon(guiset, "music-boxes", world_music_boxes);
    guiset.endGroup();

    guiset.beginGroup("level-items");
    loadImage(guiset, "player-point", player_point);
    loadImage(guiset, "player1", player1);
    loadImage(guiset, "player2", player2);
    guiset.endGroup();

    guiset.beginGroup("section-numbers");
    loadImage(guiset, "section-0-selected", section_0_selected);
    loadImage(guiset, "section-0-default", section_0_default);
    loadImage(guiset, "section-1-selected", section_1_selected);
    loadImage(guiset, "section-1-default", section_1_default);
    loadImage(guiset, "section-2-selected", section_2_selected);
    loadImage(guiset, "section-2-default", section_2_default);
    loadImage(guiset, "section-3-selected", section_3_selected);
    loadImage(guiset, "section-3-default", section_3_default);
    loadImage(guiset, "section-4-selected", section_4_selected);
    loadImage(guiset, "section-4-default", section_4_default);
    loadImage(guiset, "section-5-selected", section_5_selected);
    loadImage(guiset, "section-5-default", section_5_default);
    loadImage(guiset, "section-6-selected", section_6_selected);
    loadImage(guiset, "section-6-default", section_6_default);
    loadImage(guiset, "section-7-selected", section_7_selected);
    loadImage(guiset, "section-7-default", section_7_default);
    loadImage(guiset, "section-8-selected", section_8_selected);
    loadImage(guiset, "section-8-default", section_8_default);
    loadImage(guiset, "section-9-selected", section_9_selected);
    loadImage(guiset, "section-9-default", section_9_default);
    loadImage(guiset, "section-10-selected", section_10_selected);
    loadImage(guiset, "section-10-default", section_10_default);
    loadImage(guiset, "section-11-selected", section_11_selected);
    loadImage(guiset, "section-11-default", section_11_default);
    loadImage(guiset, "section-12-selected", section_12_selected);
    loadImage(guiset, "section-12-default", section_12_default);
    loadImage(guiset, "section-13-selected", section_13_selected);
    loadImage(guiset, "section-13-default", section_13_default);
    loadImage(guiset, "section-14-selected", section_14_selected);
    loadImage(guiset, "section-14-default", section_14_default);
    loadImage(guiset, "section-15-selected", section_15_selected);
    loadImage(guiset, "section-15-default", section_15_default);
    loadImage(guiset, "section-16-selected", section_16_selected);
    loadImage(guiset, "section-16-default", section_16_default);
    loadImage(guiset, "section-17-selected", section_17_selected);
    loadImage(guiset, "section-17-default", section_17_default);
    loadImage(guiset, "section-18-selected", section_18_selected);
    loadImage(guiset, "section-18-default", section_18_default);
    loadImage(guiset, "section-19-selected", section_19_selected);
    loadImage(guiset, "section-19-default", section_19_default);
    loadImage(guiset, "section-20-selected", section_20_selected);
    loadImage(guiset, "section-20-default", section_20_default);
    //loadImage(guiset, "section-21-selected", section_21_selected);
    //loadImage(guiset, "section-21-default", section_21_default);
    loadIcon(guiset, "section-reset-pos", section_goto_left_bottom);
    loadIcon(guiset, "section-goto-left-bottom", section_goto_left_bottom);
    loadIcon(guiset, "section-goto-left-top", section_goto_left_top);
    loadIcon(guiset, "section-goto-top-right", section_goto_top_right);
    loadIcon(guiset, "section-goto-right-bottom", section_goto_right_bottom);
    guiset.endGroup();

    guiset.beginGroup("locks-level");
    loadImage(guiset, "blocks-free", blocks_free);
    loadImage(guiset, "blocks-locked", blocks_locked);
    loadImage(guiset, "bgo-free", bgo_free);
    loadImage(guiset, "bgo-locked", bgo_locked);
    loadImage(guiset, "npc-free", npc_free);
    loadImage(guiset, "npc-locked", npc_locked);
    loadImage(guiset, "warps-free", warps_free);
    loadImage(guiset, "warps-locked", warps_locked);
    loadImage(guiset, "physenv-free", physenv_free);
    loadImage(guiset, "physenv-locked", physenv_locked);
    guiset.endGroup();

    guiset.beginGroup("locks-world");
    loadImage(guiset, "tiles-free", tiles_free);
    loadImage(guiset, "tiles-locked", tiles_locked);
    loadImage(guiset, "scenery-free", scenery_free);
    loadImage(guiset, "scenery-locked", scenery_locked);
    loadImage(guiset, "paths-free", paths_free);
    loadImage(guiset, "paths-locked", paths_locked);
    loadImage(guiset, "levels-free", levels_free);
    loadImage(guiset, "levels-locked", levels_locked);
    loadImage(guiset, "musicboxes-free", musicboxes_free);
    loadImage(guiset, "musicboxes-locked", musicboxes_locked);
    guiset.endGroup();

    guiset.beginGroup("items");
    loadImage(guiset, "level-background", dummy_bg);
    loadImage(guiset, "level-bgo", dummy_bgo);
    loadImage(guiset, "level-block", dummy_block);
    loadImage(guiset, "level-npc", dummy_npc);
    loadImage(guiset, "world-path", dummy_path);
    loadImage(guiset, "world-scenery", dummy_scenery);
    loadImage(guiset, "world-tile", dummy_terrain);
    loadImage(guiset, "world-level", dummy_wlevel);
    loadImage(guiset, "world-musicbox", dummy_musicbox);
    guiset.endGroup();

    currentThemeDir = themeDir;

    initCursors();
}

void Themes::loadStyleSheet(QSettings &s, QString value)
{
    QString val = s.value(value, "").toString();
    if(!val.isEmpty())
    {
        QFile ss(theme_dir + val);
        if(!ss.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QByteArray arr = ss.readAll();
        styleSheet = QString::fromUtf8(arr);
    }
}

void Themes::loadIcon(QSettings &s, QString value, Themes::Icons icn)
{
    QString val = s.value(value, "").toString();
    if(!val.isEmpty())
    {
        QIcon   tmpIcn = QIcon(theme_dir + val);
        if(!tmpIcn.isNull())
        {
            icons_map[icn] = tmpIcn;
            if(icons_map_dark.contains(icn))
                icons_map_dark.remove(icn);
        }
    }

    value += "-dark";

    val = s.value(value, "").toString();
    if(!val.isEmpty())
    {
        QPixmap tmpImg = QPixmap(theme_dir + val);
        if(!tmpImg.isNull())
            icons_map_dark[icn] = tmpImg;
    }
}

void Themes::loadImage(QSettings &s, QString value, Themes::Images img)
{
    QString val = s.value(value, "").toString();
    if(!val.isEmpty())
    {
        QPixmap tmpImg = QPixmap(theme_dir + val);
        if(!tmpImg.isNull())
        {
            images_map[img] = tmpImg;
            if(images_map_dark.contains(img))
                images_map_dark.remove(img);
        }
    }

    value += "-dark";

    val = s.value(value, "").toString();
    if(!val.isEmpty())
    {
        QPixmap tmpImg = QPixmap(theme_dir + val);
        if(!tmpImg.isNull())
            images_map_dark[img] = tmpImg;
    }
}

void Themes::loadInteger(QSettings &s, QString value, Themes::Images intVal)
{
    int tmpVal = s.value(value, -1).toInt();
    if(tmpVal >= 0)
        int_map[intVal] = tmpVal;
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
    if(!isLoaded)
        return QIcon();

    if(currentPaletteIsDark && icons_map_dark.contains(icn))
        return icons_map_dark[icn];
    else if(icons_map.contains(icn))
        return icons_map[icn];
    else
        return QIcon();
}

QPixmap Themes::Image(Themes::Images img)
{
    if(!isLoaded)
        return QPixmap();

    if(currentPaletteIsDark && images_map_dark.contains(img))
        return images_map_dark[img];
    else if(images_map.contains(img))
        return images_map[img];
    else
    {
        QPixmap x(32, 32);
        x.fill(Qt::gray);
        return x;
    }
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

const QString &Themes::StyleSheet()
{
    return styleSheet;
}


QStringList Themes::availablePalletes()
{
    QStringList ret;
    ret << QObject::tr("Default", "Name of pallete");
    ret << QObject::tr("Dark blue", "Name of pallete");
    return ret;
}

void Themes::togglePallete(Themes::Palettes pallete)
{
    static QPalette s_initialPallete = QApplication::palette();

    /*
     * NOTE: If you want to add a new pallete, please add it into the list above
     * (inside of availablePalletes() call) in the same order as Palletes enumeration
     * describes them.
     */

    switch(pallete)
    {
    default:
    case Palettes::Default:
    {
        currentPaletteD = s_initialPallete;
        currentPaletteIsDark = false;
        break;
    }

    case Palettes::DarkBlue:
    {
        QPalette darkPalette;

        darkPalette.setColor(QPalette::Window, QColor(24, 35, 50));
        darkPalette.setColor(QPalette::WindowText, QColor(150, 175, 200));
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(85, 100, 125));
        darkPalette.setColor(QPalette::Base, QColor(32, 37, 42));
        darkPalette.setColor(QPalette::AlternateBase, QColor(20, 40, 66));
        darkPalette.setColor(QPalette::ToolTipBase, QColor(150, 175, 200));
        darkPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
        darkPalette.setColor(QPalette::Text, QColor(150, 175, 200));
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(55, 85, 105));
        darkPalette.setColor(QPalette::Dark, QColor(15, 22, 35));
        darkPalette.setColor(QPalette::Shadow, QColor(10, 15, 20));
        darkPalette.setColor(QPalette::Button, QColor(24, 35, 50));
        darkPalette.setColor(QPalette::ButtonText, QColor(150, 175, 200));
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(85, 100, 125));
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(0, 100, 150));
        darkPalette.setColor(QPalette::Highlight, QColor(0, 100, 150));
        darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(40, 60, 80));
        darkPalette.setColor(QPalette::HighlightedText, QColor(150, 175, 200));
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(85, 100, 125));

        currentPaletteD = darkPalette;

        currentPaletteIsDark = true;
        break;
    }

    }

}

QPalette Themes::pallete()
{
    return currentPaletteD;
}

bool Themes::isPalleteDark()
{
    return currentPaletteIsDark;
}
