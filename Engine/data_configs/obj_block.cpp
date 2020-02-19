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

#include "config_manager.h"
#include "config_manager_private.h"
#include <gui/pge_msgbox.h>
#include <common_features/graphics_funcs.h>
#include <common_features/number_limiter.h>
#include <common_features/util.h>
#include <common_features/fmt_format_ne.h>
#include <Utils/files.h>

/*****Level blocks************/
obj_blockGlobalSetup            ConfigManager::lvl_block_global_setup;
PGE_DataArray<obj_block>        ConfigManager::lvl_block_indexes;
CustomDirManager                ConfigManager::Dir_Blocks;
ConfigManager::AnimatorsArray   ConfigManager::Animator_Blocks;
/*****Level blocks************/

bool ConfigManager::loadLevelBlock(obj_block &sblock, std::string section, obj_block *merge_with, std::string iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    std::string errStr;

    std::unique_ptr<IniProcessing> ptr_guard;
    if(internal)
    {
        setup = new IniProcessing(iniFile);
        ptr_guard.reset(setup);
    }

    sblock.isInit = false;
    sblock.image = NULL;
    sblock.textureArrayId = 0;
    sblock.animator_ID = -1;

    if(!setup->beginGroup(section) && internal)
        setup->beginGroup("General");
    {
        if(sblock.setup.parse(setup, blockPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
            valid = true;
        else
        {
            addError(errStr);
            valid = false;
        }
    }
    setup->endGroup();

    return valid;
}

bool ConfigManager::loadLevelBlocks()
{
    uint32_t i;
    obj_block sblock;
    uint32_t block_total = 0;
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
        setup.read("default-sizable-border-width", lvl_block_global_setup.sizable_block_border_size, -1);

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
            if(!loadLevelBlock(sblock, "block", nullptr, fmt::format_ne("{0}/block-{1}.ini", nestDir, i)))
                return false;
        }
        else
        {
            if(!loadLevelBlock(sblock, fmt::format_ne("block-{0}", i), nullptr, "", &setup))
                return false;
        }

        sblock.setup.id = i;
        //Store loaded config
        lvl_block_indexes.storeElement(sblock.setup.id, sblock);
        //Load custom config if possible
        loadCustomConfig<obj_block>(lvl_block_indexes, i, Dir_Blocks, "block", "block", &loadLevelBlock);

        if(setup.lastError() != IniProcessing::ERR_OK)
        {
            std::string msg = fmt::format_ne("ERROR LOADING lvl_blocks.ini N:{0} (block-{1})", setup.lastError(), i);
            addError(msg);
            PGE_MsgBox::error(msg);
            break;
        }
    }

    if(uint32_t(lvl_block_indexes.stored()) < block_total)
        addError(fmt::format_ne("Not all blocks loaded! Total: {0}, Loaded: {1})", block_total, lvl_block_indexes.stored()));

    return true;
}
