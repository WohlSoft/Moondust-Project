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
long GlobalSettings::animatorItemsLimit=25000;
QString GlobalSettings::openPath=".";
QString GlobalSettings::savePath=".";
QString GlobalSettings::savePath_npctxt=".";

LevelEditingSettings GlobalSettings::LvlOpts;

bool GlobalSettings::autoPlayMusic=false;

bool GlobalSettings::LevelToolBoxVis=true;
bool GlobalSettings::WorldToolBoxVis=true;

bool GlobalSettings::SectionToolBoxVis=false;
bool GlobalSettings::LevelDoorsBoxVis=false;
bool GlobalSettings::LevelLayersBoxVis=false;
bool GlobalSettings::LevelEventsBoxVis=false;
bool GlobalSettings::LevelSearchBoxVis=false;

QMdiArea::ViewMode GlobalSettings::MainWindowView = QMdiArea::TabbedView;

int GlobalSettings::lastWinType=0;

QString LvlMusPlay::currentCustomMusic;
long LvlMusPlay::currentMusicId;
bool LvlMusPlay::musicButtonChecked;
bool LvlMusPlay::musicForceReset=false;

void MainWindow::setDefaults()
{
    setPointer();

    MusicPlayer = new QMediaPlayer;

    GlobalSettings::LvlOpts.animationEnabled = true;
    GlobalSettings::LvlOpts.collisionsEnabled = true;

//    LastOpenDir = ".";
//    lastWinType=0;
//    LevelToolBoxVis = true; //Level toolbox
//    SectionToolBoxVis = false; //Section Settings
//    LevelDoorsBoxVis = false; //Doors box
//    LevelLayersBoxVis = false; //Layers box
//    LevelEventsBoxVis = false; //Events box

    LvlItemPropsLock=true;

//    WorldToolBoxVis = false;
//    autoPlayMusic = false;

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

    //Apply objects into tools
    setLevelSectionData();
    setItemBoxes();
    setSoundList();

    loadSettings();

    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)),
        this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
        this, SLOT(setActiveSubWindow(QWidget*)));

    ui->actionPlayMusic->setChecked(GlobalSettings::autoPlayMusic);

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
    ui->FindDock->hide();

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

    ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
    ui->centralWidget->setTabsClosable(true);

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

    curSearchBlock.id = 0;
    curSearchBlock.index = 0;
    curSearchBlock.npc_id = 0;

    curSearchBGO.id = 0;
    curSearchBGO.index = 0;

    curSearchNPC.id = 0;
    curSearchNPC.index = 0;

    connect(ui->LvlLayerList->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(DragAndDroppedLayer(QModelIndex,int,int,QModelIndex,int)));
    connect(ui->LVLEvents_List->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(DragAndDroppedEvent(QModelIndex,int,int,QModelIndex,int)));
    connect(ui->Find_Check_TypeBlock, SIGNAL(toggled(bool)), ui->Find_Button_TypeBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeBGO, SIGNAL(toggled(bool)), ui->Find_Button_TypeBGO, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeNPC, SIGNAL(toggled(bool)), ui->Find_Button_TypeNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_PriorityBGO, SIGNAL(toggled(bool)), ui->Find_Spin_PriorityBGO, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LayerBlock, SIGNAL(toggled(bool)), ui->Find_Combo_LayerBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LayerBGO, SIGNAL(toggled(bool)), ui->Find_Combo_LayerBGO, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LayerNPC, SIGNAL(toggled(bool)), ui->Find_Combo_LayerNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_InvisibleBlock, SIGNAL(toggled(bool)), ui->Find_Check_InvisibleActiveBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_SlipperyBlock, SIGNAL(toggled(bool)), ui->Find_Check_SlipperyActiveBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_ContainsNPCBlock, SIGNAL(toggled(bool)), ui->Find_Button_ContainsNPCBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_EventDestoryedBlock, SIGNAL(toggled(bool)), ui->Find_Combo_EventDestoryedBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_EventHitedBlock, SIGNAL(toggled(bool)), ui->Find_Combo_EventHitedBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_EventLayerEmptyBlock, SIGNAL(toggled(bool)), ui->Find_Combo_EventLayerEmptyBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_DirNPC, SIGNAL(toggled(bool)), ui->Find_Radio_DirLeftNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_DirNPC, SIGNAL(toggled(bool)), ui->Find_Radio_DirRandomNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_DirNPC, SIGNAL(toggled(bool)), ui->Find_Radio_DirRightNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_FriendlyNPC, SIGNAL(toggled(bool)), ui->Find_Check_FriendlyActiveNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_NotMoveNPC, SIGNAL(toggled(bool)), ui->Find_Check_NotMoveActiveNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_BossNPC, SIGNAL(toggled(bool)), ui->Find_Check_BossActiveNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_MsgNPC, SIGNAL(toggled(bool)), ui->Find_Edit_MsgNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_MsgNPC, SIGNAL(toggled(bool)), ui->Find_Check_MsgSensitiveNPC, SLOT(setEnabled(bool)));


    //reset if modify
    connect(ui->Find_Button_TypeBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Button_TypeBGO, SIGNAL(clicked()), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Button_TypeNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Spin_PriorityBGO, SIGNAL(valueChanged(int)), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Combo_LayerBlock, SIGNAL(activated(int)), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Combo_LayerBGO, SIGNAL(activated(int)), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Combo_LayerNPC, SIGNAL(activated(int)), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_InvisibleActiveBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_SlipperyActiveBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Button_ContainsNPCBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Combo_EventDestoryedBlock, SIGNAL(activated(int)), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Combo_EventHitedBlock, SIGNAL(activated(int)), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Combo_EventLayerEmptyBlock, SIGNAL(activated(int)), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Radio_DirLeftNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Radio_DirRandomNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Radio_DirRightNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_FriendlyActiveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_NotMoveActiveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_BossActiveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Edit_MsgNPC, SIGNAL(textEdited(QString)), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_MsgSensitiveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));

    //also checkboxes
    connect(ui->Find_Check_TypeBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_LayerBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_InvisibleBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_SlipperyBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_ContainsNPCBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_EventDestoryedBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_EventLayerEmptyBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));

    connect(ui->Find_Check_TypeBGO, SIGNAL(clicked()), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Check_LayerBGO, SIGNAL(clicked()), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Check_PriorityBGO, SIGNAL(clicked()), this, SLOT(resetBGOSearch()));

    connect(ui->Find_Check_TypeNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_LayerNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_DirNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_FriendlyNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_NotMoveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_BossNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_MsgNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));

    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(toggleNewWindow(QMdiSubWindow*)));

    updateWindowMenu();
}


