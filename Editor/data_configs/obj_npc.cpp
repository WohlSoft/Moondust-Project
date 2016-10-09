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
    if(internal)
    {
        setup=new QSettings(iniFile, QSettings::IniFormat);
        setup->setIniCodec("UTF-8");
    }

    if(!openSection( setup, section ))
        return false;

    if(snpc.setup.parse(setup, npcPath, defaultGrid.npc, merge_with ? &merge_with->setup : nullptr, &errStr))
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
    bool useDirectory=false;

    QString npc_ini = getFullIniPath("lvl_npc.ini");
    if(npc_ini.isEmpty())
        return;

    QString nestDir = "";

    QSettings setup(npc_ini, QSettings::IniFormat);
    setup.setIniCodec("UTF-8");

    main_npc.clear();   //Clear old

    if(!openSection(&setup, "npc-main")) return;
        npc_total =                 setup.value("total", 0).toUInt();
        defaultGrid.npc = setup.value("grid", defaultGrid.npc).toUInt();
        total_data +=npc_total;

        nestDir =   setup.value("config-dir", "").toString();
        if(!nestDir.isEmpty())
        {
            nestDir = config_dir + nestDir;
            useDirectory = true;
        }

        marker_npc.bubble =         setup.value("bubble", 283).toUInt();
        marker_npc.egg =            setup.value("egg", 96).toUInt();
        marker_npc.lakitu =         setup.value("lakitu", 284).toUInt();
        marker_npc.buried =         setup.value("buried", 91).toUInt();
        marker_npc.ice_cube =       setup.value("icecube", 91).toUInt();
        marker_npc.iceball =        setup.value("iceball", 265).toUInt();
        marker_npc.fireball =       setup.value("fireball", 13).toUInt();
        marker_npc.hammer =         setup.value("hammer", 171).toUInt();
        marker_npc.boomerang =      setup.value("boomerang", 292).toUInt();
        marker_npc.coin_in_block =  setup.value("coin-in-block", 10).toUInt();
    closeSection(&setup);

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

        bool valid = false;
        if(useDirectory)
        {
            valid = loadLevelNPC(snpc, "npc", nullptr, QString("%1/npc-%2.ini").arg(nestDir).arg(i));
        }
        else
        {
            valid = loadLevelNPC(snpc, QString("npc-%1").arg(i), nullptr, "", &setup);
        }
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

        if( setup.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_npc.ini N:%1 (npc-%2)").arg(setup.status()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }

    if( uint(main_npc.stored()) < npc_total )
    {
        addError(QString("Not all NPCs loaded! Total: %1, Loaded: %2)").arg(npc_total).arg(main_npc.stored()), PGE_LogLevel::Warning);
    }
}
