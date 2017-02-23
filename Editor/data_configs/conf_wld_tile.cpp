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

bool dataconfigs::loadWorldTerrain(obj_w_tile &stile, QString section, obj_w_tile *merge_with, QString iniFile, IniProcessing *setup)
{
    bool valid=true;
    bool internal=!setup;
    QString errStr;
    if(internal)
    {
        setup=new QSettings(iniFile, QSettings::IniFormat);
        setup->setIniCodec("UTF-8");
    }

    if(!openSection(setup, section))
        return false;

    if(stile.setup.parse(setup, tilePath, defaultGrid.terrain, merge_with ? &merge_with->setup : nullptr, &errStr))
    {
        stile.isValid = true;
    }
    else
    {
        addError(errStr);
        stile.isValid = false;
    }
    stile.m_itemType = ItemTypes::WLD_Tile;
    closeSection(setup);
    if(internal)
        delete setup;
    return valid;
}

void dataconfigs::loadWorldTiles()
{
    unsigned int i;

    obj_w_tile stile;
    unsigned long tiles_total=0;
    bool useDirectory=false;

    QString tile_ini = getFullIniPath("wld_tiles.ini");
    if(tile_ini.isEmpty())
        return;

    QString nestDir = "";

    QSettings setup(tile_ini, QSettings::IniFormat);
    setup.setIniCodec("UTF-8");

    main_wtiles.clear();   //Clear old

    if(!openSection(&setup, "tiles-main")) return;
        tiles_total = setup.value("total", 0).toUInt();
        defaultGrid.terrain = setup.value("grid", defaultGrid.terrain).toUInt();
        total_data +=tiles_total;
        nestDir =   setup.value("config-dir", "").toString();
        if(!nestDir.isEmpty())
        {
            nestDir = config_dir + nestDir;
            useDirectory = true;
        }
    closeSection(&setup);

    emit progressPartNumber(4);
    emit progressMax(int(tiles_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Tiles..."));

    ConfStatus::total_wtile = signed(tiles_total);

    if(ConfStatus::total_wtile==0)
    {
        addError(QString("ERROR LOADING wld_tiles.ini: number of items not define, or empty config"),  PGE_LogLevel::Critical);
        return;
    }

    main_wtiles.allocateSlots(int(tiles_total));

    for(i=1; i<=tiles_total; i++)
    {
        emit progressValue(int(i));
        bool valid = false;
        if(useDirectory)
        {
            valid = loadWorldTerrain(stile, "tile", nullptr, QString("%1/tile-%2.ini").arg(nestDir).arg(i));
        }
        else
        {
            valid = loadWorldTerrain(stile, QString("tile-%1").arg(i), 0, "", &setup);
        }
        /***************Load image*******************/
        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(tilePath,
               stile.setup.image_n, stile.setup.mask_n,
               stile.image,
               errStr);
            if(!errStr.isEmpty())
            {
                valid = false;
                addError(QString("TILE-%1 %2").arg(i).arg(errStr));
            }
        }
        /***************Load image*end***************/
        stile.setup.id = i;
        main_wtiles.storeElement(int(i), stile, valid);

        if( setup.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_tiles.ini N:%1 (tile-%2)").arg(setup.status()).arg(i), PGE_LogLevel::Critical);
        }
    }

    if(uint(main_wtiles.stored()) < tiles_total)
    {
        addError(QString("Not all Tiles loaded! Total: %1, Loaded: %2").arg(tiles_total).arg(main_wtiles.stored()));
    }

}

