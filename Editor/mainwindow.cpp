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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "levelprops.h"
#include <QFileDialog>
#include <QtWidgets>
#include <QMdiArea>
#include <QFile>
#include <QSettings>
#include <QTranslator>
#include <QtMultimedia/QMediaPlayer>
#include <QMediaPlaylist>
#include <QLocale>
#include <QSplashScreen>
#include "childwindow.h"
#include "leveledit.h"
#include "npcedit.h"
#include "dataconfigs.h"
#include "musicfilelist.h"
#include "logger.h"

QString LastOpenDir = ".";
bool LevelToolBoxVis = false;
bool WorldToolBoxVis = false;
bool SectionToolBoxVis = false;
bool autoPlayMusic = false;

QMediaPlayer * MusicPlayer;

QString currentCustomMusic = "";
long currentMusicId = 0;
bool musicButtonChecked = false;

MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //QMenuBar * menuBar = new QMenuBar(0); // test for MacOS X

    MusicPlayer = new QMediaPlayer;

    LvlOpts.animationEnabled = true;
    LvlOpts.collisionsEnabled = true;

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


    ui->setupUi(this);

    //ui->BlocksItemBox->;

    //Applay objects into tools
    setTools();

    #ifdef Q_OS_MAC
        this->setWindowIcon(QIcon(":/images/mac/mushroom.icns"));
    #endif

    //menuBar = ui->menuBar; // test for MacOS X
    //setMenuBar(menuBar); // test for MacOS X

    splash.finish(this);


    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    QString inifile = QApplication::applicationDirPath() + "/" + "plweditor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
    //resize(settings.value("size", size()).toSize());
    //move(settings.value("pos", pos()).toPoint());
    LastOpenDir = settings.value("lastpath", ".").toString();
    LevelToolBoxVis = settings.value("level-tb-visible", "false").toBool();
    WorldToolBoxVis = settings.value("world-tb-visible", "false").toBool();
    SectionToolBoxVis = settings.value("section-tb-visible", "false").toBool();
    LvlOpts.animationEnabled = settings.value("animation", "true").toBool();
    LvlOpts.collisionsEnabled = settings.value("collisions", "true").toBool();
    //if(settings.value("maximased", "false")=="true") showMaximized();
    //"lvl-section-view", dockWidgetArea(ui->LevelSectionSettings)
    //dockWidgetArea();
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    autoPlayMusic = settings.value("autoPlayMusic", false).toBool();
    settings.endGroup();

    settings.beginGroup("Recent");
    for(int i = 1; i<=10;i++){
        recentOpen.push_back(settings.value("recent"+QString::number(i),"<empty>").toString());
    }
    SyncRecentFiles();
    settings.endGroup();

    ui->actionPlayMusic->setChecked(autoPlayMusic);

    ui->centralWidget->cascadeSubWindows();
    ui->WorldToolBox->hide();
    ui->LevelSectionSettings->hide();
    ui->LevelToolBox->hide();
    ui->DoorsToolbox->hide();

    ui->menuView->setEnabled(0);
    ui->menuWindow->setEnabled(0);
    ui->menuLevel->setEnabled(0);
    ui->menuWorld->setEnabled(0);
    ui->LevelObjectToolbar->setVisible(0);

    ui->actionLVLToolBox->setVisible(0);
    ui->actionWarpsAndDoors->setVisible(0);
    ui->actionSection_Settings->setVisible(0);
    ui->actionWLDToolBox->setVisible(0);
    ui->actionGridEn->setChecked(1);

    setAcceptDrops(true);
    ui->centralWidget->cascadeSubWindows();

}

MainWindow::~MainWindow()
{
    delete ui;
    WriteToLog(QtDebugMsg, "--> Application closed <--");
}

