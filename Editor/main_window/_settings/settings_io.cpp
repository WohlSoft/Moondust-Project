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

#include <common_features/app_path.h>
#include <common_features/logger_sets.h>
#include <common_features/themes.h>
#include <common_features/util.h>
#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>
#include <main_window/tools/app_settings.h>
#include <main_window/dock/toolboxes.h>
#include <audio/sdl_music_player.h>
#include <audio/music_player.h>
#include <editing/_scenes/level/lvl_item_placing.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

static void loadToolboxProps(QSettings &s,
                             QString keyprefix,
                             MWDock_Base *widget,
                             bool defViz,
                             bool forceDefault,
                             EditorSetup::DefaultToolboxPositions::State defFloat)
{
    QDockWidget *dw = dynamic_cast<QDockWidget *>(widget);
    widget->m_lastVisibilityState = forceDefault ? defViz : s.value(keyprefix + QStringLiteral("-visible"), defViz).toBool();
    dw->setFloating(s.value(keyprefix + QStringLiteral("-float"), (defFloat.dock == EditorSetup::DefaultToolboxPositions::State::F_FLOATING)).toBool());
    dw->restoreGeometry(s.value(keyprefix + QStringLiteral("-geometry"), dw->saveGeometry()).toByteArray());
}

static void saveToolboxProps(QSettings &s,
                             QString keyprefix,
                             MWDock_Base *widget)
{
    QDockWidget *dw = dynamic_cast<QDockWidget *>(widget);
    s.setValue(keyprefix + QStringLiteral("-visible"),  widget->m_lastVisibilityState);
    s.setValue(keyprefix + QStringLiteral("-float"),    dw->isFloating());
    s.setValue(keyprefix + QStringLiteral("-geometry"), dw->saveGeometry());
}



void MainWindow::loadBasicSettings()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    settings.beginGroup("Main");
    {
        GlobalSettings::currentTheme = settings.value("current-theme", "").toString();
        GlobalSettings::currentPallete = settings.value("current-pallete", "").toInt();
        GlobalSettings::fontSize = settings.value("font-size", -1).toInt();
    }
}


//////////Load settings from INI file///////////////
void MainWindow::loadSettings()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    #ifdef Q_OS_WIN
#define FloatDocks true
#define FloatDocks_inv false
    #else
