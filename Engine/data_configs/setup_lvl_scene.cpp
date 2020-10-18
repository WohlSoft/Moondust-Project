/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include <IniProcessor/ini_processing.h>

#include "setup_lvl_scene.h"
#include "config_manager.h"
#include "config_manager_private.h"
#include <fontman/font_manager.h>
#include <common_features/fmt_format_ne.h>

LevelSetup ConfigManager::setup_Level;

void LevelSetup::init(IniProcessing &engine_ini)
{
    engine_ini.beginGroup("level");
    {
        luaFile = engine_ini.value("script", "main_level.lua").toString();
    }
    engine_ini.endGroup();
}

