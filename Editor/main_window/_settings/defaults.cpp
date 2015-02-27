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

    GlobalSettings::tools_sox_bin_path = ApplicationPath+GlobalSettings::tools_sox_bin_path;

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
    #ifdef Q_OS_WIN
    pge_thumbbar = NULL;
    #endif
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
    dock_LvlWarpProps    = new LvlWarpBox(this);
    dock_LvlSectionProps = new LvlSectionProps(this);
    dock_LvlItemProps    = new LvlItemProperties(this);
    dock_LvlItemBox      = new LevelItemBox(this);
    dock_LvlSearchBox    = new LvlSearchBox(this);
    dock_LvlLayers       = new LvlLayersBox(this);

    dock_WldItemBox      = new WorldItemBox(this);
    dock_WldItemProps    = new WLD_ItemProps(this);

    dock_TilesetBox      = new TilesetItemBox(this);

    //Define the default geometry for toolboxes
    ui->LevelEventsToolBox->setGeometry(
                mwg.x()+mwg.width()-ui->LevelEventsToolBox->width()-GOffset,
                mwg.y()+120,
                ui->LevelEventsToolBox->width(),
                ui->LevelEventsToolBox->height()
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

    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(toggleNewWindowWLD(QMdiSubWindow*)));
    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(recordSwitchedWindow(QMdiSubWindow*)));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
        this, SLOT(setActiveSubWindow(QWidget*)));

    ui->actionPlayMusic->setChecked(GlobalSettings::autoPlayMusic);

    ui->actionExport_to_image_section->setVisible(false);

    ui->actionVBAlphaEmulate->setChecked(GraphicsHelps::EnableVBEmulate);

    ui->centralWidget->cascadeSubWindows();

    ui->ResizingToolbar->setVisible(false);

    ui->PlacingToolbar->setVisible(false);
        ui->actionOverwriteMode->setVisible(false);
        ui->actionSquareFill->setVisible(false);
        ui->actionLine->setVisible(false);
        ui->actionFill->setVisible(false);
        ui->actionFloodSectionOnly->setVisible(false);
        ui->actionFloodSectionOnly->setEnabled(false);


    ui->LevelEventsToolBox->hide();
    ui->WorldSettings->hide();
    ui->WorldFindDock->hide();
    ui->debuggerBox->hide();
    ui->bookmarkBox->hide();

    ui->menuView->setEnabled(false);

    ui->menuWindow->setEnabled(true);

    ui->menuLevel->setEnabled(false);
    ui->menuWorld->setEnabled(false);
    ui->menuTest->setEnabled(false);
    #ifndef Q_OS_WIN
    //Hide a Microsoft Windows specific menuitem
    ui->actionRunTestSMBX->setVisible(false);
    #endif
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
    dock_LvlItemBox->tabWidget()->setTabPosition(GlobalSettings::LVLToolboxPos);
    dock_WldItemBox->tabWidget()->setTabPosition(GlobalSettings::WLDToolboxPos);
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

    connect(ui->LVLEvents_List->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(DragAndDroppedEvent(QModelIndex,int,int,QModelIndex,int)));
    connect(ui->bookmarkList->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(DragAndDroppedBookmark(QModelIndex,int,int,QModelIndex,int)));


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
}