dataconfigs * MainWindow::getConfigs()
{
    dataconfigs * pointer;
    pointer = &configs;
    return pointer;
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

    ui->LevelObjectToolbar->setVisible( (WinType==1) );

    if(!(WinType==1)) LevelToolBoxVis = ui->LevelToolBox->isVisible();  //Save current visible status
    if(!(WinType==1)) SectionToolBoxVis = ui->LevelSectionSettings->isVisible();

    ui->LevelToolBox->setVisible( (WinType==1) && (LevelToolBoxVis)); //Restore saved visible status
    ui->LevelSectionSettings->setVisible( (WinType==1) && (SectionToolBoxVis));

    ui->actionLVLToolBox->setVisible( (WinType==1) );
    ui->actionWarpsAndDoors->setVisible( (WinType==1) );

    ui->menuLevel->setEnabled( (WinType==1) );

    ui->actionSection_Settings->setVisible( (WinType==1) );
    ui->actionLevelProp->setEnabled( (WinType==1) );
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

    ui->menuWindow->setEnabled( (WinType!=0) );

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
}

void MainWindow::setTools()
{
    int i;
        WriteToLog(QtDebugMsg, QString("Set toolboxes"));
    ui->LVLPropsBackImage->clear();
    ui->LVLPropsMusicNumber->clear();

    ui->LVLPropsBackImage->addItem("[No image]");
    ui->LVLPropsMusicNumber->addItem("[Silence]");

    for(i=0; i< configs.main_bg.size();i++)
        ui->LVLPropsBackImage->addItem(configs.main_bg[i].name);

    for(i=0; i< configs.main_music_lvl.size();i++)
        ui->LVLPropsMusicNumber->addItem(configs.main_music_lvl[i].name);

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

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        const QString &fileName = url.toLocalFile();
        //qDebug() << "Dropped file:" << fileName;
        OpenFile(fileName);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->centralWidget->closeAllSubWindows();


    if (ui->centralWidget->currentSubWindow()) {
        event->ignore();

    }
    else
    {
    QString inifile = QApplication::applicationDirPath() + "/" + "plweditor.ini";

    QSettings settings(inifile, QSettings::IniFormat);
    settings.beginGroup("Main");
    settings.setValue("pos", pos());
    settings.setValue("lastpath", LastOpenDir);
    settings.setValue("level-tb-visible", LevelToolBoxVis);
    settings.setValue("world-tb-visible", WorldToolBoxVis);
    settings.setValue("section-tb-visible", SectionToolBoxVis);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("autoPlayMusic", autoPlayMusic);
    settings.setValue("animation", LvlOpts.animationEnabled);
    settings.setValue("collisions", LvlOpts.collisionsEnabled);
    settings.endGroup();

    settings.beginGroup("Recent");
    for(int i = 1; i<=10;i++){
        settings.setValue("recent"+QString::number(i),recentOpen[i-1]);
    }
    settings.endGroup();
    event->accept();
    }
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

/*
QMessageBox::information(this, tr("File checked"),
tr("This file is good"),
QMessageBox::Ok);
*/

}

void MainWindow::SyncRecentFiles()
{
    ui->action_recent1->setText(recentOpen[0]);
    ui->action_recent2->setText(recentOpen[1]);
    ui->action_recent3->setText(recentOpen[2]);
    ui->action_recent4->setText(recentOpen[3]);
    ui->action_recent5->setText(recentOpen[4]);
    ui->action_recent6->setText(recentOpen[5]);
    ui->action_recent7->setText(recentOpen[6]);
    ui->action_recent8->setText(recentOpen[7]);
    ui->action_recent9->setText(recentOpen[8]);
    ui->action_recent10->setText(recentOpen[9]);

    ui->action_recent1->setEnabled(!(recentOpen[0]=="<empty>"));
    ui->action_recent2->setEnabled(!(recentOpen[1]=="<empty>"));
    ui->action_recent3->setEnabled(!(recentOpen[2]=="<empty>"));
    ui->action_recent4->setEnabled(!(recentOpen[3]=="<empty>"));
    ui->action_recent5->setEnabled(!(recentOpen[4]=="<empty>"));
    ui->action_recent6->setEnabled(!(recentOpen[5]=="<empty>"));
    ui->action_recent7->setEnabled(!(recentOpen[6]=="<empty>"));
    ui->action_recent8->setEnabled(!(recentOpen[7]=="<empty>"));
    ui->action_recent9->setEnabled(!(recentOpen[8]=="<empty>"));
    ui->action_recent10->setEnabled(!(recentOpen[9]=="<empty>"));
}

