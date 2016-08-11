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
    if(cur_image==nullptr)
        npc.cur_image   = &image;
    npc.setup.display_frame   = setup.display_frame;

    /* for internal usage */
    npc.setup = setup;
}

bool dataconfigs::loadLevelNPC(obj_npc &snpc, QString section, obj_npc *merge_with, QString iniFile, QSettings *setup)
{
    bool internal = !setup;
    QString errStr;
    if(internal) setup=new QSettings(iniFile, QSettings::IniFormat);

    if(!openSection( setup, section ))
        return false;

    if(snpc.setup.parse(setup, npcPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
    {
        snpc.isValid = true;
    } else {
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
    unsigned int i;

    obj_npc snpc;
    unsigned long npc_total=0;
    QString npc_ini = getFullIniPath("lvl_npc.ini");
    if(npc_ini.isEmpty())
        return;

    QSettings npcset(npc_ini, QSettings::IniFormat);
    npcset.setIniCodec("UTF-8");

    main_npc.clear();   //Clear old

    if(!openSection(&npcset, "npc-main")) return;
        npc_total =                 npcset.value("total", 0).toUInt();
        total_data +=npc_total;

        marker_npc.bubble =         npcset.value("bubble", 283).toUInt();
        marker_npc.egg =            npcset.value("egg", 96).toUInt();
        marker_npc.lakitu =         npcset.value("lakitu", 284).toUInt();
        marker_npc.buried =         npcset.value("buried", 91).toUInt();
        marker_npc.ice_cube =       npcset.value("icecube", 91).toUInt();
        marker_npc.iceball =        npcset.value("iceball", 265).toUInt();
        marker_npc.fireball =       npcset.value("fireball", 13).toUInt();
        marker_npc.hammer =         npcset.value("hammer", 171).toUInt();
        marker_npc.boomerang =      npcset.value("boomerang", 292).toUInt();
        marker_npc.coin_in_block =  npcset.value("coin-in-block", 10).toUInt();
    closeSection(&npcset);

    emit progressPartNumber(3);
    emit progressMax(int(npc_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading NPCs..."));

    ConfStatus::total_npc = long(npc_total);

    main_npc.allocateSlots(int(npc_total));

    if(ConfStatus::total_npc==0)
    {
        addError(QString("ERROR LOADING lvl_npc.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i <= npc_total; i++)
    {
        emit progressValue(int(i));

        bool valid = loadLevelNPC(snpc, QString("npc-%1").arg(i), nullptr, "", &npcset);
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
        main_npc.storeElement( int(i), snpc, valid);

        if( npcset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_npc.ini N:%1 (npc-%2)").arg(npcset.status()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }

    if( uint(main_npc.stored()) < npc_total )
    {
        addError(QString("Not all NPCs loaded! Total: %1, Loaded: %2)").arg(npc_total).arg(main_npc.stored()), PGE_LogLevel::Warning);
    }
}
