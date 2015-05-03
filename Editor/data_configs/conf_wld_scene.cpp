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

long dataconfigs::getSceneI(unsigned long itemID)
{
    long j;
    bool found=false;

    if(itemID < (unsigned int)index_wscene.size())
    {
        j = index_wscene[itemID].i;

        if(j < main_wscene.size())
        {
            if( main_wscene[j].id == itemID)
                found=true;
        }
    }

    if(!found)
    {
        for(j=0; j < main_wscene.size(); j++)
        {
            if(main_wscene[j].id==itemID)
            {
                found=true;
                break;
            }
        }
    }

    if(!found) j=-1;
    return j;
}


void dataconfigs::loadWorldScene()
{
    unsigned int i;

    obj_w_scenery sScene;
    unsigned long scenery_total=0;
    QString scene_ini = config_dir + "wld_scenery.ini";

    if(!QFile::exists(scene_ini))
    {
        addError(QString("ERROR LOADING wld_scenery.ini: file does not exist"), QtCriticalMsg);
        return;
    }


    QSettings sceneset(scene_ini, QSettings::IniFormat);
    sceneset.setIniCodec("UTF-8");

    main_wscene.clear();   //Clear old
    index_wscene.clear();

    sceneset.beginGroup("scenery-main");
        scenery_total = sceneset.value("total", "0").toInt();
        total_data +=scenery_total;
    sceneset.endGroup();

    emit progressMax(scenery_total);
    emit progressTitle(QApplication::tr("Loading Sceneries..."));

    ConfStatus::total_wscene= scenery_total;

    //creation of empty indexes of arrayElements
        wSceneIndexes sceneIndex;
        for(i=0;i<=scenery_total; i++)
        {
            sceneIndex.i=i;
            sceneIndex.type=0;
            sceneIndex.ai=0;
            index_wscene.push_back(sceneIndex);
        }

    if(ConfStatus::total_wscene==0)
    {
        addError(QString("ERROR LOADING wld_scenery.ini: number of items not define, or empty config"), QtCriticalMsg);
        return;
    }

    for(i=1; i<=scenery_total; i++)
    {
        emit progressValue(i);
        QString errStr;

        sceneset.beginGroup( QString("scenery-"+QString::number(i)) );

            sScene.group =         sceneset.value("group", "_NoGroup").toString();
            sScene.category =      sceneset.value("category", "_Other").toString();

            sScene.image_n =       sceneset.value("image", "").toString();
            /***************Load image*******************/
            GraphicsHelps::loadMaskedImage(scenePath,
               sScene.image_n, sScene.mask_n,
               sScene.image,   sScene.mask,
               errStr);

            if(!errStr.isEmpty())
            {
                addError(QString("SCENE-%1 %2").arg(i).arg(errStr));
                goto skipScene;
            }
            /***************Load image*end***************/

            sScene.grid =          sceneset.value("grid", qRound(qreal(default_grid)/2)).toInt();

            sScene.animated =     (sceneset.value("animated", "0").toString()=="1");
            sScene.frames =        sceneset.value("frames", "1").toInt();
            sScene.framespeed =    sceneset.value("frame-speed", "125").toInt();

            sScene.frame_h =   (sScene.animated?
                      qRound(qreal(sScene.image.height())/
                                   sScene.frames)
                                 : sScene.image.height());

            sScene.display_frame = sceneset.value("display-frame", "0").toInt();



            sScene.id = i;
            main_wscene.push_back(sScene);
            /************Add to Index***************/
            if(i <= (unsigned int)index_wscene.size())
            {
                index_wscene[i].i = i-1;
            }
            /************Add to Index***************/

        skipScene:
        sceneset.endGroup();

        if( sceneset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_scenery.ini N:%1 (scene-%2)").arg(sceneset.status()).arg(i), QtCriticalMsg);
        }
    }

    if((unsigned int)main_wscene.size()<scenery_total)
    {
        addError(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(scenery_total).arg(main_wscene.size()));
    }
}

