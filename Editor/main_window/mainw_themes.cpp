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


#include "../mainwindow.h"
#include "../ui_mainwindow.h"
#include "../common_features/themes.h"
#include "global_settings.h"


void MainWindow::applyTheme(QString themeDir)
{
    if(!themeDir.isEmpty())
        Themes::loadTheme(themeDir);

    ui->actionLVLToolBox->setIcon(Themes::icon(Themes::level_24));
    ui->actionWLDToolBox->setIcon(Themes::icon(Themes::world_24));

    ui->actionNew->setIcon(Themes::icon(Themes::file_new));
    ui->actionNewLevel->setIcon(Themes::icon(Themes::level_16));
    ui->LevelToolBox->setWindowIcon(Themes::icon(Themes::level_16));

    ui->actionNewWorld_map->setIcon(Themes::icon(Themes::world_16));
    ui->WorldToolBox->setWindowIcon(Themes::icon(Themes::level_16));

    ui->actionNewNPC_config->setIcon(Themes::icon(Themes::npc_16));

    ui->OpenFile->setIcon(Themes::icon(Themes::file_open));
    ui->actionSave->setIcon(Themes::icon(Themes::file_save));
    ui->actionSave_as->setIcon(Themes::icon(Themes::file_saveas));
    ui->actionReload->setIcon(Themes::icon(Themes::file_reload));

    ui->actionDebugger->setIcon(Themes::icon(Themes::debugger));
    ui->debuggerBox->setWindowIcon(Themes::icon(Themes::debugger));

    ui->actionPlayMusic->setIcon(Themes::icon(Themes::playmusic));
    ui->actionGridEn->setIcon(Themes::icon(Themes::grid_snap));
    ui->actionAnimation->setIcon(Themes::icon(Themes::animation));

    ui->actionWLD_SearchBox->setIcon(Themes::icon(Themes::search));
    ui->WorldFindDock->setWindowIcon(Themes::icon(Themes::search));
    ui->actionLVLSearchBox->setIcon(Themes::icon(Themes::search));
    ui->FindDock->setWindowIcon(Themes::icon(Themes::search));

    ui->actionCopy->setIcon(Themes::icon(Themes::copy));
    ui->LVLEvents_duplicate->setIcon(Themes::icon(Themes::copy_16));
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

    ui->actionSquareFill->setIcon(Themes::icon(Themes::square_fill));
    ui->actionLine->setIcon(Themes::icon(Themes::line_tool));
    ui->actionOverwriteMode->setIcon(Themes::icon(Themes::overwrite_mode));

    ui->actionZoomReset->setIcon(Themes::icon(Themes::zoom_reset));
    ui->actionZoomIn->setIcon(Themes::icon(Themes::zoom_in));
    ui->actionZoomOut->setIcon(Themes::icon(Themes::zoom_out));

    ui->actionSection_Settings->setIcon(Themes::icon(Themes::section_settings));
    ui->actionWorld_settings->setIcon(Themes::icon(Themes::section_settings));
    ui->LevelSectionSettings->setWindowIcon(Themes::icon(Themes::section_settings_16));

    ui->actionWarpsAndDoors->setIcon(Themes::icon(Themes::doors));
    ui->DoorsToolbox->setWindowIcon(Themes::icon(Themes::doors));

    ui->actionLayersBox->setIcon(Themes::icon(Themes::layers));
    ui->LevelLayers->setWindowIcon(Themes::icon(Themes::layers));

    ui->actionLevelEvents->setIcon(Themes::icon(Themes::events));
    ui->LevelEventsToolBox->setWindowIcon(Themes::icon(Themes::events));

    ui->actionDrawWater->setIcon(Themes::icon(Themes::water));
    ui->actionDrawSand->setIcon(Themes::icon(Themes::quicksand));

    ui->actionSetFirstPlayer->setIcon(Themes::icon(Themes::draw_player1));
    ui->actionSetSecondPlayer->setIcon(Themes::icon(Themes::draw_player2));

    QIcon section1;
    section1.addPixmap(Themes::Image(Themes::section_1_selected), QIcon::Normal, QIcon::On);
    section1.addPixmap(Themes::Image(Themes::section_1_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_1->setIcon(section1);

    QIcon section2;
    section2.addPixmap(Themes::Image(Themes::section_2_selected), QIcon::Normal, QIcon::On);
    section2.addPixmap(Themes::Image(Themes::section_2_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_2->setIcon(section2);

    QIcon section3;
    section3.addPixmap(Themes::Image(Themes::section_3_selected), QIcon::Normal, QIcon::On);
    section3.addPixmap(Themes::Image(Themes::section_3_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_3->setIcon(section3);

    QIcon section4;
    section4.addPixmap(Themes::Image(Themes::section_4_selected), QIcon::Normal, QIcon::On);
    section4.addPixmap(Themes::Image(Themes::section_4_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_4->setIcon(section4);

    QIcon section5;
    section5.addPixmap(Themes::Image(Themes::section_5_selected), QIcon::Normal, QIcon::On);
    section5.addPixmap(Themes::Image(Themes::section_5_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_5->setIcon(section5);

    QIcon section6;
    section6.addPixmap(Themes::Image(Themes::section_6_selected), QIcon::Normal, QIcon::On);
    section6.addPixmap(Themes::Image(Themes::section_6_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_6->setIcon(section6);

    QIcon section7;
    section7.addPixmap(Themes::Image(Themes::section_7_selected), QIcon::Normal, QIcon::On);
    section7.addPixmap(Themes::Image(Themes::section_7_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_7->setIcon(section7);

    QIcon section8;
    section8.addPixmap(Themes::Image(Themes::section_8_selected), QIcon::Normal, QIcon::On);
    section8.addPixmap(Themes::Image(Themes::section_8_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_8->setIcon(section8);

    QIcon section9;
    section9.addPixmap(Themes::Image(Themes::section_9_selected), QIcon::Normal, QIcon::On);
    section9.addPixmap(Themes::Image(Themes::section_9_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_9->setIcon(section9);

    QIcon section10;
    section10.addPixmap(Themes::Image(Themes::section_10_selected), QIcon::Normal, QIcon::On);
    section10.addPixmap(Themes::Image(Themes::section_10_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_10->setIcon(section10);

    QIcon section11;
    section11.addPixmap(Themes::Image(Themes::section_11_selected), QIcon::Normal, QIcon::On);
    section11.addPixmap(Themes::Image(Themes::section_11_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_11->setIcon(section11);

    QIcon section12;
    section12.addPixmap(Themes::Image(Themes::section_12_selected), QIcon::Normal, QIcon::On);
    section12.addPixmap(Themes::Image(Themes::section_12_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_12->setIcon(section12);

    QIcon section13;
    section13.addPixmap(Themes::Image(Themes::section_13_selected), QIcon::Normal, QIcon::On);
    section13.addPixmap(Themes::Image(Themes::section_13_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_13->setIcon(section13);

    QIcon section14;
    section14.addPixmap(Themes::Image(Themes::section_14_selected), QIcon::Normal, QIcon::On);
    section14.addPixmap(Themes::Image(Themes::section_14_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_14->setIcon(section14);

    QIcon section15;
    section15.addPixmap(Themes::Image(Themes::section_15_selected), QIcon::Normal, QIcon::On);
    section15.addPixmap(Themes::Image(Themes::section_15_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_15->setIcon(section15);

    QIcon section16;
    section16.addPixmap(Themes::Image(Themes::section_16_selected), QIcon::Normal, QIcon::On);
    section16.addPixmap(Themes::Image(Themes::section_16_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_16->setIcon(section16);

    QIcon section17;
    section17.addPixmap(Themes::Image(Themes::section_17_selected), QIcon::Normal, QIcon::On);
    section17.addPixmap(Themes::Image(Themes::section_17_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_17->setIcon(section17);

    QIcon section18;
    section18.addPixmap(Themes::Image(Themes::section_18_selected), QIcon::Normal, QIcon::On);
    section18.addPixmap(Themes::Image(Themes::section_18_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_18->setIcon(section18);

    QIcon section19;
    section19.addPixmap(Themes::Image(Themes::section_19_selected), QIcon::Normal, QIcon::On);
    section19.addPixmap(Themes::Image(Themes::section_19_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_19->setIcon(section19);

    QIcon section20;
    section20.addPixmap(Themes::Image(Themes::section_20_selected), QIcon::Normal, QIcon::On);
    section20.addPixmap(Themes::Image(Themes::section_20_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_20->setIcon(section20);

    QIcon section21;
    section21.addPixmap(Themes::Image(Themes::section_21_selected), QIcon::Normal, QIcon::On);
    section21.addPixmap(Themes::Image(Themes::section_21_default), QIcon::Normal, QIcon::Off);
    ui->actionSection_21->setIcon(section21);

    ui->actionReset_position->setIcon(Themes::icon(Themes::section_reset_pos));





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
