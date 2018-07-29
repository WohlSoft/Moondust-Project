/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QMdiSubWindow>

#include <common_features/graphicsworkspace.h>
#include <audio/music_player.h>
#include <main_window/global_settings.h>
#include <main_window/dock/toolboxes.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <ui_lvl_sctc_props.h>

void MainWindow::updateMenus(QMdiSubWindow* subWindow, bool force)
{
    if(!force)
    {
        //Don't update if window is not active
        if( this->isMinimized() )
            return;
        //if(!this->isActiveWindow())
        //    qApp->setActiveWindow(this);
        //this->update();
        //Don't update if this is - same subWindow
        if( subWindow==nullptr )
        {
            if( !ui->centralWidget->subWindowList().isEmpty() )
                return;
        }
        else
        {
            if(LastActiveSubWindow == subWindow)
                return;
        }
    }
    else
    {
        if(ui->centralWidget->subWindowList().isEmpty())
            LastActiveSubWindow=nullptr;
        qApp->setActiveWindow(this);
    }

    LastActiveSubWindow = subWindow;

    LogDebug(QString("Update menus"));

    LevelEdit *lvlWin = activeLvlEditWin(LastActiveSubWindow);
    //NpcEdit *npcWin = activeNpcEditWin(LastActiveSubWindow);
    WorldEdit *wldWin = activeWldEditWin(LastActiveSubWindow);


    int WinType = activeChildWindow(LastActiveSubWindow); // 1 lvledit, 2 npcedit, 3 wldedit
    bool hasSWindow = (WinType != WND_NoWindow);

    emit windowActiveLevel( WinType == WND_Level );
    emit windowActiveLevelWorld( (WinType == WND_Level) || (WinType==WND_World) );
    emit windowActiveWorld( WinType == WND_World );
    emit windowActiveNPC( WinType == WND_NpcTxt );

    ui->PlacingToolbar->setVisible(false);
    ui->ResizingToolbar->setVisible(false);

    ui->actionSave->setEnabled(hasSWindow);
    ui->actionSave_as->setEnabled(hasSWindow);
    ui->actionSave_all->setEnabled(hasSWindow);
    ui->actionClose->setEnabled(hasSWindow);

    ui->action_openEpisodeFolder->setEnabled( WinType != 0 );
    ui->action_openCustomFolder->setEnabled( (WinType==WND_Level) || (WinType==WND_World) );

    ui->actionReload->setEnabled( (WinType==WND_Level) || (WinType == WND_NpcTxt) || (WinType == WND_World));

    dock_LvlItemProps->setVisible(false);
    dock_WldItemProps->setVisible(false);

    //Change visibility of toolboxes
    /***************Level specific toolboxes****************/
    if((!(WinType==WND_Level))&& (GlobalSettings::lastWinType == WND_Level) )   { docks_level.hideAll(); }
    if((GlobalSettings::lastWinType !=1) && (WinType==WND_Level))       { docks_level.showAll(); }

    /***************World specific toolboxes****************/
    if((!(WinType==WND_World))&& (GlobalSettings::lastWinType == WND_World) )   { docks_world.hideAll(); }
    if((GlobalSettings::lastWinType !=3) && (WinType==WND_World))       { docks_world.showAll(); }

    /***************World and Level specific toolboxes****************/
    if( (!(WinType==WND_Level))&&(!(WinType==WND_World)) && (GlobalSettings::lastWinType == WND_Level || GlobalSettings::lastWinType == WND_World) )
    { docks_level_and_world.hideAll(); }
    if( (!(GlobalSettings::lastWinType==WND_Level))&&(!(GlobalSettings::lastWinType==WND_World)) && (WinType == WND_Level || WinType == WND_World) )
    { docks_level_and_world.showAll(); }

    GlobalSettings::lastWinType =   WinType;

    if(WinType==WND_Level)
    {
        emit setSMBX64Strict(lvlWin->LvlData.meta.smbx64strict);

        if( configs.check() )
        {
            LogCritical("*.INI Configs not loaded");
            return;
        }

        if(lvlWin->sceneCreated)
        {
            dock_LvlSectionProps->switchResizeMode(
                 (lvlWin->scene->m_resizeBox != nullptr)&&(lvlWin->scene->m_resizeBox->type==0)
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

        dock_LvlSectionProps->initDefaults();
        ui->actionSelect->trigger();

        if(lvlWin->sceneCreated)
        {
            LvlScene * scn = lvlWin->scene;
            ui->actionLockBlocks->setChecked(scn->m_lockBlock);
            ui->actionLockBGO->setChecked(scn->m_lockBgo);
            ui->actionLockNPC->setChecked(scn->m_lockNpc);
            ui->actionLockWaters->setChecked(scn->m_lockPhysenv);
            ui->actionLockDoors->setChecked(scn->m_lockDoor);

            LogDebug("-> Get scene flags: grid");
            ui->actionGridEn->setChecked(scn->m_opts.grid_snap);
            ui->actionShowGrid->setChecked(scn->m_opts.grid_show);

            GlobalSettings::LvlOpts = scn->m_opts;

            ui->actionUndo->setEnabled(scn->canUndo());
            ui->actionRedo->setEnabled(scn->canRedo());

            scn->Debugger_updateItemList();
        }
        ui->actionAnimation->setChecked( GlobalSettings::LvlOpts.animationEnabled );
        ui->actionCollisions->setChecked( GlobalSettings::LvlOpts.collisionsEnabled );
    }
    else
    if(WinType==WND_World)
    {
        emit setSMBX64Strict(wldWin->WldData.meta.smbx64strict);

        dock_WldSettingsBox->setCurrentWorldSettings();
        dock_BookmarksBox->updateBookmarkBoxByData();

        ui->actionSelect->trigger();

        if(wldWin->sceneCreated)
        {
            WldScene *scn = wldWin->scene;
            LogDebug("-> Get scene flags: locks");
            ui->actionLockTiles->setChecked(scn->m_lockTerrain);
            ui->actionLockScenes->setChecked(scn->m_lockScenery);
            ui->actionLockPaths->setChecked(scn->m_lockPath);
            ui->actionLockLevels->setChecked(scn->m_lockLevel);
            ui->actionLockMusicBoxes->setChecked(scn->m_lockMusicBox);

            LogDebug("-> Get scene flags: grid");
            ui->actionGridEn->setChecked(scn->m_opts.grid_snap);
            ui->actionShowGrid->setChecked(scn->m_opts.grid_show);

            LogDebug("-> Get scene flags: animation and collision");
            GlobalSettings::LvlOpts = scn->m_opts;
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

    LogDebug("-> Music Player");
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

    QAction * SubView = ui->menuWindow->addAction(tr("Sub-windows"));
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
        action->setChecked( windows[i] == LastActiveSubWindow );

        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}


void MainWindow::UpdateCustomItems()
{
    dock_LvlItemBox->initItemLists();
    dock_WldItemBox->setWldItemBoxes(true, true);
}
