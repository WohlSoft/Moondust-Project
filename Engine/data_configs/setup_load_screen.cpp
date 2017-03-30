/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "setup_load_screen.h"
#include "config_manager.h"
#include "config_manager_private.h"

#include <fmt/fmt_format.h>

LoadingScreenSetup  ConfigManager::setup_LoadingScreen;

void LoadingScreenSetup::init(IniProcessing &engine_ini)
{
    int LoadScreenImages = 0;
    engine_ini.beginGroup("loading-scene");
    backgroundColor.setRgba(engine_ini.value("bg-color", "#000000").toString());
    backgroundImg = engine_ini.value("background", "").toString();
    ConfigManager::checkForImage(backgroundImg, ConfigManager::dirs.gcommon);

    updateDelay = engine_ini.value("updating-time", 128).toInt();
    luaFile = engine_ini.value("script", "main_loading.lua").toString();
    LoadScreenImages = engine_ini.value("additional-images", 0).toInt();
    engine_ini.endGroup();


    AdditionalImages.clear();
    for(int i = 1; i <= LoadScreenImages; i++)
    {
        engine_ini.beginGroup(fmt::format("loading-image-{0}", i));
        {
            LoadingScreenAdditionalImage img;

            img.imgFile = engine_ini.value("image", "").toString();
            ConfigManager::checkForImage(img.imgFile, ConfigManager::dirs.gcommon);
            engine_ini.read("animated", img.animated, false);
            if(img.animated)
                engine_ini.read("frames", img.frames, 1);
            else
                img.frames = 1;
            if(img.frames <= 0) img.frames = 1;
            engine_ini.read("frame-delay", img.frameDelay, updateDelay);
            engine_ini.read("pos-x", img.x, 1);
            engine_ini.read("pos-y", img.y, 1);

            AdditionalImages.push_back(img);
        }
        engine_ini.endGroup();
    }
}
