/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "global_settings.h"

EditingSettings::EditingSettings()
{
    animationEnabled=true;
    collisionsEnabled=true;
    grid_snap=true;
    grid_override=false;
    customGrid.setWidth(0);
    customGrid.setHeight(0);
    grid_show=false;
    camera_grid_show=false;
    semiTransparentPaths=false;
}

QString GlobalSettings::locale          = "";
long GlobalSettings::animatorItemsLimit = 25000;
QString GlobalSettings::openPath        = ".";
QString GlobalSettings::savePath        = ".";
QString GlobalSettings::savePath_npctxt = ".";

QString GlobalSettings::tools_sox_bin_path  = "/tools/sox/";

EditingSettings  GlobalSettings::LvlOpts;
SETTINGS_ItemDefaults GlobalSettings::LvlItemDefaults;

SETTINGS_TestSettings GlobalSettings::testing; //Testing settings
SETTINGS_ScreenGrabSettings GlobalSettings::screenGrab;

SETTINGS_Extra GlobalSettings::extra;

bool GlobalSettings::autoPlayMusic  = false;
int  GlobalSettings::musicVolume    = 54;
bool GlobalSettings::recentMusicPlayingState = false;

bool GlobalSettings::MidMouse_allowDuplicate    = true;
bool GlobalSettings::MidMouse_allowSwitchToPlace= true;
bool GlobalSettings::MidMouse_allowSwitchToDrag = true;

bool GlobalSettings::Placing_dontShowPropertiesBox  = false;

int  GlobalSettings::historyLimit   = 300;

QString GlobalSettings::currentTheme;
QFont   GlobalSettings::fontDefault;
QFont   GlobalSettings::font;
int     GlobalSettings::fontSize = -1;

bool GlobalSettings::ShowTipOfDay   = true;

QMdiArea::ViewMode      GlobalSettings::MainWindowView = QMdiArea::TabbedView;
QTabWidget::TabPosition GlobalSettings::LVLToolboxPos  = QTabWidget::North;
QTabWidget::TabPosition GlobalSettings::WLDToolboxPos  = QTabWidget::West;
QTabWidget::TabPosition GlobalSettings::TSTToolboxPos  = QTabWidget::North;

int GlobalSettings::lastWinType     = 0;


long ConfStatus::total_characters = 0;
long ConfStatus::total_blocks=0;
long ConfStatus::total_bgo=0;
long ConfStatus::total_bg=0;
long ConfStatus::total_npc=0;

long ConfStatus::total_wtile=0;
long ConfStatus::total_wpath=0;
long ConfStatus::total_wscene=0;
long ConfStatus::total_wlvl=0;

long ConfStatus::total_music_lvl=0;
long ConfStatus::total_music_wld=0;
long ConfStatus::total_music_spc=0;
long ConfStatus::total_sound=0;

QString ConfStatus::configName;
QString ConfStatus::configPath;

QString ConfStatus::configDataPath;

QString ConfStatus::SmbxEXE_Name;

ConfStatus::TestEngineType ConfStatus::defaultTestEngine = ConfStatus::ENGINE_PGE;
bool ConfStatus::hideNonDefaultEngines = false;

QString ConfStatus::defaultTheme;