void MainWindow::AddToRecentFiles(QString FilePath)
{
    int index;
    if((index = recentOpen.indexOf(FilePath))!=-1){
        recentOpen.removeAt(index);
        recentOpen.push_front(FilePath);
    }else{
        //just in case
        if(recentOpen.size() >= 10){
            recentOpen.pop_back();
        }
        recentOpen.push_front(FilePath);
    }
}

//Edit NPC
npcedit *MainWindow::createNPCChild()
{
    npcedit *child = new npcedit;
    QMdiSubWindow *npcWindow = new QMdiSubWindow;
    npcWindow->setWidget(child);
    npcWindow->setFixedSize(520,640);
    npcWindow->setAttribute(Qt::WA_DeleteOnClose);
    npcWindow->setWindowFlags(Qt::WindowCloseButtonHint);
    npcWindow->move(rand()%100,rand()%50);
    ui->centralWidget->addSubWindow(npcWindow);

 /*   connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
 */

    return child;
}



//Edit LEVEL
leveledit *MainWindow::createChild()
{
    leveledit *child = new leveledit;
    //child->setWindowIcon(QIcon(QPixmap(":/lvl16.png")));
    ui->centralWidget->addSubWindow(child)->resize(QSize(800, 602));

        return child;
}



int MainWindow::activeChildWindow()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
    {
        if(QString(activeSubWindow->widget()->metaObject()->className())=="leveledit")
            return 1;
        if(QString(activeSubWindow->widget()->metaObject()->className())=="npcedit")
            return 2;
    }

    return 0;
}

npcedit *MainWindow::activeNpcEditWin()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
        return qobject_cast<npcedit *>(activeSubWindow->widget());
    return 0;
}

leveledit *MainWindow::activeLvlEditWin()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
        return qobject_cast<leveledit *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    leveledit *ChildWindow0;
    npcedit *ChildWindow2;

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())=="leveledit")
        {
        ChildWindow0 = qobject_cast<leveledit *>(window->widget());
        if (ChildWindow0->currentFile() == canonicalFilePath)
            return window;
        }
        else if(QString(window->widget()->metaObject()->className())=="npcedit")
        {
        ChildWindow2 = qobject_cast<npcedit *>(window->widget());
        if (ChildWindow2->currentFile() == canonicalFilePath)
            return window;
        }

    }
    return 0;
}


void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    ui->centralWidget->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
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


