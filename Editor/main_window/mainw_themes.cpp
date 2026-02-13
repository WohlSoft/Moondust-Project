/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QPushButton>
#include <QToolButton>
#include <QStyleFactory>

#include <common_features/themes.h>
#include <main_window/global_settings.h>
#include <main_window/dock/toolboxes.h>

#include <mainwindow.h>
#include <ui_mainwindow.h>

static void setToolButtonMenuIcon(QToolBar *bar, QMenu *menu, const QIcon &icon)
{
    QList<QToolButton *> list = bar->findChildren<QToolButton*>();
    for(QToolButton *w : list)
    {
        if(w->menu() == menu)
        {
            w->setIcon(icon);
            break;
        }
    }
}

void MainWindow::applyCurrentTheme()
{
    QString theme;
    if(GlobalSettings::currentTheme.isEmpty())
        theme = ConfStatus::defaultTheme;
    else
        theme = GlobalSettings::currentTheme;

    Themes::togglePallete(static_cast<Themes::Palettes>(GlobalSettings::currentPallete));

    LogDebug(QString("Applying pallete..."));
    if(GlobalSettings::currentPallete != 0)
        qApp->setStyle(QStyleFactory::create("Fusion"));
    else
        qApp->setStyle("default");

    if(Themes::isPalleteDark())
        ui->centralWidget->setBackground(QBrush(QColor(QRgb(0x222222))));
    else
        ui->centralWidget->setBackground(QBrush(Qt::darkGray));

    qApp->setPalette(Themes::pallete());

    LogDebug(QString("Loading theme [%1]...").arg(theme));
    applyTheme(theme);
}

