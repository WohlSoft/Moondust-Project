/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QDesktopWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>

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

static PGE_MusPlayer MusPlayer;

void MainWindow::setDefaults()
{
    GlobalSettings::tools_sox_bin_path = ApplicationPath + GlobalSettings::tools_sox_bin_path;

    GlobalSettings::LvlItemDefaults.LockedItemOpacity               = 0.3;

    GlobalSettings::LvlItemDefaults.npc_direction                   = -1;
    GlobalSettings::LvlItemDefaults.npc_generator_type              = 1;
    GlobalSettings::LvlItemDefaults.npc_generator_delay             = 20;
    GlobalSettings::LvlItemDefaults.warp_type                       = 2;
    GlobalSettings::LvlItemDefaults.classicevents_tabs_layviz       = false;
    GlobalSettings::LvlItemDefaults.classicevents_tabs_laymov       = false;
    GlobalSettings::LvlItemDefaults.classicevents_tabs_autoscroll   = false;
    GlobalSettings::LvlItemDefaults.classicevents_tabs_secset       = false;
    GlobalSettings::LvlItemDefaults.classicevents_tabs_common       = false;
    GlobalSettings::LvlItemDefaults.classicevents_tabs_buttons      = false;
    GlobalSettings::LvlItemDefaults.classicevents_tabs_trigger      = false;

    m_isFileReloading = false;

    LvlMusPlay::currentCustomMusic  = "";
    LvlMusPlay::currentMusicId      = 0;
    LvlMusPlay::musicButtonChecked  = false;

    cat_blocks  = "[all]";
    cat_bgos    = "[all]";
    cat_npcs    = "[all]";

    FileFormats::CreateWorldData(WldBuffer);
    FileFormats::CreateLevelData(LvlBuffer);

    LastActiveSubWindow = nullptr;

    #ifdef Q_OS_WIN
    pge_thumbbar        = nullptr;
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
    QRect dg = util::getScreenGeometry();

    //Init default geometry of main window
    int margin = 100;
    setGeometry(margin, margin, dg.width() - margin * 2, dg.height() - margin * 2);
    //Set Maximized state by default
    setWindowState(windowState() | Qt::WindowMaximized);

    dock_LvlItemBox      = new LevelItemBox(this);
    dock_WldItemBox      = new WorldItemBox(this);
    dock_WldMusicBoxes   = new WorldMusicBoxItemBox(this);

    dock_LvlWarpProps    = new LvlWarpBox(this);
    dock_LvlItemProps    = new LvlItemProperties(this);
    dock_LvlSearchBox    = new LvlSearchBox(this);
    dock_LvlEvents       = new LvlEventsBox(this);
    dock_LvlLayers       = new LvlLayersBox(this);
    dock_LvlSectionProps = new LvlSectionProps(this);

    dock_WldItemProps    = new WLD_ItemProps(this);
    dock_WldSearchBox    = new WldSearchBox(this);
    dock_WldSettingsBox  = new WorldSettingsBox(this);

    dock_TilesetBox      = new TilesetItemBox(this);
    dock_BookmarksBox    = new BookmarksBox(this);
    dock_DebuggerBox     = new DebuggerBox(this);
    dock_VariablesBox    = new VariablesBox(this);

    tabifyDockWidget(dock_LvlItemBox, dock_WldItemBox);
    tabifyDockWidget(dock_WldItemBox, dock_WldMusicBoxes);

    m_sectionButtons[0] = ui->actionSection_1;
    m_sectionButtons[1] = ui->actionSection_2;
    m_sectionButtons[2] = ui->actionSection_3;
    m_sectionButtons[3] = ui->actionSection_4;
    m_sectionButtons[4] = ui->actionSection_5;
    m_sectionButtons[5] = ui->actionSection_6;
    m_sectionButtons[6] = ui->actionSection_7;
    m_sectionButtons[7] = ui->actionSection_8;
    m_sectionButtons[8] = ui->actionSection_9;
    m_sectionButtons[9] = ui->actionSection_10;
    m_sectionButtons[10] = ui->actionSection_11;
    m_sectionButtons[11] = ui->actionSection_12;
    m_sectionButtons[12] = ui->actionSection_13;
    m_sectionButtons[13] = ui->actionSection_14;
    m_sectionButtons[14] = ui->actionSection_15;
    m_sectionButtons[15] = ui->actionSection_16;
    m_sectionButtons[16] = ui->actionSection_17;
    m_sectionButtons[17] = ui->actionSection_18;
    m_sectionButtons[18] = ui->actionSection_19;
    m_sectionButtons[19] = ui->actionSection_20;
    m_sectionButtons[20] = ui->actionSection_21;

    //#ifndef Q_OS_WIN
    //tabifyDockWidget(dock_LvlWarpProps, dock_LvlSectionProps);
    //tabifyDockWidget(dock_LvlSectionProps, dock_LvlItemProps);
    //tabifyDockWidget(dock_LvlItemProps, dock_LvlSearchBox);
    //tabifyDockWidget(dock_LvlSearchBox, dock_LvlLayers);
    //tabifyDockWidget(dock_LvlLayers, dock_LvlEvents);
    //tabifyDockWidget(dock_LvlEvents, dock_WldItemProps);
    //tabifyDockWidget(dock_WldItemProps, dock_WldSearchBox);
    //tabifyDockWidget(dock_WldSearchBox, dock_WldSettingsBox);
    //tabifyDockWidget(dock_WldSettingsBox, dock_DebuggerBox);

#define DisableFloatFeature(dock) dock->setFeatures(dock->features()&(~QDockWidget::DockWidgetFloatable));
    //DisableFloatFeature(dock_LvlItemBox);
    //DisableFloatFeature(dock_WldItemBox);

    //DisableFloatFeature(dock_LvlWarpProps);
    //DisableFloatFeature(dock_LvlSectionProps);
    //DisableFloatFeature(dock_LvlItemProps);
    //DisableFloatFeature(dock_LvlSearchBox);
    //DisableFloatFeature(dock_LvlLayers);
    //DisableFloatFeature(dock_LvlWarpProps);
    //DisableFloatFeature(dock_WldItemProps);
    //DisableFloatFeature(dock_WldSearchBox);
    //DisableFloatFeature(dock_WldSettingsBox);
    //DisableFloatFeature(dock_DebuggerBox);
#ifdef Q_OS_ANDROID
    DisableFloatFeature(dock_TilesetBox);
    DisableFloatFeature(dock_BookmarksBox);
#endif
    //#endif

    //Add "New" tool button as menu
    {
        QToolButton *newAction = new QToolButton(ui->fileIoTooBar);
        newAction->setMenu(ui->menuNew);
        newAction->setIcon(ui->menuNew->icon());
        newAction->setPopupMode(QToolButton::InstantPopup);
        newAction->setToolTip(ui->menuNew->title());
        //Automatically reset label on language switching
        newAction->connect(this, &MainWindow::languageSwitched, [this, newAction](){
                                newAction->setToolTip(ui->menuNew->title());
                           });
        ui->fileIoTooBar->insertWidget(ui->OpenFile, newAction);
    }

    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateMenus(QMdiSubWindow *)));
    //connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(recordSwitchedWindow(QMdiSubWindow*)));
    connect(this, &MainWindow::setSMBX64Strict, ui->actionCreateScriptLocal, &QAction::setDisabled);
    connect(this, &MainWindow::setSMBX64Strict, ui->actionCreateScriptEpisode, &QAction::setDisabled);

    ui->actionPlayMusic->setChecked(GlobalSettings::autoPlayMusic);
    ui->centralWidget->cascadeSubWindows();

    ui->ResizingToolbar->setVisible(false);

    ui->PlacingToolbar->setVisible(false);
    ui->actionOverwriteMode->setVisible(false);
    ui->actionRectFill->setVisible(false);
    ui->actionCircleFill->setVisible(false);
    ui->actionLine->setVisible(false);
    ui->actionFill->setVisible(false);
    ui->actionFloodSectionOnly->setVisible(false);
    ui->actionFloodSectionOnly->setEnabled(false);

    auto ziKey = QKeySequence::keyBindings(QKeySequence::ZoomIn);
    ziKey.append(QKeySequence(Qt::CTRL + Qt::Key_Equal));
    ui->actionZoomIn->setShortcuts(ziKey);
    ui->actionZoomOut->setShortcuts(QKeySequence::ZoomOut);

