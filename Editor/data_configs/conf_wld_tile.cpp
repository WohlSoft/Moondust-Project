/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>

#include "data_configs.h"

long dataconfigs::getTileI(unsigned long itemID)
{
    long j;
    bool found=false;

    if(itemID < (unsigned int)index_wtiles.size())
    {
        j = index_wtiles[itemID].i;

        if(j < main_wtiles.size())
        {
            if( main_wtiles[j].id == itemID)
                found=true;
        }
    }

    if(!found)
    {
        for(j=0; j < main_wtiles.size(); j++)
        {
            if(main_wtiles[j].id==itemID)
            {
                found=true;
                break;
            }
        }
    }

    if(!found) j=-1;
    return j;
}


void dataconfigs::loadWorldTiles(QProgressDialog *prgs)
{
    unsigned int i;

    obj_w_tile stile;
    unsigned long tiles_total=0;
    QString tile_ini = config_dir + "wld_tiles.ini";

    if(!QFile::exists(tile_ini))
    {
        addError(QString("ERROR LOADING wld_tiles.ini: file does not exist"), QtCriticalMsg);
        return;
    }


    QSettings tileset(tile_ini, QSettings::IniFormat);
    tileset.setIniCodec("UTF-8");

    main_wtiles.clear();   //Clear old
    index_wtiles.clear();

    tileset.beginGroup("tiles-main");
        tiles_total = tileset.value("total", "0").toInt();
        total_data +=tiles_total;
    tileset.endGroup();

    if(prgs) prgs->setMaximum(tiles_total);
    if(prgs) prgs->setLabelText(QApplication::tr("Loading Tiles..."));

    ConfStatus::total_wtile = tiles_total;

    //creation of empty indexes of arrayElements
        wTileIndexes tileIndex;
        for(i=0;i<=tiles_total; i++)
        {
            tileIndex.i=i;
            tileIndex.type=0;
            tileIndex.ai=0;
            index_wtiles.push_back(tileIndex);
        }

    if(ConfStatus::total_wtile==0)
    {
        addError(QString("ERROR LOADING wld_tiles.ini: number of items not define, or empty config"), QtCriticalMsg);
        return;
    }

    for(i=1; i<=tiles_total; i++)
    {
        qApp->processEvents();
        if(prgs)
        {
            if(!prgs->wasCanceled()) prgs->setValue(i);
        }
        QString errStr;

        tileset.beginGroup( QString("tile-"+QString::number(i)) );

        stile.group = tileset.value("group", "_NoGroup").toString();
        stile.category = tileset.value("category", "_Other").toString();

        stile.image_n = tileset.value("image", "").toString();
        /***************Load image*******************/
        GraphicsHelps::loadMaskedImage(tilePath,
           stile.image_n, stile.mask_n,
           stile.image,   stile.mask,
           errStr);

        if(!errStr.isEmpty())
        {
            addError(QString("TILE-%1 %2").arg(i).arg(errStr));
            goto skipTile;
        }
        /***************Load image*end***************/

        stile.grid =            tileset.value("grid", default_grid).toInt();

        stile.animated =       (tileset.value("animated", "0").toString()=="1");
        stile.frames =          tileset.value("frames", "1").toInt();
        stile.framespeed =      tileset.value("frame-speed", "125").toInt();

        stile.frame_h = (stile.animated?
                             qRound(
                                 qreal(stile.image.height())/
                                 stile.frames)
                            : stile.image.height());

        stile.display_frame =   tileset.value("display-frame", "0").toInt();
        stile.row =             tileset.value("row", "0").toInt();
        stile.col =             tileset.value("col", "0").toInt();


        stile.id = i;
        main_wtiles.push_back(stile);
        /************Add to Index***************/
        if(i <= (unsigned int)index_wtiles.size())
        {
            index_wtiles[i].i = i-1;
        }
        /************Add to Index***************/

        skipTile:
        tileset.endGroup();

        if( tileset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_tiles.ini N:%1 (tile-%2)").arg(tileset.status()).arg(i), QtCriticalMsg);
        }
    }

    if((unsigned int)main_wtiles.size()<tiles_total)
    {
        addError(QString("Not all Tiles loaded! Total: %1, Loaded: %2").arg(tiles_total).arg(main_wtiles.size()));
    }
}

