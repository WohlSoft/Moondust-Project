/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

bool DataConfig::loadWorldTerrain(obj_w_tile &stile, QString section, obj_w_tile *merge_with, QString iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    QString errStr;
    if(internal)
        setup = new IniProcessing(iniFile);

    m_errOut = merge_with ? ERR_CUSTOM : ERR_GLOBAL;

    if(!openSection(setup, section.toStdString(), internal))
        return false;

    if(stile.setup.parse(setup, folderWldTerrain.graphics, defaultGrid.terrain, merge_with ? &merge_with->setup : nullptr, &errStr))
        stile.isValid = true;
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

void DataConfig::loadWorldTiles()
{
    unsigned int i;

    obj_w_tile stile;
    unsigned long tiles_total = 0;
    bool useDirectory = false;

    QString tile_ini = getFullIniPath("wld_tiles.ini");
    if(tile_ini.isEmpty())
        return;

    IniProcessing setup(tile_ini);

    folderWldTerrain.items.clear();
    main_wtiles.clear();   //Clear old

    if(!openSection(&setup, "tiles-main"))
        return;
    {
        setup.read("total", tiles_total, 0);
        setup.read("grid", defaultGrid.terrain, defaultGrid.terrain);
        total_data += tiles_total;
        setup.read("config-dir", folderWldTerrain.items, "");
        setup.read("extra-settings", folderWldTerrain.extraSettings, folderWldTerrain.items);
        if(!folderWldTerrain.items.isEmpty())
        {
            folderWldTerrain.items = config_dir + folderWldTerrain.items;
            useDirectory = true;
        }
    }
    closeSection(&setup);

    emit progressPartNumber(4);
    emit progressMax(int(tiles_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Tiles..."));

    ConfStatus::total_wtile = signed(tiles_total);

    if(ConfStatus::total_wtile == 0)
    {
        addError(QString("ERROR LOADING wld_tiles.ini: number of items not define, or empty config"),  PGE_LogLevel::Critical);
        return;
    }

    main_wtiles.allocateSlots(int(tiles_total));

    for(i = 1; i <= tiles_total; i++)
    {
        emit progressValue(int(i));
        bool valid = false;
        if(useDirectory)
            valid = loadWorldTerrain(stile, "tile", nullptr, QString("%1/tile-%2.ini").arg(folderWldTerrain.items).arg(i));
        else
            valid = loadWorldTerrain(stile, QString("tile-%1").arg(i), nullptr, "", &setup);
        /***************Load image*******************/
        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(folderWldTerrain.graphics,
                                           stile.setup.image_n, stile.setup.mask_n,
                                           stile.image,
                                           errStr);
            if(!errStr.isEmpty())
            {
                valid = false;
                addError(QString("TILE-%1 %2").arg(i).arg(errStr));
            }

            GraphicsHelps::loadIconOpt(folderWldTerrain.graphics,
                                       stile.setup.icon_n,
                                       stile.icon);
        }
        /***************Load image*end***************/
        stile.setup.id = i;
        main_wtiles.storeElement(int(i), stile, valid);

        if(setup.lastError() != IniProcessing::ERR_OK)
            addError(QString("ERROR LOADING wld_tiles.ini N:%1 (tile-%2)").arg(setup.lastError()).arg(i), PGE_LogLevel::Critical);
    }

    if(uint(main_wtiles.stored()) < tiles_total)
        addError(QString("Not all Tiles loaded! Total: %1, Loaded: %2").arg(tiles_total).arg(main_wtiles.stored()));

}
