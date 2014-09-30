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

#include "ui_mainwindow.h"
#include "../mainwindow.h"

#include "global_settings.h"
#include "music_player.h"
#include "../common_features/graphicsworkspace.h"

namespace mainwindowMenuBar
{
    QMdiSubWindow *LastActiveSubWindow = NULL;
}

void MainWindow::updateMenus(bool force)
{
    using namespace mainwindowMenuBar;
    if(!force)
    {
        //Don't update if window is not active
        if(!this->isActiveWindow()) return;
         //Don't update if this is - same subWindow
        if(LastActiveSubWindow==ui->centralWidget->activeSubWindow()) return;
    }

    LastActiveSubWindow = ui->centralWidget->activeSubWindow();

    WriteToLog(QtDebugMsg, QString("Update menus"));

    int WinType = activeChildWindow(); // 1 lvledit, 2 npcedit, 3 wldedit
    bool hasSWindow = (WinType != 0);

    ui->PlacingToolbar->setVisible(false);
    ui->ResizingToolbar->setVisible(false);

    ui->actionSave->setEnabled(hasSWindow);
    ui->actionSave_as->setEnabled(hasSWindow);
    ui->actionSave_all->setEnabled(hasSWindow);
    ui->actionClose->setEnabled(hasSWindow);

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

    ui->LevelObjectToolbar->setVisible( (WinType==1) );
    ui->WorldObjectToolbar->setVisible( (WinType==3) );

    ui->ItemProperties->setVisible(false);
    ui->WLD_ItemProps->setVisible(false);



    if((!(WinType==1))&& (GlobalSettings::lastWinType == 1) )
    {
        GlobalSettings::LevelToolBoxVis = ui->LevelToolBox->isVisible();  //Save current visible status
        GlobalSettings::SectionToolBoxVis = ui->LevelSectionSettings->isVisible();

        GlobalSettings::LevelDoorsBoxVis = ui->DoorsToolbox->isVisible();
        GlobalSettings::LevelLayersBoxVis = ui->LevelLayers->isVisible();
        GlobalSettings::LevelEventsBoxVis = ui->LevelEventsToolBox->isVisible();

        GlobalSettings::LevelSearchBoxVis = ui->FindDock->isVisible();

        GlobalSettings::TilesetBoxVis = ui->Tileset_Item_Box->isVisible();
        GlobalSettings::DebuggerBoxVis = ui->debuggerBox->isVisible();

        ui->LevelToolBox->setVisible( 0 ); //Hide level toolbars
        ui->LevelSectionSettings->setVisible( 0 );
        ui->DoorsToolbox->setVisible( 0 );
        ui->LevelLayers->setVisible( 0 );
        ui->LevelEventsToolBox->setVisible( 0 );
        ui->FindDock->setVisible( 0 );
    }
    if((GlobalSettings::lastWinType !=1) && (WinType==1))
    {
        ui->LevelToolBox->setVisible( GlobalSettings::LevelToolBoxVis ); //Restore saved visible status
        ui->LevelSectionSettings->setVisible( GlobalSettings::SectionToolBoxVis );
        ui->DoorsToolbox->setVisible( GlobalSettings::LevelDoorsBoxVis );
        ui->LevelLayers->setVisible( GlobalSettings::LevelLayersBoxVis );
        ui->LevelEventsToolBox->setVisible( GlobalSettings::LevelEventsBoxVis );
        ui->FindDock->setVisible(GlobalSettings::LevelSearchBoxVis);

        ui->Tileset_Item_Box->setVisible(GlobalSettings::TilesetBoxVis);
        ui->debuggerBox->setVisible(GlobalSettings::DebuggerBoxVis);
    }

    if((!(WinType==3))&& (GlobalSettings::lastWinType == 3) )
    {
        GlobalSettings::WorldToolBoxVis = ui->WorldToolBox->isVisible(); //Save current visible status
        GlobalSettings::WorldSettingsToolboxVis = ui->WorldSettings->isVisible();
        GlobalSettings::WorldSearchBoxVis = ui->WorldFindDock->isVisible();
        GlobalSettings::TilesetBoxVis = ui->Tileset_Item_Box->isVisible();
        GlobalSettings::DebuggerBoxVis = ui->debuggerBox->isVisible();

        ui->WorldToolBox->setVisible( 0 );
        ui->WorldSettings->setVisible( 0 );
        ui->WorldFindDock->setVisible( 0 );
    }
    if((GlobalSettings::lastWinType !=3) && (WinType==3))
    {
        ui->WorldToolBox->setVisible( GlobalSettings::WorldToolBoxVis ); //Restore saved visible status
        ui->WorldSettings->setVisible( GlobalSettings::WorldSettingsToolboxVis );
        ui->WorldFindDock->setVisible( GlobalSettings::WorldSearchBoxVis );

        ui->Tileset_Item_Box->setVisible(GlobalSettings::TilesetBoxVis);
        ui->debuggerBox->setVisible(GlobalSettings::DebuggerBoxVis);
    }

    if( (!(WinType==1))&&(!(WinType==3)) && (GlobalSettings::lastWinType == 1 || GlobalSettings::lastWinType == 3) )
    {
        GlobalSettings::TilesetBoxVis = ui->Tileset_Item_Box->isVisible();
        GlobalSettings::DebuggerBoxVis = ui->debuggerBox->isVisible();
        ui->Tileset_Item_Box->setVisible( 0 );
        ui->debuggerBox->setVisible( 0 );
    }


    GlobalSettings::lastWinType =   WinType;


    ui->actionLVLToolBox->setVisible( (WinType==1) );
    ui->actionWarpsAndDoors->setVisible( (WinType==1) );
    ui->actionSection_Settings->setVisible( (WinType==1) );
    ui->actionLevelProp->setVisible( (WinType==1) );
    ui->actionLayersBox->setVisible( (WinType==1) );
    ui->actionLevelEvents->setVisible( (WinType==1) );
    ui->actionWarpsAndDoors->setVisible( (WinType==1) );
    ui->actionLVLSearchBox->setVisible( (WinType==1) );

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

    ui->actionGridEn->setEnabled( (WinType==1)|| (WinType==3) );

    if(WinType==1)
    {
        if( configs.check() )
        {
            WriteToLog(QtCriticalMsg, "*.INI Configs not loaded");
            return;
        }

        if(activeLvlEditWin()->sceneCreated)
        {
            if(activeLvlEditWin()->scene->pResizer==NULL)
            {
                ui->ResizeSection->setVisible(true);
                ui->applyResize->setVisible(false);
                ui->cancelResize->setVisible(false);
            }
            else
            {
                if(activeLvlEditWin()->scene->pResizer->type == 0)
                {
                    ui->ResizeSection->setVisible(false);
                    ui->applyResize->setVisible(true);
                    ui->cancelResize->setVisible(true);
                }
            }
        }

        zoom->setText(QString::number(activeLvlEditWin()->getZoom()));

        SetCurrentLevelSection(0, 1);

        setDoorsToolbox();
        setLayersBox();
        setEventsBox();

        //Sync lists in properties windows
        EventListsSync();
        setLayerLists();

        setLevelSectionData();

        if(LvlMusPlay::musicType!=LvlMusPlay::LevelMusic) LvlMusPlay::musicForceReset=true;
        LvlMusPlay::musicType=LvlMusPlay::LevelMusic;
        setMusic( ui->actionPlayMusic->isChecked() );
        ui->actionSelect->trigger();


        if(activeLvlEditWin()->sceneCreated)
        {
            ui->actionLockBlocks->setChecked(activeLvlEditWin()->scene->lock_block);
            ui->actionLockBGO->setChecked(activeLvlEditWin()->scene->lock_bgo);
            ui->actionLockNPC->setChecked(activeLvlEditWin()->scene->lock_npc);
            ui->actionLockWaters->setChecked(activeLvlEditWin()->scene->lock_water);
            ui->actionLockDoors->setChecked(activeLvlEditWin()->scene->lock_door);

            ui->actionGridEn->setChecked(activeLvlEditWin()->scene->grid);

            GlobalSettings::LvlOpts = activeLvlEditWin()->scene->opts;

            ui->actionUndo->setEnabled(activeLvlEditWin()->scene->canUndo());
            ui->actionRedo->setEnabled(activeLvlEditWin()->scene->canRedo());

            activeLvlEditWin()->scene->Debugger_updateItemList();
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

        setCurrentWorldSettings();

        WriteToLog(QtDebugMsg, "-> Music Player");

        if(LvlMusPlay::musicType!=LvlMusPlay::WorldMusic) LvlMusPlay::musicForceReset=true;
        LvlMusPlay::musicType=LvlMusPlay::WorldMusic;
        setMusic( ui->actionPlayMusic->isChecked() );
        ui->actionSelect->trigger();

        if(activeWldEditWin()->sceneCreated)
        {
            WriteToLog(QtDebugMsg, "-> Get scene flags: locks");
            ui->actionLockTiles->setChecked(activeWldEditWin()->scene->lock_tile);
            ui->actionLockScenes->setChecked(activeWldEditWin()->scene->lock_scene);
            ui->actionLockPaths->setChecked(activeWldEditWin()->scene->lock_path);
            ui->actionLockLevels->setChecked(activeWldEditWin()->scene->lock_level);
            ui->actionLockMusicBoxes->setChecked(activeWldEditWin()->scene->lock_musbox);

            WriteToLog(QtDebugMsg, "-> Get scene flags: grid");
            ui->actionGridEn->setChecked(activeWldEditWin()->scene->grid);

            WriteToLog(QtDebugMsg, "-> Get scene flags: animation and collision");
            GlobalSettings::LvlOpts = activeWldEditWin()->scene->opts;
            ui->actionUndo->setEnabled(activeWldEditWin()->scene->canUndo());
            ui->actionRedo->setEnabled(activeWldEditWin()->scene->canRedo());

            activeWldEditWin()->scene->Debugger_updateItemList();
        }

        zoom->setText(QString::number(activeWldEditWin()->getZoom()));

        ui->actionAnimation->setChecked( GlobalSettings::LvlOpts.animationEnabled );
        ui->actionCollisions->setChecked( GlobalSettings::LvlOpts.collisionsEnabled );
        ui->actionSemi_transparent_paths->setChecked( GlobalSettings::LvlOpts.semiTransparentPaths );

    }
    else
    {
        ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(false);
    }

    setTileSetBox();
    UpdateLvlCustomItems();
    updateWindowMenu();
}


//QList<QMenu * > menu_delete_list;
//QList<QAction * > action_delete_list;

void MainWindow::updateWindowMenu()
{
    //Window menu
    ui->menuWindow->clear();

//    while(!action_delete_list.isEmpty())
//    {
//        QAction *tmp = action_delete_list.first();
//        action_delete_list.pop_back();
//        if(tmp!=NULL) {
//            WriteToLog(QtDebugMsg, QString("->>>>Removed trash!<<<<-"));
//            WriteToLog(QtDebugMsg, QString(tmp->text()));
//            delete tmp;
//        }
//    }

    QAction * SubView = ui->menuWindow->addAction(tr("Sub Windows"));
    connect(SubView, SIGNAL(triggered()), this, SLOT(setSubView()));
    SubView->setCheckable(true);
    if(GlobalSettings::MainWindowView==QMdiArea::SubWindowView)
        SubView->setChecked(true);

    //action_delete_list.push_back(SubView);


    QAction * TabView = ui->menuWindow->addAction(tr("Tab Windows"));
    connect(TabView, SIGNAL(triggered()), this, SLOT(setTabView()));
    TabView->setCheckable(true);
    if(GlobalSettings::MainWindowView==QMdiArea::TabbedView)
        TabView->setChecked(true);

    //action_delete_list.push_back(TabView);

    ui->menuWindow->addSeparator();
    //action_delete_list.push_back(ui->menuWindow->addSeparator());

    QList<QMdiSubWindow *> windows = ui->centralWidget->subWindowList();
    QAction * closeC = ui->menuWindow->addAction(tr("Close current"));
        connect(closeC, SIGNAL(triggered()), this, SLOT( on_actionClose_triggered() ) );
        closeC->setEnabled( !windows.isEmpty() );

    //action_delete_list.push_back(closeC);


    ui->menuWindow->addSeparator();
    //action_delete_list.push_back(ui->menuWindow->addSeparator());

    QAction * cascade = ui->menuWindow->addAction(tr("Cascade"));
        connect(cascade, SIGNAL(triggered()), this, SLOT( SWCascade() ) );
        cascade->setEnabled( !windows.isEmpty() );
    //action_delete_list.push_back(cascade);

    QAction * tiledW = ui->menuWindow->addAction(tr("Tiled"));
        connect(tiledW, SIGNAL(triggered()), this, SLOT( SWTile() ) );
        tiledW->setEnabled( !windows.isEmpty() );
    //action_delete_list.push_back(tiledW);

    ui->menuWindow->addSeparator();
    //action_delete_list.push_back(ui->menuWindow->addSeparator());

    QAction * empty = ui->menuWindow->addAction( tr("[No files open]") );
        empty->setDisabled(1);

        empty->setVisible( windows.isEmpty() );
    //action_delete_list.push_back(empty);


    for (int i = 0; i < windows.size(); ++i) {
        QString text;
        text = QString("%1").arg( windows.at(i)->windowTitle() ) ;
        QAction *action  = ui->menuWindow->addAction(text);
        action->setCheckable(true);
        action->setChecked( windows[i] == ui->centralWidget->activeSubWindow() );
        //action_delete_list.push_back(action);

        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

void MainWindow::applyTextZoom(){
    bool ok = false;
    int zoomPercent = 100;
    zoomPercent = zoom->text().toInt(&ok);
    if(!ok)
        return;

    if(activeChildWindow()==1){
        activeLvlEditWin()->setZoom(zoomPercent);
    }else if(activeChildWindow()==3){
        activeWldEditWin()->setZoom(zoomPercent);
    }
}
