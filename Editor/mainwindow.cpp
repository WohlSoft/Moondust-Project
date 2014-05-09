/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    setDefaults(); // Apply default common settings

    QPixmap splashimg(":/images/splash.png");
    QSplashScreen splash(splashimg);
    splash.show();

    if(!configs.loadconfigs(true))
    {
        QMessageBox::critical(this, "Configuration error", "Configuration can't be load.\nSee in debug_log.txt for more information.", QMessageBox::Ok);
        splash.finish(this);
        WriteToLog(QtFatalMsg, "<Application emergency closed>");
        exit(EXIT_FAILURE);
        return;
    }

    splash.finish(this);

    ui->setupUi(this);

    setUiDefults(); //Apply default UI settings

}

//Scene Event Detector
void MainWindow::TickTack()
{
    if(TickTackLock) return;

    TickTackLock = true;

    try
    {
        if(activeChildWindow()==1)
        {
            //Capturing flags from active Window
            if(activeLvlEditWin()->scene->wasPasted)
            {
                activeLvlEditWin()->changeCursor(0);
                activeLvlEditWin()->scene->wasPasted=false;
            }
            else
            if(activeLvlEditWin()->scene->doCut)
            {
                on_actionCut_triggered();
                activeLvlEditWin()->scene->doCut=false;
            }
            else
            if(activeLvlEditWin()->scene->doCopy)
            {
                on_actionCopy_triggered();
                activeLvlEditWin()->scene->doCopy=false;
            }

        }
        /*
        else
        if(activeChildWindow()==2)
        {
            if(activeNpcEditWin()->NpcData.ReadFileValid);
        }*/
    }
    catch(int e)
    {
        WriteToLog(QtWarningMsg, QString("CLASS TYPE MISMATCH IN TIMER ON WINDOWS SWITCH: %1").arg(e));
    }

    TickTackLock = false;
}

MainWindow::~MainWindow()
{
    TickTackLock = false;
    delete ui;
    WriteToLog(QtDebugMsg, "--> Application closed <--");
}


