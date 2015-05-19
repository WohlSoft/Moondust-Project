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

long dataconfigs::getWLevelI(unsigned long itemID)
{
    long j;
    bool found=false;

    if(itemID < (unsigned int)index_wlvl.size())
    {
        j = index_wlvl[itemID].i;

        if(j < main_wlevels.size())
        {
            if( main_wlevels[j].id == itemID)
                found=true;
        }
    }

    if(!found)
    {
        for(j=0; j < main_wlevels.size(); j++)
        {
            if(main_wlevels[j].id==itemID)
            {
                found=true;
                break;
            }
        }
    }

    if(!found) j=-1;
    return j;
}

long dataconfigs::getCharacterI(unsigned long itemID)
{
    long j;
    bool found=false;


    for(j=0; j < characters.size(); j++)
    {
        if(characters[j].id==itemID)
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
    QString level_ini = config_dir + "wld_levels.ini";

    if(!QFile::exists(level_ini))
    {
        addError(QString("ERROR LOADING wld_levels.ini: file does not exist"), QtCriticalMsg);
        return;
    }


    QSettings levelset(level_ini, QSettings::IniFormat);
    levelset.setIniCodec("UTF-8");

    main_wlevels.clear();   //Clear old
    index_wlvl.clear();

    levelset.beginGroup("levels-main");
        levels_total = levelset.value("total", "0").toInt();
        marker_wlvl.path = levelset.value("path", "0").toInt();
        marker_wlvl.bigpath = levelset.value("bigpath", "0").toInt();
        total_data +=levels_total;
    levelset.endGroup();

    emit progressMax(levels_total);
    emit progressTitle(QApplication::tr("Loading Level images..."));

    ConfStatus::total_wlvl = levels_total;

    //creation of empty indexes of arrayElements
        wLevelIndexes levelIndex;
        for(i=0;i<=levels_total; i++)
        {
            levelIndex.i=i;
            levelIndex.type=0;
            levelIndex.ai=0;
            index_wlvl.push_back(levelIndex);
        }

    if(ConfStatus::total_wlvl==0)
    {
        addError(QString("ERROR LOADING wld_levels.ini: number of items not define, or empty config"), QtCriticalMsg);
        return;
    }

    for(i=0; i<=levels_total; i++)
    {
        emit progressValue(i);
        QString errStr;

        levelset.beginGroup( QString("level-"+QString::number(i)) );

        slevel.group =      levelset.value("group", "_NoGroup").toString();
        slevel.category =   levelset.value("category", "_Other").toString();

        slevel.image_n =    levelset.value("image", "").toString();
        /***************Load image*******************/
        GraphicsHelps::loadMaskedImage(wlvlPath,
           slevel.image_n, slevel.mask_n,
           slevel.image,   slevel.mask,
           errStr);

        if(!errStr.isEmpty())
        {
            addError(QString("LEVEL-%1 %2").arg(i).arg(errStr));
            goto skipLevel;
        }
        /***************Load image*end***************/

        slevel.grid =       levelset.value("grid", default_grid).toInt();

        slevel.animated =  (levelset.value("animated", "0").toString()=="1");
        slevel.frames =     levelset.value("frames", "1").toInt();
        slevel.framespeed = levelset.value("frame-speed", "125").toInt();

        slevel.frame_h =   (slevel.animated?
                                qRound(qreal(slevel.image.height())/
                                             slevel.frames)
                                    : slevel.image.height());

        slevel.display_frame = levelset.value("display-frame", "0").toInt();


        slevel.id = i;
        main_wlevels.push_back(slevel);

        /************Add to Index***************/
        if(i <= (unsigned int)index_wlvl.size())
        {
            index_wlvl[i].i = i;
        }
        /************Add to Index***************/

        skipLevel:
        levelset.endGroup();

        if( levelset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_levels.ini N:%1 (level-%2)").arg(levelset.status()).arg(i), QtCriticalMsg);
        }
    }

    if((unsigned int)main_wlevels.size()<levels_total)
    {
        addError(QString("Not all Level images loaded! Total: %1, Loaded: %2").arg(levels_total).arg(main_wlevels.size()));
    }
}

