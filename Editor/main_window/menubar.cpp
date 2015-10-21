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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common_features/graphicsworkspace.h>
#include <audio/music_player.h>
#include <main_window/global_settings.h>
#include <main_window/dock/toolboxes.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <ui_lvl_sctc_props.h>

void MainWindow::updateMenus(bool force)
{
    if(!force)
    {
        //Don't update if window is not active
        if(this->isMinimized()) return;

        if(!this->isActiveWindow())
            qApp->setActiveWindow(this);
        this->update();
        //Don't update if this is - same subWindow
        if(LastActiveSubWindow==ui->centralWidget->activeSubWindow()) return;
    }
    else
    {
        if(ui->centralWidget->subWindowList().isEmpty())
            LastActiveSubWindow=NULL;
        qApp->setActiveWindow(this);
    }

    LastActiveSubWindow=ui->centralWidget->activeSubWindow();

    WriteToLog(QtDebugMsg, QString("Update menus"));

    LevelEdit *lvlWin = activeLvlEditWin(LastActiveSubWindow);
    //NpcEdit *npcWin = activeNpcEditWin(LastActiveSubWindow);
    WorldEdit *wldWin = activeWldEditWin(LastActiveSubWindow);


    int WinType = activeChildWindow(LastActiveSubWindow); // 1 lvledit, 2 npcedit, 3 wldedit
    bool hasSWindow = (WinType != 0);

    ui->PlacingToolbar->setVisible(false);
    ui->ResizingToolbar->setVisible(false);

    ui->actionSave->setEnabled(hasSWindow);
    ui->actionSave_as->setEnabled(hasSWindow);
    ui->actionSave_all->setEnabled(hasSWindow);
    ui->actionClose->setEnabled(hasSWindow);

    ui->action_openEpisodeFolder->setEnabled( WinType!=0 );
    ui->action_openCustomFolder->setEnabled( (WinType==1) || (WinType==3) );

    ui->menuView->setEnabled( (hasSWindow) && (WinType!=2) );

    ui->actionSelect->setEnabled( (WinType==1) || (WinType==3));
    ui->actionSelectOnly->setEnabled( (WinType==1) || (WinType==3));
    ui->actionEriser->setEnabled( (WinType==1) || (WinType==3));
    ui->actionHandScroll->setEnabled( (WinType==1) || (WinType==3));
    ui->actionReload->setEnabled( (WinType==1) || (WinType==2) || (WinType==3));

    ui->menuWorld->setEnabled(( WinType==3) );
    ui->actionWLDToolBox->setVisible( (WinType==3));

    ui->actionCopy->setEnabled( (WinType==1) || (WinType==3) );
    ui->actionPaste->setEnabled( (WinType==1) || (WinType==3) );
    ui->actionCut->setEnabled( (WinType==1) || (WinType==3) );

    ui->menuTest->setEnabled( (WinType==1)||(WinType==3) );
    ui->action_doTest->setEnabled( (WinType==1) );
    ui->action_doSafeTest->setEnabled( (WinType==1) || (WinType==3) );
    #ifdef Q_OS_WIN
    ui->actionRunTestSMBX->setEnabled( (WinType==1) );
    #endif

    ui->LevelObjectToolbar->setVisible( (WinType==1) );
    ui->WorldObjectToolbar->setVisible( (WinType==3) );

    dock_LvlItemProps->setVisible(false);
    dock_WldItemProps->setVisible(false);

    //Change visibility of toolboxes
    /***************Level specific toolboxes****************/
    if((!(WinType==1))&& (GlobalSettings::lastWinType == 1) )   { docks_level.hideAll(); }
    if((GlobalSettings::lastWinType !=1) && (WinType==1))       { docks_level.showAll(); }

    /***************World specific toolboxes****************/
    if((!(WinType==3))&& (GlobalSettings::lastWinType == 3) )   { docks_world.hideAll(); }
    if((GlobalSettings::lastWinType !=3) && (WinType==3))       { docks_world.showAll(); }

    /***************World and Level specific toolboxes****************/
    if( (!(WinType==1))&&(!(WinType==3)) && (GlobalSettings::lastWinType == 1 || GlobalSettings::lastWinType == 3) )
    { docks_level_and_world.hideAll(); }
    if( (!(GlobalSettings::lastWinType==1))&&(!(GlobalSettings::lastWinType==3)) && (WinType == 1 || WinType == 3) )
    { docks_level_and_world.showAll(); }

    GlobalSettings::lastWinType =   WinType;

    ui->actionLVLToolBox->setVisible( (WinType==1) );
    ui->actionWarpsAndDoors->setVisible( (WinType==1) );
    ui->actionSection_Settings->setVisible( (WinType==1) );
    ui->actionLevelProp->setVisible( (WinType==1) );
    ui->actionLayersBox->setVisible( (WinType==1) );
    ui->actionLevelEvents->setVisible( (WinType==1) );
    ui->actionWarpsAndDoors->setVisible( (WinType==1) );
    ui->actionLVLSearchBox->setVisible( (WinType==1) );

    ui->actionTilesetBox->setVisible( (WinType==1) || (WinType==3));
    ui->actionBookmarkBox->setVisible( (WinType==1) || (WinType==3));
    ui->actionDebugger->setVisible( (WinType==1) || (WinType==3));

    ui->actionWLDToolBox->setVisible( (WinType==3) );
    ui->actionWorld_settings->setVisible( (WinType==3) );
    ui->actionWLD_SearchBox->setVisible( (WinType==3) );

    ui->actionSemi_transparent_paths->setVisible( (WinType==3) );

    ui->menuLevel->setEnabled( (WinType==1) );



    ui->actionLevNoBack->setEnabled( (WinType==1) );
    ui->actionLevOffScr->setEnabled( (WinType==1) );
    ui->actionLevWarp->setEnabled( (WinType==1) );
    ui->actionLevUnderW->setEnabled( (WinType==1) );

    ui->actionLevelProp->setEnabled( (WinType==1) );

    ui->actionExport_to_image->setEnabled( (WinType==1) || (WinType==3) );
    ui->actionExport_to_image_section->setVisible( (WinType==1) );

    ui->actionZoomIn->setEnabled( (WinType==1) || (WinType==3) );
    ui->actionZoomOut->setEnabled( (WinType==1) || (WinType==3) );
    ui->actionZoomReset->setEnabled( (WinType==1) || (WinType==3) );
    zoom->setEnabled( (WinType==1) || (WinType==3));

    ui->actionReset_position->setEnabled( (WinType==1) || (WinType==3) );
    ui->actionGo_to_Section->setEnabled( (WinType==1) );

    ui->actionSection_1->setEnabled( (WinType==1) );
    ui->actionSection_2->setEnabled( (WinType==1) );
    ui->actionSection_3->setEnabled( (WinType==1) );
    ui->actionSection_4->setEnabled( (WinType==1) );
    ui->actionSection_5->setEnabled( (WinType==1) );
    ui->actionSection_6->setEnabled( (WinType==1) );
    ui->actionSection_7->setEnabled( (WinType==1) );
    ui->actionSection_8->setEnabled( (WinType==1) );
    ui->actionSection_9->setEnabled( (WinType==1) );
    ui->actionSection_10->setEnabled( (WinType==1) );
    ui->actionSection_11->setEnabled( (WinType==1) );
    ui->actionSection_12->setEnabled( (WinType==1) );
    ui->actionSection_13->setEnabled( (WinType==1) );
    ui->actionSection_14->setEnabled( (WinType==1) );
    ui->actionSection_15->setEnabled( (WinType==1) );
    ui->actionSection_16->setEnabled( (WinType==1) );
    ui->actionSection_17->setEnabled( (WinType==1) );
    ui->actionSection_18->setEnabled( (WinType==1) );
    ui->actionSection_19->setEnabled( (WinType==1) );
    ui->actionSection_20->setEnabled( (WinType==1) );
    ui->actionSection_21->setEnabled( (WinType==1) );
    ui->actionSectionMore->setEnabled( (WinType==1) );

    ui->actionGridEn->setEnabled( (WinType==1)|| (WinType==3) );

    ui->actionFixWrongMasks->setEnabled( (WinType==1)|| (WinType==3) );
    ui->actionCDATA_clear_unused->setEnabled( (WinType==1)|| (WinType==3) );
    ui->actionCDATA_Import->setEnabled( (WinType==1)|| (WinType==3) );

    ui->actionAlign_selected->setEnabled(  (WinType==1)|| (WinType==3)  );
    ui->actionFlipHorizontal->setEnabled(  (WinType==1)|| (WinType==3)  );
    ui->actionFlipVertical->setEnabled(  (WinType==1)|| (WinType==3)  );

    ui->actionRotateLeft->setEnabled(  (WinType==1)|| (WinType==3)  );
    ui->actionRotateRight->setEnabled(  (WinType==1)|| (WinType==3)  );

    ui->actionCloneSectionTo->setEnabled( (WinType==1) );
    ui->actionSCT_Delete->setEnabled( (WinType==1) );
    ui->actionSCT_FlipHorizontal->setEnabled( (WinType==1) );
    ui->actionSCT_FlipVertical->setEnabled( (WinType==1) );
    ui->actionSCT_RotateLeft->setEnabled( (WinType==1) );
    ui->actionSCT_RotateRight->setEnabled( (WinType==1) );

    ui->actionAdditional_Settings->setEnabled( (WinType==1) );

    ui->menuScript->setEnabled( WinType == 1 );
    ui->actionCompile_To->setEnabled( false );
    ui->menuSwitch_Compiler->setEnabled( false );

    if(WinType==1)
    {
        if(lvlWin->LvlData.metaData.script)
        {
            Script::CompilerType ct = lvlWin->LvlData.metaData.script->usingCompilerType();
            if(ct == Script::COMPILER_LUNALUA){
                ui->actionCompile_To->setText(tr("Compile To: LunaLua"));
                ui->actionCompile_To->setEnabled( true );
                ui->menuSwitch_Compiler->setEnabled( true );
            }else if(ct == Script::COMPILER_AUTOCODE){
                ui->actionCompile_To->setText(tr("Compile To: Autocode [Lunadll Original Language]"));
                ui->actionCompile_To->setEnabled( true );
                ui->menuSwitch_Compiler->setEnabled( true );
            }else{
                ui->actionCompile_To->setText(tr("Compile To:"));
                ui->actionCompile_To->setEnabled( false );
                ui->menuSwitch_Compiler->setEnabled( true );
            }
            ui->actionAutocode_Lunadll_Original_Language->setChecked(ct==Script::COMPILER_AUTOCODE);
            ui->actionLunaLua->setChecked(ct==Script::COMPILER_LUNALUA);
        }
        else
        {
            ui->actionCompile_To->setText(tr("Compile To:"));
            ui->actionCompile_To->setEnabled( false );
        }

        if( configs.check() )
        {
            WriteToLog(QtCriticalMsg, "*.INI Configs not loaded");
            return;
        }

        if(lvlWin->sceneCreated)
        {
            dock_LvlSectionProps->switchResizeMode(
                 (lvlWin->scene->pResizer!=NULL)&&(lvlWin->scene->pResizer->type==0)
                        );
        }

        zoom->setText(QString::number(lvlWin->getZoom()));

        SetCurrentLevelSection(0, 1);

        dock_LvlWarpProps->init();
        dock_LvlLayers->setLayersBox();
        dock_LvlEvents->setEventsBox();

        //Sync lists in properties windows
        EventListsSync();
        LayerListsSync();
        dock_BookmarksBox->updateBookmarkBoxByData();

        dock_LvlSectionProps->setLevelSectionData();
        ui->actionSelect->trigger();

        if(lvlWin->sceneCreated)
        {
            LvlScene * scn = lvlWin->scene;
            ui->actionLockBlocks->setChecked(scn->lock_block);
            ui->actionLockBGO->setChecked(scn->lock_bgo);
            ui->actionLockNPC->setChecked(scn->lock_npc);
            ui->actionLockWaters->setChecked(scn->lock_water);
            ui->actionLockDoors->setChecked(scn->lock_door);

            ui->actionGridEn->setChecked(scn->grid);

            GlobalSettings::LvlOpts = scn->opts;

            ui->actionUndo->setEnabled(scn->canUndo());
            ui->actionRedo->setEnabled(scn->canRedo());

            scn->Debugger_updateItemList();
        }
        ui->actionAnimation->setChecked( GlobalSettings::LvlOpts.animationEnabled );
        ui->actionCollisions->setChecked( GlobalSettings::LvlOpts.collisionsEnabled );
    }
    else
    if(WinType==3)
    {
        if( configs.check() )
        {
            WriteToLog(QtCriticalMsg, "*.INI Configs not loaded");
            return;
        }

        WriteToLog(QtDebugMsg, "-> Current world settings");

        dock_WldSettingsBox->setCurrentWorldSettings();
        dock_BookmarksBox->updateBookmarkBoxByData();

        ui->actionSelect->trigger();

        if(wldWin->sceneCreated)
        {
            WldScene *scn = wldWin->scene;
            WriteToLog(QtDebugMsg, "-> Get scene flags: locks");
            ui->actionLockTiles->setChecked(scn->lock_tile);
            ui->actionLockScenes->setChecked(scn->lock_scene);
            ui->actionLockPaths->setChecked(scn->lock_path);
            ui->actionLockLevels->setChecked(scn->lock_level);
            ui->actionLockMusicBoxes->setChecked(scn->lock_musbox);

            WriteToLog(QtDebugMsg, "-> Get scene flags: grid");
            ui->actionGridEn->setChecked(scn->grid);

            WriteToLog(QtDebugMsg, "-> Get scene flags: animation and collision");
            GlobalSettings::LvlOpts = scn->opts;
            ui->actionUndo->setEnabled(scn->canUndo());
            ui->actionRedo->setEnabled(scn->canRedo());

            scn->Debugger_updateItemList();
        }

        zoom->setText(QString::number(wldWin->getZoom()));

        ui->actionAnimation->setChecked( GlobalSettings::LvlOpts.animationEnabled );
        ui->actionCollisions->setChecked( GlobalSettings::LvlOpts.collisionsEnabled );
        ui->actionSemi_transparent_paths->setChecked( GlobalSettings::LvlOpts.semiTransparentPaths );

    }
    else
    {
        ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(false);
    }

    WriteToLog(QtDebugMsg, "-> Music Player");
    LvlMusPlay::updateMusic();

    setTileSetBox();
    UpdateCustomItems();
    updateWindowMenu();

    //Update debugger's custom counters
    dock_DebuggerBox->on_DEBUG_RefreshCoutners_clicked();

    qApp->setActiveWindow(this);
}


