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
#include <PGE_File_Formats/file_formats.h>

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

    askConfigAgain=false;

    LvlMusPlay::currentCustomMusic = "";
    LvlMusPlay::currentMusicId = 0;
    LvlMusPlay::musicButtonChecked = false;

    cat_blocks="[all]";
    cat_bgos="[all]";
    cat_npcs="[all]";

    WldBuffer=FileFormats::dummyWldDataArray();
    LvlBuffer=FileFormats::dummyLvlDataArray();

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
    QRect dg = qApp->desktop()->availableGeometry(qApp->desktop()->primaryScreen());
    //Init default geometry of main window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                       QSize(dg.width()-100,
                                             dg.height()-100), dg));

    dock_LvlWarpProps    = new LvlWarpBox(this);
    dock_LvlSectionProps = new LvlSectionProps(this);
    dock_LvlItemProps    = new LvlItemProperties(this);
    dock_LvlItemBox      = new LevelItemBox(this);
    dock_LvlSearchBox    = new LvlSearchBox(this);
    dock_LvlLayers       = new LvlLayersBox(this);
    dock_LvlEvents       = new LvlEventsBox(this);

    dock_WldItemBox      = new WorldItemBox(this);
    dock_WldItemProps    = new WLD_ItemProps(this);
    dock_WldSearchBox    = new WldSearchBox(this);
    dock_WldSettingsBox  = new WorldSettingsBox(this);

    dock_TilesetBox      = new TilesetItemBox(this);
    dock_BookmarksBox    = new BookmarksBox(this);
    dock_DebuggerBox     = new DebuggerBox(this);

    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
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

    /*********************Music volume regulator*************************/
    muVol = new QSlider(Qt::Horizontal);
    muVol->setMaximumWidth(70);
    muVol->setMinimumWidth(70);
    muVol->setMinimum(0);
    muVol->setMaximum(MIX_MAX_VOLUME);
    muVol->setValue(GlobalSettings::musicVolume);

    MusPlayer.setVolume(muVol->value());
    ui->EditionToolBar->insertWidget(ui->actionAnimation, muVol);
    ui->EditionToolBar->insertSeparator(ui->actionAnimation);
    connect(muVol, SIGNAL(valueChanged(int)), &MusPlayer, SLOT(setVolume(int)));
    /*********************Music volume regulator*************************/

    /*********************Zoom field*************************/
    zoom = new QLineEdit();
    zoom->setValidator(new QIntValidator(0,2001));
    zoom->setText("100");
    zoom->setMaximumWidth(50);
    zoom->setEnabled(false);

    ui->LevelSectionsToolBar->insertWidget(ui->actionZoomReset,zoom);
    connect(zoom, SIGNAL(editingFinished()), this, SLOT(applyTextZoom()));
    /*********************Zoom field*************************/

}
