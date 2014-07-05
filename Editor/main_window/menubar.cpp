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


void MainWindow::updateMenus(bool force)
{
    if(!force) if(!this->isActiveWindow()) return;

    WriteToLog(QtDebugMsg, QString("Update menus"));

    int WinType = activeChildWindow(); // 1 lvledit, 2 npcedit, 3 wldedit
    bool hasSWindow = (WinType != 0);

    ui->actionSave->setEnabled(hasSWindow);
    ui->actionSave_as->setEnabled(hasSWindow);
    ui->actionSave_all->setEnabled(hasSWindow);
    ui->actionClose->setEnabled(hasSWindow);

    ui->menuView->setEnabled( (hasSWindow) && (WinType!=2) );

    ui->actionSelect->setEnabled( (WinType==1) || (WinType==3));
    ui->actionSelectOnly->setEnabled( (WinType==1) || (WinType==3));
    ui->actionEriser->setEnabled( (WinType==1) || (WinType==3));
    ui->actionHandScroll->setEnabled( (WinType==1) || (WinType==3));
    ui->actionReload->setEnabled( (WinType==1) || (WinType==3));

    if(!(WinType==3)) GlobalSettings::WorldToolBoxVis = ui->WorldToolBox->isVisible(); //Save current visible status
    ui->WorldToolBox->setVisible( (WinType==3) && (GlobalSettings::WorldToolBoxVis)); //Restore saved visible status


    ui->menuWorld->setEnabled(( WinType==3) );
    ui->actionWLDToolBox->setVisible( (WinType==3));

    ui->actionCopy->setEnabled( (WinType==1) || (WinType==3) );
    ui->actionPaste->setEnabled( (WinType==1) || (WinType==3) );
    ui->actionCut->setEnabled( (WinType==1) || (WinType==3) );

    ui->LevelObjectToolbar->setVisible( (WinType==1) );
    ui->ItemProperties->setVisible(false);



    if((!(WinType==1))&& (GlobalSettings::lastWinType == 1) )
    {
        GlobalSettings::LevelToolBoxVis = ui->LevelToolBox->isVisible();  //Save current visible status
        GlobalSettings::SectionToolBoxVis = ui->LevelSectionSettings->isVisible();

        GlobalSettings::LevelDoorsBoxVis = ui->DoorsToolbox->isVisible();
        GlobalSettings::LevelLayersBoxVis = ui->LevelLayers->isVisible();
        GlobalSettings::LevelEventsBoxVis = ui->LevelEventsToolBox->isVisible();

        GlobalSettings::LevelSearchBoxVis = ui->FindDock->isVisible();

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
    }
    GlobalSettings::lastWinType =   WinType;

    ui->actionLVLToolBox->setVisible( (WinType==1) );
    ui->actionWarpsAndDoors->setVisible( (WinType==1) );
    ui->actionSection_Settings->setVisible( (WinType==1) );
    ui->actionLevelProp->setEnabled( (WinType==1) );
    ui->actionLevelEvents->setEnabled( (WinType==1) );
    ui->actionWarpsAndDoors->setVisible( (WinType==1) );
    ui->actionLVLSearchBox->setVisible( (WinType==1) );

    ui->menuLevel->setEnabled( (WinType==1) );



    ui->actionLevNoBack->setEnabled( (WinType==1) );
    ui->actionLevOffScr->setEnabled( (WinType==1) );
    ui->actionLevWarp->setEnabled( (WinType==1) );
    ui->actionLevUnderW->setEnabled( (WinType==1) );
    ui->actionExport_to_image->setEnabled( (WinType==1) );

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

    ui->actionReset_position->setEnabled( (WinType==1) );

    ui->actionGo_to_Section->setEnabled( (WinType==1) );

    ui->actionGridEn->setEnabled( (WinType==1) );

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

        SetCurrentLevelSection(0, 1);
        setDoorsToolbox();
        setLayersBox();
        setEventsBox();

        //Sync lists in properties windows
        EventListsSync();
        setLayerLists();

        setMusic( ui->actionPlayMusic->isChecked() );
        ui->actionSelect->trigger();


        if(activeLvlEditWin()->sceneCreated)
        {
            ui->actionLockBlocks->setChecked(activeLvlEditWin()->scene->lock_block);
            ui->actionLockBGO->setChecked(activeLvlEditWin()->scene->lock_bgo);
            ui->actionLockNPC->setChecked(activeLvlEditWin()->scene->lock_npc);
            ui->actionLockWaters->setChecked(activeLvlEditWin()->scene->lock_water);
            ui->actionLockDoors->setChecked(activeLvlEditWin()->scene->lock_door);

            GlobalSettings::LvlOpts.animationEnabled = activeLvlEditWin()->scene->opts.animationEnabled;
            GlobalSettings::LvlOpts.collisionsEnabled = activeLvlEditWin()->scene->opts.collisionsEnabled;
            ui->actionUndo->setEnabled(activeLvlEditWin()->scene->canUndo());
            ui->actionRedo->setEnabled(activeLvlEditWin()->scene->canRedo());

            UpdateCustomItems(); // Update custom item lists for Level
        }
        ui->actionAnimation->setChecked( GlobalSettings::LvlOpts.animationEnabled );
        ui->actionCollisions->setChecked( GlobalSettings::LvlOpts.collisionsEnabled );
    }
    else
    {
        ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(false);
    }

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
        if (i < 9) {
            text = QString("&%1").arg( windows.at(i)->windowTitle() ) ;
        } else {
            text = QString("%1").arg( windows.at(i)->windowTitle() ) ;
        }
        QAction *action  = ui->menuWindow->addAction(text);
        action->setCheckable(true);
        action->setChecked( windows[i] == ui->centralWidget->activeSubWindow() );
        //action_delete_list.push_back(action);

        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}
