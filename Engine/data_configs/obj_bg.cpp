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

#include "config_manager.h"
#include "config_manager_private.h"
#include <common_features/number_limiter.h>
#include <common_features/util.h>
#include <fmt/fmt_format.h>
#include <Utils/files.h>
#include <graphics/gl_color.h>

/*****Level BG************/
PGE_DataArray<obj_BG>           ConfigManager::lvl_bg_indexes;
CustomDirManager                ConfigManager::Dir_BG;
ConfigManager::AnimatorsArray   ConfigManager::Animator_BG;
/*****Level BG************/

bool ConfigManager::loadLevelBackground(obj_BG &sbg, std::string section, obj_BG *merge_with, std::string iniFile, IniProcessing *setup)
{
    #define pMerge(param, def) (merge_with ? (merge_with->param) : (def))
    bool valid = true;
    bool internal = !setup;
    std::string errStr;

    std::unique_ptr<IniProcessing> ptr_guard;
    if(internal)
    {
        setup = new IniProcessing(iniFile);
        ptr_guard.reset(setup);
    }

    sbg.isInit = pMerge(isInit, false);
    sbg.image  = pMerge(image, nullptr);
    sbg.textureArrayId = pMerge(textureArrayId, 0);
    sbg.animator_ID = pMerge(animator_ID, 0);

    sbg.second_isInit = pMerge(second_isInit, false);
    sbg.second_image  = pMerge(second_image, nullptr);
    sbg.second_textureArrayId = pMerge(second_textureArrayId, 0);
    sbg.second_animator_ID = pMerge(second_animator_ID, 0);

    if(!setup->beginGroup(section) && internal)
        setup->beginGroup("General");
    {
        if(sbg.setup.parse(setup, BGPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
            valid = true;
        else
        {
            addError(errStr);
            valid = false;
        }
    }

    if(sbg.setup.fill_color != "auto")
    {
        GlColor color(sbg.setup.fill_color);
        sbg.fill_color.r = color.Red();
        sbg.fill_color.g = color.Green();
        sbg.fill_color.b = color.Blue();
    }

    setup->endGroup();

    return valid;
    #undef pMerge
}

bool ConfigManager::loadLevelBackG()
{
    unsigned int i;
    obj_BG sbg;
    unsigned long bg_total = 0;
    bool useDirectory = false;
    std::string bg_ini = config_dirSTD + "lvl_bkgrd.ini";
    std::string nestDir = "";

    if(!Files::fileExists(bg_ini))
    {
        addError("ERROR LOADING lvl_bkgrd.ini: file does not exist");
        return false;
    }

    IniProcessing bgset(bg_ini);
    lvl_bg_indexes.clear();//Clear old
    bgset.beginGroup("background2-main");
    bg_total =  bgset.value("total", 0).toULongLong();
    nestDir =   bgset.value("config-dir", "").toString();

    if(!nestDir.empty())
    {
        nestDir = config_dirSTD + nestDir;
        useDirectory = true;
    }

    bgset.endGroup();
    lvl_bg_indexes.allocateSlots(bg_total);

    for(i = 1; i <= bg_total; i++)
    {
        sbg.isInit = false;
        sbg.image = NULL;
        sbg.textureArrayId = 0;
        sbg.animator_ID = 0;
        sbg.second_isInit = false;
        sbg.second_image = NULL;
        sbg.second_textureArrayId = 0;
        sbg.second_animator_ID = 0;
        bool valid = false;

        if(useDirectory)
            valid = loadLevelBackground(sbg, "background2", nullptr, fmt::format("{0}/background2-{1}.ini", nestDir, i));
        else
            valid = loadLevelBackground(sbg, std::string("background2-{0}", i), 0, "", &bgset);

        if(valid)
        {
            sbg.setup.id = i;
            lvl_bg_indexes.storeElement(sbg.setup.id, sbg);
            //Load custom config if possible
            loadCustomConfig<obj_BG>(lvl_bg_indexes, i, Dir_BG, "background2", "background2", &loadLevelBackground, true);
        }

        if(bgset.lastError() != IniProcessing::ERR_OK)
            addError(fmt::format("ERROR LOADING lvl_bgrnd.ini N:{0} (background2-{1})", bgset.lastError(), i));
    }

    return true;
}