#ifndef Q_OS_WIN
    addToolBar(Qt::LeftToolBarArea, ui->LevelObjectToolbar);
    addToolBar(Qt::LeftToolBarArea, ui->WorldObjectToolbar);
#endif

    {
        QAction *action = ui->menuSetGridSize->addAction(tr("Default by item"));
        action->setData(0);
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);
        ui->menuSetGridSize->addSeparator();

        action = ui->menuSetGridSize->addAction("2 x 2");
        action->setData(2);
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);

        action = ui->menuSetGridSize->addAction("4 x 4");
        action->setData(4);
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);

        action = ui->menuSetGridSize->addAction("8 x 8");
        action->setData(8);
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);

        action = ui->menuSetGridSize->addAction("16 x 16");
        action->setData(16);
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);

        action = ui->menuSetGridSize->addAction("32 x 32");
        action->setData(32);
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);

        action = ui->menuSetGridSize->addAction("64 x 64");
        action->setData(64);
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);

        action = ui->menuSetGridSize->addAction("128 x 128");
        action->setData(128);
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);

        action = ui->menuSetGridSize->addAction(tr("Custom..."));
        action->setData(-1);
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);

        ui->menuSetGridSize->addSeparator();

        action = ui->menuSetGridSize->addAction(tr("Next grid size"));
        action->setData(-2);
        action->setShortcut(QKeySequence(Qt::Key_BracketRight));
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);

        action = ui->menuSetGridSize->addAction(tr("Previous grid size"));
        action->setData(-3);
        action->setShortcut(QKeySequence(Qt::Key_BracketLeft));
        connect(action, &QAction::triggered, this, &MainWindow::customGrid);
    }

    {
        //Vanilla editor's toolbar
        m_toolbarVanilla = new QToolBar("Vanilla toolbar", this);
        m_toolbarVanilla->setObjectName("m_toolbarVanilla");

        QPushButton *select = new QPushButton(m_toolbarVanilla);
        select->setText(tr("Select", "Vanilla-like toolbar"));
        select->connect(this, &MainWindow::languageSwitched, [select](){
                                select->setText(tr("Select", "Vanilla-like toolbar"));
                           });
        select->setCheckable(true);
        select->setChecked(ui->actionSelect->isChecked());
        connect(ui->actionSelect, &QAction::toggled, select, &QPushButton::setChecked);
        connect(select, &QPushButton::clicked, ui->actionSelect, &QAction::trigger);
        m_toolbarVanilla->insertWidget(nullptr, select);
        QAction *selectA = m_toolbarVanilla->insertWidget(nullptr, select);
        connect(this, &MainWindow::windowActiveLevelWorld, selectA, &QAction::setVisible);

        QPushButton *erase  = new QPushButton(m_toolbarVanilla);
        erase->setText(tr("Erase", "Vanilla-like toolbar"));
        erase->connect(this, &MainWindow::languageSwitched, [erase](){
                                erase->setText(tr("Erase", "Vanilla-like toolbar"));
                           });
        erase->setCheckable(true);
        erase->setChecked(ui->actionEriser->isChecked());
        connect(ui->actionEriser, &QAction::toggled, erase, &QPushButton::setChecked);
        connect(erase, &QPushButton::clicked, ui->actionEriser, &QAction::trigger);
        m_toolbarVanilla->insertWidget(nullptr, erase);
        QAction *eraseA = m_toolbarVanilla->insertWidget(nullptr, erase);
        connect(this, &MainWindow::windowActiveLevelWorld, eraseA, &QAction::setVisible);

        QPushButton *items  = new QPushButton(m_toolbarVanilla);
        items->setText(tr("Items", "Vanilla-like toolbar"));
        items->connect(this, &MainWindow::languageSwitched, [items](){
                                items->setText(tr("Items", "Vanilla-like toolbar"));
                           });
        items->setCheckable(true);
        items->setChecked(ui->actionTilesetBox->isChecked());
        connect(ui->actionTilesetBox, &QAction::toggled, items , &QPushButton::setChecked);
        connect(items , &QPushButton::clicked, ui->actionTilesetBox, &QAction::trigger);
        m_toolbarVanilla->insertWidget(nullptr, items);
        QAction *itemsA = m_toolbarVanilla->insertWidget(nullptr, items);
        connect(this, &MainWindow::windowActiveLevelWorld, itemsA, &QAction::setVisible);

        QPushButton *player  = new QPushButton(m_toolbarVanilla);
        player->setText(tr("Player", "Vanilla-like toolbar"));
        player->connect(this, &MainWindow::languageSwitched, [player]() {
                                player->setText(tr("Player", "Vanilla-like toolbar"));
                           });
        connect(player, &QPushButton::clicked, [ = ](bool)
        {
            QMenu menu;
            menu.insertAction(nullptr, ui->actionSetFirstPlayer);
            menu.insertAction(nullptr, ui->actionSetSecondPlayer);
            menu.exec(QCursor::pos());
        });
        m_toolbarVanilla->insertWidget(nullptr, player);
        QAction *playerA = m_toolbarVanilla->insertWidget(nullptr, player);
        connect(this, &MainWindow::windowActiveLevel, playerA, &QAction::setVisible);

        QPushButton *section  = new QPushButton(m_toolbarVanilla);
        section->setText(tr("Section", "Vanilla-like toolbar"));
        section->connect(this, &MainWindow::languageSwitched, [section](){
                                section->setText(tr("Section", "Vanilla-like toolbar"));
                           });
        section->setCheckable(true);
        section->setChecked(ui->actionSection_Settings->isChecked());
        connect(ui->actionSection_Settings, &QAction::toggled, section , &QPushButton::setChecked);
        connect(section , &QPushButton::clicked, ui->actionSection_Settings, &QAction::trigger);
        QAction *sectionA = m_toolbarVanilla->insertWidget(nullptr, section);
        connect(this, &MainWindow::windowActiveLevel, sectionA, &QAction::setVisible);

        QPushButton *wldProps  = new QPushButton(m_toolbarVanilla);
        wldProps->setText(tr("World settings", "Vanilla-like toolbar"));
        wldProps->connect(this, &MainWindow::languageSwitched, [wldProps](){
                                wldProps->setText(tr("World settings", "Vanilla-like toolbar"));
                           });
        wldProps->setCheckable(true);
        wldProps->setChecked(ui->actionWorld_settings->isChecked());
        connect(ui->actionWorld_settings, &QAction::toggled, wldProps , &QPushButton::setChecked);
        connect(wldProps, &QPushButton::clicked, ui->actionWorld_settings, &QAction::trigger);
        QAction *wldPropsAction = m_toolbarVanilla->insertWidget(nullptr, wldProps);
        connect(this, &MainWindow::windowActiveWorld, wldPropsAction, &QAction::setVisible);

        QPushButton *doors  = new QPushButton(m_toolbarVanilla);
        doors->setText(tr("Warps and Doors", "Vanilla-like toolbar"));
        doors->connect(this, &MainWindow::languageSwitched, [doors](){
                                doors->setText(tr("Warps and Doors", "Vanilla-like toolbar"));
                           });
        doors->setCheckable(true);
        doors->setChecked(ui->actionWarpsAndDoors->isChecked());
        connect(ui->actionWarpsAndDoors, &QAction::toggled, doors , &QPushButton::setChecked);
        connect(doors, &QPushButton::clicked, ui->actionWarpsAndDoors, &QAction::trigger);
        QAction *doorsA = m_toolbarVanilla->insertWidget(nullptr, doors);
        connect(this, &MainWindow::windowActiveLevel, doorsA, &QAction::setVisible);

        QPushButton *water  = new QPushButton(m_toolbarVanilla);
        water->setText(tr("Water", "Vanilla-like toolbar"));
        water->connect(this, &MainWindow::languageSwitched, [water](){
                                water->setText(tr("Water", "Vanilla-like toolbar"));
                           });
        connect(water, &QPushButton::clicked, [ = ](bool)
        {
            QMenu menu;
            menu.insertAction(nullptr, ui->actionDrawWater);
            menu.insertAction(nullptr, ui->actionDrawSand);
            menu.exec(QCursor::pos());
        });
        m_toolbarVanilla->insertWidget(nullptr, water);
        QAction *waterA = m_toolbarVanilla->insertWidget(nullptr, water);
        connect(this, &MainWindow::windowActiveLevel, waterA, &QAction::setVisible);

        {
            QToolButton *options = new QToolButton(m_toolbarVanilla);
            QMenu *menu = new QMenu(options);

            menu->insertAction(nullptr, ui->actionGridEn);
            menu->insertAction(nullptr, ui->actionShowGrid);
            menu->insertAction(nullptr, ui->actionShowCameraGrid);
            menu->addMenu(ui->menuSetGridSize);
            menu->addSeparator();
            menu->insertAction(nullptr, ui->actionAnimation);
            menu->insertAction(nullptr, ui->actionCollisions);
            menu->insertAction(nullptr, ui->actionSemi_transparent_paths);

            options->setMenu(menu);
            options->setText(tr("Options"));
            options->connect(this, &MainWindow::languageSwitched, [options](){
                                    options->setText(tr("Options"));
                               });
            options->setPopupMode(QToolButton::InstantPopup);
            options->setToolButtonStyle(Qt::ToolButtonTextOnly);
            m_toolbarVanilla->insertWidget(nullptr, options);
        }
        addToolBar(Qt::BottomToolBarArea, m_toolbarVanilla);

        m_toolbarVanilla->setVisible(false);
    }

    // Register hot key without adding into toolbar
    addAction(ui->actionNewFile);

    QObject::connect(&m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::runAutoSave);

    ui->menuWindow->menuAction()->setEnabled(true);
    ui->menuLevel->menuAction()->setEnabled(false);
    ui->menuWorld->menuAction()->setEnabled(false);
    ui->menuTest->menuAction()->setEnabled(false);

    ui->LevelObjectToolbar->setVisible(false);
    ui->WorldObjectToolbar->setVisible(false);

    ui->actionLVLToolBox->setVisible(false);
    ui->actionWarpsAndDoors->setVisible(false);
    ui->actionSection_Settings->setVisible(false);
    ui->actionWarpsAndDoors->setVisible(false);
    ui->actionWLDToolBox->setVisible(false);
    ui->actionMusicBoxes->setVisible(false);
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

    /**********************Default font size********************************/
    GlobalSettings::fontDefault.reset(new QFont(qApp->font()));
    GlobalSettings::font.reset(new QFont(qApp->font()));

    /*********************Music volume regulator*************************/
    m_ui_musicVolume = new QSlider(Qt::Horizontal, ui->EditionToolBar);
    m_ui_musicVolume->setMaximumWidth(70);
    m_ui_musicVolume->setMinimumWidth(70);
    m_ui_musicVolume->setMinimum(0);
