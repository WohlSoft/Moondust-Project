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
    cur_image   = nullptr;
}

void obj_bgo::copyTo(obj_bgo &bgo)
{
    /* for internal usage */
    bgo.isValid         = isValid;
    bgo.animator_id     = animator_id;
    bgo.cur_image       = cur_image;
    if(cur_image==nullptr)
        bgo.cur_image   = &image;
    bgo.setup.frame_h         = setup.frame_h;
    /* for internal usage */

    bgo.setup.id              = setup.id;
    bgo.setup.name            = setup.name;
    bgo.setup.group           = setup.group;
    bgo.setup.category        = setup.category;
    bgo.setup.grid            = setup.grid;

    bgo.setup.zLayer          = setup.zLayer;
    bgo.setup.offsetX         = setup.offsetX;
    bgo.setup.offsetY         = setup.offsetY;
    bgo.setup.zOffset         = setup.zOffset;

    bgo.setup.image_n         = setup.image_n;
    bgo.setup.mask_n          = setup.mask_n;

    bgo.setup.climbing        = setup.climbing;
    bgo.setup.animated        = setup.animated;
    bgo.setup.frames          = setup.frames;
    bgo.setup.framespeed      = setup.framespeed;
    bgo.setup.display_frame   = setup.display_frame;
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

    if(!openSection(setup, section))
        return false;

    sbgo.setup.name = setup->value("name", (merge_with? merge_with->setup.name : section) ).toString();
    if(sbgo.setup.name=="")
    {
        addError(QString("%1 Item name isn't defined").arg(section.toUpper()));
        valid=false;
        goto abort;
    }

    sbgo.setup.group =    setup->value("group", (merge_with? merge_with->setup.group : "_NoGroup")).toString();
    sbgo.setup.category = setup->value("category", (merge_with? merge_with->setup.category : "_Other")).toString();
    sbgo.setup.grid =     setup->value("grid", (merge_with? merge_with->setup.grid : default_grid)).toUInt();

    {
        tmpStr=setup->value("view", "background").toString();

        if(tmpStr=="foreground2")
            sbgo.setup.zLayer = obj_bgo::z_foreground_2;
        else
        if(tmpStr=="foreground")
            sbgo.setup.zLayer = obj_bgo::z_foreground_1;
        else
        if(tmpStr=="background")
            sbgo.setup.zLayer = (merge_with ? merge_with->setup.zLayer : obj_bgo::z_background_1);
        else
        if(tmpStr=="background2")
            sbgo.setup.zLayer = obj_bgo::z_background_2;
        else
            sbgo.setup.zLayer = obj_bgo::z_background_1;
    }

    sbgo.setup.offsetX =      setup->value("offset-x", (merge_with? merge_with->setup.offsetX : 0)).toInt();
    sbgo.setup.offsetY =      setup->value("offset-y", (merge_with? merge_with->setup.offsetY : 0)).toInt();
    sbgo.setup.zOffset =      setup->value("z-offset", (merge_with? merge_with->setup.zOffset : 0.0)).toDouble();

    sbgo.setup.image_n =      setup->value("image", (merge_with ? merge_with->setup.image_n : "")).toString();
    /***************Load image*******************/
    if(!merge_with)
    {
        GraphicsHelps::loadMaskedImage(bgoPath,
           sbgo.setup.image_n, sbgo.setup.mask_n,
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

    sbgo.setup.climbing = (setup->value("climbing", (merge_with? merge_with->setup.climbing : false)).toBool());
    sbgo.setup.animated = (setup->value("animated", (merge_with? merge_with->setup.animated : false)).toBool());
    sbgo.setup.frames =        setup->value("frames", (merge_with? merge_with->setup.frames : 1)).toUInt();
    sbgo.setup.framespeed =    setup->value("frame-speed", (merge_with? merge_with->setup.framespeed : 125)).toUInt();

    sbgo.setup.frame_h =   uint(sbgo.setup.animated?
                             qRound(
                                 qreal(sbgo.image.height())/
                                 sbgo.setup.frames)
                           : sbgo.image.height());

    sbgo.setup.display_frame = setup->value("display-frame", (merge_with? merge_with->setup.display_frame : 0)).toUInt();
    sbgo.isValid = true; //Mark BGO as valid object

    abort:
        closeSection(setup);

        if(internal) delete setup;
    return valid;
}


void dataconfigs::loadLevelBGO()
{
    unsigned int i;

    obj_bgo sbgo;
    unsigned long bgo_total=0;
    QString bgo_ini = getFullIniPath("lvl_bgo.ini");
    if(bgo_ini.isEmpty())
        return;

    QSettings bgoset(bgo_ini, QSettings::IniFormat);
    bgoset.setIniCodec("UTF-8");

    main_bgo.clear();   //Clear old

    if(!openSection( &bgoset, "background-main")) return;
        bgo_total = bgoset.value("total", 0).toUInt();
        total_data +=bgo_total;
    closeSection(&bgoset);

    emit progressPartNumber(1);
    emit progressMax(int(bgo_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading BGOs..."));

    ConfStatus::total_bgo = long(bgo_total);

    main_bgo.allocateSlots(int(bgo_total));

    if(ConfStatus::total_bgo==0)
    {
        addError(QString("ERROR LOADING lvl_bgo.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i<=bgo_total; i++)
    {
        emit progressValue(int(i));
        bool valid = loadLevelBGO(sbgo, QString("background-%1").arg(i), 0, "", &bgoset);
        sbgo.setup.id = i;
        main_bgo.storeElement(int(i), sbgo, valid);

        if( bgoset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_bgo.ini N:%1 (bgo-%2)").arg(bgoset.status()).arg(i), PGE_LogLevel::Critical);
        }
    }

    if(uint(main_bgo.stored()) < bgo_total)
    {
        addError(QString("Not all BGOs loaded! Total: %1, Loaded: %2").arg(bgo_total).arg(main_bgo.stored()));
    }
}