#define FloatDocks false
#define FloatDocks_inv true
    #endif

    settings.beginGroup("Main");
    {
        //GlobalSettings::LastOpenDir = settings.value("lastpath", ".").toString();
        GlobalSettings::LvlOpts.animationEnabled = settings.value("animation", true).toBool();
        GlobalSettings::LvlOpts.collisionsEnabled = settings.value("collisions", true).toBool();
        GlobalSettings::LvlOpts.grid_show = settings.value("grid-show", false).toBool();
        GlobalSettings::LvlOpts.camera_grid_show = settings.value("camera-grid-show", false).toBool();
        GlobalSettings::LvlOpts.default_zoom = settings.value("default-zoom", 100).toUInt();

        GlobalSettings::LvlItemDefaults.LockedItemOpacity = settings.value("locked-item-opacity", 0.3).toDouble();
        GlobalSettings::LvlItemDefaults.npc_direction = settings.value("defaults-npc-directuin", -1).toInt();
        GlobalSettings::LvlItemDefaults.npc_generator_type = settings.value("defaults-npc-gen-type", 1).toInt();
        GlobalSettings::LvlItemDefaults.npc_generator_delay = settings.value("defaults-npc-gen-delay", 20).toInt();
        LvlPlacingItems::npcSet.direct = GlobalSettings::LvlItemDefaults.npc_direction;
        LvlPlacingItems::npcSet.generator_type = GlobalSettings::LvlItemDefaults.npc_generator_type;
        LvlPlacingItems::npcSet.generator_period = GlobalSettings::LvlItemDefaults.npc_generator_delay;
        GlobalSettings::LvlItemDefaults.warp_type = settings.value("defaults-warp-type", 2).toInt();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_layviz     = settings.value("defaults-classicevents-tabs-layerviz", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_laymov     = settings.value("defaults-classicevents-tabs-layermov", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_autoscroll = settings.value("defaults-classicevents-tabs-autoscroll", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_secset     = settings.value("defaults-classicevents-tabs-secset", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_common     = settings.value("defaults-classicevents-tabs-common", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_buttons    = settings.value("defaults-classicevents-tabs-buttons", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_trigger    = settings.value("defaults-classicevents-tabs-trigger", 0).toBool();

        restoreGeometry(settings.value("geometry", saveGeometry()).toByteArray());
        restoreState(settings.value("windowState", saveState()).toByteArray());

        GlobalSettings::autoPlayMusic = settings.value("autoPlayMusic", false).toBool();
        GlobalSettings::musicVolume = settings.value("music-volume", 128).toInt();

        GlobalSettings::MidMouse_allowDuplicate = settings.value("editor-midmouse-allowdupe", true).toBool();
        GlobalSettings::MidMouse_allowSwitchToPlace = settings.value("editor-midmouse-allowplace", true).toBool();
        GlobalSettings::MidMouse_allowSwitchToDrag = settings.value("editor-midmouse-allowdrag", true).toBool();

        GlobalSettings::Placing_dontShowPropertiesBox = settings.value("editor-placing-no-propsbox", false).toBool();

        GlobalSettings::historyLimit = settings.value("history-limit", 300).toInt();

        GlobalSettings::MainWindowView = (settings.value("tab-view", true).toBool()) ? QMdiArea::TabbedView : QMdiArea::SubWindowView;
        GlobalSettings::LVLToolboxPos = static_cast<QTabWidget::TabPosition>(settings.value("level-toolbox-pos", static_cast<int>(QTabWidget::North)).toInt());
        GlobalSettings::WLDToolboxPos = static_cast<QTabWidget::TabPosition>(settings.value("world-toolbox-pos", static_cast<int>(QTabWidget::West)).toInt());
        GlobalSettings::TSTToolboxPos = static_cast<QTabWidget::TabPosition>(settings.value("tileset-toolbox-pos", static_cast<int>(QTabWidget::North)).toInt());

        GlobalSettings::currentTheme = settings.value("current-theme", "").toString();
        GlobalSettings::currentPallete = settings.value("current-pallete", "").toInt();
        GlobalSettings::fontSize = settings.value("font-size", -1).toInt();

        GlobalSettings::ShowTipOfDay = settings.value("show-tip-of-a-day", configs.editor.enable_tip_of_the_day).toBool();

        MixerX::initAudio(settings.value("sdl-sample-rate", MixerX::sampleRate()).toInt());

        GlobalSettings::openPath = settings.value("lastpath", QString()).toString();
        GlobalSettings::savePath = settings.value("lastsavepath", QString()).toString();
        GlobalSettings::savePath_npctxt = settings.value("lastsavepath-npctxt", QString()).toString();

        GlobalSettings::animatorItemsLimit = settings.value("animation-item-limit", "30000").toInt();

        //                         toolbox parameter prefix             pointer to toolbox      saved visibility state flag   defaults: vis.        force default visibility state                          flaoting
        loadToolboxProps(settings, QStringLiteral("level-item-box"),    dock_LvlItemBox,        configs.editor.default_visibility.lvl_itembox,      configs.editor.default_visibility_enforce.lvl_itembox,   configs.editor.default_widget_state.level_item_browser);
        loadToolboxProps(settings, QStringLiteral("level-itemprops-box"), dock_LvlItemProps,    false,  false, configs.editor.default_widget_state.level_item_properties);
        loadToolboxProps(settings, QStringLiteral("level-section-set"), dock_LvlSectionProps,   configs.editor.default_visibility.lvl_section_props,configs.editor.default_visibility_enforce.lvl_section_props,   configs.editor.default_widget_state.level_section_properties);
        loadToolboxProps(settings, QStringLiteral("level-warps-box"),   dock_LvlWarpProps,      configs.editor.default_visibility.lvl_warp_props,   configs.editor.default_visibility_enforce.lvl_warp_props,  configs.editor.default_widget_state.level_warps_box);
        loadToolboxProps(settings, QStringLiteral("level-layers"),      dock_LvlLayers,         configs.editor.default_visibility.lvl_layers,       configs.editor.default_visibility_enforce.lvl_layers,   configs.editor.default_widget_state.level_layers_box);
        loadToolboxProps(settings, QStringLiteral("level-events"),      dock_LvlEvents,         configs.editor.default_visibility.lvl_events,       configs.editor.default_visibility_enforce.lvl_events,   configs.editor.default_widget_state.level_classic_events_box);
        loadToolboxProps(settings, QStringLiteral("level-search"),      dock_LvlSearchBox,      configs.editor.default_visibility.lvl_search,       configs.editor.default_visibility_enforce.lvl_search,  configs.editor.default_widget_state.level_search_box);

        loadToolboxProps(settings, QStringLiteral("world-item-box"),    dock_WldItemBox,        configs.editor.default_visibility.wld_itembox,      configs.editor.default_visibility_enforce.wld_itembox,   configs.editor.default_widget_state.world_item_browser);
        loadToolboxProps(settings, QStringLiteral("world-muscboxes-box"), dock_WldMusicBoxes,   configs.editor.default_visibility.wld_musicboxes,   configs.editor.default_visibility_enforce.wld_musicboxes,configs.editor.default_widget_state.world_music_boxes);
        loadToolboxProps(settings, QStringLiteral("world-settings-box"), dock_WldSettingsBox,   configs.editor.default_visibility.wld_settings,     configs.editor.default_visibility_enforce.wld_settings,  configs.editor.default_widget_state.world_settings_box);
        loadToolboxProps(settings, QStringLiteral("world-itemprops-box"), dock_WldItemProps,    false,  false, configs.editor.default_widget_state.world_item_properties);
        loadToolboxProps(settings, QStringLiteral("world-search"),      dock_WldSearchBox,      configs.editor.default_visibility.wld_search,       configs.editor.default_visibility_enforce.wld_search,  configs.editor.default_widget_state.world_search_box);

        loadToolboxProps(settings, QStringLiteral("tileset-box"),       dock_TilesetBox,        configs.editor.default_visibility.tilesets_box,     configs.editor.default_visibility_enforce.tilesets_box, configs.editor.default_widget_state.tilesets_item_box);
        loadToolboxProps(settings, QStringLiteral("debugger-box"),      dock_DebuggerBox,       configs.editor.default_visibility.debugger_box,     configs.editor.default_visibility_enforce.debugger_box, configs.editor.default_widget_state.debugger_box);
        loadToolboxProps(settings, QStringLiteral("bookmarks-box"),     dock_BookmarksBox,      configs.editor.default_visibility.bookmarks_box,    configs.editor.default_visibility_enforce.bookmarks_box, configs.editor.default_widget_state.bookmarks_box);
        loadToolboxProps(settings, QStringLiteral("variables-box"),     dock_VariablesBox,      configs.editor.default_visibility.variables_box,    configs.editor.default_visibility_enforce.variables_box, configs.editor.default_widget_state.variables_box);

        ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
        dock_LvlItemBox->tabWidget()->setTabPosition(GlobalSettings::LVLToolboxPos);
        dock_WldItemBox->tabWidget()->setTabPosition(GlobalSettings::WLDToolboxPos);

        dock_TilesetBox->setTabPosition(GlobalSettings::TSTToolboxPos);

        if(!m_isAppInited)
        {
            //Hide all unitialized dock widgets
            QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();
            foreach(QDockWidget *dw, dockWidgets)
                dw->hide();

            //Refresh state of menubars
            updateWindowMenu();
            updateMenus(nullptr, true);
        }
    }
    settings.endGroup();

    settings.beginGroup("autosave");
    {
        GlobalSettings::LvlOpts.autoSave_enable = settings.value("enabled", true).toBool();
        GlobalSettings::LvlOpts.autoSave_interval = settings.value("interval", 3).toInt();
    }
    settings.endGroup();

    settings.beginGroup("ext-tools");
    {
        GlobalSettings::tools_sox_bin_path = settings.value("sox-bin-path", GlobalSettings::tools_sox_bin_path).toString();
    }
    settings.endGroup();

    settings.beginGroup("testing");
    {
        GlobalSettings::testing.enginePath = settings.value("engine-path", QString()).toString();
        GlobalSettings::testing.lunaLuaPath = settings.value("lunalua-path", QString()).toString();
        GlobalSettings::testing.xtra_god = settings.value("ex-god", false).toBool();
        GlobalSettings::testing.xtra_flyup = settings.value("ex-flyup", false).toBool();
        GlobalSettings::testing.xtra_chuck = settings.value("ex-chuck", false).toBool();
        GlobalSettings::testing.xtra_worldfreedom = settings.value("ex-wfree", false).toBool();
        GlobalSettings::testing.xtra_debug = settings.value("ex-debug", false).toBool();
        GlobalSettings::testing.xtra_showFPS = settings.value("ex-show-fps", false).toBool();
        GlobalSettings::testing.xtra_physdebug = settings.value("ex-physdebug", false).toBool();
        GlobalSettings::testing.numOfPlayers = settings.value("num-players", 1).toInt();
        GlobalSettings::testing.p1_char = settings.value("p1-char", 1).toInt();
        GlobalSettings::testing.p1_state = settings.value("p1-state", 1).toInt();
        GlobalSettings::testing.p1_vehicleID    = settings.value("p1-vehicle-id", 0).toInt();
        GlobalSettings::testing.p1_vehicleType  = settings.value("p1-vehicle-type", 0).toInt();
        GlobalSettings::testing.p2_char = settings.value("p2-char", 1).toInt();
        GlobalSettings::testing.p2_state = settings.value("p2-state", 1).toInt();
        GlobalSettings::testing.p2_vehicleID    = settings.value("p2-vehicle-id", 0).toInt();
        GlobalSettings::testing.p2_vehicleType  = settings.value("p2-vehicle-type", 0).toInt();
    }
    settings.endGroup();

    settings.beginGroup("screen-grab");
    {
        GlobalSettings::screenGrab.sizeType   = settings.value("grab-size", 0).toInt();
        GlobalSettings::screenGrab.width      = settings.value("grab-w", 800).toInt();
        GlobalSettings::screenGrab.height      = settings.value("grab-h", 600).toInt();
    }
    settings.endGroup();

    settings.beginGroup("Recent");
    {
        for(int i = 1; i <= 10; i++)
            recentOpen.push_back(settings.value("recent" + QString::number(i), "<empty>").toString());
        SyncRecentFiles();
    }
    settings.endGroup();

    settings.beginGroup("extra");
    {
        GlobalSettings::extra.attr_hdpi = settings.value("high-dpi-scaling", GlobalSettings::extra.attr_hdpi).toBool();
        GlobalSettings::extra.maintainerPath = settings.value("maintainer-path", QString()).toString();
    }
    settings.endGroup();
}





//////////Save settings into INI file///////////////
void MainWindow::saveSettings()
{
    QString inifile = AppPathManager::settingsFile();

    QSettings settings(inifile, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    settings.beginGroup("Main");
    {
        settings.setValue("pos", pos());
        settings.setValue("lastpath", GlobalSettings::openPath);
        settings.setValue("lastsavepath", GlobalSettings::savePath);
        settings.setValue("lastsavepath-npctxt", GlobalSettings::savePath_npctxt);

        saveToolboxProps(settings, QStringLiteral("level-item-box"),    dock_LvlItemBox);
        saveToolboxProps(settings, QStringLiteral("level-itemprops-box"), dock_LvlItemProps);
        saveToolboxProps(settings, QStringLiteral("level-section-set"), dock_LvlSectionProps);
        saveToolboxProps(settings, QStringLiteral("level-warps-box"),   dock_LvlWarpProps);
        saveToolboxProps(settings, QStringLiteral("level-layers"),      dock_LvlLayers);
        saveToolboxProps(settings, QStringLiteral("level-events"),      dock_LvlEvents);
        saveToolboxProps(settings, QStringLiteral("level-search"),      dock_LvlSearchBox);

        saveToolboxProps(settings, QStringLiteral("world-item-box"),    dock_WldItemBox);
        saveToolboxProps(settings, QStringLiteral("world-muscboxes-box"), dock_WldMusicBoxes);
        saveToolboxProps(settings, QStringLiteral("world-settings-box"), dock_WldSettingsBox);
        saveToolboxProps(settings, QStringLiteral("world-itemprops-box"), dock_WldItemProps);
        saveToolboxProps(settings, QStringLiteral("world-search"),      dock_WldSearchBox);

        saveToolboxProps(settings, QStringLiteral("tileset-box"),       dock_TilesetBox);
        saveToolboxProps(settings, QStringLiteral("debugger-box"),      dock_DebuggerBox);
        saveToolboxProps(settings, QStringLiteral("bookmarks-box"),     dock_BookmarksBox);
        saveToolboxProps(settings, QStringLiteral("variables-box"),     dock_VariablesBox);

        settings.setValue("geometry", saveGeometry());
        settings.setValue("windowState", saveState());

        settings.setValue("autoPlayMusic", GlobalSettings::autoPlayMusic);
        settings.setValue("music-volume", PGE_MusPlayer::currentVolume());

        settings.setValue("editor-midmouse-allowdupe", GlobalSettings::MidMouse_allowDuplicate);
        settings.setValue("editor-midmouse-allowplace", GlobalSettings::MidMouse_allowSwitchToPlace);
        settings.setValue("editor-midmouse-allowdrag", GlobalSettings::MidMouse_allowSwitchToDrag);

        settings.setValue("editor-placing-no-propsbox", GlobalSettings::Placing_dontShowPropertiesBox);

        settings.setValue("history-limit", GlobalSettings::historyLimit);

        settings.setValue("tab-view", (GlobalSettings::MainWindowView == QMdiArea::TabbedView));
        settings.setValue("level-toolbox-pos", static_cast<int>(GlobalSettings::LVLToolboxPos));
        settings.setValue("world-toolbox-pos", static_cast<int>(GlobalSettings::WLDToolboxPos));
        settings.setValue("tileset-toolbox-pos", static_cast<int>(GlobalSettings::TSTToolboxPos));

        settings.setValue("animation", GlobalSettings::LvlOpts.animationEnabled);
        settings.setValue("collisions", GlobalSettings::LvlOpts.collisionsEnabled);
        settings.setValue("grid-show", GlobalSettings::LvlOpts.grid_show);
        settings.setValue("camera-grid-show", GlobalSettings::LvlOpts.camera_grid_show);
        settings.setValue("default-zoom", GlobalSettings::LvlOpts.default_zoom);
        settings.setValue("animation-item-limit", QString::number(GlobalSettings::animatorItemsLimit));

        settings.setValue("defaults-npc-directuin", GlobalSettings::LvlItemDefaults.npc_direction);
        settings.setValue("defaults-npc-gen-type", GlobalSettings::LvlItemDefaults.npc_generator_type);
        settings.setValue("defaults-npc-gen-delay", GlobalSettings::LvlItemDefaults.npc_generator_delay);
        settings.setValue("defaults-warp-type", GlobalSettings::LvlItemDefaults.warp_type);
        settings.setValue("defaults-classicevents-tabs-layerviz", GlobalSettings::LvlItemDefaults.classicevents_tabs_layviz);
        settings.setValue("defaults-classicevents-tabs-layermov", GlobalSettings::LvlItemDefaults.classicevents_tabs_laymov);
        settings.setValue("defaults-classicevents-tabs-autoscroll", GlobalSettings::LvlItemDefaults.classicevents_tabs_autoscroll);
        settings.setValue("defaults-classicevents-tabs-secset", GlobalSettings::LvlItemDefaults.classicevents_tabs_secset);
        settings.setValue("defaults-classicevents-tabs-common", GlobalSettings::LvlItemDefaults.classicevents_tabs_common);
        settings.setValue("defaults-classicevents-tabs-buttons", GlobalSettings::LvlItemDefaults.classicevents_tabs_buttons);
        settings.setValue("defaults-classicevents-tabs-trigger", GlobalSettings::LvlItemDefaults.classicevents_tabs_trigger);

        settings.setValue("language", GlobalSettings::locale);

        settings.setValue("current-theme", GlobalSettings::currentTheme);
        settings.setValue("current-pallete", GlobalSettings::currentPallete);
        settings.setValue("font-size", GlobalSettings::fontSize);
        settings.setValue("show-tip-of-a-day", GlobalSettings::ShowTipOfDay);

        settings.setValue("sdl-sample-rate", MixerX::sampleRate());
    }
    settings.endGroup();

    settings.beginGroup("autosave");
    {
        settings.setValue("enabled", GlobalSettings::LvlOpts.autoSave_enable);
        settings.setValue("interval", GlobalSettings::LvlOpts.autoSave_interval);
    }
    settings.endGroup();

    settings.beginGroup("ext-tools");
    {
        settings.setValue("sox-bin-path", GlobalSettings::tools_sox_bin_path);
    }
    settings.endGroup();

    settings.beginGroup("testing");
    {
        settings.setValue("engine-path", GlobalSettings::testing.enginePath);
        settings.setValue("lunalua-path", GlobalSettings::testing.lunaLuaPath);
        settings.setValue("ex-god", GlobalSettings::testing.xtra_god);
        settings.setValue("ex-flyup", GlobalSettings::testing.xtra_flyup);
        settings.setValue("ex-chuck", GlobalSettings::testing.xtra_chuck);
        settings.setValue("ex-wfree", GlobalSettings::testing.xtra_worldfreedom);
        settings.setValue("ex-debug", GlobalSettings::testing.xtra_debug);
        settings.setValue("ex-show-fps", GlobalSettings::testing.xtra_showFPS);
        settings.setValue("ex-physdebug", GlobalSettings::testing.xtra_physdebug);
        settings.setValue("num-players", GlobalSettings::testing.numOfPlayers);
        settings.setValue("p1-char", GlobalSettings::testing.p1_char);
        settings.setValue("p1-state", GlobalSettings::testing.p1_state);
        settings.setValue("p1-vehicle-id", GlobalSettings::testing.p1_vehicleID);
        settings.setValue("p1-vehicle-type", GlobalSettings::testing.p1_vehicleType);
        settings.setValue("p2-char", GlobalSettings::testing.p2_char);
        settings.setValue("p2-state", GlobalSettings::testing.p2_state);
        settings.setValue("p2-vehicle-id", GlobalSettings::testing.p2_vehicleID);
        settings.setValue("p2-vehicle-type", GlobalSettings::testing.p2_vehicleType);
    }
    settings.endGroup();

    settings.beginGroup("screen-grab");
    {
        settings.setValue("grab-size", GlobalSettings::screenGrab.sizeType);
        settings.setValue("grab-w", GlobalSettings::screenGrab.width);
        settings.setValue("grab-h", GlobalSettings::screenGrab.height);
    }
    settings.endGroup();

    settings.beginGroup("extra");
    {
        settings.setValue("high-dpi-scaling", GlobalSettings::extra.attr_hdpi);
        settings.setValue("maintainer-path", GlobalSettings::extra.maintainerPath);
    }
    settings.endGroup();

    settings.beginGroup("Recent");
    {
        for(int i = 1; i <= 10; i++)
            settings.setValue("recent" + QString::number(i), recentOpen[i - 1]);
    }
    settings.endGroup();

    settings.beginGroup("logging");
    {
        settings.setValue("log-path",  LogWriter::DebugLogFile);
        settings.setValue("log-level", int(LogWriter::logLevel));
    }
    settings.endGroup();

    //Output-only
    settings.beginGroup("message-boxes");
    {
        settings.setValue("uidesign-editor-greeting", false);
    }
    settings.endGroup();

    //Save settings of custom counters in the debugger
    dock_DebuggerBox->Debugger_saveCustomCounters();
}


//Application settings
void MainWindow::on_actionApplication_settings_triggered()
{
    if(!m_isAppInited) return;

    AppSettings *appSettings = new AppSettings(this);
    util::DialogToCenter(appSettings, true);
    //appSettings->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    //appSettings->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, appSettings->size(), util::getScreenGeometry()));

    if(appSettings->exec() == QDialog::Accepted)
    {
        applySetup(false);
        saveSettings();
    }

    delete appSettings;
}

void MainWindow::applySetup(bool startup)
{
    ui->actionAnimation->setChecked(GlobalSettings::LvlOpts.animationEnabled);
    on_actionAnimation_triggered(GlobalSettings::LvlOpts.animationEnabled);

    ui->actionCollisions->setChecked(GlobalSettings::LvlOpts.collisionsEnabled);
    on_actionCollisions_triggered(GlobalSettings::LvlOpts.collisionsEnabled);

    ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
    dock_LvlItemBox->tabWidget()->setTabPosition(GlobalSettings::LVLToolboxPos);
    dock_WldItemBox->tabWidget()->setTabPosition(GlobalSettings::WLDToolboxPos);
    dock_TilesetBox->setTabPosition(GlobalSettings::TSTToolboxPos);

    applyCurrentTheme();

    if(GlobalSettings::fontSize > 0)
    {
        GlobalSettings::font->setPointSize(GlobalSettings::fontSize);
        qApp->setFont(*GlobalSettings::font);
    }
    else if(!startup)
    {
        qApp->setFont(*GlobalSettings::fontDefault);
    }

    updateAutoSaver();
}