#ifndef MIX_MAX_VOLUME
#   define MIX_MAX_VOLUME 128
#endif
    m_ui_musicVolume->setMaximum(MIX_MAX_VOLUME);
    m_ui_musicVolume->setValue(GlobalSettings::musicVolume);

    MusPlayer.setVolume(m_ui_musicVolume->value());
    ui->EditionToolBar->insertWidget(ui->actionAnimation, m_ui_musicVolume);
    ui->EditionToolBar->insertSeparator(ui->actionAnimation);
    connect(m_ui_musicVolume, SIGNAL(valueChanged(int)), &MusPlayer, SLOT(setVolume(int)));
    /*********************Music volume regulator*************************/

    /*********************Zoom field*************************/
    zoom = new QLineEdit(ui->LevelSectionsToolBar);
    zoom->setValidator(new QIntValidator(0, 2001, zoom));
    zoom->setText("100");
#ifdef Q_OS_OSX
    zoom->setMinimumWidth(40);
    zoom->setMaximumWidth(50);
#else
    zoom->setMaximumWidth(28);
#endif
    zoom->setEnabled(false);

    ui->LevelSectionsToolBar->insertWidget(ui->actionZoomReset, zoom);
    connect(zoom, SIGNAL(editingFinished()), this, SLOT(applyTextZoom()));
    /*********************Zoom field*************************/

    /***Hide all "under construction" elements in release builds***/
