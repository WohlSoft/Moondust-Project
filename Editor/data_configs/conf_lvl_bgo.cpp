/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "data_configs.h"

void dataconfigs::loadLevelBGO()
{
    unsigned int i;

    obj_bgo sbgo;
    unsigned long bgo_total=0;
    QString bgo_ini = config_dir + "lvl_bgo.ini";

    if(!QFile::exists(bgo_ini))
    {
        WriteToLog(QtCriticalMsg, QString("ERROR LOADING OF lvl_bgo.ini: file not exist"));
          return;
    }


    QSettings bgoset(bgo_ini, QSettings::IniFormat);
    bgoset.setIniCodec("UTF-8");

    main_bgo.clear();   //Clear old

    bgoset.beginGroup("background-main");
        bgo_total = bgoset.value("total", "0").toInt();
        total_data +=bgo_total;
    bgoset.endGroup();

    //creation of empty indexes of arrayElements
        bgoIndexes bgoIndex;
        for(i=0;i<bgo_total+1; i++)
        {
            bgoIndex.i=i;
            bgoIndex.type=0;
            index_bgo.push_back(bgoIndex);
        }


    for(i=1; i<=bgo_total; i++)
    {
        bgoset.beginGroup( QString("background-"+QString::number(i)) );
            sbgo.name = bgoset.value("name", "").toString();
            sbgo.type = bgoset.value("type", "other").toString();
            sbgo.grid = bgoset.value("grid", "32").toInt();
            sbgo.view = (int)(bgoset.value("view", "background").toString()=="foreground");
            sbgo.offsetX = bgoset.value("offset-x", "0").toInt();
            sbgo.offsetY = bgoset.value("offset-y", "0").toInt();
            sbgo.zOffset = bgoset.value("z-offset", "0").toInt();
            imgFile = bgoset.value("image", "").toString();
            sbgo.image_n = imgFile;
            if( (imgFile!="") )
            {
                tmp = imgFile.split(".", QString::SkipEmptyParts);
                if(tmp.size()==2)
                    imgFileM = tmp[0] + "m." + tmp[1];
                else
                    imgFileM = "";
                sbgo.mask_n = imgFileM;
                if(tmp.size()==2) mask = QBitmap(bgoPath + imgFileM);
                sbgo.mask = mask;
                sbgo.image = QPixmap(bgoPath + imgFile);
                if(tmp.size()==2) sbgo.image.setMask(mask);
            }
            else
            {
                sbgo.image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");
                sbgo.mask_n = "";
            }
            sbgo.climbing = (bgoset.value("climbing", "0").toString()=="1");
            sbgo.animated = (bgoset.value("animated", "0").toString()=="1");
            sbgo.frames = bgoset.value("frames", "1").toInt();
            sbgo.framespeed = bgoset.value("frame-speed", "125").toInt();
            sbgo.id = i;
            main_bgo.push_back(sbgo);

            //Add to Index
            if(i < (unsigned int)main_bgo.size())
                index_bgo[i].i = i;

        bgoset.endGroup();

        if( bgoset.status() != QSettings::NoError )
        {
            WriteToLog(QtCriticalMsg, QString("ERROR LOADING OF lvl_bgo.ini N:%1 (bgo-%2)").arg(bgoset.status()).arg(i));
        }
    }

    if((unsigned int)main_bgo.size()<bgo_total)
    {
        WriteToLog(QtWarningMsg, QString("Not all BGOs loaded: total:%1, loaded: %2)").arg(bgo_total).arg(main_bgo.size()));
    }
}