//////////Load settings from INI file///////////////
void MainWindow::loadSettings()
{
    QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
        //GlobalSettings::LastOpenDir = settings.value("lastpath", ".").toString();
        GlobalSettings::openPath = settings.value("lastpath", ".").toString();
        GlobalSettings::savePath = settings.value("lastsavepath", ".").toString();
        GlobalSettings::savePath_npctxt = settings.value("lastsavepath-npctxt", ".").toString();

        GlobalSettings::LevelToolBoxVis = settings.value("level-tb-visible", "true").toBool();
        GlobalSettings::WorldToolBoxVis = settings.value("world-tb-visible", "true").toBool();
        GlobalSettings::SectionToolBoxVis = settings.value("section-tb-visible", "false").toBool();
        GlobalSettings::LevelDoorsBoxVis = settings.value("level-doors-vis", "false").toBool();
        GlobalSettings::LevelLayersBoxVis = settings.value("level-layers-vis", "false").toBool();
        GlobalSettings::LevelEventsBoxVis = settings.value("level-events-vis", "false").toBool();
        GlobalSettings::LevelSearchBoxVis = settings.value("level-search-vis", "false").toBool();

        GlobalSettings::LvlOpts.animationEnabled = settings.value("animation", "true").toBool();
        GlobalSettings::LvlOpts.collisionsEnabled = settings.value("collisions", "true").toBool();
        restoreGeometry(settings.value("geometry", saveGeometry() ).toByteArray());
        restoreState(settings.value("windowState", saveState() ).toByteArray());
        GlobalSettings::autoPlayMusic = settings.value("autoPlayMusic", false).toBool();

        GlobalSettings::MainWindowView = (settings.value("tab-view", true).toBool()) ? QMdiArea::TabbedView : QMdiArea::SubWindowView;

        ui->DoorsToolbox->setFloating(settings.value("doors-tool-box-float", true).toBool());
        ui->LevelSectionSettings->setFloating(settings.value("level-section-set-float", true).toBool());
        ui->LevelLayers->setFloating(settings.value("level-layers-float", true).toBool());
        ui->LevelEventsToolBox->setFloating(settings.value("level-events-float", true).toBool());
        ui->ItemProperties->setFloating(settings.value("item-props-box-float", true).toBool());
        ui->FindDock->setFloating(settings.value("level-search-float", true).toBool());

        ui->DoorsToolbox->restoreGeometry(settings.value("doors-tool-box-geometry", ui->DoorsToolbox->saveGeometry()).toByteArray());
        ui->LevelSectionSettings->restoreGeometry(settings.value("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry()).toByteArray());
        ui->LevelLayers->restoreGeometry(settings.value("level-layers-geometry", ui->LevelLayers->saveGeometry()).toByteArray());
        ui->LevelEventsToolBox->restoreGeometry(settings.value("level-events-geometry", ui->LevelLayers->saveGeometry()).toByteArray());
        ui->ItemProperties->restoreGeometry(settings.value("item-props-box-geometry", ui->ItemProperties->saveGeometry()).toByteArray());
        ui->FindDock->restoreGeometry(settings.value("level-search-geometry", ui->FindDock->saveGeometry()).toByteArray());

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
    settings.setValue("lastpath", GlobalSettings::openPath);
    settings.setValue("lastsavepath", GlobalSettings::savePath);
    settings.setValue("lastsavepath-npctxt", GlobalSettings::savePath_npctxt);

    settings.setValue("world-tb-visible", GlobalSettings::WorldToolBoxVis);

    settings.setValue("level-tb-visible", GlobalSettings::LevelToolBoxVis);
    settings.setValue("section-tb-visible", GlobalSettings::SectionToolBoxVis);
    settings.setValue("level-layers-vis", GlobalSettings::LevelLayersBoxVis);
    settings.setValue("level-events-vis", GlobalSettings::LevelEventsBoxVis);
    settings.setValue("level-doors-vis", GlobalSettings::LevelDoorsBoxVis);
    settings.setValue("level-search-vis", GlobalSettings::LevelSearchBoxVis);

    settings.setValue("doors-tool-box-float", ui->DoorsToolbox->isFloating());
    settings.setValue("level-section-set-float", ui->LevelSectionSettings->isFloating());
    settings.setValue("level-layers-float", ui->LevelLayers->isFloating());
    settings.setValue("level-events-float", ui->LevelEventsToolBox->isFloating());
    settings.setValue("item-props-box-float", ui->ItemProperties->isFloating());
    settings.setValue("level-search-float", ui->FindDock->isFloating());

    settings.setValue("doors-tool-box-geometry", ui->DoorsToolbox->saveGeometry());
    settings.setValue("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry());
    settings.setValue("level-layers-geometry", ui->LevelLayers->saveGeometry());
    settings.setValue("level-events-geometry", ui->LevelEventsToolBox->saveGeometry());
    settings.setValue("item-props-box-geometry", ui->ItemProperties->saveGeometry());
    settings.setValue("level-search-geometry", ui->FindDock->saveGeometry());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("autoPlayMusic", GlobalSettings::autoPlayMusic);

    settings.setValue("tab-view", (GlobalSettings::MainWindowView==QMdiArea::TabbedView));

    settings.setValue("animation", GlobalSettings::LvlOpts.animationEnabled);
    settings.setValue("collisions", GlobalSettings::LvlOpts.collisionsEnabled);
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

    appSettings->autoPlayMusic = GlobalSettings::autoPlayMusic;
    appSettings->Animation = GlobalSettings::LvlOpts.animationEnabled;
    appSettings->Collisions = GlobalSettings::LvlOpts.collisionsEnabled;

    appSettings->AnimationItemLimit = GlobalSettings::animatorItemsLimit;

    appSettings->MainWindowView = GlobalSettings::MainWindowView;

    appSettings->applySettings();

    if(appSettings->exec()==QDialog::Accepted)
    {
        GlobalSettings::autoPlayMusic = appSettings->autoPlayMusic;
        GlobalSettings::animatorItemsLimit = appSettings->AnimationItemLimit;
        GlobalSettings::LvlOpts.animationEnabled = appSettings->Animation;
        GlobalSettings::LvlOpts.collisionsEnabled = appSettings->Collisions;

        ui->actionAnimation->setChecked(GlobalSettings::LvlOpts.animationEnabled);
        on_actionAnimation_triggered(GlobalSettings::LvlOpts.animationEnabled);
        ui->actionCollisions->setChecked(GlobalSettings::LvlOpts.collisionsEnabled);
        on_actionCollisions_triggered(GlobalSettings::LvlOpts.collisionsEnabled);

        GlobalSettings::MainWindowView = appSettings->MainWindowView;

        ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);

        saveSettings();
    }
    delete appSettings;

}
