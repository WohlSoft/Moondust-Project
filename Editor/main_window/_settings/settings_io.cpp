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
#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>
#include <main_window/tools/app_settings.h>
#include <main_window/dock/toolboxes.h>
#include <audio/sdl_music_player.h>
#include <audio/music_player.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

//////////Load settings from INI file///////////////
void MainWindow::loadSettings()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
        //GlobalSettings::LastOpenDir = settings.value("lastpath", ".").toString();
        GlobalSettings::openPath = settings.value("lastpath", AppPathManager::userAppDir()).toString();
        GlobalSettings::savePath = settings.value("lastsavepath", AppPathManager::userAppDir()).toString();
        GlobalSettings::savePath_npctxt = settings.value("lastsavepath-npctxt", AppPathManager::userAppDir()).toString();

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
        GlobalSettings::BookmarksBoxVis = settings.value("bookmarks-box-visible", "false").toBool();

        GlobalSettings::LvlOpts.animationEnabled = settings.value("animation", "true").toBool();
        GlobalSettings::LvlOpts.collisionsEnabled = settings.value("collisions", "true").toBool();
        GraphicsHelps::EnableVBEmulate = settings.value("enable-gfx-fix", "true").toBool();

        restoreGeometry(settings.value("geometry", saveGeometry() ).toByteArray());
        restoreState(settings.value("windowState", saveState() ).toByteArray());

        GlobalSettings::autoPlayMusic = settings.value("autoPlayMusic", false).toBool();
        GlobalSettings::musicVolume = settings.value("music-volume",100).toInt();

        GlobalSettings::MidMouse_allowDuplicate = settings.value("editor-midmouse-allowdupe", true).toBool();
        GlobalSettings::MidMouse_allowSwitchToPlace = settings.value("editor-midmouse-allowplace", true).toBool();
        GlobalSettings::MidMouse_allowSwitchToDrag = settings.value("editor-midmouse-allowdrag", true).toBool();

        GlobalSettings::Placing_dontShowPropertiesBox = settings.value("editor-placing-no-propsbox", false).toBool();

        GlobalSettings::MainWindowView = (settings.value("tab-view", true).toBool()) ? QMdiArea::TabbedView : QMdiArea::SubWindowView;
        GlobalSettings::LVLToolboxPos = static_cast<QTabWidget::TabPosition>(settings.value("level-toolbox-pos", static_cast<int>(QTabWidget::North)).toInt());
        GlobalSettings::WLDToolboxPos = static_cast<QTabWidget::TabPosition>(settings.value("world-toolbox-pos", static_cast<int>(QTabWidget::West)).toInt());
        GlobalSettings::TSTToolboxPos = static_cast<QTabWidget::TabPosition>(settings.value("tileset-toolbox-pos", static_cast<int>(QTabWidget::North)).toInt());

        GlobalSettings::currentTheme = settings.value("current-theme", "").toString();

        PGE_MusPlayer::setSampleRate(settings.value("sdl-sample-rate", PGE_MusPlayer::sampleRate()).toInt());

        dock_LvlWarpProps->setFloating(settings.value("doors-tool-box-float", true).toBool());
        ui->LevelSectionSettings->setFloating(settings.value("level-section-set-float", true).toBool());
        ui->LevelLayers->setFloating(settings.value("level-layers-float", true).toBool());
        ui->LevelEventsToolBox->setFloating(settings.value("level-events-float", true).toBool());
        dock_LvlItemProps->setFloating(settings.value("item-props-box-float", true).toBool());
        ui->FindDock->setFloating(settings.value("level-search-float", true).toBool());
        //ui->WorldToolBox->setFloating(settings.value("world-item-box-float", false).toBool());
        ui->WorldSettings->setFloating(settings.value("world-settings-box-float", true).toBool());
        ui->WLD_ItemProps->setFloating(settings.value("world-itemprops-box-float", true).toBool());
        ui->WorldFindDock->setFloating(settings.value("world-search-float", true).toBool());
        dock_TilesetBox->setFloating(settings.value("tileset-box-float", true).toBool());
        ui->debuggerBox->setFloating(settings.value("debugger-box-float", true).toBool());
        ui->bookmarkBox->setFloating(settings.value("bookmarks-box-float", true).toBool());

        dock_LvlWarpProps->restoreGeometry(settings.value("doors-tool-box-geometry", dock_LvlWarpProps->saveGeometry()).toByteArray());
        ui->LevelSectionSettings->restoreGeometry(settings.value("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry()).toByteArray());
        ui->LevelLayers->restoreGeometry(settings.value("level-layers-geometry", ui->LevelLayers->saveGeometry()).toByteArray());
        ui->LevelEventsToolBox->restoreGeometry(settings.value("level-events-geometry", ui->LevelLayers->saveGeometry()).toByteArray());
        dock_LvlItemProps->restoreGeometry(settings.value("item-props-box-geometry", dock_LvlItemProps->saveGeometry()).toByteArray());
        ui->FindDock->restoreGeometry(settings.value("level-search-geometry", ui->FindDock->saveGeometry()).toByteArray());
        ui->WorldToolBox->restoreGeometry(settings.value("world-item-box-geometry", ui->WorldToolBox->saveGeometry()).toByteArray());
        ui->WorldSettings->restoreGeometry(settings.value("world-settings-box-geometry", ui->WorldSettings->saveGeometry()).toByteArray());
        ui->WLD_ItemProps->restoreGeometry(settings.value("world-itemprops-box-geometry", ui->WLD_ItemProps->saveGeometry()).toByteArray());
        ui->WorldFindDock->restoreGeometry(settings.value("world-search-geometry", ui->WorldFindDock->saveGeometry()).toByteArray());

        dock_TilesetBox->restoreGeometry(settings.value("tileset-itembox-geometry", dock_TilesetBox->saveGeometry()).toByteArray());
        ui->debuggerBox->restoreGeometry(settings.value("debugger-box-geometry", ui->debuggerBox->saveGeometry()).toByteArray());
        ui->bookmarkBox->restoreGeometry(settings.value("bookmarks-box-geometry", ui->bookmarkBox->saveGeometry()).toByteArray());

        GlobalSettings::animatorItemsLimit = settings.value("animation-item-limit", "30000").toInt();

        ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
        ui->LevelToolBoxTabs->setTabPosition(GlobalSettings::LVLToolboxPos);
        ui->WorldToolBoxTabs->setTabPosition(GlobalSettings::WLDToolboxPos);

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
    settings.setValue("bookmarks-box-visible", GlobalSettings::BookmarksBoxVis);

    settings.setValue("doors-tool-box-float", dock_LvlWarpProps->isFloating());
    settings.setValue("level-section-set-float", ui->LevelSectionSettings->isFloating());
    settings.setValue("level-layers-float", ui->LevelLayers->isFloating());
    settings.setValue("level-events-float", ui->LevelEventsToolBox->isFloating());
    settings.setValue("item-props-box-float", dock_LvlItemProps->isFloating());
    settings.setValue("level-search-float", ui->FindDock->isFloating());

    settings.setValue("world-item-box-float", ui->WorldToolBox->isFloating());
    settings.setValue("world-settings-box-float", ui->WorldSettings->isFloating());
    settings.setValue("world-itemprops-box-float", ui->WLD_ItemProps->isFloating());
    settings.setValue("world-search-float", ui->WorldFindDock->isFloating());
    settings.setValue("tileset-box-float", dock_TilesetBox->isFloating());
    settings.setValue("debugger-box-float", ui->debuggerBox->isFloating());
    settings.setValue("bookmarks-box-float", ui->bookmarkBox->isFloating());

    settings.setValue("doors-tool-box-geometry", dock_LvlWarpProps->saveGeometry());
    settings.setValue("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry());
    settings.setValue("level-layers-geometry", ui->LevelLayers->saveGeometry());
    settings.setValue("level-events-geometry", ui->LevelEventsToolBox->saveGeometry());
    settings.setValue("item-props-box-geometry", dock_LvlItemProps->saveGeometry());
    settings.setValue("level-search-geometry", ui->FindDock->saveGeometry());

    settings.setValue("world-item-box-geometry", ui->WorldToolBox->saveGeometry());
    settings.setValue("world-settings-box-geometry", ui->WorldSettings->saveGeometry());
    settings.setValue("world-itemprops-box-geometry", ui->WLD_ItemProps->saveGeometry());
    settings.setValue("world-search-geometry", ui->WorldFindDock->saveGeometry());

    settings.setValue("tileset-itembox-geometry", dock_TilesetBox->saveGeometry());
    settings.setValue("debugger-box-geometry", ui->debuggerBox->saveGeometry());
    settings.setValue("bookmarks-box-geometry", ui->bookmarkBox->saveGeometry());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("autoPlayMusic", GlobalSettings::autoPlayMusic);
    settings.setValue("music-volume", PGE_MusPlayer::currentVolume());

    settings.setValue("editor-midmouse-allowdupe", GlobalSettings::MidMouse_allowDuplicate);
    settings.setValue("editor-midmouse-allowplace", GlobalSettings::MidMouse_allowSwitchToPlace);
    settings.setValue("editor-midmouse-allowdrag", GlobalSettings::MidMouse_allowSwitchToDrag);

    settings.setValue("editor-placing-no-propsbox", GlobalSettings::Placing_dontShowPropertiesBox);

    settings.setValue("tab-view", (GlobalSettings::MainWindowView==QMdiArea::TabbedView));
    settings.setValue("level-toolbox-pos", static_cast<int>(GlobalSettings::LVLToolboxPos));
    settings.setValue("world-toolbox-pos", static_cast<int>(GlobalSettings::WLDToolboxPos));
    settings.setValue("tileset-toolbox-pos", static_cast<int>(GlobalSettings::TSTToolboxPos));

    settings.setValue("animation", GlobalSettings::LvlOpts.animationEnabled);
    settings.setValue("collisions", GlobalSettings::LvlOpts.collisionsEnabled);
    settings.setValue("animation-item-limit", QString::number(GlobalSettings::animatorItemsLimit));
    settings.setValue("enable-gfx-fix", GraphicsHelps::EnableVBEmulate);

    settings.setValue("language", GlobalSettings::locale);

    settings.setValue("current-config", currentConfigDir);
    settings.setValue("ask-config-again", askConfigAgain);
    settings.setValue("current-theme", GlobalSettings::currentTheme);

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

    //Save settings of custom counters in the debugger
    Debugger_saveCustomCounters();
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
    appSettings->TSTToolboxPos = GlobalSettings::TSTToolboxPos;

    appSettings->midmouse_allowDupe = GlobalSettings::MidMouse_allowDuplicate;
    appSettings->midmouse_allowPlace = GlobalSettings::MidMouse_allowSwitchToPlace;
    appSettings->midmouse_allowDragMode = GlobalSettings::MidMouse_allowSwitchToDrag;

    appSettings->placing_dont_show_props_box = GlobalSettings::Placing_dontShowPropertiesBox;

    appSettings->selectedTheme = GlobalSettings::currentTheme;

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
        GlobalSettings::TSTToolboxPos = appSettings->TSTToolboxPos;
        GlobalSettings::currentTheme = appSettings->selectedTheme;

        GlobalSettings::MidMouse_allowDuplicate = appSettings->midmouse_allowDupe;
        GlobalSettings::MidMouse_allowSwitchToPlace = appSettings->midmouse_allowPlace;
        GlobalSettings::MidMouse_allowSwitchToDrag = appSettings->midmouse_allowDragMode;

        GlobalSettings::Placing_dontShowPropertiesBox = appSettings->placing_dont_show_props_box;

        ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
        ui->LevelToolBoxTabs->setTabPosition(GlobalSettings::LVLToolboxPos);
        ui->WorldToolBoxTabs->setTabPosition(GlobalSettings::WLDToolboxPos);
        dock_TilesetBox->setTabPosition(GlobalSettings::TSTToolboxPos);

        applyTheme(GlobalSettings::currentTheme.isEmpty() ?
                       ( Themes::currentTheme().isEmpty() ? ConfStatus::defaultTheme : Themes::currentTheme() )
                     : GlobalSettings::currentTheme);

        saveSettings();
    }
    delete appSettings;

}

