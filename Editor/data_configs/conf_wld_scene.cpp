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

obj_w_scenery::obj_w_scenery()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = nullptr;
}

void obj_w_scenery::copyTo(obj_w_scenery &scenery)
{
    /* for internal usage */
    scenery.isValid         = isValid;
    scenery.animator_id     = animator_id;
    scenery.cur_image       = cur_image;
    if(cur_image==nullptr)
        scenery.cur_image   = &image;
    scenery.frame_h         = frame_h;
    /* for internal usage */

    scenery.id              = id;
    scenery.group           = group;
    scenery.category        = category;
    scenery.grid            = grid;

    scenery.image_n         = image_n;
    scenery.mask_n          = mask_n;

    scenery.animated        = animated;
    scenery.frames          = frames;
    scenery.framespeed      = framespeed;
    scenery.display_frame   = display_frame;
}

void dataconfigs::loadWorldScene()
{
    unsigned int i;

    obj_w_scenery sScene;
    unsigned long scenery_total=0;

    QString scene_ini = getFullIniPath("wld_scenery.ini");
    if(scene_ini.isEmpty())
        return;

    QSettings sceneset(scene_ini, QSettings::IniFormat);
    sceneset.setIniCodec("UTF-8");

    main_wscene.clear();   //Clear old

    if(!openSection(&sceneset, "scenery-main")) return;
        scenery_total = sceneset.value("total", 0).toUInt();
        total_data +=scenery_total;
    closeSection(&sceneset);

    emit progressPartNumber(5);
    emit progressMax(int(scenery_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Sceneries..."));

    ConfStatus::total_wscene= long(scenery_total);

    if(ConfStatus::total_wscene==0)
    {
        addError(QString("ERROR LOADING wld_scenery.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    main_wscene.allocateSlots(int(scenery_total));

    for(i=1; i <= scenery_total; i++)
    {
        bool valid = true;
        emit progressValue(int(i));
        QString errStr;

        if( !openSection(&sceneset, QString("scenery-%1").arg(i)) )
            break;

        sScene.group =         sceneset.value("group", "_NoGroup").toString();
        sScene.category =      sceneset.value("category", "_Other").toString();

        sScene.image_n =       sceneset.value("image", "").toString();
        /***************Load image*******************/
        GraphicsHelps::loadMaskedImage(scenePath,
           sScene.image_n, sScene.mask_n,
           sScene.image,
           errStr);

        if(!errStr.isEmpty())
        {
            valid = false;
            addError(QString("SCENE-%1 %2").arg(i).arg(errStr));
            //goto skipScene;
        }
        /***************Load image*end***************/

        sScene.grid =           sceneset.value("grid", qRound(qreal(default_grid)/2)).toUInt();

        sScene.animated =       sceneset.value("animated", 0).toBool();
        sScene.frames =         sceneset.value("frames", 1).toUInt();
        sScene.framespeed =     sceneset.value("frame-speed", 175).toUInt();

        sScene.frame_h =   uint(sScene.animated?
                    qRound(qreal(sScene.image.height())/
                               sScene.frames)
                             : sScene.image.height());

        sScene.display_frame = sceneset.value("display-frame", 0).toUInt();

        sScene.isValid=true;

        sScene.id = i;
        main_wscene.storeElement(int(i), sScene, valid);

        closeSection(&sceneset);

        if( sceneset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_scenery.ini N:%1 (scene-%2)").arg(sceneset.status()).arg(i), PGE_LogLevel::Critical);
        }
    }

    if(uint(main_wscene.stored()) < scenery_total)
    {
        addError(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(scenery_total).arg(main_wscene.stored()));
    }
}
