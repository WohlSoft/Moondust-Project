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

#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>

#include "data_configs.h"

obj_w_path::obj_w_path()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = NULL;
}

void obj_w_path::copyTo(obj_w_path &wpath)
{
    /* for internal usage */
    wpath.isValid         = isValid;
    wpath.animator_id     = animator_id;
    wpath.cur_image       = cur_image;
    if(cur_image==NULL)
        wpath.cur_image   = &image;
    wpath.frame_h         = frame_h;
    /* for internal usage */

    wpath.id              = id;
    wpath.group           = group;
    wpath.category        = category;
    wpath.grid            = grid;

    wpath.image_n         = image_n;
    wpath.mask_n          = mask_n;

    wpath.animated        = animated;
    wpath.frames          = frames;
    wpath.framespeed      = framespeed;
    wpath.display_frame   = display_frame;
    wpath.row             = row;
    wpath.col             = col;
}

void dataconfigs::loadWorldPaths()
{
    unsigned int i;

    obj_w_path sPath;
    unsigned long path_total=0;
    QString scene_ini = config_dir + "wld_paths.ini";

    if(!QFile::exists(scene_ini))
    {
        addError(QString("ERROR LOADING wld_paths.ini: file does not exist"), PGE_LogLevel::Critical);
        return;
    }


    QSettings pathset(scene_ini, QSettings::IniFormat);
    pathset.setIniCodec("UTF-8");

    main_wpaths.clear();   //Clear old

    pathset.beginGroup("path-main");
        path_total = pathset.value("total", "0").toInt();
        total_data +=path_total;
    pathset.endGroup();

    emit progressPartNumber(6);
    emit progressMax(path_total);
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Paths images..."));

    ConfStatus::total_wpath= path_total;

    main_wpaths.allocateSlots(path_total);

    if(ConfStatus::total_wpath==0)
    {
        addError(QString("ERROR LOADING wld_paths.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i<=path_total; i++)
    {
        emit progressValue(i);
        QString errStr;

        pathset.beginGroup( QString("path-"+QString::number(i)) );

        sPath.group =       pathset.value("group", "_NoGroup").toString();
        sPath.category =    pathset.value("category", "_Other").toString();

        sPath.image_n =     pathset.value("image", "").toString();
        /***************Load image*******************/
        GraphicsHelps::loadMaskedImage(pathPath,
           sPath.image_n, sPath.mask_n,
           sPath.image,
           errStr);

        if(!errStr.isEmpty())
        {
            addError(QString("PATH-%1 %2").arg(i).arg(errStr));
            goto skipPath;
        }
        /***************Load image*end***************/

        sPath.grid =            pathset.value("grid", default_grid).toInt();

        sPath.animated =       (pathset.value("animated", "0").toString()=="1");
        sPath.frames =          pathset.value("frames", "1").toInt();
        sPath.framespeed =      pathset.value("frame-speed", "175").toInt();

        sPath.frame_h = (sPath.animated? qRound(qreal(sPath.image.height())/sPath.frames) : sPath.image.height());

        sPath.display_frame=    pathset.value("display-frame", "0").toInt();
        sPath.row =             pathset.value("row", "0").toInt();
        sPath.col =             pathset.value("col", "0").toInt();


        sPath.isValid = true;
        sPath.id = i;
        main_wpaths.storeElement(i, sPath);

    skipPath:
        pathset.endGroup();

        if( pathset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_paths.ini N:%1 (path-%2)").arg(pathset.status()).arg(i), PGE_LogLevel::Critical);
        }
    }

    if((unsigned int)main_wpaths.size()<path_total)
    {
        addError(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(path_total).arg(main_wpaths.size()));
    }
}

