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


void dataconfigs::loadWorldPaths(QProgressDialog *prgs)
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

    if(prgs) prgs->setMaximum(path_total);
    if(prgs) prgs->setLabelText(QApplication::tr("Loading Paths images..."));

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
        qApp->processEvents();
        if(prgs)
        {
            if(!prgs->wasCanceled()) prgs->setValue(i);
        }

        pathset.beginGroup( QString("path-"+QString::number(i)) );
            //sPath.name = pathset.value("name", "").toString();

            //   if(sPath.name=="")
            //   {
            //       addError(QString("PATH-%1 Item name isn't defined").arg(i));
            //       goto skipBGO;
            //   }
            sPath.group = pathset.value("group", "_NoGroup").toString();
            sPath.category = pathset.value("category", "_Other").toString();

            imgFile = pathset.value("image", "").toString();
            sPath.image_n = imgFile;
            if( (imgFile!="") )
            {
                tmp = imgFile.split(".", QString::SkipEmptyParts);
                if(tmp.size()==2)
                    imgFileM = tmp[0] + "m." + tmp[1];
                else
                    imgFileM = "";
                sPath.mask_n = imgFileM;
                mask = QPixmap();
                if(tmp.size()==2) mask = QPixmap(pathPath + imgFileM);
                sPath.mask = mask;
                sPath.image = GraphicsHelps::setAlphaMask(QPixmap(pathPath + imgFile), sPath.mask);
                if(sPath.image.isNull())
                {
                    addError(QString("PATH-%1 Brocken image file").arg(i));
                    goto skipPath;
                }

            }
            else
            {
                addError(QString("PATH-%1 Image filename isn't defined").arg(i));
                goto skipPath;
            }

            sPath.grid = pathset.value("grid", default_grid).toInt();

            sPath.animated = (pathset.value("animated", "0").toString()=="1");
            sPath.frames = pathset.value("frames", "1").toInt();
            sPath.framespeed = pathset.value("frame-speed", "125").toInt();

            sPath.frame_h = (sPath.animated? qRound(qreal(sPath.image.height())/sPath.frames) : sPath.image.height());

            sPath.display_frame = pathset.value("display-frame", "0").toInt();
            sPath.row = pathset.value("row", "0").toInt();
            sPath.col = pathset.value("col", "0").toInt();
            sPath.id = i;
            main_wpaths.push_back(sPath);

            //Add to Index
            if(i <= (unsigned int)index_wpaths.size())
            {
                index_wpaths[i].i = i-1;
            }

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