void MainWindow::SetCurrentLevelSection(int SctId, int open)
{
    int SectionId = SctId;
    int WinType = activeChildWindow();

    WriteToLog(QtDebugMsg, "Set Current Section");
    if ((open==1)&&(WinType==1)) // Only Set Checked section number without section select
    {
        WriteToLog(QtDebugMsg, "get Current Section");
        SectionId = activeLvlEditWin()->LvlData.CurSection;
    }

    WriteToLog(QtDebugMsg, "Set checkbox to");
    ui->actionSection_1->setChecked( (SectionId==0) );
    ui->actionSection_2->setChecked( (SectionId==1) );
    ui->actionSection_3->setChecked( (SectionId==2) );
    ui->actionSection_4->setChecked( (SectionId==3) );
    ui->actionSection_5->setChecked( (SectionId==4) );
    ui->actionSection_6->setChecked( (SectionId==5) );
    ui->actionSection_7->setChecked( (SectionId==6) );
    ui->actionSection_8->setChecked( (SectionId==7) );
    ui->actionSection_9->setChecked( (SectionId==8) );
    ui->actionSection_10->setChecked( (SectionId==9) );
    ui->actionSection_11->setChecked( (SectionId==10) );
    ui->actionSection_12->setChecked( (SectionId==11) );
    ui->actionSection_13->setChecked( (SectionId==12) );
    ui->actionSection_14->setChecked( (SectionId==13) );
    ui->actionSection_15->setChecked( (SectionId==14) );
    ui->actionSection_16->setChecked( (SectionId==15) );
    ui->actionSection_17->setChecked( (SectionId==16) );
    ui->actionSection_18->setChecked( (SectionId==17) );
    ui->actionSection_19->setChecked( (SectionId==18) );
    ui->actionSection_20->setChecked( (SectionId==19) );
    ui->actionSection_21->setChecked( (SectionId==20) );

    if ((WinType==1) && (open==0))
    {
       WriteToLog(QtDebugMsg, "Call to setCurrentSection()");
       activeLvlEditWin()->setCurrentSection(SectionId);
    }

    if (WinType==1)
    {
        WriteToLog(QtDebugMsg, "Set Section Data in menu");
        //Set Section Data in menu
        ui->actionLevNoBack->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].noback);
        ui->actionLevOffScr->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].OffScreenEn);
        ui->actionLevUnderW->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].underwater);
        ui->actionLevWarp->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].IsWarp);

        WriteToLog(QtDebugMsg, "Set text label");
        //set data in Section Settings Widget
        ui->LVLProp_CurSect->setText(QString::number(SectionId+1));

        WriteToLog(QtDebugMsg, "Set ToolBar data");
        ui->LVLPropsNoTBack->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].noback);
        ui->LVLPropsOffScr->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].OffScreenEn);
        ui->LVLPropsUnderWater->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].underwater);
        ui->LVLPropsLevelWarp->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].IsWarp);

        WriteToLog(QtDebugMsg, "Set text to custom music file");
        ui->LVLPropsMusicCustom->setText(activeLvlEditWin()->LvlData.sections[SectionId].music_file);

        WriteToLog(QtDebugMsg, "Set standart Music index");
        ui->LVLPropsMusicNumber->setCurrentIndex( activeLvlEditWin()->LvlData.sections[SectionId].music_id );

        WriteToLog(QtDebugMsg, "Set Custom music checkbox");
        ui->LVLPropsMusicCustomEn->setChecked( (activeLvlEditWin()->LvlData.sections[SectionId].music_id == configs.music_custom_id) );

        WriteToLog(QtDebugMsg, "Set background index");
        ui->LVLPropsBackImage->setCurrentIndex( activeLvlEditWin()->LvlData.sections[SectionId].background );
    }
}


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


// Level tool box show/hide
void MainWindow::on_LevelToolBox_visibilityChanged(bool visible)
{
    if(visible)
    {
        //MainWindow::menuBar()->actions()->;
        ui->actionLVLToolBox->setChecked(true);
    }
     else
    {
        ui->actionLVLToolBox->setChecked(false);
    }
}

void MainWindow::on_actionLVLToolBox_triggered()
{
        if(ui->actionLVLToolBox->isChecked())
            ui->LevelToolBox->setVisible(true);
        else
            ui->LevelToolBox->setVisible(false);
}

// World tool box show/hide
void MainWindow::on_WorldToolBox_visibilityChanged(bool visible)
{
    if(visible)
    {
        //MainWindow::menuBar()->actions()->;
        ui->actionWLDToolBox->setChecked(true);
    }
     else
    {
        ui->actionWLDToolBox->setChecked(false);
    }
}

void MainWindow::on_actionWLDToolBox_triggered()
{
    if(ui->actionWLDToolBox->isChecked())
        ui->WorldToolBox->setVisible(true);
    else
        ui->WorldToolBox->setVisible(false);
}

// Level Section tool box show/hide
void MainWindow::on_LevelSectionSettings_visibilityChanged(bool visible)
{
    if(visible)
    {
        //MainWindow::menuBar()->actions()->;
        ui->actionSection_Settings->setChecked(true);
    }
     else
    {
        ui->actionSection_Settings->setChecked(false);
    }
}

void MainWindow::on_actionSection_Settings_triggered()
{
    if(ui->actionSection_Settings->isChecked())
        ui->LevelSectionSettings->setVisible(true);
    else
        ui->LevelSectionSettings->setVisible(false);
}


//Open Level Properties
void MainWindow::on_actionLevelProp_triggered()
{
    if(activeChildWindow()==1)
    {
        LevelProps LevProps(activeLvlEditWin()->LvlData);
        if(LevProps.exec()==QDialog::Accepted)
        {
            activeLvlEditWin()->LvlData.playmusic = LevProps.AutoPlayMusic;
            activeLvlEditWin()->LvlData.LevelName = LevProps.LevelTitle;
            autoPlayMusic = LevProps.AutoPlayMusic;
        }
    }

}

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


}


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

