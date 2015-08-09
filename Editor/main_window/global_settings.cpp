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

#include "global_settings.h"

QString GlobalSettings::locale="";
long GlobalSettings::animatorItemsLimit=25000;
QString GlobalSettings::openPath=".";
QString GlobalSettings::savePath=".";
QString GlobalSettings::savePath_npctxt=".";

QString GlobalSettings::tools_sox_bin_path="/tools/sox/";

LevelEditingSettings  GlobalSettings::LvlOpts;
SETTINGS_ItemDefaults GlobalSettings::LvlItemDefaults;

bool GlobalSettings::autoPlayMusic=false;
int GlobalSettings::musicVolume=128;

bool GlobalSettings::WorldItemBoxVis=true;
bool GlobalSettings::WorldSettingsToolboxVis=false;
bool GlobalSettings::WorldSearchBoxVis=false;

bool GlobalSettings::LevelItemBoxVis=true;
bool GlobalSettings::LevelSectionBoxVis=false;
bool GlobalSettings::LevelDoorsBoxVis=false;
bool GlobalSettings::LevelLayersBoxVis=false;
bool GlobalSettings::LevelEventsBoxVis=false;
bool GlobalSettings::LevelSearchBoxVis=false;

bool GlobalSettings::TilesetBoxVis=false;
bool GlobalSettings::DebuggerBoxVis=false;
bool GlobalSettings::BookmarksBoxVis=false;

bool GlobalSettings::MidMouse_allowDuplicate=true;
bool GlobalSettings::MidMouse_allowSwitchToPlace=true;
bool GlobalSettings::MidMouse_allowSwitchToDrag=true;

bool GlobalSettings::Placing_dontShowPropertiesBox=false;

int  GlobalSettings::historyLimit=300;

QString GlobalSettings::currentTheme="";

bool GlobalSettings::ShowTipOfDay=true;

QMdiArea::ViewMode GlobalSettings::MainWindowView = QMdiArea::TabbedView;
QTabWidget::TabPosition GlobalSettings::LVLToolboxPos = QTabWidget::North;
QTabWidget::TabPosition GlobalSettings::WLDToolboxPos = QTabWidget::West;
QTabWidget::TabPosition GlobalSettings::TSTToolboxPos = QTabWidget::North;

int GlobalSettings::lastWinType=0;
