/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <gui/pge_msgbox.h>
#include <common_features/graphics_funcs.h>
#include <common_features/number_limiter.h>
#include <common_features/util.h>

/*****Level blocks************/
PGE_DataArray<obj_block>   ConfigManager::lvl_block_indexes;
CustomDirManager        ConfigManager::Dir_Blocks;
QList<SimpleAnimator >  ConfigManager::Animator_Blocks;
/*****Level blocks************/

bool ConfigManager::loadLevelBlock(obj_block &sblock, QString section, obj_block *merge_with, QString iniFile, QSettings *setup)
{
    bool valid = true;
    bool internal = !setup;
    QString errStr;

    if(internal)
    {
        setup = new QSettings(iniFile, QSettings::IniFormat);
        setup->setIniCodec("UTF-8");
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
    QString block_ini = config_dir + "lvl_blocks.ini";
    QString nestDir = "";

    if(!QFile::exists(block_ini))
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal(QString("ERROR LOADING lvl_blocks.ini: file does not exist"));
        return false;
    }

    QSettings setup(block_ini, QSettings::IniFormat);
    setup.setIniCodec("UTF-8");
    lvl_block_indexes.clear();//Clear old
    setup.beginGroup("blocks-main");
    block_total = setup.value("total", 0).toULongLong();
    nestDir =     setup.value("config-dir", "").toString();

    if(!nestDir.isEmpty())
    {
        nestDir = config_dir + nestDir;
        useDirectory = true;
    }

    setup.endGroup();

    if(block_total == 0)
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal(QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"));
        return false;
    }

    lvl_block_indexes.allocateSlots(block_total);

    for(i = 1; i <= block_total; i++)
    {
        if(useDirectory)
        {
            if(!loadLevelBlock(sblock, "block", nullptr, QString("%1/block-%2.ini").arg(nestDir).arg(i)))
                return false;
        }
        else
        {
            if(!loadLevelBlock(sblock, QString("block-%1").arg(i), nullptr, "", &setup))
                return false;
        }

        sblock.setup.id = i;
        //Store loaded config
        lvl_block_indexes.storeElement(sblock.setup.id, sblock);
        //Load custom config if possible
        loadCustomConfig<obj_block>(lvl_block_indexes, i, Dir_Blocks, "block", "block", &loadLevelBlock);

        if(setup.status() != QSettings::NoError)
        {
            addError(QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(setup.status()).arg(i), QtCriticalMsg);
            PGE_MsgBox::error(QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(setup.status()).arg(i));
            break;
        }
    }

    if(uint(lvl_block_indexes.stored()) < block_total)
        addError(QString("Not all blocks loaded! Total: %1, Loaded: %2)").arg(block_total).arg(lvl_block_indexes.stored()), QtWarningMsg);

    return true;
}
