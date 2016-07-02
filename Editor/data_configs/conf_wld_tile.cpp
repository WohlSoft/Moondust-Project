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

obj_w_tile::obj_w_tile()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = nullptr;
}

void obj_w_tile::copyTo(obj_w_tile &tile)
{
    /* for internal usage */
    tile.isValid         = isValid;
    tile.animator_id     = animator_id;
    tile.cur_image       = cur_image;
    if(cur_image==nullptr)
        tile.cur_image   = &image;
    tile.frame_h         = frame_h;
    /* for internal usage */

    tile.id              = id;
    tile.group           = group;
    tile.category        = category;
    tile.grid            = grid;

    tile.image_n         = image_n;
    tile.mask_n          = mask_n;

    tile.animated        = animated;
    tile.frames          = frames;
    tile.framespeed      = framespeed;
    tile.display_frame   = display_frame;
    tile.row             = row;
    tile.col             = col;
}

void dataconfigs::loadWorldTiles()
{
    unsigned int i;

    obj_w_tile stile;
    unsigned long tiles_total=0;

    QString tile_ini = getFullIniPath("wld_tiles.ini");
    if(tile_ini.isEmpty())
        return;

    QSettings tileset(tile_ini, QSettings::IniFormat);
    tileset.setIniCodec("UTF-8");

    main_wtiles.clear();   //Clear old

    if(!openSection(&tileset, "tiles-main")) return;
        tiles_total = tileset.value("total", 0).toUInt();
        total_data +=tiles_total;
    closeSection(&tileset);

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
        bool valid=true;
        emit progressValue(int(i));
        QString errStr;

        if( !openSection(&tileset, QString("tile-%1").arg(i)) )
            break;

        stile.group     = tileset.value("group", "_NoGroup").toString();
        stile.category  = tileset.value("category", "_Other").toString();

        stile.image_n   = tileset.value("image", "").toString();
        /***************Load image*******************/
        GraphicsHelps::loadMaskedImage(tilePath,
           stile.image_n, stile.mask_n,
           stile.image,
           errStr);

        if(!errStr.isEmpty())
        {
            valid = false;
            addError(QString("TILE-%1 %2").arg(i).arg(errStr));
            //goto skipTile;
        }
        /***************Load image*end***************/

        stile.grid =            tileset.value("grid", default_grid).toUInt();

        stile.animated =        tileset.value("animated", 0).toBool();
        stile.frames =          tileset.value("frames", 1).toUInt();
        stile.framespeed =      tileset.value("frame-speed", 175).toUInt();

        stile.frame_h = uint(stile.animated?
                             qRound(
                                 qreal(stile.image.height())/
                                 stile.frames)
                            : stile.image.height());

        stile.display_frame =   tileset.value("display-frame", 0).toUInt();
        stile.row =             tileset.value("row", 0).toUInt();
        stile.col =             tileset.value("col", 0).toUInt();

        stile.isValid = true;

        stile.id = i;
        main_wtiles.storeElement(int(i), stile, valid);

        closeSection(&tileset);

        if( tileset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_tiles.ini N:%1 (tile-%2)").arg(tileset.status()).arg(i), PGE_LogLevel::Critical);
        }
    }

    if(uint(main_wtiles.stored()) < tiles_total)
    {
        addError(QString("Not all Tiles loaded! Total: %1, Loaded: %2").arg(tiles_total).arg(main_wtiles.stored()));
    }

}

