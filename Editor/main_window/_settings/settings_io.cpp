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

//////////Load settings from INI file///////////////
void MainWindow::loadSettings()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

#ifdef Q_OS_WIN
#define FloatDocks true
#define FloatDocks_inv false
#else
#define FloatDocks false
#define FloatDocks_inv true
#endif


    settings.beginGroup("Main");
        //GlobalSettings::LastOpenDir = settings.value("lastpath", ".").toString();
        GlobalSettings::openPath = settings.value("lastpath", AppPathManager::userAppDir()).toString();
        GlobalSettings::savePath = settings.value("lastsavepath", AppPathManager::userAppDir()).toString();
        GlobalSettings::savePath_npctxt = settings.value("lastsavepath-npctxt", AppPathManager::userAppDir()).toString();

        GlobalSettings::LevelItemBoxVis = settings.value("level-tb-visible", "true").toBool();
        GlobalSettings::LevelSectionBoxVis = settings.value("section-tb-visible", "false").toBool();
        GlobalSettings::LevelDoorsBoxVis = settings.value("level-doors-vis", "false").toBool();
        GlobalSettings::LevelLayersBoxVis = settings.value("level-layers-vis", "false").toBool();
        GlobalSettings::LevelEventsBoxVis = settings.value("level-events-vis", "false").toBool();
        GlobalSettings::LevelSearchBoxVis = settings.value("level-search-vis", "false").toBool();

        GlobalSettings::WorldItemBoxVis = settings.value("world-tb-visible", "true").toBool();
        GlobalSettings::WorldSettingsToolboxVis = settings.value("world-props-visible", "false").toBool();
        GlobalSettings::WorldSearchBoxVis = settings.value("world-search-visible", "false").toBool();

        GlobalSettings::TilesetBoxVis = settings.value("tileset-box-visible", "false").toBool();
        GlobalSettings::DebuggerBoxVis = settings.value("debugger-box-visible", "false").toBool();
        GlobalSettings::BookmarksBoxVis = settings.value("bookmarks-box-visible", "false").toBool();

        GlobalSettings::LvlOpts.animationEnabled = settings.value("animation", "true").toBool();
        GlobalSettings::LvlOpts.collisionsEnabled = settings.value("collisions", "true").toBool();
        GraphicsHelps::EnableBitBlitMerge = settings.value("enable-gfx-fix", "true").toBool();

        GlobalSettings::LvlItemDefaults.npc_direction=settings.value("defaults-npc-directuin", -1).toInt();
        GlobalSettings::LvlItemDefaults.npc_generator_type=settings.value("defaults-npc-gen-type", 1).toInt();
        GlobalSettings::LvlItemDefaults.npc_generator_delay=settings.value("defaults-npc-gen-delay", 20).toInt();
            LvlPlacingItems::npcSet.direct=GlobalSettings::LvlItemDefaults.npc_direction;
            LvlPlacingItems::npcSet.generator_type=GlobalSettings::LvlItemDefaults.npc_generator_type;
            LvlPlacingItems::npcSet.generator_period=GlobalSettings::LvlItemDefaults.npc_generator_delay;
        GlobalSettings::LvlItemDefaults.warp_type=settings.value("defaults-warp-type", 2).toInt();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_layviz     = settings.value("defaults-classicevents-tabs-layerviz", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_laymov     = settings.value("defaults-classicevents-tabs-layermov", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_autoscroll = settings.value("defaults-classicevents-tabs-autoscroll", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_secset     = settings.value("defaults-classicevents-tabs-secset", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_common     = settings.value("defaults-classicevents-tabs-common", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_buttons    = settings.value("defaults-classicevents-tabs-buttons", 0).toBool();
        GlobalSettings::LvlItemDefaults.classicevents_tabs_trigger    = settings.value("defaults-classicevents-tabs-trigger", 0).toBool();

        restoreGeometry(settings.value("geometry", saveGeometry() ).toByteArray());
        restoreState(settings.value("windowState", saveState() ).toByteArray());

        GlobalSettings::autoPlayMusic = settings.value("autoPlayMusic", false).toBool();
        GlobalSettings::musicVolume = settings.value("music-volume",128).toInt();

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

        GlobalSettings::ShowTipOfDay = settings.value("show-tip-of-a-day", true).toBool();

        PGE_MusPlayer::setSampleRate(settings.value("sdl-sample-rate", PGE_MusPlayer::sampleRate()).toInt());

        dock_LvlWarpProps->setFloating(settings.value("doors-tool-box-float", FloatDocks).toBool());
        dock_LvlSectionProps->setFloating(settings.value("level-section-set-float", FloatDocks).toBool());
        dock_LvlLayers->setFloating(settings.value("level-layers-float", FloatDocks).toBool());
        dock_LvlEvents->setFloating(settings.value("level-events-float", FloatDocks).toBool());
        dock_LvlItemProps->setFloating(settings.value("item-props-box-float", FloatDocks).toBool());
        dock_LvlSearchBox->setFloating(settings.value("level-search-float", FloatDocks).toBool());
        dock_WldItemBox->setFloating(settings.value("world-item-box-float", false).toBool());
        dock_WldSettingsBox->setFloating(settings.value("world-settings-box-float", FloatDocks).toBool());
        dock_WldItemProps->setFloating(settings.value("world-itemprops-box-float", FloatDocks).toBool());
        dock_WldSearchBox->setFloating(settings.value("world-search-float", FloatDocks).toBool());
        dock_TilesetBox->setFloating(settings.value("tileset-box-float", FloatDocks).toBool());
        dock_DebuggerBox->setFloating(settings.value("debugger-box-float", FloatDocks).toBool());
        dock_BookmarksBox->setFloating(settings.value("bookmarks-box-float", FloatDocks).toBool());

        dock_LvlWarpProps->restoreGeometry(settings.value("doors-tool-box-geometry", dock_LvlWarpProps->saveGeometry()).toByteArray());
        dock_LvlSectionProps->restoreGeometry(settings.value("level-section-set-geometry", dock_LvlSectionProps->saveGeometry()).toByteArray());
        dock_LvlLayers->restoreGeometry(settings.value("level-layers-geometry", dock_LvlLayers->saveGeometry()).toByteArray());
        dock_LvlEvents->restoreGeometry(settings.value("level-events-geometry", dock_LvlLayers->saveGeometry()).toByteArray());
        dock_LvlItemProps->restoreGeometry(settings.value("item-props-box-geometry", dock_LvlItemProps->saveGeometry()).toByteArray());
        dock_LvlSearchBox->restoreGeometry(settings.value("level-search-geometry", dock_LvlSearchBox->saveGeometry()).toByteArray());
        dock_WldItemBox->restoreGeometry(settings.value("world-item-box-geometry", dock_WldItemBox->saveGeometry()).toByteArray());
        dock_WldSettingsBox->restoreGeometry(settings.value("world-settings-box-geometry", dock_WldSettingsBox->saveGeometry()).toByteArray());
        dock_WldItemProps->restoreGeometry(settings.value("world-itemprops-box-geometry", dock_WldItemProps->saveGeometry()).toByteArray());
        dock_WldSearchBox->restoreGeometry(settings.value("world-search-geometry", dock_WldSearchBox->saveGeometry()).toByteArray());

        dock_TilesetBox->restoreGeometry(settings.value("tileset-itembox-geometry", dock_TilesetBox->saveGeometry()).toByteArray());
        dock_DebuggerBox->restoreGeometry(settings.value("debugger-box-geometry", dock_DebuggerBox->saveGeometry()).toByteArray());
        dock_BookmarksBox->restoreGeometry(settings.value("bookmarks-box-geometry", dock_BookmarksBox->saveGeometry()).toByteArray());

        GlobalSettings::animatorItemsLimit = settings.value("animation-item-limit", "30000").toInt();

        ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
        dock_LvlItemBox->tabWidget()->setTabPosition(GlobalSettings::LVLToolboxPos);
        dock_WldItemBox->tabWidget()->setTabPosition(GlobalSettings::WLDToolboxPos);

        dock_TilesetBox->setTabPosition(GlobalSettings::TSTToolboxPos);

        if(!continueLoad)
        { //Hide all unitialized dock widgets
           QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();
           foreach(QDockWidget* dw, dockWidgets)
               dw->hide();

           //Refresh state of menubars
           updateWindowMenu();
           updateMenus(true);
        }

    settings.endGroup();

    settings.beginGroup("ext-tools");
        GlobalSettings::tools_sox_bin_path = settings.value("sox-bin-path", GlobalSettings::tools_sox_bin_path).toString();
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
    QString inifile = AppPathManager::settingsFile();

    QSettings settings(inifile, QSettings::IniFormat);
    settings.beginGroup("Main");
    settings.setValue("pos", pos());
    settings.setValue("lastpath", GlobalSettings::openPath);
    settings.setValue("lastsavepath", GlobalSettings::savePath);
    settings.setValue("lastsavepath-npctxt", GlobalSettings::savePath_npctxt);

    settings.setValue("world-tb-visible", GlobalSettings::WorldItemBoxVis);
    settings.setValue("world-props-visible", GlobalSettings::WorldSettingsToolboxVis);
    settings.setValue("world-search-visible", GlobalSettings::WorldSearchBoxVis);

    settings.setValue("level-tb-visible", GlobalSettings::LevelItemBoxVis);
    settings.setValue("section-tb-visible", GlobalSettings::LevelSectionBoxVis);
    settings.setValue("level-layers-vis", GlobalSettings::LevelLayersBoxVis);
    settings.setValue("level-events-vis", GlobalSettings::LevelEventsBoxVis);
    settings.setValue("level-doors-vis", GlobalSettings::LevelDoorsBoxVis);
    settings.setValue("level-search-vis", GlobalSettings::LevelSearchBoxVis);

    settings.setValue("tileset-box-visible", GlobalSettings::TilesetBoxVis);
    settings.setValue("debugger-box-visible", GlobalSettings::DebuggerBoxVis);
    settings.setValue("bookmarks-box-visible", GlobalSettings::BookmarksBoxVis);

    settings.setValue("doors-tool-box-float", dock_LvlWarpProps->isFloating());
    settings.setValue("level-section-set-float", dock_LvlSectionProps->isFloating());
    settings.setValue("level-layers-float", dock_LvlLayers->isFloating());
    settings.setValue("level-events-float", dock_LvlEvents->isFloating());
    settings.setValue("item-props-box-float", dock_LvlItemProps->isFloating());
    settings.setValue("level-search-float", dock_LvlSearchBox->isFloating());

    settings.setValue("world-item-box-float", dock_WldItemBox->isFloating());
    settings.setValue("world-settings-box-float", dock_WldSettingsBox->isFloating());
    settings.setValue("world-itemprops-box-float", dock_WldItemProps->isFloating());
    settings.setValue("world-search-float", dock_WldSearchBox->isFloating());
    settings.setValue("tileset-box-float", dock_TilesetBox->isFloating());
    settings.setValue("debugger-box-float", dock_DebuggerBox->isFloating());
    settings.setValue("bookmarks-box-float", dock_BookmarksBox->isFloating());

    settings.setValue("doors-tool-box-geometry", dock_LvlWarpProps->saveGeometry());
    settings.setValue("level-section-set-geometry", dock_LvlSectionProps->saveGeometry());
    settings.setValue("level-layers-geometry", dock_LvlLayers->saveGeometry());
    settings.setValue("level-events-geometry", dock_LvlEvents->saveGeometry());
    settings.setValue("item-props-box-geometry", dock_LvlItemProps->saveGeometry());
    settings.setValue("level-search-geometry", dock_LvlSearchBox->saveGeometry());

    settings.setValue("world-item-box-geometry", dock_WldItemBox->saveGeometry());
    settings.setValue("world-settings-box-geometry", dock_WldSettingsBox->saveGeometry());
    settings.setValue("world-itemprops-box-geometry", dock_WldItemProps->saveGeometry());
    settings.setValue("world-search-geometry", dock_WldSearchBox->saveGeometry());

    settings.setValue("tileset-itembox-geometry", dock_TilesetBox->saveGeometry());
    settings.setValue("debugger-box-geometry", dock_DebuggerBox->saveGeometry());
    settings.setValue("bookmarks-box-geometry", dock_BookmarksBox->saveGeometry());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("autoPlayMusic", GlobalSettings::autoPlayMusic);
    settings.setValue("music-volume", PGE_MusPlayer::currentVolume());

    settings.setValue("editor-midmouse-allowdupe", GlobalSettings::MidMouse_allowDuplicate);
    settings.setValue("editor-midmouse-allowplace", GlobalSettings::MidMouse_allowSwitchToPlace);
    settings.setValue("editor-midmouse-allowdrag", GlobalSettings::MidMouse_allowSwitchToDrag);

    settings.setValue("editor-placing-no-propsbox", GlobalSettings::Placing_dontShowPropertiesBox);

    settings.setValue("history-limit", GlobalSettings::historyLimit);

    settings.setValue("tab-view", (GlobalSettings::MainWindowView==QMdiArea::TabbedView));
    settings.setValue("level-toolbox-pos", static_cast<int>(GlobalSettings::LVLToolboxPos));
    settings.setValue("world-toolbox-pos", static_cast<int>(GlobalSettings::WLDToolboxPos));
    settings.setValue("tileset-toolbox-pos", static_cast<int>(GlobalSettings::TSTToolboxPos));

    settings.setValue("animation", GlobalSettings::LvlOpts.animationEnabled);
    settings.setValue("collisions", GlobalSettings::LvlOpts.collisionsEnabled);
    settings.setValue("animation-item-limit", QString::number(GlobalSettings::animatorItemsLimit));
    settings.setValue("enable-gfx-fix", GraphicsHelps::EnableBitBlitMerge);

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

    settings.setValue("current-config", currentConfigDir);
    settings.setValue("ask-config-again", askConfigAgain);
    settings.setValue("current-theme", GlobalSettings::currentTheme);
    settings.setValue("show-tip-of-a-day", GlobalSettings::ShowTipOfDay);

    settings.setValue("sdl-sample-rate", PGE_MusPlayer::sampleRate());

    settings.endGroup();

    settings.beginGroup("ext-tools");
    settings.setValue("sox-bin-path", GlobalSettings::tools_sox_bin_path);
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
            default:
                settings.setValue("log-level", "4"); break;
            }
        else
            settings.setValue("log-level", "0");
    settings.endGroup();

    //Save settings of custom counters in the debugger
    dock_DebuggerBox->Debugger_saveCustomCounters();
}


//Application settings
void MainWindow::on_actionApplication_settings_triggered()
{
    if(!continueLoad) return;

    AppSettings * appSettings = new AppSettings(this);
    util::DialogToCenter(appSettings, true);
    //appSettings->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    //appSettings->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, appSettings->size(), qApp->desktop()->availableGeometry()));

    appSettings->applySettings();

    if(appSettings->exec()==QDialog::Accepted)
    {
        ui->actionAnimation->setChecked(GlobalSettings::LvlOpts.animationEnabled);
        on_actionAnimation_triggered(GlobalSettings::LvlOpts.animationEnabled);
        ui->actionCollisions->setChecked(GlobalSettings::LvlOpts.collisionsEnabled);
        on_actionCollisions_triggered(GlobalSettings::LvlOpts.collisionsEnabled);

        ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
        dock_LvlItemBox->tabWidget()->setTabPosition(GlobalSettings::LVLToolboxPos);
        dock_WldItemBox->tabWidget()->setTabPosition(GlobalSettings::WLDToolboxPos);
        dock_TilesetBox->setTabPosition(GlobalSettings::TSTToolboxPos);

        applyTheme(GlobalSettings::currentTheme.isEmpty() ?
                       ( Themes::currentTheme().isEmpty() ? ConfStatus::defaultTheme : Themes::currentTheme() )
                     : GlobalSettings::currentTheme);

        saveSettings();
    }
    delete appSettings;

}

