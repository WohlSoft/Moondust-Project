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
#include <tgmath.h>

#include <common_features/graphics_funcs.h>
#include <number_limiter.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>

#include "data_configs.h"

obj_npc::obj_npc()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = nullptr;
}

void obj_npc::copyTo(obj_npc &npc)
{
    /* for internal usage */
    npc.isValid         = isValid;
    npc.animator_id     = animator_id;
    npc.cur_image       = cur_image;

    if(cur_image == nullptr)
        npc.cur_image   = &image;

    npc.setup.display_frame   = setup.display_frame;
    /* for internal usage */
    npc.setup = setup;
}

bool dataconfigs::loadLevelNPC(obj_npc &snpc, QString section, obj_npc *merge_with, QString iniFile, IniProcessing *setup)
{
    bool internal = !setup;
    QString errStr;

    if(internal)
        setup = new IniProcessing(iniFile);

    m_errOut = merge_with ? ERR_CUSTOM : ERR_GLOBAL;

    if(!openSection(setup, section.toStdString(), internal))
        return false;

    if(snpc.setup.parse(setup, npcPath, defaultGrid.npc, merge_with ? &merge_with->setup : nullptr, &errStr))
        snpc.isValid = true;
    else
    {
        addError(errStr);
        snpc.isValid = false;
    }

    closeSection(setup);

    if(internal)
        delete setup;

    return snpc.isValid;
}



void dataconfigs::loadLevelNPC()
{
    unsigned long i;
    obj_npc snpc;
    unsigned long npc_total = 0;
    bool useDirectory = false;
    QString npc_ini = getFullIniPath("lvl_npc.ini");

    if(npc_ini.isEmpty())
        return;

    QString nestDir = "";
    IniProcessing setup(npc_ini);

    main_npc.clear();   //Clear old

    if(!openSection(&setup, "npc-main"))
        return;
    {
        setup.read("total", npc_total, 0);
        setup.read("grid", defaultGrid.npc, defaultGrid.npc);
        total_data += npc_total;

        setup.read("config-dir", nestDir, "");
        if(!nestDir.isEmpty())
        {
            nestDir = config_dir + nestDir;
            useDirectory = true;
        }
        setup.read("coin-in-block", marker_npc.coin_in_block, 10);
    }
    closeSection(&setup);
    emit progressPartNumber(3);
    emit progressMax(static_cast<int>(npc_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading NPCs..."));
    ConfStatus::total_npc = static_cast<long>(npc_total);
    main_npc.allocateSlots(static_cast<int>(npc_total));

    if(ConfStatus::total_npc == 0)
    {
        addError(QString("ERROR LOADING lvl_npc.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i = 1; i <= npc_total; i++)
    {
        emit progressValue(static_cast<int>(i));
        bool valid = false;

        if(useDirectory)
            valid = loadLevelNPC(snpc, "npc", nullptr, QString("%1/npc-%2.ini").arg(nestDir).arg(i));
        else
            valid = loadLevelNPC(snpc, QString("npc-%1").arg(i), nullptr, "", &setup);

        /***************Load image*******************/
        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(npcPath,
                                           snpc.setup.image_n, snpc.setup.mask_n,
                                           snpc.image,
                                           errStr);

            if(!errStr.isEmpty())
            {
                valid = false;
                addError(QString("NPC-%1 %2").arg(i).arg(errStr));
            }
        }

        /***************Load image*end***************/
        snpc.setup.id = i;
        main_npc.storeElement(static_cast<int>(i), snpc, valid);

        if(setup.lastError() != IniProcessing::ERR_OK)
        {
            addError(QString("ERROR LOADING lvl_npc.ini N:%1 (npc-%2)").arg(setup.lastError()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }

    if(static_cast<unsigned long>(main_npc.stored()) < npc_total)
        addError(QString("Not all NPCs loaded! Total: %1, Loaded: %2)").arg(npc_total).arg(main_npc.stored()), PGE_LogLevel::Warning);
}
