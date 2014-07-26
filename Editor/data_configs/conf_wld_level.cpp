/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "data_configs.h"

#include "../main_window/global_settings.h"
#include "../common_features/graphics_funcs.h"


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


void dataconfigs::loadWorldLevels(QProgressDialog *prgs)
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


    QSettings tileset(level_ini, QSettings::IniFormat);
    tileset.setIniCodec("UTF-8");

    main_wlevels.clear();   //Clear old
    index_wlvl.clear();

    tileset.beginGroup("levels-main");
        levels_total = tileset.value("total", "0").toInt();
        marker_wlvl.path = tileset.value("path", "0").toInt();
        marker_wlvl.bigpath = tileset.value("bigpath", "0").toInt();
        total_data +=levels_total;
    tileset.endGroup();

    if(prgs) prgs->setMaximum(levels_total);
    if(prgs) prgs->setLabelText(QApplication::tr("Loading Level images..."));

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

    if(ConfStatus::total_wtile==0)
    {
        addError(QString("ERROR LOADING wld_levels.ini: number of items not define, or empty config"), QtCriticalMsg);
        return;
    }

    for(i=0; i<=levels_total; i++)
    {
        qApp->processEvents();
        if(prgs)
        {
            if(!prgs->wasCanceled()) prgs->setValue(i);
        }

        tileset.beginGroup( QString("level-"+QString::number(i)) );
            //slevel.name = tileset.value("name", "").toString();

            //   if(slevel.name=="")
            //   {
            //       addError(QString("TILE-%1 Item name isn't defined").arg(i));
            //       goto skipBGO;
            //   }
            slevel.group = tileset.value("group", "_NoGroup").toString();
            slevel.category = tileset.value("category", "_Other").toString();

            imgFile = tileset.value("image", "").toString();
            slevel.image_n = imgFile;
            if( (imgFile!="") )
            {
                tmp = imgFile.split(".", QString::SkipEmptyParts);
                if(tmp.size()==2)
                    imgFileM = tmp[0] + "m." + tmp[1];
                else
                    imgFileM = "";
                slevel.mask_n = imgFileM;
                mask = QPixmap();
                if(tmp.size()==2) mask = QPixmap(wlvlPath + imgFileM);
                slevel.mask = mask;
                slevel.image = GraphicsHelps::setAlphaMask(QPixmap(wlvlPath + imgFile), slevel.mask);
                if(slevel.image.isNull())
                {
                    addError(QString("LEVEL-%1 Brocken image file").arg(i));
                    goto skipLevel;
                }

            }
            else
            {
                addError(QString("LEVEL-%1 Image filename isn't defined").arg(i));
                goto skipLevel;
            }

            slevel.animated = (tileset.value("animated", "0").toString()=="1");
            slevel.frames = tileset.value("frames", "1").toInt();
            slevel.framespeed = tileset.value("frame-speed", "125").toInt();
            slevel.display_frame = tileset.value("display-frame", "0").toInt();
            slevel.id = i;
            main_wlevels.push_back(slevel);

            //Add to Index
            if(i <= (unsigned int)index_wlvl.size())
            {
                index_wlvl[i].i = i-1;
            }

        skipLevel:
        tileset.endGroup();

        if( tileset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_levels.ini N:%1 (level-%2)").arg(tileset.status()).arg(i), QtCriticalMsg);
        }
    }

    if((unsigned int)main_wlevels.size()<levels_total)
    {
        addError(QString("Not all Level images loaded! Total: %1, Loaded: %2").arg(levels_total).arg(main_wlevels.size()));
    }
}