void MainWindow::updateWindowMenu()
{
    //Window menu
    ui->menuWindow->clear();

    QAction * SubView = ui->menuWindow->addAction(tr("Sub Windows"));
    connect(SubView, SIGNAL(triggered()), this, SLOT(setSubView()));
    SubView->setCheckable(true);
    if(GlobalSettings::MainWindowView==QMdiArea::SubWindowView)
        SubView->setChecked(true);

    QAction * TabView = ui->menuWindow->addAction(tr("Tab Windows"));
    connect(TabView, SIGNAL(triggered()), this, SLOT(setTabView()));
    TabView->setCheckable(true);
    if(GlobalSettings::MainWindowView==QMdiArea::TabbedView)
        TabView->setChecked(true);

    ui->menuWindow->addSeparator();

    QList<QMdiSubWindow *> windows = ui->centralWidget->subWindowList();
    QAction * closeC = ui->menuWindow->addAction(tr("Close current"));
        connect(closeC, SIGNAL(triggered()), this, SLOT( on_actionClose_triggered() ) );
        closeC->setEnabled( !windows.isEmpty() );

    ui->menuWindow->addSeparator();

    QAction * cascade = ui->menuWindow->addAction(tr("Cascade"));
        connect(cascade, SIGNAL(triggered()), this, SLOT( SWCascade() ) );
        cascade->setEnabled( !windows.isEmpty() );

    QAction * tiledW = ui->menuWindow->addAction(tr("Tiled"));
        connect(tiledW, SIGNAL(triggered()), this, SLOT( SWTile() ) );
        tiledW->setEnabled( !windows.isEmpty() );

    ui->menuWindow->addSeparator();

    QAction * empty = ui->menuWindow->addAction( tr("[No files open]") );
        empty->setDisabled(1);

        empty->setVisible( windows.isEmpty() );

    for (int i = 0; i < windows.size(); ++i) {
        QString text;
        text = QString("%1").arg( windows.at(i)->windowTitle() ) ;
        QAction *action  = ui->menuWindow->addAction(text);
        action->setCheckable(true);
        action->setChecked( windows[i] == ui->centralWidget->activeSubWindow() );

        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}


void MainWindow::UpdateCustomItems()
{
    dock_LvlItemBox->setLvlItemBoxes(true, true);
    dock_WldItemBox->setWldItemBoxes(true, true);
}
