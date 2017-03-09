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
#include "../gui/pge_msgbox.h"
#include <common_features/graphics_funcs.h>
#include <common_features/number_limiter.h>
#include <Utils/files.h>
#include <fmt/fmt_format.h>

/*****Level BGO************/
PGE_DataArray<obj_bgo>          ConfigManager::lvl_bgo_indexes;
CustomDirManager                ConfigManager::Dir_BGO;
ConfigManager::AnimatorsArray   ConfigManager::Animator_BGO;
/*****Level BGO************/

bool ConfigManager::loadLevelBGO(obj_bgo &sbgo, std::string section, obj_bgo *merge_with, std::string iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    std::string errStr;

    if(internal)
    {
        setup = new IniProcessing(iniFile);
    }

    sbgo.isInit = merge_with ? merge_with->isInit : false;
    sbgo.image  = merge_with ? merge_with->image : nullptr;
    sbgo.textureArrayId = merge_with ? merge_with->textureArrayId : 0;
    sbgo.animator_ID = merge_with ? merge_with->animator_ID : 0;
    setup->beginGroup(section);

    if(sbgo.setup.parse(setup, bgoPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
        valid = true;
    else
    {
        addError(errStr);
        valid = false;
    }

    setup->endGroup();

    if(internal)
        delete setup;

    return valid;
}


bool ConfigManager::loadLevelBGO()
{
    unsigned int i;
    obj_bgo sbgo;
    unsigned long bgo_total = 0;
    bool useDirectory = false;
    std::string bgo_ini = config_dirSTD + "lvl_bgo.ini";
    std::string nestDir = "";

    if(!Files::fileExists(bgo_ini))
    {
        const char* msg = "ERROR LOADING lvl_bgo.ini: file does not exist";
        addError(msg);
        PGE_MsgBox msgBox(NULL, msg, PGE_MsgBox::msg_fatal);
        msgBox.exec();
        return false;
    }

    IniProcessing bgoset(bgo_ini);
    lvl_bgo_indexes.clear();//Clear old
    bgoset.beginGroup("background-main");
    bgo_total = bgoset.value("total", 0).toULongLong();
    nestDir =   bgoset.value("config-dir", "").toString();

    if(!nestDir.empty())
    {
        nestDir = config_dirSTD + nestDir;
        useDirectory = true;
    }

    bgoset.endGroup();
    lvl_bgo_indexes.allocateSlots(bgo_total);

    for(i = 1; i <= bgo_total; i++)
    {
        if(useDirectory)
        {
            if(!loadLevelBGO(sbgo, "background", nullptr, fmt::format("{0}/background-{1}.ini", nestDir, i)))
                return false;
        }
        else
        {
            if(!loadLevelBGO(sbgo, fmt::format("background-{0}", i), nullptr, "", &bgoset))
                return false;
        }

        sbgo.setup.id = i;
        //Store loaded config
        lvl_bgo_indexes.storeElement(sbgo.setup.id, sbgo);
        //Load custom config if possible
        loadCustomConfig<obj_bgo>(lvl_bgo_indexes, i, Dir_BGO, "background", "background", &loadLevelBGO);

        if(bgoset.lastError() != IniProcessing::ERR_OK)
            addError(fmt::format("ERROR LOADING lvl_bgo.ini N:{0} (bgo-{1})", bgoset.lastError(), i));
    }

    if(lvl_bgo_indexes.stored() < bgo_total)
    {
        std::string msg = fmt::format("Not all BGOs loaded! Total: {0}, Loaded: {1}", bgo_total, lvl_bgo_indexes.stored());
        addError(msg);
        PGE_MsgBox msgBox(NULL, msg, PGE_MsgBox::msg_error);
        msgBox.exec();
    }

    return true;
}
