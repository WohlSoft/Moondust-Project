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

bool dataconfigs::loadWorldPath(obj_w_path &spath, QString section, obj_w_path *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal=!setup;
    QString errStr;
    if(internal) setup=new QSettings(iniFile, QSettings::IniFormat);

    if(!openSection(setup, section))
        return false;

    if(spath.setup.parse(setup, pathPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
    {
        spath.isValid = true;
    }
    else
    {
        addError(errStr);
        spath.isValid = false;
    }
    spath.m_itemType = ItemTypes::WLD_Path;
    closeSection(setup);
    if(internal)
        delete setup;
    return valid;
}

void dataconfigs::loadWorldPaths()
{
    unsigned int i;

    obj_w_path sPath;
    unsigned long path_total=0;
    bool useDirectory=false;

    QString scene_ini = getFullIniPath("wld_paths.ini");
    if(scene_ini.isEmpty())
        return;

    QString nestDir = "";

    QSettings setup(scene_ini, QSettings::IniFormat);
    setup.setIniCodec("UTF-8");

    main_wpaths.clear();   //Clear old

    if(!openSection(&setup, "path-main")) return;
        path_total = setup.value("total", 0).toUInt();
        total_data +=path_total;
        nestDir =   setup.value("config-dir", "").toString();
        if(!nestDir.isEmpty())
        {
            nestDir = config_dir + nestDir;
            useDirectory = true;
        }
    closeSection(&setup);

    emit progressPartNumber(6);
    emit progressMax(int(path_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Paths images..."));

    ConfStatus::total_wpath= long(path_total);

    main_wpaths.allocateSlots(int(path_total));

    if(ConfStatus::total_wpath==0)
    {
        addError(QString("ERROR LOADING wld_paths.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i<=path_total; i++)
    {
        emit progressValue(int(i));
        bool valid = false;
        if(useDirectory)
        {
            valid = loadWorldPath(sPath, "path", nullptr, QString("%1/path-%2.ini").arg(nestDir).arg(i));
        }
        else
        {
            valid = loadWorldPath(sPath, QString("path-%1").arg(i), 0, "", &setup);
        }
        /***************Load image*******************/
        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(pathPath,
               sPath.setup.image_n, sPath.setup.mask_n,
               sPath.image,
               errStr);
            if(!errStr.isEmpty())
            {
                valid = false;
                addError(QString("PATH-%1 %2").arg(i).arg(errStr));
            }
        }
        /***************Load image*end***************/
        sPath.setup.id = i;
        main_wpaths.storeElement(int(i), sPath, valid);

        if( setup.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_paths.ini N:%1 (path-%2)").arg(setup.status()).arg(i), PGE_LogLevel::Critical);
        }
    }

    if( uint(main_wpaths.stored()) < path_total )
    {
        addError(QString("Not all Paths loaded! Total: %1, Loaded: %2").arg(path_total).arg(main_wpaths.stored()));
    }
}

