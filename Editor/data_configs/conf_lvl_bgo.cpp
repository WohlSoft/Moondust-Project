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

void dataconfigs::loadLevelBGO(QProgressDialog *prgs)
{
    unsigned int i;

    obj_bgo sbgo;
    unsigned long bgo_total=0;
    QString bgo_ini = config_dir + "lvl_bgo.ini";

    if(!QFile::exists(bgo_ini))
    {
        addError(QString("ERROR LOADING lvl_bgo.ini: file does not exist"), QtCriticalMsg);
        return;
    }


    QSettings bgoset(bgo_ini, QSettings::IniFormat);
    bgoset.setIniCodec("UTF-8");

    main_bgo.clear();   //Clear old

    bgoset.beginGroup("background-main");
        bgo_total = bgoset.value("total", "0").toInt();
        total_data +=bgo_total;
    bgoset.endGroup();

    if(prgs) prgs->setMaximum(bgo_total);
    if(prgs) prgs->setLabelText(QApplication::tr("Loading BGOs..."));

    ConfStatus::total_bgo = bgo_total;

    //creation of empty indexes of arrayElements
        bgoIndexes bgoIndex;
        for(i=0;i<=bgo_total; i++)
        {
            bgoIndex.i=i;
            bgoIndex.type=0;
            bgoIndex.smbx64_sp=0;
            index_bgo.push_back(bgoIndex);
        }

    if(ConfStatus::total_bgo==0)
    {
        addError(QString("ERROR LOADING lvl_bgo.ini: number of items not define, or empty config"), QtCriticalMsg);
        return;
    }

    for(i=1; i<=bgo_total; i++)
    {
        qApp->processEvents();
        if(prgs)
        {
            if(!prgs->wasCanceled()) prgs->setValue(i);
        }

        bgoset.beginGroup( QString("background-"+QString::number(i)) );
            sbgo.name = bgoset.value("name", "").toString();

                if(sbgo.name=="")
                {
                    addError(QString("BGO-%1 Item name isn't defined").arg(i));
                    goto skipBGO;
                }
            sbgo.group = bgoset.value("group", "_NoGroup").toString();
            sbgo.category = bgoset.value("category", "_Other").toString();
            sbgo.grid = bgoset.value("grid", default_grid).toInt();
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
                mask = QPixmap();
                if(tmp.size()==2) mask = QPixmap(bgoPath + imgFileM);
                sbgo.mask = mask;
                sbgo.image = GraphicsHelps::setAlphaMask(QPixmap(bgoPath + imgFile), sbgo.mask);
                if(sbgo.image.isNull())
                {
                    addError(QString("BGO-%1 Brocken image file").arg(i));
                    goto skipBGO;
                }

            }
            else
            {
                addError(QString("BGO-%1 Image filename isn't defined").arg(i));
                goto skipBGO;
            }
                /*
                {
                    sbgo.image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");
                    sbgo.mask_n = "";
                }*/
            sbgo.climbing = (bgoset.value("climbing", "0").toString()=="1");
            sbgo.animated = (bgoset.value("animated", "0").toString()=="1");
            sbgo.frames = bgoset.value("frames", "1").toInt();
            sbgo.framespeed = bgoset.value("frame-speed", "125").toInt();
            sbgo.id = i;
            main_bgo.push_back(sbgo);

            //Add to Index
            if(i <= (unsigned int)index_bgo.size())
            {
                index_bgo[i].i = i-1;
                index_bgo[i].smbx64_sp = bgoset.value("smbx64-sort-priority", "0").toLongLong();
                //WriteToLog(QtDebugMsg, QString("Got SMBX64 BGO Sorting priority -> %1").arg( index_bgo[i].smbx64_sp ) );
            }

        skipBGO:
        bgoset.endGroup();

        if( bgoset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_bgo.ini N:%1 (bgo-%2)").arg(bgoset.status()).arg(i), QtCriticalMsg);
        }
    }

    if((unsigned int)main_bgo.size()<bgo_total)
    {
        addError(QString("Not all BGOs loaded! Total: %1, Loaded: %2").arg(bgo_total).arg(main_bgo.size()));
    }
}
