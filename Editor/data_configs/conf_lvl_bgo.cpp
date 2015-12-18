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

#include <main_window/global_settings.h>
#include <common_features/graphics_funcs.h>

#include "data_configs.h"

obj_bgo::obj_bgo()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = NULL;
}

void obj_bgo::copyTo(obj_bgo &bgo)
{
    /* for internal usage */
    bgo.isValid         = isValid;
    bgo.animator_id     = animator_id;
    bgo.cur_image       = cur_image;
    if(cur_image==NULL)
        bgo.cur_image   = &image;
    bgo.frame_h         = frame_h;
    /* for internal usage */

    bgo.id              = id;
    bgo.name            = name;
    bgo.group           = group;
    bgo.category        = category;
    bgo.grid            = grid;
    bgo.offsetX         = offsetX;
    bgo.offsetY         = offsetY;
    bgo.zOffset         = zOffset;
    bgo.image_n         = image_n;
    bgo.mask_n          = mask_n;
    bgo.climbing        = climbing;
    bgo.animated        = animated;
    bgo.frames          = frames;
    bgo.framespeed      = framespeed;
    bgo.display_frame   = display_frame;
}

//long dataconfigs::getBgoI(unsigned long itemID)
//{
//    long j;
//    bool found=false;

//    if(itemID < (unsigned int)index_bgo.size())
//    {
//        j = index_bgo[itemID].i;

//        if(j < main_bgo.size())
//        {
//            if( main_bgo[j].id == itemID)
//                found=true;
//        }
//    }

//    if(!found)
//    {
//        for(j=0; j < main_bgo.size(); j++)
//        {
//            if(main_bgo[j].id==itemID)
//            {
//                found=true;
//                break;
//            }
//        }
//    }

//    if(!found) j=-1;
//    return j;
//}


bool dataconfigs::loadLevelBGO(obj_bgo &sbgo, QString section, obj_bgo *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal=!setup;
    QString tmpStr, errStr;
    if(internal) setup=new QSettings(iniFile, QSettings::IniFormat);

    setup->beginGroup( section );


    sbgo.name = setup->value("name", (merge_with? merge_with->name : "") ).toString();
    if(sbgo.name=="")
    {
        addError(QString("%1 Item name isn't defined").arg(section.toUpper()));
        goto abort;
    }

    sbgo.group =    setup->value("group", "_NoGroup").toString();
    sbgo.category = setup->value("category", "_Other").toString();
    sbgo.grid =     setup->value("grid", default_grid).toInt();

    {
        tmpStr=setup->value("view", "background").toString();

        if(tmpStr=="foreground2")
            sbgo.view = 2;
        else
        if(tmpStr=="foreground")
            sbgo.view = 1;
        else
        if(tmpStr=="background")
            sbgo.view = 0;
        else
        if(tmpStr=="background2")
            sbgo.view = -1;
        else
            sbgo.view = 0;
    }

    sbgo.offsetX =      setup->value("offset-x", "0").toInt();
    sbgo.offsetY =      setup->value("offset-y", "0").toInt();
    sbgo.zOffset =      setup->value("z-offset", "0").toInt();

    sbgo.image_n =      setup->value("image", "").toString();
    /***************Load image*******************/
    GraphicsHelps::loadMaskedImage(bgoPath,
       sbgo.image_n, sbgo.mask_n,
       sbgo.image,   sbgo.mask,
       errStr);

    if(!errStr.isEmpty())
    {
        addError(QString("%1 %2").arg(section.toUpper()).arg(errStr));
        goto abort;
    }
    /***************Load image*end***************/

    sbgo.climbing =     (setup->value("climbing", "0").toString()=="1");
    sbgo.animated =     (setup->value("animated", "0").toString()=="1");
    sbgo.frames =        setup->value("frames", "1").toInt();
    sbgo.framespeed =    setup->value("frame-speed", "125").toInt();

    sbgo.frame_h =      (sbgo.animated?
                             qRound(
                                 qreal(sbgo.image.height())/
                                 sbgo.frames)
                           : sbgo.image.height());

    sbgo.display_frame = setup->value("display-frame", "0").toInt();
    sbgo.isValid = true;//Mark BGO as valid object

    abort:
        setup->endGroup();
        if(internal) delete setup;
    return valid;
}


void dataconfigs::loadLevelBGO()
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
//    index_bgo.clear();

    bgoset.beginGroup("background-main");
        bgo_total = bgoset.value("total", "0").toInt();
        total_data +=bgo_total;
    bgoset.endGroup();

    emit progressMax(bgo_total);
    emit progressTitle(QObject::tr("Loading BGOs..."));

    ConfStatus::total_bgo = bgo_total;

//    //creation of empty indexes of arrayElements
//    bgoIndexes bgoIndex;
//    for(i=0;i<=bgo_total; i++)
//    {
//        bgoIndex.i=i;
//        bgoIndex.type=0;
//        bgoIndex.smbx64_sp=0;
//        index_bgo.push_back(bgoIndex);
//    }

    if(ConfStatus::total_bgo==0)
    {
        addError(QString("ERROR LOADING lvl_bgo.ini: number of items not define, or empty config"), QtCriticalMsg);
        return;
    }

    for(i=1; i<=bgo_total; i++)
    {
        emit progressValue(i);

        if( loadLevelBGO(sbgo, QString("background-"+QString::number(i)), 0, "", &bgoset) )
        {
            //index_bgo[i].i = i-1;
            //index_bgo[i].smbx64_sp = setup->value("smbx64-sort-priority", "0").toLongLong();
            sbgo.id = i;
            main_bgo[i] = sbgo;
        }

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
