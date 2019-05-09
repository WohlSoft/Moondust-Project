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

#include "setup_title_screen.h"
#include "config_manager.h"
#include "config_manager_private.h"

#include <IniProcessor/ini_processing.h>
#include <common_features/fmt_format_ne.h>
#include <unordered_map>

typedef std::unordered_map<std::string, TitleScreenAdditionalImage::align> TssAlignHash;
TssAlignHash TSS_AlignHash =
{
    {"left",    TitleScreenAdditionalImage::LEFT_ALIGN},
    {"top",     TitleScreenAdditionalImage::TOP_ALIGN},
    {"right",   TitleScreenAdditionalImage::RIGHT_ALIGN},
    {"bottom",  TitleScreenAdditionalImage::BOTTOM_ALIGN},
    {"center",  TitleScreenAdditionalImage::CENTER_ALIGN},
};

TitleScreenSetup ConfigManager::setup_TitleScreen;

void TitleScreenSetup::init(IniProcessing &engine_ini)
{
    int TitleScreenImages = 0;
    engine_ini.beginGroup("title-screen");
    {
        backgroundImg = engine_ini.value("background", "").toString();
        backgroundColor.setRgba(engine_ini.value("bg-color", "#000000").toString());
        ConfigManager::checkForImage(backgroundImg, ConfigManager::dirs.gcommon);
        luaFile = engine_ini.value("script", "main_title.lua").toString();
        TitleScreenImages = engine_ini.value("additional-images", 0).toInt();
    }
    engine_ini.endGroup();
    AdditionalImages.clear();

    for(int i = 1; i <= TitleScreenImages; i++)
    {
        engine_ini.beginGroup(fmt::format_ne("title-image-{0}", i));
        {
            TitleScreenAdditionalImage img;
            img.imgFile = engine_ini.value("image", "").toString();
            ConfigManager::checkForImage(img.imgFile, ConfigManager::dirs.gcommon);
            img.animated = engine_ini.value("animated", false).toBool();

            if(img.animated)
            {
                img.frames = engine_ini.value("frames", 1).toInt();
                img.framespeed = engine_ini.value("framespeed", 128).toUInt();
            }
            else
            {
                img.frames = 1;
                img.framespeed = 128;
            }

            if(img.frames <= 0) img.frames = 1;

            img.x =  engine_ini.value("pos-x", 0).toInt();
            img.y =  engine_ini.value("pos-y", 0).toInt();
            img.center_x =  engine_ini.value("center-x", false).toBool();
            img.center_y =  engine_ini.value("center-y", false).toBool();
            std::string align =   engine_ini.value("align", "none").toString();
            {
                TssAlignHash::iterator i = TSS_AlignHash.find(align);
                if(i == TSS_AlignHash.end())
                    img.align_to = TitleScreenAdditionalImage::NO_ALIGN;
                else
                    img.align_to = i->second;
            }

            AdditionalImages.push_back(img);
        }
        engine_ini.endGroup();
    }
}
