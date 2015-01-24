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

#include <QFont>

#include <common_features/app_path.h>
#include <common_features/logger_sets.h>
#include <common_features/graphics_funcs.h>
#include <common_features/themes.h>
#include <main_window/global_settings.h>
#include <main_window/tools/app_settings.h>
#include <main_window/dock/toolboxes.h>
#include <audio/music_player.h>
#include <audio/sdl_music_player.h>
#include <file_formats/file_formats.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

PGE_MusPlayer MusPlayer;

void MainWindow::setDefaults()
{
    setPointer();

    GlobalSettings::LvlOpts.animationEnabled = true;
    GlobalSettings::LvlOpts.collisionsEnabled = true;
    GlobalSettings::LvlOpts.semiTransparentPaths = false;

    LvlEventBoxLock=false;

    askConfigAgain=false;

    LvlMusPlay::currentCustomMusic = "";
    LvlMusPlay::currentMusicId = 0;
    LvlMusPlay::musicButtonChecked = false;

    cat_blocks="[all]";
    cat_bgos="[all]";
    cat_npcs="[all]";

    WldBuffer=FileFormats::dummyWldDataArray();
    LvlBuffer=FileFormats::dummyLvlDataArray();

    dock_TilesetBox = NULL;
    dock_LvlItemProps = NULL;
    dock_LvlWarpProps = NULL;

    LastActiveSubWindow = NULL;
    pge_thumbbar = NULL;
}

void MainWindow::setUiDefults()
{
    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));
    #endif

    //MainWindow Geometry;
    QRect mwg = this->geometry();
    QRect dg = qApp->desktop()->availableGeometry(qApp->desktop()->primaryScreen());
    //Init default geometry of main window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                       QSize(dg.width()-100,
                                             dg.height()-100), dg));

    int GOffset=240;
    //Define the default geometry for toolboxes
    dock_LvlWarpProps = new LvlWarpBox(this, this);
    dock_LvlWarpProps->setVisible(false);
    addDockWidget(Qt::NoDockWidgetArea, dock_LvlWarpProps);
    dock_LvlWarpProps->setGeometry(
                mwg.x()+mwg.width()-dock_LvlWarpProps->width()-GOffset,
                mwg.y()+120,
                dock_LvlWarpProps->width(),
                dock_LvlWarpProps->height()
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

    dock_LvlItemProps = new LvlItemProperties(this,this);
    dock_LvlItemProps->setVisible(false);
    addDockWidget(Qt::NoDockWidgetArea, dock_LvlItemProps);
    dock_LvlItemProps->setGeometry(
                mwg.x()+mwg.width()-dock_LvlItemProps->width()-GOffset,
                mwg.y()+120,
                dock_LvlItemProps->width(),
                dock_LvlItemProps->height()
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


    dock_TilesetBox = new TilesetItemBox(this, this);
    dock_TilesetBox->setVisible(false);
    addDockWidget(Qt::NoDockWidgetArea, dock_TilesetBox);
    dock_TilesetBox->setGeometry(
                dg.x()+GOffset,
                dg.y()+dg.height()-600,
                800,
                300
                );

    ui->debuggerBox->setGeometry(
                mwg.x()+mwg.width()-ui->debuggerBox->width()-GOffset,
                mwg.y()+120,
                ui->debuggerBox->width(),
                ui->debuggerBox->height()
                );


    ui->bookmarkBox->setGeometry(
                mwg.x()+mwg.width()-ui->bookmarkBox->width()-GOffset,
                mwg.y()+120,
                ui->bookmarkBox->width(),
                ui->bookmarkBox->height()
                );

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setWeight(8);
    ui->DEBUG_Items->setFont(font);

    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)),
        this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
        this, SLOT(setActiveSubWindow(QWidget*)));



    ui->actionPlayMusic->setChecked(GlobalSettings::autoPlayMusic);

    ui->actionExport_to_image_section->setVisible(false);

    ui->actionVBAlphaEmulate->setChecked(GraphicsHelps::EnableVBEmulate);

    ui->centralWidget->cascadeSubWindows();

    ui->ResizingToolbar->setVisible(false);
        ui->applyResize->setVisible(false);
        ui->cancelResize->setVisible(false);

    ui->PlacingToolbar->setVisible(false);
        ui->actionOverwriteMode->setVisible(false);
        ui->actionSquareFill->setVisible(false);
        ui->actionLine->setVisible(false);
        ui->actionFill->setVisible(false);
        ui->actionFloodSectionOnly->setVisible(false);
        ui->actionFloodSectionOnly->setEnabled(false);

    ui->LevelToolBox->hide();


    dock_LvlWarpProps->hide();
    ui->LevelLayers->hide();
    ui->LevelEventsToolBox->hide();
    ui->LevelSectionSettings->hide();

    dock_LvlItemProps->hide();
    ui->FindDock->hide();

    ui->WorldToolBox->hide();
    ui->WorldSettings->hide();
    ui->WLD_ItemProps->hide();
    ui->WorldFindDock->hide();

    dock_TilesetBox->hide();
    ui->debuggerBox->hide();
    ui->bookmarkBox->hide();

    ui->menuView->setEnabled(false);

    ui->menuWindow->setEnabled(true);

    ui->menuLevel->setEnabled(false);
    ui->menuWorld->setEnabled(false);
    ui->menuTest->setEnabled(false);
    ui->LevelObjectToolbar->setVisible(false);
    ui->WorldObjectToolbar->setVisible(false);

    ui->actionLVLToolBox->setVisible(false);
    ui->actionWarpsAndDoors->setVisible(false);
    ui->actionSection_Settings->setVisible(false);
    ui->actionWarpsAndDoors->setVisible(false);
    ui->actionWLDToolBox->setVisible(false);
    ui->actionGridEn->setChecked(true);

    ui->actionTilesetBox->setVisible(false);
    ui->actionBookmarkBox->setVisible(false);
    ui->actionDebugger->setVisible(false);

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
    connect(ui->bookmarkList->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(DragAndDroppedBookmark(QModelIndex,int,int,QModelIndex,int)));

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
    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(recordSwitchedWindow(QMdiSubWindow*)));
}
