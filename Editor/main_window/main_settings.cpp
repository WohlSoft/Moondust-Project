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


#include "../ui_mainwindow.h"
#include "../mainwindow.h"
#include "../common_features/logger_sets.h"

void MainWindow::setDefaults()
{
    MusicPlayer = new QMediaPlayer;

    LvlOpts.animationEnabled = true;
    LvlOpts.collisionsEnabled = true;

    LastOpenDir = ".";
    lastWinType=0;
    LevelToolBoxVis = true; //Level toolbox
    SectionToolBoxVis = false; //Section Settings
    LevelDoorsBoxVis = false; //Doors box
    LevelLayersBoxVis = false; //Layers box

    WorldToolBoxVis = false;
    autoPlayMusic = false;

    currentCustomMusic = "";
    currentMusicId = 0;
    musicButtonChecked = false;

    animatorItemsLimit=10000;

    cat_blocks="[all]";
    cat_bgos="[all]";
    cat_npcs="[all]";
}

void MainWindow::setUiDefults()
{
    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/images/mac/mushroom.icns"));
    #endif

    //Applay objects into tools
    setTools();
    setItemBoxes();


    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)),
        this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
        this, SLOT(setActiveSubWindow(QWidget*)));

    loadSettings();

    ui->actionPlayMusic->setChecked(autoPlayMusic);

    ui->centralWidget->cascadeSubWindows();
    ui->WorldToolBox->hide();

    ui->LevelSectionSettings->hide();

    ui->applyResize->setVisible(false);
    ui->cancelResize->setVisible(false);

    ui->LevelToolBox->hide();
    ui->DoorsToolbox->hide();
    ui->LevelLayers->hide();
    ui->ItemProperties->hide();

    ui->menuView->setEnabled(0);

    ui->menuWindow->setEnabled(1);

    ui->menuLevel->setEnabled(0);
    ui->menuWorld->setEnabled(0);
    ui->LevelObjectToolbar->setVisible(0);

    ui->actionLVLToolBox->setVisible(0);
    ui->actionWarpsAndDoors->setVisible(0);
    ui->actionSection_Settings->setVisible(0);
    ui->actionWarpsAndDoors->setVisible(0);
    ui->actionWLDToolBox->setVisible(0);
    ui->actionGridEn->setChecked(1);

    setAcceptDrops(true);
    ui->centralWidget->cascadeSubWindows();


    //Start event detector
    TickTackLock = false;

    //set timer for event detector loop
    TickTackTimer = new QTimer(this);
    connect(
            TickTackTimer, SIGNAL(timeout()),
            this,
            SLOT( TickTack() ) );

    //start event detection loop
    TickTackTimer->start(1);
}


//////////Load settings from INI file///////////////
void MainWindow::loadSettings()
{
    QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
        LastOpenDir = settings.value("lastpath", ".").toString();
        LevelToolBoxVis = settings.value("level-tb-visible", "true").toBool();
        WorldToolBoxVis = settings.value("world-tb-visible", "true").toBool();
        SectionToolBoxVis = settings.value("section-tb-visible", "false").toBool();
        LevelDoorsBoxVis = settings.value("level-doors-vis", "false").toBool();
        LevelLayersBoxVis = settings.value("level-layers-vis", "false").toBool();

        LvlOpts.animationEnabled = settings.value("animation", "true").toBool();
        LvlOpts.collisionsEnabled = settings.value("collisions", "true").toBool();
        restoreGeometry(settings.value("geometry", saveGeometry() ).toByteArray());
        restoreState(settings.value("windowState", saveState() ).toByteArray());
        autoPlayMusic = settings.value("autoPlayMusic", false).toBool();

        animatorItemsLimit = settings.value("animation-item-limit", "10000").toInt();

    settings.endGroup();

    settings.beginGroup("Recent");
        for(int i = 1; i<=10;i++){
            recentOpen.push_back(settings.value("recent"+QString::number(i),"<empty>").toString());
        }
        SyncRecentFiles();
    settings.endGroup();
}

//////////Save settings into INI file///////////////
void MainWindow::saveSettings()
{
    QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";

    QSettings settings(inifile, QSettings::IniFormat);
    settings.beginGroup("Main");
    settings.setValue("pos", pos());
    settings.setValue("lastpath", LastOpenDir);

    settings.setValue("level-tb-visible", LevelToolBoxVis);
    settings.setValue("world-tb-visible", WorldToolBoxVis);
    settings.setValue("section-tb-visible", SectionToolBoxVis);
    settings.setValue("level-layers-vis", LevelLayersBoxVis);
    settings.setValue("level-doors-vis", LevelDoorsBoxVis);

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("autoPlayMusic", autoPlayMusic);

    settings.setValue("animation", LvlOpts.animationEnabled);
    settings.setValue("collisions", LvlOpts.collisionsEnabled);
    settings.setValue("animation-item-limit", QString::number(animatorItemsLimit));
    settings.endGroup();

    settings.beginGroup("Recent");
    for(int i = 1; i<=10;i++){
        settings.setValue("recent"+QString::number(i),recentOpen[i-1]);
    }
    settings.endGroup();

    settings.beginGroup("logging");
        settings.setValue("log-path", LogWriter::DebugLogFile);

        if(LogWriter::enabled)
            switch(LogWriter::logLevel)
            {
            case QtDebugMsg:
                settings.setValue("log-level", "4");
            case QtWarningMsg:
                settings.setValue("log-level", "3");
            case QtCriticalMsg:
                settings.setValue("log-level", "2");
            case QtFatalMsg:
                settings.setValue("log-level", "1");
            }
        else
            settings.setValue("log-level", "0");

    settings.endGroup();
}
