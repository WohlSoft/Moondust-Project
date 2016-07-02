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

obj_w_level::obj_w_level()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = nullptr;
}

void obj_w_level::copyTo(obj_w_level &level)
{
    /* for internal usage */
    level.isValid         = isValid;
    level.animator_id     = animator_id;
    level.cur_image       = cur_image;
    if(cur_image==nullptr)
        level.cur_image   = &image;
    level.frame_h         = frame_h;
    /* for internal usage */

    level.id              = id;
    level.group           = group;
    level.category        = category;
    level.grid            = grid;

    level.image_n         = image_n;
    level.mask_n          = mask_n;

    level.animated        = animated;
    level.frames          = frames;
    level.framespeed      = framespeed;
    level.display_frame   = display_frame;
}

long dataconfigs::getCharacterI(unsigned long itemID)
{
    long j;
    bool found=false;

    for(j=0; j < characters.size(); j++)
    {
        if(characters[int(j)].id == itemID)
        {
            found=true;
            break;
        }
    }

    if(!found) j=-1;
    return j;
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
        bool valid = true;
        emit progressValue(int(i));
        QString errStr;

        if( !openSection(&levelset, QString("level-%1").arg(i)) )
            break;

        slevel.group =      levelset.value("group", "_NoGroup").toString();
        slevel.category =   levelset.value("category", "_Other").toString();

        slevel.image_n =    levelset.value("image", "").toString();
        /***************Load image*******************/
        GraphicsHelps::loadMaskedImage(wlvlPath,
           slevel.image_n, slevel.mask_n,
           slevel.image,
           errStr);

        if(!errStr.isEmpty())
        {
            valid=false;
            addError(QString("LEVEL-%1 %2").arg(i).arg(errStr));
            //goto skipLevel;
        }
        /***************Load image*end***************/

        slevel.grid =       levelset.value("grid", default_grid).toUInt();

        slevel.animated =   levelset.value("animated", 0).toBool();
        slevel.frames   =   levelset.value("frames", 1).toUInt();
        slevel.framespeed = levelset.value("frame-speed", 175).toUInt();

        slevel.frame_h =   (slevel.animated ?
                                    uint( qRound(qreal(slevel.image.height())/
                                             slevel.frames) )
                                  : uint( slevel.image.height()) );

        slevel.display_frame = levelset.value("display-frame", 0).toUInt();

        slevel.isValid = true;

        slevel.id = i;
        main_wlevels.storeElement(int(i), slevel, valid);

        closeSection(&levelset);

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

