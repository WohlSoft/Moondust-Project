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

long dataconfigs::getPathI(unsigned long itemID)
{
    long j;
    bool found=false;

    if(itemID < (unsigned int)index_wpaths.size())
    {
        j = index_wpaths[itemID].i;

        if(j < main_wpaths.size())
        {
            if( main_wpaths[j].id == itemID)
                found=true;
        }
    }

    if(!found)
    {
        for(j=0; j < main_wpaths.size(); j++)
        {
            if(main_wpaths[j].id==itemID)
            {
                found=true;
                break;
            }
        }
    }

    if(!found) j=-1;
    return j;
}


void dataconfigs::loadWorldPaths()
{
    unsigned int i;

    obj_w_path sPath;
    unsigned long path_total=0;
    QString scene_ini = config_dir + "wld_paths.ini";

    if(!QFile::exists(scene_ini))
    {
        addError(QString("ERROR LOADING wld_paths.ini: file does not exist"), QtCriticalMsg);
        return;
    }


    QSettings pathset(scene_ini, QSettings::IniFormat);
    pathset.setIniCodec("UTF-8");

    main_wpaths.clear();   //Clear old
    index_wpaths.clear();

    pathset.beginGroup("path-main");
        path_total = pathset.value("total", "0").toInt();
        total_data +=path_total;
    pathset.endGroup();

    emit progressMax(path_total);
    emit progressTitle(QApplication::tr("Loading Paths images..."));

    ConfStatus::total_wpath= path_total;

    //creation of empty indexes of arrayElements
        wPathIndexes pathIndex;
        for(i=0;i<=path_total; i++)
        {
            pathIndex.i=i;
            pathIndex.type=0;
            pathIndex.ai=0;
            index_wpaths.push_back(pathIndex);
        }

    if(ConfStatus::total_wpath==0)
    {
        addError(QString("ERROR LOADING wld_paths.ini: number of items not define, or empty config"), QtCriticalMsg);
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
               sPath.image,   sPath.mask,
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


            sPath.id = i;
            main_wpaths.push_back(sPath);
            /************Add to Index***************/
            if(i <= (unsigned int)index_wpaths.size())
            {
                index_wpaths[i].i = i-1;
            }
            /************Add to Index***************/

        skipPath:
        pathset.endGroup();

        if( pathset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_paths.ini N:%1 (path-%2)").arg(pathset.status()).arg(i), QtCriticalMsg);
        }
    }

    if((unsigned int)main_wpaths.size()<path_total)
    {
        addError(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(path_total).arg(main_wpaths.size()));
    }
}