void MainWindow::applyTheme(QString themeDir)
{
    if(!themeDir.isEmpty())
        Themes::loadTheme(themeDir);

    this->setStyleSheet( Themes::StyleSheet() );

    ui->actionLVLToolBox->setIcon(Themes::icon(Themes::level_item_browser));
    ui->actionWLDToolBox->setIcon(Themes::icon(Themes::world_item_browser));
    ui->actionMusicBoxes->setIcon(Themes::icon(Themes::world_music_boxes));

    ui->menuNew->setIcon(Themes::icon(Themes::file_new));
    setToolButtonMenuIcon(ui->fileIoTooBar, ui->menuNew, Themes::icon(Themes::file_new));
    ui->actionNewLevel->setIcon(Themes::icon(Themes::level_16));
    dock_LvlItemBox->setWindowIcon(Themes::icon(Themes::level_item_browser));

    ui->actionNewWorld_map->setIcon(Themes::icon(Themes::world_16));
    dock_WldItemBox->setWindowIcon(Themes::icon(Themes::world_item_browser));

    ui->actionNewNPC_config->setIcon(Themes::icon(Themes::npc_16));

    ui->OpenFile->setIcon(Themes::icon(Themes::file_open));
    ui->actionSave->setIcon(Themes::icon(Themes::file_save));
    ui->actionSave_as->setIcon(Themes::icon(Themes::file_saveas));
    ui->actionReload->setIcon(Themes::icon(Themes::file_reload));

    ui->actionDebugger->setIcon(Themes::icon(Themes::debugger));
    dock_DebuggerBox->setWindowIcon(Themes::icon(Themes::debugger));

    ui->actionTilesetBox->setIcon(Themes::icon(Themes::tileset_box));
    dock_TilesetBox->setWindowIcon(Themes::icon(Themes::tileset_box));

    ui->actionBookmarkBox->setIcon(Themes::icon(Themes::bookmarks));
    dock_BookmarksBox->setWindowIcon(Themes::icon(Themes::bookmarks));

    ui->actionPlayMusic->setIcon(Themes::icon(Themes::playmusic));
    ui->actionGridEn->setIcon(Themes::icon(Themes::grid_snap));
    ui->actionShowGrid->setIcon(Themes::icon(Themes::show_grid));
    ui->actionShowCameraGrid->setIcon(Themes::icon(Themes::camera_grid));
    ui->actionAnimation->setIcon(Themes::icon(Themes::animation));

    ui->actionWLD_SearchBox->setIcon(Themes::icon(Themes::search));
    dock_WldSearchBox->setWindowIcon(Themes::icon(Themes::search));
    ui->actionLVLSearchBox->setIcon(Themes::icon(Themes::search));
    dock_LvlSearchBox->setWindowIcon(Themes::icon(Themes::search));

    ui->actionCopy->setIcon(Themes::icon(Themes::copy));
    dock_LvlEvents->button_event_dupe()->setIcon(Themes::icon(Themes::copy_16));
    ui->actionCut->setIcon(Themes::icon(Themes::cut));
    ui->actionPaste->setIcon(Themes::icon(Themes::paste));

    ui->actionUndo->setIcon(Themes::icon(Themes::undo));
    ui->actionRedo->setIcon(Themes::icon(Themes::redo));

    ui->actionSelect->setIcon(Themes::icon(Themes::selection));
    ui->actionSelectOnly->setIcon(Themes::icon(Themes::selection_only));
    ui->actionHandScroll->setIcon(Themes::icon(Themes::hand_drag));
    ui->actionEriser->setIcon(Themes::icon(Themes::erasing));

    ui->actionResizeApply->setIcon(Themes::icon(Themes::accept));
    ui->actionResizeCancel->setIcon(Themes::icon(Themes::cancel));

    ui->actionRectFill->setIcon(Themes::icon(Themes::rect_fill));
    ui->actionCircleFill->setIcon(Themes::icon(Themes::circle_fill));
    ui->actionLine->setIcon(Themes::icon(Themes::line_tool));
    ui->actionFill->setIcon(Themes::icon(Themes::flood_fill));
    ui->actionOverwriteMode->setIcon(Themes::icon(Themes::overwrite_mode));

    ui->action_Placing_ShowProperties->setIcon(Themes::icon(Themes::properties));

    ui->actionZoomReset->setIcon(Themes::icon(Themes::zoom_reset));
    ui->actionZoomIn->setIcon(Themes::icon(Themes::zoom_in));
    ui->actionZoomOut->setIcon(Themes::icon(Themes::zoom_out));

    ui->actionSection_Settings->setIcon(Themes::icon(Themes::section_settings));
    ui->actionWorld_settings->setIcon(Themes::icon(Themes::section_settings));
    dock_LvlSectionProps->setWindowIcon(Themes::icon(Themes::section_settings_16));
    dock_LvlSectionProps->getEditBackground2IniButton()->setIcon(Themes::icon(Themes::pencil));

    ui->actionWarpsAndDoors->setIcon(Themes::icon(Themes::doors));
    dock_LvlWarpProps->setWindowIcon(Themes::icon(Themes::doors));

    ui->actionLayersBox->setIcon(Themes::icon(Themes::layers));
    dock_LvlLayers->setWindowIcon(Themes::icon(Themes::layers));

    ui->actionLevelEvents->setIcon(Themes::icon(Themes::events));
    dock_LvlEvents->setWindowIcon(Themes::icon(Themes::events));

    ui->actionDrawWater->setIcon(Themes::icon(Themes::water));
    ui->actionDrawSand->setIcon(Themes::icon(Themes::quicksand));

    ui->actionSetFirstPlayer->setIcon(Themes::icon(Themes::draw_player1));
    ui->actionSetSecondPlayer->setIcon(Themes::icon(Themes::draw_player2));

    applyThemeSections();

    ui->actionGotoLeftBottom->setIcon(Themes::icon(Themes::section_goto_left_bottom));
    ui->actionGotoLeftTop->setIcon(Themes::icon(Themes::section_goto_left_top));
    ui->actionGotoTopRight->setIcon(Themes::icon(Themes::section_goto_top_right));
    ui->actionGotoRightBottom->setIcon(Themes::icon(Themes::section_goto_right_bottom));



    QIcon lock_blocks;
    lock_blocks.addPixmap(Themes::Image(Themes::blocks_locked), QIcon::Normal, QIcon::On);
    lock_blocks.addPixmap(Themes::Image(Themes::blocks_free), QIcon::Normal, QIcon::Off);
    ui->actionLockBlocks->setIcon(lock_blocks);

    QIcon lock_bgo;
    lock_bgo.addPixmap(Themes::Image(Themes::bgo_locked), QIcon::Normal, QIcon::On);
    lock_bgo.addPixmap(Themes::Image(Themes::bgo_free), QIcon::Normal, QIcon::Off);
    ui->actionLockBGO->setIcon(lock_bgo);

    QIcon lock_npc;
    lock_npc.addPixmap(Themes::Image(Themes::npc_locked), QIcon::Normal, QIcon::On);
    lock_npc.addPixmap(Themes::Image(Themes::npc_free), QIcon::Normal, QIcon::Off);
    ui->actionLockNPC->setIcon(lock_npc);

    QIcon lock_warps;
    lock_warps.addPixmap(Themes::Image(Themes::warps_locked), QIcon::Normal, QIcon::On);
    lock_warps.addPixmap(Themes::Image(Themes::warps_free), QIcon::Normal, QIcon::Off);
    ui->actionLockDoors->setIcon(lock_warps);

    QIcon lock_physenv;
    lock_physenv.addPixmap(Themes::Image(Themes::physenv_locked), QIcon::Normal, QIcon::On);
    lock_physenv.addPixmap(Themes::Image(Themes::physenv_free), QIcon::Normal, QIcon::Off);
    ui->actionLockWaters->setIcon(lock_physenv);

    QIcon lock_tiles;
    lock_tiles.addPixmap(Themes::Image(Themes::tiles_locked), QIcon::Normal, QIcon::On);
    lock_tiles.addPixmap(Themes::Image(Themes::tiles_free), QIcon::Normal, QIcon::Off);
    ui->actionLockTiles->setIcon(lock_tiles);

    QIcon lock_scenery;
    lock_scenery.addPixmap(Themes::Image(Themes::scenery_locked), QIcon::Normal, QIcon::On);
    lock_scenery.addPixmap(Themes::Image(Themes::scenery_free), QIcon::Normal, QIcon::Off);
    ui->actionLockScenes->setIcon(lock_scenery);

    QIcon lock_paths;
    lock_paths.addPixmap(Themes::Image(Themes::paths_locked), QIcon::Normal, QIcon::On);
    lock_paths.addPixmap(Themes::Image(Themes::paths_free), QIcon::Normal, QIcon::Off);
    ui->actionLockPaths->setIcon(lock_paths);

    QIcon lock_levels;
    lock_levels.addPixmap(Themes::Image(Themes::levels_locked), QIcon::Normal, QIcon::On);
    lock_levels.addPixmap(Themes::Image(Themes::levels_free), QIcon::Normal, QIcon::Off);
    ui->actionLockLevels->setIcon(lock_levels);

    QIcon lock_musicboxes;
    lock_musicboxes.addPixmap(Themes::Image(Themes::musicboxes_locked), QIcon::Normal, QIcon::On);
    lock_musicboxes.addPixmap(Themes::Image(Themes::musicboxes_free), QIcon::Normal, QIcon::Off);
    ui->actionLockMusicBoxes->setIcon(lock_musicboxes);

}