void MainWindow::updateMenus()
{
    WriteToLog(QtDebugMsg, QString("Update menus"));

    int WinType = activeChildWindow(); // 1 lvledit, 2 npcedit, 3 wldedit
    bool hasSWindow = (WinType != 0);

    ui->actionSave->setEnabled(hasSWindow);
    ui->actionSave_as->setEnabled(hasSWindow);
    ui->actionSave_all->setEnabled(hasSWindow);
    ui->actionClose->setEnabled(hasSWindow);

    ui->menuView->setEnabled( (hasSWindow) && (WinType!=2) );

    ui->actionSelect->setEnabled( (WinType==1) || (WinType==3));
    ui->actionEriser->setEnabled( (WinType==1) || (WinType==3));
    ui->actionHandScroll->setEnabled( (WinType==1) || (WinType==3));
    ui->actionReload->setEnabled( (WinType==1) || (WinType==3));

    if(!(WinType==3)) WorldToolBoxVis = ui->WorldToolBox->isVisible(); //Save current visible status

    ui->WorldToolBox->setVisible( (WinType==3) && (WorldToolBoxVis)); //Restore saved visible status
    ui->menuWorld->setEnabled(( WinType==3) );
    ui->actionWLDToolBox->setVisible( (WinType==3));

    ui->actionCopy->setEnabled( (WinType==1) || (WinType==3) );
    ui->actionPaste->setEnabled( (WinType==1) || (WinType==3) );

    ui->LevelObjectToolbar->setVisible( (WinType==1) );



    if((!(WinType==1))&& (lastWinType == 1) )
    {
        LevelToolBoxVis = ui->LevelToolBox->isVisible();  //Save current visible status
        SectionToolBoxVis = ui->LevelSectionSettings->isVisible();
        LevelDoorsBoxVis = ui->DoorsToolbox->isVisible();
        LevelLayersBoxVis = ui->LevelLayers->isVisible();

        ui->LevelToolBox->setVisible( 0 ); //Hide level toolbars
        ui->LevelSectionSettings->setVisible( 0 );
        ui->DoorsToolbox->setVisible( 0 );
        ui->LevelLayers->setVisible( 0 );
    }

    if((lastWinType !=1) && (WinType==1))
    {
        ui->LevelToolBox->setVisible( LevelToolBoxVis ); //Restore saved visible status
        ui->LevelSectionSettings->setVisible( SectionToolBoxVis );
        ui->DoorsToolbox->setVisible( LevelDoorsBoxVis );
        ui->LevelLayers->setVisible( LevelLayersBoxVis );
    }
    lastWinType =   WinType;

    ui->actionLVLToolBox->setVisible( (WinType==1) );
    ui->actionWarpsAndDoors->setVisible( (WinType==1) );
    ui->actionSection_Settings->setVisible( (WinType==1) );
    ui->actionLevelProp->setEnabled( (WinType==1) );
    ui->actionWarpsAndDoors->setVisible( (WinType==1) );

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
    ui->actionGridEn->setEnabled( (WinType==1) );

    if(WinType==1)
    {
        if(
                (configs.main_bgo.size()<=0)||
                (configs.main_bg.size()<=0)||
                (configs.main_block.size()<=0)||
                (configs.main_music_lvl.size()<=0)||
                (configs.main_music_wld.size()<=0)||
                (configs.main_music_spc.size()<=0)
          )
        {
            WriteToLog(QtCriticalMsg, "*.INI Configs not loaded");
            return;
        }

        SetCurrentLevelSection(0, 1);
        setDoorsToolbox();
        setLayersBox();

        setMusic( ui->actionPlayMusic->isChecked() );
        ui->actionSelect->trigger();
        ui->actionLockBlocks->setChecked(activeLvlEditWin()->scene->lock_block);
        ui->actionLockBGO->setChecked(activeLvlEditWin()->scene->lock_bgo);
        ui->actionLockNPC->setChecked(activeLvlEditWin()->scene->lock_npc);
        ui->actionLockWaters->setChecked(activeLvlEditWin()->scene->lock_water);
        ui->actionLockDoors->setChecked(activeLvlEditWin()->scene->lock_door);

        LvlOpts.animationEnabled = activeLvlEditWin()->scene->opts.animationEnabled;
        LvlOpts.collisionsEnabled = activeLvlEditWin()->scene->opts.collisionsEnabled;
        ui->actionAnimation->setChecked( LvlOpts.animationEnabled );
        ui->actionCollisions->setChecked( LvlOpts.collisionsEnabled );
    }

    /*
    closeAllAct->setEnabled(hasSWindow);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasSWindow);
    nextAct->setEnabled(hasSWindow);
    previousAct->setEnabled(hasSWindow);
    separatorAct->setVisible(hasSWindow);
    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
    */



    //Window menu
    ui->menuWindow->clear();
    /*
        ui->menuWindow->addAction(closeAct);
        ui->menuWindow->addAction(closeAllAct);
        ui->menuWindow->addSeparator();
        ui->menuWindow->addAction(tileAct);
        ui->menuWindow->addAction(cascadeAct);
        ui->menuWindow->addSeparator();
        ui->menuWindow->addAction(nextAct);
        ui->menuWindow->addAction(previousAct);
        ui->menuWindow->addAction(separatorAct);
    */


    QAction * empty = ui->menuWindow->addAction( tr("[No opened files]") );
        empty->setDisabled(1);

    QList<QMdiSubWindow *> windows = ui->centralWidget->subWindowList();
        empty->setVisible( windows.isEmpty() );


    for (int i = 0; i < windows.size(); ++i) {
        //QM *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1").arg( windows.at(i)->windowTitle() ) ;
        } else {
            text = tr("%1").arg( windows.at(i)->windowTitle() ) ;
        }
        QAction *action  = ui->menuWindow->addAction(text);
        action->setCheckable(true);
        action->setChecked( windows[i] == ui->centralWidget->activeSubWindow() );

        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));

        windowMapper->setMapping(action, windows.at(i));
    }
}



