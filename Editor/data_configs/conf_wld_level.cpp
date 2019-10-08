/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

bool dataconfigs::loadWorldLevel(obj_w_level &slevel, QString section, obj_w_level *merge_with, QString iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    PGEString errStr;
    if(internal)
        setup = new IniProcessing(iniFile);

    m_errOut = merge_with ? ERR_CUSTOM : ERR_GLOBAL;

    if(!openSection(setup, section.toStdString(), internal))
        return false;

    if(slevel.setup.parse(setup, folderWldLevelPoints.graphics, defaultGrid.levels, merge_with ? &merge_with->setup : nullptr, &errStr))
        slevel.isValid = true;
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
    unsigned long levels_total = 0;
    bool useDirectory = false;

    QString level_ini = getFullIniPath("wld_levels.ini");
    if(level_ini.isEmpty())
        return;

    IniProcessing setup(level_ini);

    folderWldLevelPoints.items.clear();
    main_wlevels.clear();   //Clear old

    if(!openSection(&setup, "levels-main"))
        return;
    {
        levels_total        = setup.value("total", 0).toUInt();
        defaultGrid.levels = setup.value("grid", defaultGrid.levels).toUInt();
        marker_wlvl.path    = setup.value("path", 0).toUInt();
        marker_wlvl.bigpath = setup.value("bigpath", 0).toUInt();
        total_data += levels_total;
        setup.read("config-dir", folderWldLevelPoints.items, "");
        setup.read("extra-settings", folderWldLevelPoints.extraSettings, folderWldLevelPoints.items);
        setup.read("extra-settings-combined-view", folderWldLevelPoints.extraSettingsCombinedView, true);
        if(!folderWldLevelPoints.items.isEmpty())
        {
            folderWldLevelPoints.items = config_dir + folderWldLevelPoints.items;
            useDirectory = true;
        }
    }
    closeSection(&setup);

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

    for(i = 0; i <= levels_total; i++)
    {
        emit progressValue(int(i));
        bool valid = false;
        if(useDirectory)
            valid = loadWorldLevel(slevel, "level", nullptr, QString("%1/level-%2.ini").arg(folderWldLevelPoints.items).arg(i));
        else
            valid = loadWorldLevel(slevel, QString("level-%1").arg(i), 0, "", &setup);

        /***************Load image*******************/
        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(folderWldLevelPoints.graphics,
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

        if(setup.lastError() != IniProcessing::ERR_OK)
            addError(QString("ERROR LOADING wld_levels.ini N:%1 (level-%2)").arg(setup.lastError()).arg(i), PGE_LogLevel::Critical);
    }

    if(uint(main_wlevels.stored()) < levels_total)
        addError(QString("Not all Level images loaded! Total: %1, Loaded: %2").arg(levels_total).arg(main_wlevels.stored()));
}
