/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_manager.h"
#include "../gui/pge_msgbox.h"
#include <common_features/graphics_funcs.h>
#include <common_features/number_limiter.h>

/*****Level BGO************/
PGE_DataArray<obj_bgo>   ConfigManager::lvl_bgo_indexes;
CustomDirManager ConfigManager::Dir_BGO;
QList<SimpleAnimator > ConfigManager::Animator_BGO;
/*****Level BGO************/

bool ConfigManager::loadLevelBGO()
{
    unsigned int i;

    obj_bgo sbgo;
    unsigned long bgo_total=0;
    QString bgo_ini = config_dir + "lvl_bgo.ini";

    if(!QFile::exists(bgo_ini))
    {
        addError(QString("ERROR LOADING lvl_bgo.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox msgBox(NULL, QString("ERROR LOADING lvl_bgo.ini: file does not exist"),
                          PGE_MsgBox::msg_fatal);
        msgBox.exec();
        return false;
    }

    QSettings bgoset(bgo_ini, QSettings::IniFormat);
    bgoset.setIniCodec("UTF-8");

    lvl_bgo_indexes.clear();//Clear old

    bgoset.beginGroup("background-main");
        bgo_total = bgoset.value("total", "0").toInt();
    bgoset.endGroup();

    lvl_bgo_indexes.allocateSlots(bgo_total);

    for(i=1; i<=bgo_total; i++)
    {

        sbgo.isInit = false;
        sbgo.image = NULL;
        sbgo.textureArrayId = 0;
        sbgo.animator_ID = 0;

        bgoset.beginGroup( QString("background-"+QString::number(i)) );

            sbgo.setup.name = bgoset.value("name", "").toString();

                if(sbgo.setup.name=="")
                {
                    addError(QString("BGO-%1 Item name isn't defined").arg(i));
                    goto skipBGO;
                }
            sbgo.setup.group = bgoset.value("group", "_NoGroup").toString();
            sbgo.setup.category = bgoset.value("category", "_Other").toString();
            //sbgo.grid = bgoset.value("grid", default_grid).toInt();
            {
                QString tmpStr=bgoset.value("view", "background").toString();

                if(tmpStr=="foreground2")
                    sbgo.setup.zLayer = 2;
                else
                if(tmpStr=="foreground")
                    sbgo.setup.zLayer = 1;
                else
                if(tmpStr=="background")
                    sbgo.setup.zLayer = 0;
                else
                if(tmpStr=="background2")
                    sbgo.setup.zLayer = -1;
                else
                    sbgo.setup.zLayer = 0;
            }

            sbgo.setup.offsetX = bgoset.value("offset-x", "0").toInt();
            sbgo.setup.offsetY = bgoset.value("offset-y", "0").toInt();
            sbgo.setup.zOffset = bgoset.value("z-offset", 0.0).toDouble();
            imgFile = bgoset.value("image", "").toString();
            {
                QString err;
                GraphicsHelps::getMaskedImageInfo(bgoPath, imgFile, sbgo.setup.mask_n, err);
                sbgo.setup.image_n = imgFile;
                if( imgFile=="" )
                {
                    addError(QString("BGO-%1 Image filename isn't defined.\n%2").arg(i).arg(err));
                    goto skipBGO;
                }
            }
            sbgo.setup.climbing = (bgoset.value("climbing", 0).toBool());
            sbgo.setup.animated = (bgoset.value("animated", 0).toBool());
            sbgo.setup.frames = bgoset.value("frames", "1").toInt();
                NumberLimiter::apply(sbgo.setup.frames, 1u);
            sbgo.setup.framespeed = bgoset.value("frame-speed", "125").toInt();
                NumberLimiter::apply(sbgo.setup.framespeed, 1u);

            sbgo.setup.frame_h = 0;//(sbgo.animated? qRound(qreal(sbgo.image.height())/sbgo.frames) : sbgo.image.height());
                NumberLimiter::apply(sbgo.setup.frame_h, 0u);

            sbgo.setup.display_frame = bgoset.value("display-frame", "0").toInt();
                NumberLimiter::apply(sbgo.setup.display_frame, 0u);
            sbgo.setup.id = i;
            //lvl_bgo.push_back(sbgo);

            //Add to Index
            lvl_bgo_indexes.storeElement(sbgo.setup.id, sbgo);

        skipBGO:
        bgoset.endGroup();

        if( bgoset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_bgo.ini N:%1 (bgo-%2)").arg(bgoset.status()).arg(i), QtCriticalMsg);
        }
    }

    if((unsigned int)lvl_bgo_indexes.stored()<bgo_total)
    {
        addError(QString("Not all BGOs loaded! Total: %1, Loaded: %2").arg(bgo_total).arg(lvl_bgo_indexes.stored()));
        PGE_MsgBox msgBox(NULL, QString("Not all BGOs loaded! Total: %1, Loaded: %2").arg(bgo_total).arg(lvl_bgo_indexes.stored()),
                          PGE_MsgBox::msg_error);
        msgBox.exec();
    }
    return true;
}