void MainWindow::save()
{
    bool saved=false;
    int WinType = activeChildWindow();
    if (WinType!=0)
    {
        if(WinType==2)
            saved = activeNpcEditWin()->save();
        if(WinType==1)
            saved = activeLvlEditWin()->save();

        if(saved) statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::save_as()
{
    bool saved=false;
    int WinType = activeChildWindow();
    if (WinType!=0)
    {
        if(WinType==2)
            saved = activeNpcEditWin()->saveAs();
        if(WinType==1)
            saved = activeLvlEditWin()->saveAs();

        if(saved) statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::save_all()
{
    leveledit *ChildWindow0;
    npcedit *ChildWindow2;

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())=="leveledit")
        {
        ChildWindow0 = qobject_cast<leveledit *>(window->widget());
            ChildWindow0->save();
        }
        else if(QString(window->widget()->metaObject()->className())=="npcedit")
        {
        ChildWindow2 = qobject_cast<npcedit *>(window->widget());
            ChildWindow2->save();
        }

    }
}

void MainWindow::close_sw()
{
    ui->centralWidget->activeSubWindow()->close();
}


///////////////////////////////////////////////////////


//////////////////////////////////////////////////////

void MainWindow::on_OpenFile_triggered()
{
     QString fileName_DATA = QFileDialog::getOpenFileName(this,
        trUtf8("Open file"),LastOpenDir,
        tr("All SMBX files (*.LVL *.WLD npc-*.TXT *.INI)\n"
        "SMBX Level (*.LVL)\n"
        "SMBX World (*.WLD)\n"
        "SMBX NPC Config (npc-*.TXT)\n"
        "All Files (*.*)"));

        if(fileName_DATA==NULL) return;

        OpenFile(fileName_DATA);

}

void MainWindow::OpenFile(QString FilePath)
{

    QFile file(FilePath);


    QMdiSubWindow *existing = findMdiChild(FilePath);
            if (existing) {
                ui->centralWidget->setActiveSubWindow(existing);
                return;
            }


    if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, tr("File open error"),
    tr("Can't open the file."), QMessageBox::Ok);
        return;
    }

    QFileInfo in_1(FilePath);

    LastOpenDir = in_1.absoluteDir().absolutePath();

    if(in_1.suffix() == "lvl")
    {

        LevelData FileData = ReadLevelFile(file); //function in file_formats.cpp
        if( !FileData.ReadFileValid ) return;

        FileData.filename = in_1.baseName();
        FileData.path = in_1.absoluteDir().absolutePath();
        FileData.playmusic = autoPlayMusic;

        leveledit *child = createChild();
        if ( (bool)(child->loadFile(FilePath, FileData, configs, LvlOpts)) ) {
            statusBar()->showMessage(tr("Level file loaded"), 2000);
            child->show();

            SetCurrentLevelSection(0);
            setDoorsToolbox();
            setLayersBox();

        } else {
            WriteToLog(QtDebugMsg, ">>File loading aborted");
            child->show();
            WriteToLog(QtDebugMsg, ">>Window showed");
            if(activeChildWindow()==1) activeLvlEditWin()->LvlData.modified = false;
            WriteToLog(QtDebugMsg, ">>Option seted");
            ui->centralWidget->activeSubWindow()->close();
            WriteToLog(QtDebugMsg, ">>Windows closed");
        }
    }
    else
    if(in_1.suffix() == "wld")
    {
        WorldData FileData = ReadWorldFile(file);
        if( !FileData.ReadFileValid ) return;

        /*
        leveledit *child = createChild();
        if (child->loadFile(FilePath)) {
            statusBar()->showMessage(tr("World map file loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
        */

    }
    else
    if(in_1.suffix() == "txt")
    {
        NPCConfigFile FileData = ReadNpcTXTFile(file);
        if( !FileData.ReadFileValid ) return;

        npcedit *child = createNPCChild();
        if (child->loadFile(FilePath, FileData)) {
            statusBar()->showMessage(tr("NPC Config loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
    else
    {
    QMessageBox::warning(this, tr("Bad file"),
     tr("This file have unknown extension"),
     QMessageBox::Ok);
    return;
    }

    // Add to recent fileList
    AddToRecentFiles(FilePath);
    SyncRecentFiles();
}


/*
void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    pasteAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

*/


//////////////////SLOTS///////////////////////////

//Exit from application
void MainWindow::on_Exit_triggered()
{
    MainWindow::close();
    exit(0);
}

//Open About box
void MainWindow::on_actionAbout_triggered()
{
    aboutDialog about;
    about.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    about.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, about.size(), qApp->desktop()->availableGeometry()));
    about.exec();
}





/*
void MainWindow::on_pushButton_4_clicked()
{
    //ui->xxxxlistWidget->insertItem(1, "test");
    //MyListItem *itm = new MyListItem;
    //insertItem(1, "test image");
    //itm->setText("My Item");
    //itm->icon_to_be_shown = "./data/graphics/worldmap/tile/tile-1.gif";
    //ui->TestItemBox->addItem(itm);

    //ui->TilesItemBox
    //ui->TilesItemBox->model()->setData(  ui->TilesItemBox->model()->index(1, 1),);

    QString first = "file-1.35.gif";
    QStringList mid1 = first.split(".",QString::SkipEmptyParts);
    QString second = mid1[0] + "m." + mid1[1];

    QMessageBox::information(this, tr("QRegExp test"),
       tr(QString("First: "+first+"\nSecond: "+second).toStdString().c_str()),
     QMessageBox::Ok);


}*/


void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_actionSave_as_triggered()
{
    save_as();
}

void MainWindow::on_actionClose_triggered()
{
    close_sw();
}

void MainWindow::on_actionSave_all_triggered()
{
    save_all();
}