#ifndef DEBUG_BUILD
    ui->actionVariables->setVisible(false);
    ui->actionScriptEditor->setVisible(false);
    ui->actionSprite_editor->setVisible(false);
    // ui->separatorSpriteEditor->setVisible(false);
#endif
    /**************************************************************/

    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionSelect, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionSelectOnly, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionEriser, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionHandScroll, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionCopy, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionPaste, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionCut, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionGridEn, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionShowGrid, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionShowCameraGrid, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionCollisions, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->menuSetGridSize, &QMenu::setEnabled);

    connect(this, &MainWindow::windowActiveLevel,  ui->menuLevel->menuAction(), &QAction::setVisible);
    connect(this, &MainWindow::windowActiveWorld,  ui->menuWorld->menuAction(), &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevel,  ui->menuLevel->menuAction(), &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveWorld,  ui->menuWorld->menuAction(), &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevelWorld,  ui->menuView->menuAction(), &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveWorld,   ui->actionSemi_transparent_paths, &QAction::setVisible);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->menuTest->menuAction(), &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevel,       ui->action_doTest, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel,       ui->action_doTest, &QAction::setVisible);

    connect(this, &MainWindow::windowActiveWorld,       ui->action_doTestWld, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveWorld,       ui->action_doTestWld, &QAction::setVisible);

    connect(this, &MainWindow::windowActiveLevel,  ui->action_doSafeTest, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel,  ui->action_doSafeTest, &QAction::setVisible);

    connect(this, &MainWindow::windowActiveWorld,  ui->action_doSafeTestWld, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveWorld,  ui->action_doSafeTestWld, &QAction::setVisible);

    connect(this, &MainWindow::windowActiveLevel, ui->LevelObjectToolbar, &QWidget::setVisible);
    connect(this, &MainWindow::windowActiveWorld, ui->WorldObjectToolbar, &QWidget::setVisible);

    connect(this, &MainWindow::windowActiveLevel, ui->actionLVLToolBox, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevel, ui->actionWarpsAndDoors, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevel, ui->actionSection_Settings, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevel, ui->actionLevelProp, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevel, ui->actionLayersBox, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevel, ui->actionLevelEvents, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevel, ui->actionWarpsAndDoors, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevel, ui->actionLVLSearchBox, &QAction::setVisible);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionTilesetBox, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionBookmarkBox, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionDebugger, &QAction::setVisible);

    connect(this, &MainWindow::windowActiveWorld, ui->actionWLDToolBox, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveWorld, ui->actionMusicBoxes, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveWorld, ui->actionWorld_settings, &QAction::setVisible);
    connect(this, &MainWindow::windowActiveWorld, ui->actionWLD_SearchBox, &QAction::setVisible);

    connect(this, &MainWindow::windowActiveLevel, ui->actionLevNoBack, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionLevOffScr, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionWrapHorizontal, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionWrapVertically, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionLevUnderW, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevel, ui->actionLevelProp, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevel, ui->actionExport_to_image_section, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionExport_to_image, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionZoomIn, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionZoomOut, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionZoomReset, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, zoom, &QLineEdit::setEnabled);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionGotoLeftBottom, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionGotoLeftTop, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionGotoTopRight, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionGotoRightBottom, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionFixWrongMasks, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionCDATA_clear_unused, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionCDATA_Import, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionAlign_selected, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionFlipHorizontal, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionFlipVertical, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionRotateLeft, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionRotateRight, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevel, ui->actionCloneSectionTo, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionSCT_Delete, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionSCT_FlipHorizontal, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionSCT_FlipVertical, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionSCT_RotateLeft, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel, ui->actionSCT_RotateRight, &QAction::setEnabled);

    connect(this, &MainWindow::windowActiveLevelWorld, ui->menuScript->menuAction(), &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionCreateScriptLocal, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->actionCreateScriptEpisode, &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevelWorld, ui->menuLunaLUA_scripts->menuAction(), &QAction::setEnabled);

    for(size_t i = 0; i < m_sectionButtonsCount; i++)
        connect(this, &MainWindow::windowActiveLevel,   m_sectionButtons[i], &QAction::setEnabled);
    connect(this, &MainWindow::windowActiveLevel,   ui->actionSectionMore, &QAction::setEnabled);
}