//////////////////////////////////// GoTo Section  ///////////////////////////////////////////////
void MainWindow::on_actionSection_1_triggered()
{
    SetCurrentLevelSection(0);
}

void MainWindow::on_actionSection_2_triggered()
{
    SetCurrentLevelSection(1);
}

void MainWindow::on_actionSection_3_triggered()
{
    SetCurrentLevelSection(2);
}

void MainWindow::on_actionSection_4_triggered()
{
    SetCurrentLevelSection(3);
}

void MainWindow::on_actionSection_5_triggered()
{
    SetCurrentLevelSection(4);
}

void MainWindow::on_actionSection_6_triggered()
{
    SetCurrentLevelSection(5);
}

void MainWindow::on_actionSection_7_triggered()
{
    SetCurrentLevelSection(6);
}

void MainWindow::on_actionSection_8_triggered()
{
    SetCurrentLevelSection(7);
}

void MainWindow::on_actionSection_9_triggered()
{
    SetCurrentLevelSection(8);
}

void MainWindow::on_actionSection_10_triggered()
{
    SetCurrentLevelSection(9);
}

void MainWindow::on_actionSection_11_triggered()
{
    SetCurrentLevelSection(10);
}

void MainWindow::on_actionSection_12_triggered()
{
    SetCurrentLevelSection(11);
}


void MainWindow::on_actionSection_13_triggered()
{
    SetCurrentLevelSection(12);
}

void MainWindow::on_actionSection_14_triggered()
{
    SetCurrentLevelSection(13);
}

void MainWindow::on_actionSection_15_triggered()
{
    SetCurrentLevelSection(14);
}

void MainWindow::on_actionSection_16_triggered()
{
    SetCurrentLevelSection(15);
}

void MainWindow::on_actionSection_17_triggered()
{
    SetCurrentLevelSection(16);
}

void MainWindow::on_actionSection_18_triggered()
{
    SetCurrentLevelSection(17);
}

void MainWindow::on_actionSection_19_triggered()
{
    SetCurrentLevelSection(18);
}

void MainWindow::on_actionSection_20_triggered()
{
    SetCurrentLevelSection(19);
}

void MainWindow::on_actionSection_21_triggered()
{
    SetCurrentLevelSection(20);
}


void MainWindow::on_actionLoad_configs_triggered()
{
    //Reload configs
    configs.loadconfigs();

    //Set tools from loaded configs
    //setTools();

    QMessageBox::information(this, tr("Reload configuration"),
     tr("Configuration succesfully reloaded!"),
     QMessageBox::Ok);
}



void MainWindow::on_actionExport_to_image_triggered()
{
    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->ExportToImage_fn();
    }
}

void MainWindow::on_LVLPropsMusicNumber_currentIndexChanged(int index)
{
    unsigned int test = index;
    if(ui->LVLPropsMusicNumber->hasFocus())
    {
        ui->LVLPropsMusicCustomEn->setChecked(  test == configs.music_custom_id );
    }

    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
        activeLvlEditWin()->LvlData.modified = true;
    }

    WriteToLog(QtDebugMsg, "Call to Set Music if playing");
    setMusic(ui->actionPlayMusic->isChecked());
}

void MainWindow::on_LVLPropsMusicCustomEn_toggled(bool checked)
{
    if(ui->LVLPropsMusicCustomEn->hasFocus())
    {
        if(checked)
        {
            ui->LVLPropsMusicNumber->setCurrentIndex( configs.music_custom_id );
            if(activeChildWindow()==1)
            {
                activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
                activeLvlEditWin()->LvlData.modified = true;
            }
        }
    }
}

void MainWindow::on_LVLPropsMusicCustomBrowse_clicked()
{
    QString dirPath;
    if(activeChildWindow()==1)
    {
        dirPath = activeLvlEditWin()->LvlData.path;
    }
    else return;

    MusicFileList musicList(dirPath);
    if( musicList.exec() == QDialog::Accepted )
    {
        ui->LVLPropsMusicCustom->setText(musicList.SelectedFile);
    }

}

