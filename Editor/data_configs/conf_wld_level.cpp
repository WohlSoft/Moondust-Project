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
#include <main_window/global_settings.h>

#include "data_configs.h"

bool dataconfigs::loadWorldLevel(obj_w_level &slevel, QString section, obj_w_level *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal=!setup;
    QString errStr;
    if(internal) setup=new QSettings(iniFile, QSettings::IniFormat);

    if(!openSection(setup, section))
        return false;

    if(slevel.setup.parse(setup, wlvlPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
    {
        slevel.isValid = true;
    }
    else
    {
        addError(errStr);
        slevel.isValid = false;
    }
    slevel.m_itemType = ItemTypes::WLD_Level;
    closeSection(setup);
    if(internal)
        delete setup;
    return valid;
}

void dataconfigs::loadWorldLevels()
{
    unsigned int i;

    obj_w_level slevel;
    unsigned long levels_total=0;

    QString level_ini = getFullIniPath("wld_levels.ini");
    if(level_ini.isEmpty())
        return;

    QSettings levelset(level_ini, QSettings::IniFormat);
    levelset.setIniCodec("UTF-8");

    main_wlevels.clear();   //Clear old

    if(!openSection(&levelset, "levels-main")) return;
        levels_total        = levelset.value("total", 0).toUInt();
        marker_wlvl.path    = levelset.value("path", 0).toUInt();
        marker_wlvl.bigpath = levelset.value("bigpath", 0).toUInt();
        total_data +=levels_total;
    closeSection(&levelset);

    emit progressPartNumber(7);
    emit progressMax(int(levels_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Level images..."));

    ConfStatus::total_wlvl = long(levels_total);

    main_wlevels.allocateSlots(int(levels_total));

    if(ConfStatus::total_wlvl == 0)
    {
        addError(QString("ERROR LOADING wld_levels.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=0; i<=levels_total; i++)
    {
        emit progressValue(int(i));
        bool valid = loadWorldLevel(slevel, QString("level-%1").arg(i), 0, "", &levelset);
        /***************Load image*******************/
        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(wlvlPath,
               slevel.setup.image_n, slevel.setup.mask_n,
               slevel.image,
               errStr);
            if(!errStr.isEmpty())
            {
                valid = false;
                addError(QString("LEVEL-%1 %2").arg(i).arg(errStr));
            }
        }
        /***************Load image*end***************/
        slevel.setup.id = i;
        main_wlevels.storeElement(int(i), slevel, valid);

        if( levelset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_levels.ini N:%1 (level-%2)").arg(levelset.status()).arg(i), PGE_LogLevel::Critical);
        }
    }

    if(uint(main_wlevels.stored()) < levels_total)
    {
        addError(QString("Not all Level images loaded! Total: %1, Loaded: %2").arg(levels_total).arg(main_wlevels.stored()));
    }
}

