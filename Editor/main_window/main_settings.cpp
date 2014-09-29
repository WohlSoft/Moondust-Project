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
#include "../common_features/app_path.h"

#include "../common_features/logger_sets.h"
#include "../common_features/sdl_music_player.h"
#include "../common_features/themes.h"

#include "appsettings.h"

#include "music_player.h"
#include "global_settings.h"

#include <QFont>

QString GlobalSettings::locale="";
long GlobalSettings::animatorItemsLimit=25000;
QString GlobalSettings::openPath=".";
QString GlobalSettings::savePath=".";
QString GlobalSettings::savePath_npctxt=".";

LevelEditingSettings GlobalSettings::LvlOpts;

bool GlobalSettings::autoPlayMusic=false;
int GlobalSettings::musicVolume=100;

bool GlobalSettings::LevelToolBoxVis=true;
bool GlobalSettings::WorldToolBoxVis=true;
bool GlobalSettings::WorldSettingsToolboxVis=false;
bool GlobalSettings::WorldSearchBoxVis=false;

bool GlobalSettings::SectionToolBoxVis=false;
bool GlobalSettings::LevelDoorsBoxVis=false;
bool GlobalSettings::LevelLayersBoxVis=false;
bool GlobalSettings::LevelEventsBoxVis=false;
bool GlobalSettings::LevelSearchBoxVis=false;

bool GlobalSettings::TilesetBoxVis=false;
bool GlobalSettings::DebuggerBoxVis=false;


bool GlobalSettings::MidMouse_allowDuplicate=false;
bool GlobalSettings::MidMouse_allowSwitchToPlace=false;
bool GlobalSettings::MidMouse_allowSwitchToDrag=false;

QMdiArea::ViewMode GlobalSettings::MainWindowView = QMdiArea::TabbedView;
QTabWidget::TabPosition GlobalSettings::LVLToolboxPos = QTabWidget::North;
QTabWidget::TabPosition GlobalSettings::WLDToolboxPos = QTabWidget::West;

int GlobalSettings::lastWinType=0;

QString LvlMusPlay::currentCustomMusic;
long LvlMusPlay::currentMusicId=0;
long LvlMusPlay::currentWldMusicId=0;
long LvlMusPlay::currentSpcMusicId=0;
bool LvlMusPlay::musicButtonChecked;
bool LvlMusPlay::musicForceReset=false;
int LvlMusPlay::musicType=LvlMusPlay::LevelMusic;

PGE_MusPlayer MusPlayer;

