/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QSettings>

#include "setup_credits_screen.h"
#include "config_manager.h"

CreditsScreenSetup ConfigManager::setup_CreditsScreen;

void CreditsScreenSetup::init(QSettings &engine_ini)
{
    int LoadScreenImages=0;
    engine_ini.beginGroup("credits-scene");
        backgroundColor.setNamedColor(engine_ini.value("bg-color", "#000000").toString());
        backgroundImg = engine_ini.value("background", "").toString();
        ConfigManager::checkForImage(backgroundImg, ConfigManager::dirs.gcommon);

        updateDelay = engine_ini.value("updating-time", 128).toInt();
        luaFile = engine_ini.value("script", "main_credits.lua").toString();
        LoadScreenImages = engine_ini.value("additional-images", 0).toInt();
    engine_ini.endGroup();


    AdditionalImages.clear();
    for(int i=1; i<=LoadScreenImages; i++)
    {
        engine_ini.beginGroup(QString("credits-image-%1").arg(i));
            CreditsScreenAdditionalImage img;

            img.imgFile = engine_ini.value("image", "").toString();
            ConfigManager::checkForImage(img.imgFile, ConfigManager::dirs.gcommon);

            img.animated = engine_ini.value("animated", false).toBool();
            if(img.animated)
                img.frames = engine_ini.value("frames", 1).toInt();
            else
                img.frames = 1;
            if(img.frames<=0) img.frames = 1;

            img.x =  engine_ini.value("pos-x", 1).toInt();
            img.y =  engine_ini.value("pos-y", 1).toInt();
            AdditionalImages.push_back(img);
        engine_ini.endGroup();
    }
}

