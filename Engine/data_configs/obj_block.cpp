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
#include <gui/pge_msgbox.h>
#include <common_features/graphics_funcs.h>
#include <common_features/number_limiter.h>
#include <common_features/util.h>
#include <fmt/fmt_format.h>
#include <Utils/files.h>

/*****Level blocks************/
PGE_DataArray<obj_block>        ConfigManager::lvl_block_indexes;
CustomDirManager                ConfigManager::Dir_Blocks;
std::vector<SimpleAnimator >    ConfigManager::Animator_Blocks;
/*****Level blocks************/

bool ConfigManager::loadLevelBlock(obj_block &sblock, std::string section, obj_block *merge_with, std::string iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    std::string errStr;

    if(internal)
    {
        setup = new IniProcessing(iniFile);
    }

    sblock.isInit = false;
    sblock.image = NULL;
    sblock.textureArrayId = 0;
    sblock.animator_ID = -1;
    setup->beginGroup(section);

    if(sblock.setup.parse(setup, blockPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
        valid = true;
    else
    {
        addError(errStr);
        valid = false;
    }

    setup->endGroup();

    if(internal) delete setup;

    return valid;
}

bool ConfigManager::loadLevelBlocks()
{
    unsigned long i;
    obj_block sblock;
    unsigned long block_total = 0;
    bool useDirectory = false;
    std::string block_ini = config_dirSTD + "lvl_blocks.ini";
    std::string nestDir = "";

    if(!Files::fileExists(block_ini))
    {
        const char *msg = "ERROR LOADING lvl_blocks.ini: file does not exist";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    IniProcessing setup(block_ini);
    lvl_block_indexes.clear();//Clear old
    setup.beginGroup("blocks-main");
    {
        setup.read("total",      block_total,   0);
        setup.read("config-dir", nestDir,       "");
        if(!nestDir.empty())
        {
            nestDir = config_dirSTD + nestDir;
            useDirectory = true;
        }
    }
    setup.endGroup();

    if(block_total == 0)
    {
        const char *msg = "ERROR LOADING lvl_blocks.ini: number of items not define, or empty config";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    lvl_block_indexes.allocateSlots(block_total);

    for(i = 1; i <= block_total; i++)
    {
        if(useDirectory)
        {
            if(!loadLevelBlock(sblock, "block", nullptr, fmt::format("{0}/block-{1}.ini", nestDir, i)))
                return false;
        }
        else
        {
            if(!loadLevelBlock(sblock, fmt::format("block-{0}", i), nullptr, "", &setup))
                return false;
        }

        sblock.setup.id = i;
        //Store loaded config
        lvl_block_indexes.storeElement(sblock.setup.id, sblock);
        //Load custom config if possible
        loadCustomConfig<obj_block>(lvl_block_indexes, i, Dir_Blocks, "block", "block", &loadLevelBlock);

        if(setup.lastError() != IniProcessing::ERR_OK)
        {
            std::string msg = fmt::format("ERROR LOADING lvl_blocks.ini N:{0} (block-{1})", setup.lastError(), i);
            addError(msg);
            PGE_MsgBox::error(msg);
            break;
        }
    }

    if(uint(lvl_block_indexes.stored()) < block_total)
        addError(fmt::format("Not all blocks loaded! Total: {0}, Loaded: {1})", block_total, lvl_block_indexes.stored()));

    return true;
}