static int s_mw_setDockWidgetState(MainWindow *mw, QDockWidget *w, const EditorSetup::DefaultToolboxPositions::State &p)
{
    QRect g = w->geometry();
    int ret = 0;

    // Set the initial floating state:

    if(p.dock == EditorSetup::DefaultToolboxPositions::State::F_FLOATING)
    {
        w->setFloating(true);
        ret = 0;
    }
    else
    {
        w->setFloating(false);
        mw->addDockWidget(static_cast<Qt::DockWidgetArea>(p.dock), w);
        ret = (int)p.dock;
    }

    // Set the initial geometry:

    if(p.width > 0)
        g.setWidth(p.width);

    if(p.height > 0)
        g.setHeight(p.height);

    QRect mwg = mw->geometry();
    const int GOffset = 10;

    if(p.x != 0)
    {
        if(p.x < 0)
            g.setX(mwg.right() - g.width() + p.x - GOffset);
        else
            g.setX(mwg.left() + p.x + GOffset);
    }

    if(p.y != 0)
    {
        if(p.y < 0)
            g.setY(mwg.bottom() - g.height() + p.y - GOffset);
        else
            g.setY(mwg.top() + p.y + GOffset);
    }

    w->setGeometry(g);

    if(ret != 0)
    {
        if(ret == Qt::LeftDockWidgetArea || ret == Qt::RightDockWidgetArea)
            mw->resizeDocks({w}, {g.width()}, Qt::Horizontal);
        else if(ret == Qt::LeftDockWidgetArea || ret == Qt::RightDockWidgetArea)
            mw->resizeDocks({w}, {g.height()}, Qt::Vertical);
    }

    return ret;
}

