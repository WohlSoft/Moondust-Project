/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QFile>

#include <common_features/graphics_funcs.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>

#include "data_configs.h"


obj_block::obj_block()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = nullptr;
}


void obj_block::copyTo(obj_block &block)
{
    /* for internal usage */
    block.isValid         = isValid;
    block.animator_id     = animator_id;
    block.cur_image       = cur_image;
    if(cur_image==nullptr)
        block.cur_image   = &image;

    block.setup = setup;
}

/*!
 * \brief Loads signe block configuration
 * \param sblock Target block structure
 * \param section Name of INI-section where look for a block
 * \param merge_with Source of already loaded block structure to provide default settings per every block
 * \param iniFile INI-file where look for a block
 * \param setup loaded INI-file descriptor to load from global nested INI-file
 * \return true on success loading, false if error has occouped
 */
bool dataconfigs::loadLevelBlock(obj_block &sblock, QString section, obj_block *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal=!setup;
    QString errStr;
    if(internal) setup=new QSettings(iniFile, QSettings::IniFormat);

    if(!openSection( setup, section ))
        return false;

    if(sblock.setup.parse(setup, blockPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
    {
        sblock.isValid = true;
    } else {
        addError(errStr);
        sblock.isValid = false;
    }

    closeSection(setup);
    if(internal)
        delete setup;
    return valid;
}


void dataconfigs::loadLevelBlocks()
{
    unsigned int i;

    obj_block sblock;
    unsigned long block_total=0;
    bool useDirectory=false;

    QString block_ini = getFullIniPath("lvl_blocks.ini");
    if(block_ini.isEmpty())
        return;

    QString blocks_nestDir = "";

    QSettings blockset(block_ini, QSettings::IniFormat);
    blockset.setIniCodec("UTF-8");

    main_block.clear();   //Clear old
    //index_blocks.clear();

    if(!openSection(&blockset, "blocks-main")) return;
        block_total = blockset.value("total", 0).toUInt();
        total_data += block_total;
        blocks_nestDir = blockset.value("config-dir", "").toString();
        if(!blocks_nestDir.isEmpty())
        {
            blocks_nestDir = config_dir + blocks_nestDir;
            useDirectory = true;
        }
    closeSection(&blockset);

    emit progressPartNumber(2);
    emit progressMax(int(block_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Blocks..."));

    ConfStatus::total_blocks = signed(block_total);

    main_block.allocateSlots(int(block_total));

    if(ConfStatus::total_blocks==0)
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i<=block_total; i++)
    {
        emit progressValue(int(i));

        bool valid = false;
        if(useDirectory)
        {
            valid = loadLevelBlock(sblock, "block", nullptr, QString("%1/block-%2.ini").arg(blocks_nestDir).arg(i), &blockset);
        }
        else
        {
            valid = loadLevelBlock(sblock, QString("block-%1").arg(i), 0, "", &blockset);
        }

        /***************Load image*******************/
        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(blockPath,
               sblock.setup.image_n, sblock.setup.mask_n,
               sblock.image,
               errStr);

            if(!errStr.isEmpty())
            {
                valid=false;
                addError(QString("BLOCK-%1 %2").arg(i).arg(errStr));
            }
        }
        /***************Load image*end***************/
        sblock.setup.id = i;
        main_block.storeElement(int(i), sblock, valid);

        if( blockset.status()!=QSettings::NoError)
        {
            addError(QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(blockset.status()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }

    if(unsigned(main_block.stored()) < block_total)
    {
        addError(QString("Not all blocks loaded! Total: %1, Loaded: %2)").arg(block_total).arg(main_block.size()), PGE_LogLevel::Warning);
    }
}
