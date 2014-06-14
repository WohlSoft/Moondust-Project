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

#include "../ui_mainwindow.h"
#include "../mainwindow.h"
#include "../common_features/logger_sets.h"

#include "appsettings.h"

#include "music_player.h"
#include "global_settings.h"

QString GlobalSettings::locale="";
long GlobalSettings::animatorItemsLimit=10000;

QString LvlMusPlay::currentCustomMusic;
long LvlMusPlay::currentMusicId;
bool LvlMusPlay::musicButtonChecked;
bool LvlMusPlay::musicForceReset=false;


void MainWindow::setDefaults()
{
    setPointer();

    MusicPlayer = new QMediaPlayer;

    LvlOpts.animationEnabled = true;
    LvlOpts.collisionsEnabled = true;

    LastOpenDir = ".";
    lastWinType=0;
    LevelToolBoxVis = true; //Level toolbox
    SectionToolBoxVis = false; //Section Settings
    LevelDoorsBoxVis = false; //Doors box
    LevelLayersBoxVis = false; //Layers box
    LevelEventsBoxVis = false; //Events box

    WorldToolBoxVis = false;
    autoPlayMusic = false;

    LvlMusPlay::currentCustomMusic = "";
    LvlMusPlay::currentMusicId = 0;
    LvlMusPlay::musicButtonChecked = false;

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
    setLevelSectionData();
    setItemBoxes();

    loadSettings();

    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)),
        this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
        this, SLOT(setActiveSubWindow(QWidget*)));

    ui->actionPlayMusic->setChecked(autoPlayMusic);

    ui->centralWidget->cascadeSubWindows();

    ui->applyResize->setVisible(false);
    ui->cancelResize->setVisible(false);


    ui->WorldToolBox->hide();
    ui->LevelToolBox->hide();


    ui->DoorsToolbox->hide();
    ui->LevelLayers->hide();
    ui->LevelEventsToolBox->hide();
    ui->LevelSectionSettings->hide();

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


//    //Start event detector
//    TickTackLock = false;

//    //set timer for event detector loop
//    TickTackTimer = new QTimer(this);
//    connect(
//            TickTackTimer, SIGNAL(timeout()),
//            this,
//            SLOT( TickTack() ) );

//    //start event detection loop
//    TickTackTimer->start(1);

    connect(ui->LvlLayerList->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(DragAndDroppedLayer(QModelIndex,int,int,QModelIndex,int)));
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
        LevelEventsBoxVis = settings.value("level-events-vis", "false").toBool();

        LvlOpts.animationEnabled = settings.value("animation", "true").toBool();
        LvlOpts.collisionsEnabled = settings.value("collisions", "true").toBool();
        restoreGeometry(settings.value("geometry", saveGeometry() ).toByteArray());
        restoreState(settings.value("windowState", saveState() ).toByteArray());
        autoPlayMusic = settings.value("autoPlayMusic", false).toBool();

        ui->DoorsToolbox->setFloating(settings.value("doors-tool-box-float", true).toBool());
        ui->LevelSectionSettings->setFloating(settings.value("level-section-set-float", true).toBool());
        ui->LevelLayers->setFloating(settings.value("level-layers-float", true).toBool());
        ui->LevelEventsToolBox->setFloating(settings.value("level-events-float", true).toBool());
        ui->ItemProperties->setFloating(settings.value("item-props-box-float", true).toBool());

        ui->DoorsToolbox->restoreGeometry(settings.value("doors-tool-box-geometry", ui->DoorsToolbox->saveGeometry()).toByteArray());
        ui->LevelSectionSettings->restoreGeometry(settings.value("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry()).toByteArray());
        ui->LevelLayers->restoreGeometry(settings.value("level-layers-geometry", ui->LevelLayers->saveGeometry()).toByteArray());
        ui->LevelEventsToolBox->restoreGeometry(settings.value("level-events-geometry", ui->LevelLayers->saveGeometry()).toByteArray());
        ui->ItemProperties->restoreGeometry(settings.value("item-props-box-geometry", ui->ItemProperties->saveGeometry()).toByteArray());

        GlobalSettings::animatorItemsLimit = settings.value("animation-item-limit", "10000").toInt();

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
    settings.setValue("level-events-vis", LevelEventsBoxVis);
    settings.setValue("level-doors-vis", LevelDoorsBoxVis);

    settings.setValue("doors-tool-box-float", ui->DoorsToolbox->isFloating());
    settings.setValue("level-section-set-float", ui->LevelSectionSettings->isFloating());
    settings.setValue("level-layers-float", ui->LevelLayers->isFloating());
    settings.setValue("level-events-float", ui->LevelEventsToolBox->isFloating());
    settings.setValue("item-props-box-float", ui->ItemProperties->isFloating());

    settings.setValue("doors-tool-box-geometry", ui->DoorsToolbox->saveGeometry());
    settings.setValue("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry());
    settings.setValue("level-layers-geometry", ui->LevelLayers->saveGeometry());
    settings.setValue("level-events-geometry", ui->LevelEventsToolBox->saveGeometry());
    settings.setValue("item-props-box-geometry", ui->ItemProperties->saveGeometry());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("autoPlayMusic", autoPlayMusic);

    settings.setValue("animation", LvlOpts.animationEnabled);
    settings.setValue("collisions", LvlOpts.collisionsEnabled);
    settings.setValue("animation-item-limit", QString::number(GlobalSettings::animatorItemsLimit));

    settings.setValue("language", GlobalSettings::locale);

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
                settings.setValue("log-level", "4"); break;
            case QtWarningMsg:
                settings.setValue("log-level", "3"); break;
            case QtCriticalMsg:
                settings.setValue("log-level", "2"); break;
            case QtFatalMsg:
                settings.setValue("log-level", "1"); break;
            }
        else
            settings.setValue("log-level", "0");
    settings.endGroup();
}


//Application settings
void MainWindow::on_actionApplication_settings_triggered()
{
    AppSettings * appSettings = new AppSettings;
    appSettings->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    appSettings->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, appSettings->size(), qApp->desktop()->availableGeometry()));

    appSettings->autoPlayMusic = autoPlayMusic;
    appSettings->Animation = LvlOpts.animationEnabled;
    appSettings->Collisions = LvlOpts.collisionsEnabled;

    appSettings->AnimationItemLimit = GlobalSettings::animatorItemsLimit;

    appSettings->applySettings();

    if(appSettings->exec()==QDialog::Accepted)
    {
        autoPlayMusic = appSettings->autoPlayMusic;
        GlobalSettings::animatorItemsLimit = appSettings->AnimationItemLimit;
        LvlOpts.animationEnabled = appSettings->Animation;
        LvlOpts.collisionsEnabled = appSettings->Collisions;

        ui->actionAnimation->setChecked(LvlOpts.animationEnabled);
        on_actionAnimation_triggered(LvlOpts.animationEnabled);
        ui->actionCollisions->setChecked(LvlOpts.collisionsEnabled);
        on_actionCollisions_triggered(LvlOpts.collisionsEnabled);

        saveSettings();
    }

}