void MainWindow::setDefaults()
{
    setPointer();

    GlobalSettings::LvlOpts.animationEnabled = true;
    GlobalSettings::LvlOpts.collisionsEnabled = true;
    GlobalSettings::LvlOpts.semiTransparentPaths = false;

    LvlItemPropsLock=true;
    lockTilesetBox=false;

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
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif

    //Apply objects into tools
    setLevelSectionData();
    setLvlItemBoxes();
    setWldItemBoxes();

    setSoundList();

    applyTheme(Themes::currentTheme().isEmpty() ? ConfStatus::defaultTheme : Themes::currentTheme());

    WldLvlExitTypeListReset();

    //MainWindow Geometry;
    QRect mwg = this->geometry();

    int GOffset=240;
    //Define the default geometry for toolboxes
    ui->DoorsToolbox->setGeometry(
                mwg.x()+mwg.width()-ui->DoorsToolbox->width()-GOffset,
                mwg.y()+120,
                ui->DoorsToolbox->width(),
                ui->DoorsToolbox->height()
                );

    ui->LevelSectionSettings->setGeometry(
                mwg.x()+mwg.width()-ui->LevelSectionSettings->width()-GOffset,
                mwg.y()+120,
                ui->LevelSectionSettings->width(),
                ui->LevelSectionSettings->height()
                );
    ui->LevelLayers->setGeometry(
                mwg.x()+mwg.width()-ui->LevelLayers->width()-GOffset,
                mwg.y()+120,
                ui->LevelLayers->width(),
                ui->LevelLayers->height()
                );
    ui->LevelEventsToolBox->setGeometry(
                mwg.x()+mwg.width()-ui->LevelEventsToolBox->width()-GOffset,
                mwg.y()+120,
                ui->LevelEventsToolBox->width(),
                ui->LevelEventsToolBox->height()
                );
    ui->ItemProperties->setGeometry(
                mwg.x()+mwg.width()-ui->ItemProperties->width()-GOffset,
                mwg.y()+120,
                ui->ItemProperties->width(),
                ui->ItemProperties->height()
                );
    ui->FindDock->setGeometry(
                mwg.x()+mwg.width()-ui->FindDock->width()-GOffset,
                mwg.y()+120,
                ui->FindDock->width(),
                ui->FindDock->height()
                );

    ui->WLD_ItemProps->setGeometry(
                mwg.x()+mwg.width()-ui->WLD_ItemProps->width()-GOffset,
                mwg.y()+120,
                ui->WLD_ItemProps->width(),
                ui->WLD_ItemProps->height()
                );

    ui->WorldSettings->setGeometry(
                mwg.x()+mwg.width()-ui->WorldSettings->width()-GOffset-200,
                mwg.y()+120,
                ui->WorldSettings->width(),
                ui->WorldSettings->height()
                );

    ui->WorldFindDock->setGeometry(
                mwg.x()+mwg.width()-ui->WorldFindDock->width()-GOffset,
                mwg.y()+120,
                ui->WorldFindDock->width(),
                ui->WorldFindDock->height()
                );

    ui->Tileset_Item_Box->setGeometry(
                mwg.x()+GOffset,
                mwg.y()+mwg.height()-600,
                800,
                300
                );

    ui->debuggerBox->setGeometry(
                mwg.x()+mwg.width()-ui->debuggerBox->width()-GOffset,
                mwg.y()+120,
                ui->debuggerBox->width(),
                ui->debuggerBox->height()
                );

    ui->Tileset_Item_Box->hide();

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setWeight(8);
    ui->DEBUG_Items->setFont(font);


    loadSettings();

    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)),
        this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
        this, SLOT(setActiveSubWindow(QWidget*)));

    ui->actionPlayMusic->setChecked(GlobalSettings::autoPlayMusic);

    ui->actionExport_to_image_section->setVisible(false);

    ui->centralWidget->cascadeSubWindows();

    ui->applyResize->setVisible(false);
    ui->cancelResize->setVisible(false);

    ui->ResizingToolbar->setVisible(false);
    ui->PlacingToolbar->setVisible(false);

    ui->LevelToolBox->hide();


    ui->DoorsToolbox->hide();
    ui->LevelLayers->hide();
    ui->LevelEventsToolBox->hide();
    ui->LevelSectionSettings->hide();

    ui->ItemProperties->hide();
    ui->FindDock->hide();

    ui->WorldToolBox->hide();
    ui->WorldSettings->hide();
    ui->WLD_ItemProps->hide();
    ui->WorldFindDock->hide();

    ui->Tileset_Item_Box->hide();
    ui->debuggerBox->hide();

    ui->menuView->setEnabled(false);

    ui->menuWindow->setEnabled(true);

    ui->menuLevel->setEnabled(false);
    ui->menuWorld->setEnabled(false);
    ui->LevelObjectToolbar->setVisible(false);
    ui->WorldObjectToolbar->setVisible(false);

    ui->actionLVLToolBox->setVisible(false);
    ui->actionWarpsAndDoors->setVisible(false);
    ui->actionSection_Settings->setVisible(false);
    ui->actionWarpsAndDoors->setVisible(false);
    ui->actionWLDToolBox->setVisible(false);
    ui->actionGridEn->setChecked(true);

    ui->actionZoomReset->setEnabled(false);
    ui->actionZoomIn->setEnabled(false);
    ui->actionZoomOut->setEnabled(false);

    setAcceptDrops(true);
    ui->centralWidget->cascadeSubWindows();

    ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
    ui->LevelToolBoxTabs->setTabPosition(GlobalSettings::LVLToolboxPos);
    ui->WorldToolBoxTabs->setTabPosition(GlobalSettings::WLDToolboxPos);
    ui->centralWidget->setTabsClosable(true);

    muVol = new QSlider(Qt::Horizontal);
    muVol->setMaximumWidth(70);
    muVol->setMinimumWidth(70);
    muVol->setMinimum(0);
    muVol->setMaximum(MIX_MAX_VOLUME);
    muVol->setValue(GlobalSettings::musicVolume);

    MusPlayer.setVolume(muVol->value());
    ui->EditionToolBar->insertWidget(ui->actionAnimation, muVol);
    ui->EditionToolBar->insertSeparator(ui->actionAnimation);

    zoom = new QLineEdit();
    zoom->setValidator(new QIntValidator(0,2001));
    zoom->setText("100");
    zoom->setMaximumWidth(50);
    zoom->setEnabled(false);

    ui->LevelSectionsToolBar->insertWidget(ui->actionZoomReset,zoom);
    connect(zoom, SIGNAL(editingFinished()), this, SLOT(applyTextZoom()));

    connect(muVol, SIGNAL(valueChanged(int)), &MusPlayer, SLOT(setVolume(int)));

    curSearchBlock.id = 0;
    curSearchBlock.index = 0;
    curSearchBlock.npc_id = 0;

    curSearchBGO.id = 0;
    curSearchBGO.index = 0;

    curSearchNPC.id = 0;
    curSearchNPC.index = 0;

    curSearchTile.id = 0;
    curSearchTile.index = 0;

    curSearchScenery.id = 0;
    curSearchScenery.index = 0;

    curSearchPath.id = 0;
    curSearchPath.index = 0;

    curSearchLevel.id = 0;
    curSearchLevel.index = 0;

    curSearchMusic.id = 0;
    curSearchMusic.index = 0;

    connect(ui->LvlLayerList->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(DragAndDroppedLayer(QModelIndex,int,int,QModelIndex,int)));
    connect(ui->LVLEvents_List->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(DragAndDroppedEvent(QModelIndex,int,int,QModelIndex,int)));
    //enable & disable
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

    //for world search
    connect(ui->Find_Check_TypeLevel, SIGNAL(toggled(bool)), ui->Find_Button_TypeLevel, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeTile, SIGNAL(toggled(bool)), ui->Find_Button_TypeTile, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeScenery, SIGNAL(toggled(bool)), ui->Find_Button_TypeScenery, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypePath, SIGNAL(toggled(bool)), ui->Find_Button_TypePath, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeMusic, SIGNAL(toggled(bool)), ui->Find_Button_TypeMusic, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_PathBackground, SIGNAL(toggled(bool)), ui->Find_Check_PathBackgroundActive, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_BigPathBackground, SIGNAL(toggled(bool)), ui->Find_Check_BigPathBackgroundActive, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_AlwaysVisible, SIGNAL(toggled(bool)), ui->Find_Check_AlwaysVisibleActive, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_GameStartPoint, SIGNAL(toggled(bool)), ui->Find_Check_GameStartPointActive, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LevelFile, SIGNAL(toggled(bool)), ui->Find_Edit_LevelFile, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LevelFile, SIGNAL(toggled(bool)), ui->Find_Button_LevelFile, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_ContainsTitle, SIGNAL(toggled(bool)), ui->Find_Edit_ContainsTitle, SLOT(setEnabled(bool)));

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

    //for world
    connect(ui->Find_Button_TypeTile, SIGNAL(clicked()), this, SLOT(resetTileSearch()));
    connect(ui->Find_Button_TypeScenery, SIGNAL(clicked()), this, SLOT(resetScenerySearch()));
    connect(ui->Find_Button_TypePath, SIGNAL(clicked()), this, SLOT(resetPathSearch()));
    connect(ui->Find_Button_TypeLevel, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Button_TypeMusic, SIGNAL(clicked()), this, SLOT(resetMusicSearch()));
    connect(ui->Find_Check_PathBackgroundActive, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_BigPathBackgroundActive, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_AlwaysVisibleActive, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_GameStartPointActive, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Edit_LevelFile, SIGNAL(textEdited(QString)), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Button_LevelFile, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Edit_ContainsTitle, SIGNAL(textEdited(QString)), this, SLOT(resetLevelSearch()));

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

    //for world
    connect(ui->Find_Check_TypeTile, SIGNAL(clicked()), this, SLOT(resetTileSearch()));
    connect(ui->Find_Check_TypeScenery, SIGNAL(clicked()), this, SLOT(resetScenerySearch()));
    connect(ui->Find_Check_TypePath, SIGNAL(clicked()), this, SLOT(resetPathSearch()));
    connect(ui->Find_Check_TypeLevel, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_TypeMusic, SIGNAL(clicked()), this, SLOT(resetMusicSearch()));
    connect(ui->Find_Check_PathBackground, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_BigPathBackground, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_AlwaysVisible, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_GameStartPoint, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_LevelFile, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_ContainsTitle, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));

    connect(ui->Find_Button_LevelFile, SIGNAL(clicked()), this, SLOT(selectLevelForSearch()));
    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(toggleNewWindowLVL(QMdiSubWindow*)));
    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(toggleNewWindowWLD(QMdiSubWindow*)));

    //for tileset dock
    //connect(ui->TileSetsCategories, SIGNAL(currentChanged(int)), this, SLOT(makeCurrentTileset()));

    //for tileset
    connect(ui->customTilesetSearchEdit, SIGNAL(textChanged(QString)), this, SLOT(makeCurrentTileset()));

    updateWindowMenu();
}


