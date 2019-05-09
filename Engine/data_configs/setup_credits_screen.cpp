/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "setup_credits_screen.h"
#include "config_manager.h"
#include "config_manager_private.h"

#include <IniProcessor/ini_processing.h>
#include <common_features/fmt_format_ne.h>

CreditsScreenSetup ConfigManager::setup_CreditsScreen;

void CreditsScreenSetup::init(IniProcessing &engine_ini)
{
    int LoadScreenImages = 0;
    engine_ini.beginGroup("credits-scene");
    {
        backgroundColor.setRgba(engine_ini.value("bg-color", "#000000").toString());
        backgroundImg = engine_ini.value("background", "").toString();
        ConfigManager::checkForImage(backgroundImg, ConfigManager::dirs.gcommon);
        updateDelay = engine_ini.value("updating-time", 128).toInt();
        luaFile = engine_ini.value("script", "main_credits.lua").toString();
        LoadScreenImages = engine_ini.value("additional-images", 0).toInt();
    }
    engine_ini.endGroup();
    AdditionalImages.clear();

    for(int i = 1; i <= LoadScreenImages; i++)
    {
        engine_ini.beginGroup(fmt::format_ne("credits-image-{0}", i));
        {
            CreditsScreenAdditionalImage img;
            img.imgFile = engine_ini.value("image", "").toString();
            ConfigManager::checkForImage(img.imgFile, ConfigManager::dirs.gcommon);
            img.animated = engine_ini.value("animated", false).toBool();

            if(img.animated)
                img.frames = engine_ini.value("frames", 1).toInt();
            else
                img.frames = 1;

            if(img.frames <= 0) img.frames = 1;

            img.x =  engine_ini.value("pos-x", 1).toInt();
            img.y =  engine_ini.value("pos-y", 1).toInt();
            AdditionalImages.push_back(img);
        }
        engine_ini.endGroup();
    }
}