void MainWindow::on_actionPlayMusic_triggered(bool checked)
{
    WriteToLog(QtDebugMsg, "Clicked play music button");
    currentCustomMusic = ui->LVLPropsMusicCustom->text();
    currentMusicId = ui->LVLPropsMusicNumber->currentIndex();
    setMusic(checked);
}

/*
void MainWindow::on_LVLPropsMusicPlay_toggled(bool checked)
{
    WriteToLog(QtDebugMsg, "Clicked play music button");
    currentCustomMusic = ui->LVLPropsMusicCustom->text();
    currentMusicId = ui->LVLPropsMusicNumber->currentIndex();
    setMusic(checked);
}*/

void MainWindow::setMusic(bool checked)
{
    QString dirPath;
    QString musicFile;
    QString musicFilePath;
    bool silent;
    unsigned int CurMusNum;

    if(
            (configs.main_music_lvl.size()==0)||
            (configs.main_music_spc.size()==0)||
            (configs.main_music_wld.size()==0)
            )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs not loaded"));
        return;
    }

    if( ( currentMusicId == ui->LVLPropsMusicNumber->currentIndex() ) &&
            (currentCustomMusic == ui->LVLPropsMusicCustom->text()) &&
            (musicButtonChecked == ui->actionPlayMusic->isChecked()) ) return;

    WriteToLog(QtDebugMsg, "-> New MediaPlayList");
    QMediaPlaylist * CurrentMusic = new QMediaPlaylist;


    if(activeChildWindow()!=1)
    {
        if(checked)
        {
            if(
                    ((currentMusicId>0)&&(((unsigned long)currentMusicId!=configs.music_custom_id)))||
                    (((unsigned long)currentMusicId==configs.music_custom_id)&&(currentCustomMusic!=""))
                    )
            {
                MusicPlayer->play();
                silent=false;
            }
            else
            {
                silent=true;
            }
        }
        else
        {
            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by Checked"));
                MusicPlayer->stop();
            silent=true;
        }
        return;
    }

    if(activeChildWindow()==1)
    {
        WriteToLog(QtDebugMsg, "Get Level File Path");
        dirPath = activeLvlEditWin()->LvlData.path;
    }
    else
        return;

    WriteToLog(QtDebugMsg, "Check for Sielent");

    if(ui->LVLPropsMusicNumber->currentIndex() <= 0)
        silent=true;
    else
        silent=false;

    WriteToLog(QtDebugMsg, "ifNot Sielent, play Music");

    if(!silent)
    {
        CurMusNum = ui->LVLPropsMusicNumber->currentIndex()-1;

        if(CurMusNum==configs.music_custom_id-1)
        {
                WriteToLog(QtDebugMsg, QString("get Custom music path"));
            musicFile = ui->LVLPropsMusicCustom->text();
            musicFilePath = dirPath + "/" + musicFile;
        }
        else
        {
                WriteToLog(QtDebugMsg, QString("get standart music path"));
            musicFile = configs.main_music_lvl[CurMusNum].file;
            dirPath = configs.dirs.music;
            musicFilePath = configs.dirs.music + musicFile;
            //QMessageBox::information(this, "test", "music is \n"+musicFile+"\n"+QString::number());
        }

        if(checked)
        {
            if( (QFile::exists(musicFilePath)) && (QFileInfo(musicFilePath)).isFile() )
            {
                WriteToLog(QtDebugMsg, QString("Set music player -> addMedia"));
                CurrentMusic->addMedia(QUrl::fromLocalFile( musicFilePath ));
                CurrentMusic->setPlaybackMode(QMediaPlaylist::Loop);
                WriteToLog(QtDebugMsg, QString("Set music player -> stop Current"));
                MusicPlayer->stop();
                WriteToLog(QtDebugMsg, QString("Set music player -> set PlayList"));
                MusicPlayer->setPlaylist(CurrentMusic);
                WriteToLog(QtDebugMsg, QString("Set music player -> setVolme and play"));
                MusicPlayer->setVolume(100);
                MusicPlayer->play();
            }
            else
                MusicPlayer->stop();
        }
        else
        {
            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by checker"));
            MusicPlayer->stop();
        }
    }
    else
    {
        WriteToLog(QtDebugMsg, QString("Set music player -> Stop by sielent"));
        MusicPlayer->stop();
    }

    currentCustomMusic = ui->LVLPropsMusicCustom->text();
    currentMusicId = ui->LVLPropsMusicNumber->currentIndex();
    musicButtonChecked  = ui->actionPlayMusic->isChecked();
}