//////////Load settings from INI file///////////////
void MainWindow::loadSettings()
{
    QString inifile = ApplicationPath + "/" + "pge_editor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
        //GlobalSettings::LastOpenDir = settings.value("lastpath", ".").toString();
        GlobalSettings::openPath = settings.value("lastpath", ".").toString();
        GlobalSettings::savePath = settings.value("lastsavepath", ".").toString();
        GlobalSettings::savePath_npctxt = settings.value("lastsavepath-npctxt", ".").toString();

        GlobalSettings::LevelToolBoxVis = settings.value("level-tb-visible", "true").toBool();
        GlobalSettings::SectionToolBoxVis = settings.value("section-tb-visible", "false").toBool();
        GlobalSettings::LevelDoorsBoxVis = settings.value("level-doors-vis", "false").toBool();
        GlobalSettings::LevelLayersBoxVis = settings.value("level-layers-vis", "false").toBool();
        GlobalSettings::LevelEventsBoxVis = settings.value("level-events-vis", "false").toBool();
        GlobalSettings::LevelSearchBoxVis = settings.value("level-search-vis", "false").toBool();

        GlobalSettings::WorldToolBoxVis = settings.value("world-tb-visible", "true").toBool();
        GlobalSettings::WorldSettingsToolboxVis = settings.value("world-props-visible", "false").toBool();
        GlobalSettings::WorldSearchBoxVis = settings.value("world-search-visible", "false").toBool();

        GlobalSettings::TilesetBoxVis = settings.value("tileset-box-visible", "false").toBool();
        GlobalSettings::DebuggerBoxVis = settings.value("debugger-box-visible", "false").toBool();

        GlobalSettings::LvlOpts.animationEnabled = settings.value("animation", "true").toBool();
        GlobalSettings::LvlOpts.collisionsEnabled = settings.value("collisions", "true").toBool();
        restoreGeometry(settings.value("geometry", saveGeometry() ).toByteArray());
        restoreState(settings.value("windowState", saveState() ).toByteArray());
        GlobalSettings::autoPlayMusic = settings.value("autoPlayMusic", false).toBool();
        GlobalSettings::musicVolume = settings.value("music-volume",100).toInt();

        GlobalSettings::MidMouse_allowDuplicate = settings.value("editor-midmouse-allowdupe", false).toBool();
        GlobalSettings::MidMouse_allowSwitchToPlace = settings.value("editor-midmouse-allowplace", false).toBool();
        GlobalSettings::MidMouse_allowSwitchToDrag = settings.value("editor-midmouse-allowdrag", false).toBool();

        GlobalSettings::MainWindowView = (settings.value("tab-view", true).toBool()) ? QMdiArea::TabbedView : QMdiArea::SubWindowView;
        GlobalSettings::LVLToolboxPos = static_cast<QTabWidget::TabPosition>(settings.value("level-toolbox-pos", static_cast<int>(QTabWidget::North)).toInt());
        GlobalSettings::WLDToolboxPos = static_cast<QTabWidget::TabPosition>(settings.value("world-toolbox-pos", static_cast<int>(QTabWidget::West)).toInt());

        PGE_MusPlayer::setSampleRate(settings.value("sdl-sample-rate", PGE_MusPlayer::sampleRate()).toInt());

        ui->DoorsToolbox->setFloating(settings.value("doors-tool-box-float", true).toBool());
        ui->LevelSectionSettings->setFloating(settings.value("level-section-set-float", true).toBool());
        ui->LevelLayers->setFloating(settings.value("level-layers-float", true).toBool());
        ui->LevelEventsToolBox->setFloating(settings.value("level-events-float", true).toBool());
        ui->ItemProperties->setFloating(settings.value("item-props-box-float", true).toBool());
        ui->FindDock->setFloating(settings.value("level-search-float", true).toBool());
        //ui->WorldToolBox->setFloating(settings.value("world-item-box-float", false).toBool());
        ui->WorldSettings->setFloating(settings.value("world-settings-box-float", true).toBool());
        ui->WLD_ItemProps->setFloating(settings.value("world-itemprops-box-float", true).toBool());
        ui->WorldFindDock->setFloating(settings.value("world-search-float", true).toBool());
        ui->Tileset_Item_Box->setFloating(settings.value("tileset-box-float", true).toBool());
        ui->debuggerBox->setFloating(settings.value("debugger-box-float", true).toBool());

        ui->DoorsToolbox->restoreGeometry(settings.value("doors-tool-box-geometry", ui->DoorsToolbox->saveGeometry()).toByteArray());
        ui->LevelSectionSettings->restoreGeometry(settings.value("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry()).toByteArray());
        ui->LevelLayers->restoreGeometry(settings.value("level-layers-geometry", ui->LevelLayers->saveGeometry()).toByteArray());
        ui->LevelEventsToolBox->restoreGeometry(settings.value("level-events-geometry", ui->LevelLayers->saveGeometry()).toByteArray());
        ui->ItemProperties->restoreGeometry(settings.value("item-props-box-geometry", ui->ItemProperties->saveGeometry()).toByteArray());
        ui->FindDock->restoreGeometry(settings.value("level-search-geometry", ui->FindDock->saveGeometry()).toByteArray());
        ui->WorldToolBox->restoreGeometry(settings.value("world-item-box-geometry", ui->WorldToolBox->saveGeometry()).toByteArray());
        ui->WorldSettings->restoreGeometry(settings.value("world-settings-box-geometry", ui->WorldSettings->saveGeometry()).toByteArray());
        ui->WLD_ItemProps->restoreGeometry(settings.value("world-itemprops-box-geometry", ui->WLD_ItemProps->saveGeometry()).toByteArray());
        ui->WorldFindDock->restoreGeometry(settings.value("world-search-geometry", ui->WorldFindDock->saveGeometry()).toByteArray());
        ui->Tileset_Item_Box->restoreGeometry(settings.value("tileset-itembox-geometry", ui->Tileset_Item_Box->saveGeometry()).toByteArray());
        ui->debuggerBox->restoreGeometry(settings.value("debugger-box-geometry", ui->debuggerBox->saveGeometry()).toByteArray());

        GlobalSettings::animatorItemsLimit = settings.value("animation-item-limit", "25000").toInt();

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
    QString inifile = ApplicationPath + "/" + "pge_editor.ini";

    QSettings settings(inifile, QSettings::IniFormat);
    settings.beginGroup("Main");
    settings.setValue("pos", pos());
    settings.setValue("lastpath", GlobalSettings::openPath);
    settings.setValue("lastsavepath", GlobalSettings::savePath);
    settings.setValue("lastsavepath-npctxt", GlobalSettings::savePath_npctxt);

    settings.setValue("world-tb-visible", GlobalSettings::WorldToolBoxVis);
    settings.setValue("world-props-visible", GlobalSettings::WorldSettingsToolboxVis);
    settings.setValue("world-search-visible", GlobalSettings::WorldSearchBoxVis);

    settings.setValue("level-tb-visible", GlobalSettings::LevelToolBoxVis);
    settings.setValue("section-tb-visible", GlobalSettings::SectionToolBoxVis);
    settings.setValue("level-layers-vis", GlobalSettings::LevelLayersBoxVis);
    settings.setValue("level-events-vis", GlobalSettings::LevelEventsBoxVis);
    settings.setValue("level-doors-vis", GlobalSettings::LevelDoorsBoxVis);
    settings.setValue("level-search-vis", GlobalSettings::LevelSearchBoxVis);

    settings.setValue("tileset-box-visible", GlobalSettings::TilesetBoxVis);
    settings.setValue("debugger-box-visible", GlobalSettings::DebuggerBoxVis);

    settings.setValue("doors-tool-box-float", ui->DoorsToolbox->isFloating());
    settings.setValue("level-section-set-float", ui->LevelSectionSettings->isFloating());
    settings.setValue("level-layers-float", ui->LevelLayers->isFloating());
    settings.setValue("level-events-float", ui->LevelEventsToolBox->isFloating());
    settings.setValue("item-props-box-float", ui->ItemProperties->isFloating());
    settings.setValue("level-search-float", ui->FindDock->isFloating());

    settings.setValue("world-item-box-float", ui->WorldToolBox->isFloating());
    settings.setValue("world-settings-box-float", ui->WorldSettings->isFloating());
    settings.setValue("world-itemprops-box-float", ui->WLD_ItemProps->isFloating());
    settings.setValue("world-search-float", ui->WorldFindDock->isFloating());
    settings.setValue("tileset-box-float", ui->Tileset_Item_Box->isFloating());
    settings.setValue("debugger-box-float", ui->debuggerBox->isFloating());

    settings.setValue("doors-tool-box-geometry", ui->DoorsToolbox->saveGeometry());
    settings.setValue("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry());
    settings.setValue("level-layers-geometry", ui->LevelLayers->saveGeometry());
    settings.setValue("level-events-geometry", ui->LevelEventsToolBox->saveGeometry());
    settings.setValue("item-props-box-geometry", ui->ItemProperties->saveGeometry());
    settings.setValue("level-search-geometry", ui->FindDock->saveGeometry());

    settings.setValue("world-item-box-geometry", ui->WorldToolBox->saveGeometry());
    settings.setValue("world-settings-box-geometry", ui->WorldSettings->saveGeometry());
    settings.setValue("world-itemprops-box-geometry", ui->WLD_ItemProps->saveGeometry());
    settings.setValue("world-search-geometry", ui->WorldFindDock->saveGeometry());

    settings.setValue("tileset-itembox-geometry", ui->Tileset_Item_Box->saveGeometry());
    settings.setValue("debugger-box-geometry", ui->debuggerBox->saveGeometry());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("autoPlayMusic", GlobalSettings::autoPlayMusic);
    settings.setValue("music-volume", PGE_MusPlayer::currentVolume());

    settings.setValue("editor-midmouse-allowdupe", GlobalSettings::MidMouse_allowDuplicate);
    settings.setValue("editor-midmouse-allowplace", GlobalSettings::MidMouse_allowSwitchToPlace);
    settings.setValue("editor-midmouse-allowdrag", GlobalSettings::MidMouse_allowSwitchToDrag);

    settings.setValue("tab-view", (GlobalSettings::MainWindowView==QMdiArea::TabbedView));
    settings.setValue("level-toolbox-pos", static_cast<int>(GlobalSettings::LVLToolboxPos));
    settings.setValue("world-toolbox-pos", static_cast<int>(GlobalSettings::WLDToolboxPos));

    settings.setValue("animation", GlobalSettings::LvlOpts.animationEnabled);
    settings.setValue("collisions", GlobalSettings::LvlOpts.collisionsEnabled);
    settings.setValue("animation-item-limit", QString::number(GlobalSettings::animatorItemsLimit));

    settings.setValue("language", GlobalSettings::locale);

    settings.setValue("current-config", currentConfigDir);
    settings.setValue("current-theme", Themes::currentTheme());

    settings.setValue("sdl-sample-rate", PGE_MusPlayer::sampleRate());

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
    appSettings->LVLToolboxPos = GlobalSettings::LVLToolboxPos;
    appSettings->WLDToolboxPos = GlobalSettings::WLDToolboxPos;

    appSettings->midmouse_allowDupe = GlobalSettings::MidMouse_allowDuplicate;
    appSettings->midmouse_allowPlace = GlobalSettings::MidMouse_allowSwitchToPlace;
    appSettings->midmouse_allowDragMode = GlobalSettings::MidMouse_allowSwitchToDrag;

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
        GlobalSettings::LVLToolboxPos = appSettings->LVLToolboxPos;
        GlobalSettings::WLDToolboxPos = appSettings->WLDToolboxPos;
        GlobalSettings::MidMouse_allowDuplicate = appSettings->midmouse_allowDupe;
        GlobalSettings::MidMouse_allowSwitchToPlace = appSettings->midmouse_allowPlace;
        GlobalSettings::MidMouse_allowSwitchToDrag = appSettings->midmouse_allowDragMode;

        ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
        ui->LevelToolBoxTabs->setTabPosition(GlobalSettings::LVLToolboxPos);
        ui->WorldToolBoxTabs->setTabPosition(GlobalSettings::WLDToolboxPos);

        applyTheme(Themes::currentTheme().isEmpty() ? ConfStatus::defaultTheme : Themes::currentTheme());

        saveSettings();
    }
    delete appSettings;

}