typedef QVector<QDockWidget*> TabifyList;

static void s_mw_addWidget(MainWindow *mw, QDockWidget *w,
                           EditorSetup::DefaultToolboxPositions::State p,
                           TabifyList &l, TabifyList &b, TabifyList &r)
{
    int ret = s_mw_setDockWidgetState(mw, w, p);
    switch(ret)
    {
    default:
    case 0:
        break;
    case Qt::LeftDockWidgetArea:
        l.push_back(w);
        break;
    case Qt::BottomDockWidgetArea:
        b.push_back(w);
        break;
    case Qt::RightDockWidgetArea:
        r.push_back(w);
        break;
    }
}

static void s_tabifyWidgets(MainWindow *w, TabifyList &l)
{
    if(l.size() < 2)
        return;

    for(int i = 1; i < l.size(); ++i)
    {
        auto *i1 = l[i - 1];
        auto *i2 = l[i];
        w->tabifyDockWidget(i1, i2);
    }
}

void MainWindow::setUiDefultsConfigPack()
{
    // 21+ More sections
    if(configs.editor.supported_features.level_section_21plus == EditorSetup::FeaturesSupport::F_DISABLED)
        ui->actionSectionMore->setEnabled(false);
    else if(configs.editor.supported_features.level_section_21plus == EditorSetup::FeaturesSupport::F_HIDDEN)
        ui->actionSectionMore->setVisible(false);
    else
        QObject::connect(this, &MainWindow::setSMBX64Strict, ui->actionSectionMore, &QAction::setDisabled);

    TabifyList l, r, b;

    // These entries ordered in sequence to be tabified when docked at the same area
    s_mw_addWidget(this, dock_LvlSearchBox, configs.editor.default_widget_state.level_search_box, l, b, r);
    s_mw_addWidget(this, dock_LvlEvents, configs.editor.default_widget_state.level_classic_events_box, l, b, r);
    s_mw_addWidget(this, dock_LvlLayers, configs.editor.default_widget_state.level_layers_box, l, b, r);
    s_mw_addWidget(this, dock_LvlWarpProps, configs.editor.default_widget_state.level_warps_box, l, b, r);
    s_mw_addWidget(this, dock_WldMusicBoxes, configs.editor.default_widget_state.world_music_boxes, l, b, r);
    s_mw_addWidget(this, dock_WldItemProps, configs.editor.default_widget_state.world_item_properties, l, b, r);
    s_mw_addWidget(this, dock_WldSettingsBox, configs.editor.default_widget_state.world_settings_box, l, b, r);
    s_mw_addWidget(this, dock_WldSearchBox, configs.editor.default_widget_state.world_search_box, l, b, r);
    s_mw_addWidget(this, dock_BookmarksBox, configs.editor.default_widget_state.bookmarks_box, l, b, r);
    s_mw_addWidget(this, dock_DebuggerBox, configs.editor.default_widget_state.debugger_box, l, b, r);
#ifdef DEBUG_BUILD
    s_mw_addWidget(this, dock_VariablesBox, configs.editor.default_widget_state.variables_box, l, b, r);
#endif
    s_mw_addWidget(this, dock_LvlSectionProps, configs.editor.default_widget_state.level_section_properties, l, b, r);
    s_mw_addWidget(this, dock_LvlItemBox, configs.editor.default_widget_state.level_item_browser, l, b, r);
    s_mw_addWidget(this, dock_TilesetBox, configs.editor.default_widget_state.tilesets_item_box, l, b, r);
    s_mw_addWidget(this, dock_WldItemBox, configs.editor.default_widget_state.world_item_browser, l, b, r);
    s_mw_addWidget(this, dock_LvlItemProps, configs.editor.default_widget_state.level_item_properties, l, b, r);

    s_tabifyWidgets(this, l);
    s_tabifyWidgets(this, b);
    s_tabifyWidgets(this, r);
}