void MainWindow::on_LVLPropsMusicCustom_textChanged(const QString &arg1)
{
    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_file = arg1;
    }

    setMusic( ui->actionPlayMusic->isChecked() );
}

void MainWindow::on_actionReset_position_triggered()
{
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->ResetPosition();
    }
}

void MainWindow::on_actionGridEn_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->scene->grid = checked;
    }
}

void MainWindow::on_actionSelect_triggered()
{
    ui->actionSelect->setChecked(1);
    ui->actionEriser->setChecked(0);
    ui->actionHandScroll->setChecked(0);
    if ((activeChildWindow()==1) && (ui->actionSelect->isChecked()))
    {
       activeLvlEditWin()->changeCursor(0);
       activeLvlEditWin()->scene->EditingMode = 0;
       activeLvlEditWin()->scene->EraserEnabled = false;
    }
}

void MainWindow::on_actionEriser_triggered()
{
    ui->actionSelect->setChecked(0);
    ui->actionEriser->setChecked(1);
    ui->actionHandScroll->setChecked(0);
    if ((activeChildWindow()==1) && (ui->actionEriser->isChecked()))
    {
       activeLvlEditWin()->changeCursor(1);
       activeLvlEditWin()->scene->EditingMode = 1;
       activeLvlEditWin()->scene->EraserEnabled = false;
    }
}


void MainWindow::on_actionHandScroll_triggered()
{
    ui->actionSelect->setChecked(0);
    ui->actionEriser->setChecked(0);
    ui->actionHandScroll->setChecked(1);
    if ((activeChildWindow()==1) && (ui->actionHandScroll->isChecked()))
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(-1);
       activeLvlEditWin()->scene->EditingMode = 0;
       activeLvlEditWin()->scene->EraserEnabled = false;
    }
}


void MainWindow::on_LVLPropsBackImage_currentIndexChanged(int index)
{
    if(configs.main_bg.size()==0)
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs not loaded"));
        return;
    }

    if((ui->LVLPropsBackImage->hasFocus())||(ui->LVLPropsBackImage->hasMouseTracking()))
    {
        ui->LVLPropsBackImage->setEnabled(false);
        WriteToLog(QtDebugMsg, "Change BG to "+QString::number(index));
        if (activeChildWindow()==1)
        {
           activeLvlEditWin()->scene->ChangeSectionBG(index, activeLvlEditWin()->LvlData);
        }
        ui->LVLPropsBackImage->setEnabled(true);
    }
    else
    {
        if (activeChildWindow()==1)
        {
           ui->LVLPropsBackImage->setCurrentIndex(
                       activeLvlEditWin()->LvlData.sections[
                       activeLvlEditWin()->LvlData.CurSection].background);
        }
    }
}


//Reload opened file data
void MainWindow::on_actionReload_triggered()
{
    LevelData FileData;
    QString filePath;
    QRect wnGeom;

    if (activeChildWindow()==1)
    {
        filePath = activeLvlEditWin()->curFile;

        QFile fileIn(filePath);

        if (!fileIn.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("File open error"),
        tr("Can't open the file."), QMessageBox::Ok);
            return;
        }

        FileData = ReadLevelFile(fileIn); //function in file_formats.cpp
        if( !FileData.ReadFileValid ){
            statusBar()->showMessage(tr("Reloading error"), 2000);
            return;}

        FileData.filename = QFileInfo(filePath).baseName();
        FileData.path = QFileInfo(filePath).absoluteDir().absolutePath();
        FileData.playmusic = autoPlayMusic;
        activeLvlEditWin()->LvlData.modified = false;
        activeLvlEditWin()->close();
        wnGeom = ui->centralWidget->activeSubWindow()->geometry();
        ui->centralWidget->activeSubWindow()->close();

        leveledit *child = createChild();
        if ((bool) (child->loadFile(filePath, FileData, configs, LvlOpts))) {
            statusBar()->showMessage(tr("Level file reloaded"), 2000);
            child->show();
            ui->centralWidget->activeSubWindow()->setGeometry(wnGeom);
            SetCurrentLevelSection(0);
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
}

void MainWindow::on_actionLockBlocks_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(1, checked);
    }
}