void MainWindow::applyThemeSections()
{
    for(size_t i = 0; i < m_sectionButtonsCount; ++i)
    {
        QIcon section;
        section.addPixmap(Themes::Image((Themes::Images)m_sectionButtonsOnIcon[i]), QIcon::Normal, QIcon::On);
        section.addPixmap(Themes::Image((Themes::Images)m_sectionButtonsOffIcon[i]), QIcon::Normal, QIcon::Off);
        m_sectionButtons[i]->setIcon(section);
    }

    LevelEdit *lvlWin = activeLvlEditWin(LastActiveSubWindow);

    if(lvlWin && lvlWin->sceneCreated && lvlWin->scene)
    {
        LvlScene *scn = lvlWin->scene;

        // Check for unused sections
        for(size_t i = 0; i < m_sectionButtonsCount; ++i)
        {
            LevelSection *s = nullptr;
            if(i < (size_t)scn->m_data->sections.size())
                s = &scn->m_data->sections[i];

            if(!s || (s->size_left == 0 && s->size_top == 0 && s->size_right == 0 && s->size_bottom == 0))
                applyThemeSectionUnused(i);
        }
    }
}

void MainWindow::applyThemeSectionResetIcon(size_t id)
{
    if(id >= m_sectionButtonsCount)
        return;

    QIcon section;
    section.addPixmap(Themes::Image((Themes::Images)m_sectionButtonsOnIcon[id]), QIcon::Normal, QIcon::On);
    section.addPixmap(Themes::Image((Themes::Images)m_sectionButtonsOffIcon[id]), QIcon::Normal, QIcon::Off);
    m_sectionButtons[id]->setIcon(section);
}

void MainWindow::applyThemeSectionUnused(size_t id)
{
    if(id >= m_sectionButtonsCount)
        return;

    QIcon section;
    section.addPixmap(Themes::Image((Themes::Images)m_sectionButtonsOnIcon[id]), QIcon::Normal, QIcon::On);
    section.addPixmap(Themes::ImageCombiRB((Themes::Images)m_sectionButtonsOffIcon[id], Themes::section_init_new_sign, QSize(24, 24)), QIcon::Normal, QIcon::Off);
    m_sectionButtons[id]->setIcon(section);
}
