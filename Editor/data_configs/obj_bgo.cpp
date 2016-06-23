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

#include <QFile>

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

    bgo.zLayer          = zLayer;
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

/*!
 * \brief Loads signe BGO configuration
 * \param sbgo Target BGO structure
 * \param section Name of INI-section where look for a BGO
 * \param merge_with Source of already loaded BGO structure to provide default settings per every BGO
 * \param iniFile INI-file where look for a BGO
 * \param setup loaded INI-file descriptor to load from global nested INI-file
 * \return true on success loading, false if error has occouped
 */
bool dataconfigs::loadLevelBGO(obj_bgo &sbgo, QString section, obj_bgo *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal=!setup;
    QString tmpStr, errStr;
    if(internal) setup=new QSettings(iniFile, QSettings::IniFormat);

    setup->beginGroup( section );


    sbgo.name = setup->value("name", (merge_with? merge_with->name : section) ).toString();
    if(sbgo.name=="")
    {
        addError(QString("%1 Item name isn't defined").arg(section.toUpper()));
        valid=false;
        goto abort;
    }

    sbgo.group =    setup->value("group", (merge_with? merge_with->group : "_NoGroup")).toString();
    sbgo.category = setup->value("category", (merge_with? merge_with->category : "_Other")).toString();
    sbgo.grid =     setup->value("grid", (merge_with? merge_with->grid : default_grid)).toInt();

    {
        tmpStr=setup->value("view", "background").toString();

        if(tmpStr=="foreground2")
            sbgo.zLayer = obj_bgo::z_foreground_2;
        else
        if(tmpStr=="foreground")
            sbgo.zLayer = obj_bgo::z_foreground_1;
        else
        if(tmpStr=="background")
            sbgo.zLayer = (merge_with ? merge_with->zLayer : obj_bgo::z_background_1);
        else
        if(tmpStr=="background2")
            sbgo.zLayer = obj_bgo::z_background_2;
        else
            sbgo.zLayer = obj_bgo::z_background_1;
    }

    sbgo.offsetX =      setup->value("offset-x", (merge_with? merge_with->offsetX : 0)).toInt();
    sbgo.offsetY =      setup->value("offset-y", (merge_with? merge_with->offsetY : 0)).toInt();
    sbgo.zOffset =      setup->value("z-offset", (merge_with? merge_with->zOffset : 0.0)).toDouble();

    sbgo.image_n =      setup->value("image", (merge_with ? merge_with->image_n : "")).toString();
    /***************Load image*******************/
    if(!merge_with)
    {
        GraphicsHelps::loadMaskedImage(bgoPath,
           sbgo.image_n, sbgo.mask_n,
           sbgo.image,
           errStr);

        if(!errStr.isEmpty())
        {
            addError(QString("%1 %2").arg(section.toUpper()).arg(errStr));
            valid=false;
            //goto abort;
        }
    }
    /***************Load image*end***************/

    sbgo.climbing = (setup->value("climbing", (merge_with? merge_with->climbing : false)).toBool());
    sbgo.animated = (setup->value("animated", (merge_with? merge_with->animated : false)).toBool());
    sbgo.frames =        setup->value("frames", (merge_with? merge_with->frames : 1)).toInt();
    sbgo.framespeed =    setup->value("frame-speed", (merge_with? merge_with->framespeed : 125)).toInt();

    sbgo.frame_h =      (sbgo.animated?
                             qRound(
                                 qreal(sbgo.image.height())/
                                 sbgo.frames)
                           : sbgo.image.height());

    sbgo.display_frame = setup->value("display-frame", (merge_with? merge_with->display_frame : 0)).toInt();
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
        addError(QString("ERROR LOADING lvl_bgo.ini: file does not exist"),  PGE_LogLevel::Critical);
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

    emit progressPartNumber(1);
    emit progressMax(bgo_total);
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading BGOs..."));

    ConfStatus::total_bgo = bgo_total;

    main_bgo.allocateSlots(bgo_total);

    if(ConfStatus::total_bgo==0)
    {
        addError(QString("ERROR LOADING lvl_bgo.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i<=bgo_total; i++)
    {
        emit progressValue(i);
        if( loadLevelBGO(sbgo, QString("background-"+QString::number(i)), 0, "", &bgoset) )
        {
            sbgo.id = i;
            main_bgo.storeElement(i, sbgo);
        }

        if( bgoset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_bgo.ini N:%1 (bgo-%2)").arg(bgoset.status()).arg(i), PGE_LogLevel::Critical);
        }
    }

    if((unsigned int)main_bgo.stored()<bgo_total)
    {
        addError(QString("Not all BGOs loaded! Total: %1, Loaded: %2").arg(bgo_total).arg(main_bgo.stored()));
    }
}