void MainWindow::on_actionLockBGO_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(2, checked);
    }
}

void MainWindow::on_actionLockNPC_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(3, checked);
    }
}

void MainWindow::on_actionLockWaters_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(4, checked);
    }
}

void MainWindow::on_actionLockDoors_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setLocked(5, checked);
    }
}


// Level Section Settings
void MainWindow::on_LVLPropsLevelWarp_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->actionLevWarp->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].IsWarp = checked;
    }
}

void MainWindow::on_actionLevWarp_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->LVLPropsLevelWarp->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].IsWarp = checked;
    }
}



void MainWindow::on_LVLPropsOffScr_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->actionLevOffScr->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].OffScreenEn = checked;
    }
}

void MainWindow::on_actionLevOffScr_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->LVLPropsOffScr->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].OffScreenEn = checked;
    }
}



void MainWindow::on_LVLPropsNoTBack_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->actionLevNoBack->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].noback = checked;
    }
}

void MainWindow::on_actionLevNoBack_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->LVLPropsNoTBack->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].noback = checked;
    }
}


void MainWindow::on_LVLPropsUnderWater_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->actionLevUnderW->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].underwater = checked;
    }
}

void MainWindow::on_actionLevUnderW_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        ui->LVLPropsUnderWater->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].underwater = checked;
    }
}

void MainWindow::on_actionAnimation_triggered(bool checked)
{
    LvlOpts.animationEnabled = checked;
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->opts.animationEnabled = LvlOpts.animationEnabled;
        if(LvlOpts.animationEnabled)
        {
            activeLvlEditWin()->scene->startBlockAnimation();
        }
        else
            activeLvlEditWin()->scene->stopAnimation();
    }
}


void MainWindow::on_actionCollisions_triggered(bool checked)
{
    LvlOpts.collisionsEnabled = checked;
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->opts.collisionsEnabled = LvlOpts.collisionsEnabled;
    }

}


void MainWindow::on_action_recent1_triggered()
{
    OpenFile(ui->action_recent1->text());
}

void MainWindow::on_action_recent2_triggered()
{
    OpenFile(ui->action_recent2->text());
}

void MainWindow::on_action_recent3_triggered()
{
    OpenFile(ui->action_recent3->text());
}

void MainWindow::on_action_recent4_triggered()
{
    OpenFile(ui->action_recent4->text());
}

void MainWindow::on_action_recent5_triggered()
{
    OpenFile(ui->action_recent5->text());
}

void MainWindow::on_action_recent6_triggered()
{
    OpenFile(ui->action_recent6->text());
}

void MainWindow::on_action_recent7_triggered()
{
    OpenFile(ui->action_recent7->text());
}

void MainWindow::on_action_recent8_triggered()
{
    OpenFile(ui->action_recent8->text());
}

void MainWindow::on_action_recent9_triggered()
{
    OpenFile(ui->action_recent9->text());
}

void MainWindow::on_action_recent10_triggered()
{
    OpenFile(ui->action_recent10->text());
}




//Copy
void MainWindow::on_actionCopy_triggered()
{
    int q1=0, q2=0, q3=0;
    if (activeChildWindow()==1) //if active window is a levelEditor
    {
       LvlBuffer=activeLvlEditWin()->scene->copy();
       q1 += LvlBuffer.blocks.size();
       q2 += LvlBuffer.bgo.size();
       q3 += LvlBuffer.npc.size();
       statusBar()->showMessage(tr("%1 blocks, %2 BGO, %3 NPC items are copied in clipboard").arg(q1).arg(q2).arg(q3), 2000);
    }

}

