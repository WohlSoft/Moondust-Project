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

#include <qmath.h>

#include <common_features/app_path.h>
#include <common_features/themes.h>
#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>

#include "data_configs.h"

void obj_BG::copyTo(obj_BG &bg)
{
    /* for internal usage */
    bg.isValid         = isValid;
    bg.animator_id     = animator_id;
    bg.cur_image       = cur_image;
    if(cur_image == nullptr)
        bg.cur_image   = &image;
    if(cur_image_second == nullptr)
        bg.cur_image_second = &image;
    bg.frame_h         = frame_h;
    /* for internal usage */
}

bool dataconfigs::loadLevelBackground(obj_BG &sbg,
                                      QString section,
                                      obj_BG *merge_with,
                                      QString iniFile,
                                      IniProcessing *setup)
{
    #define pMerge(param, def) (merge_with ? (merge_with->param) : (def))
    bool valid=true;
    bool internal = !setup;
    QString errStr, imgFile;
    if(internal)
    {
        setup = new IniProcessing(iniFile);
    }

    if(!openSection(setup, section.toStdString()))
        return false;

        setup->read("name", sbg.name, pMerge(name, PGEString()));
        if(sbg.name.isEmpty())
        {
            addError(QString("%1 Item name isn't defined").arg(section.toUpper()));
            valid=false;
            goto abort;
        }

        setup->readEnum("type", sbg.type,
                        (merge_with ? merge_with->type : 0),
                        {
                            {"single-row", 0},
                            {"double-row", 1},
                            {"tiled", 2}
                        });

        sbg.repeat_h =(qFabs(setup->value("repeat-h", pMerge(repeat_h, 2.0)).toDouble()));

        setup->readEnum("repeat-v", sbg.repead_v,
                        (merge_with ? merge_with->repead_v : 0),
                        {
                            {"NR", 0},
                            {"ZR", 1},
                            {"RP", 2},
                            {"RZ", 3}
                        });

        setup->read("image", sbg.image_n, pMerge(image_n, sbg.image_n));
        if(!merge_with)
        {
            if(!sbg.image_n.isEmpty())
            {
                GraphicsHelps::loadMaskedImage(BGPath,
                    sbg.image_n, imgFile,
                    sbg.image,
                    errStr);

                if(!errStr.isEmpty())
                {
                    addError(QString("%1 %2").arg(section).arg(errStr));
                    valid=false;
                    //goto abort;
                }

            } else {
                addError(QString("%1 (%2) Image filename isn't defined").arg(section).arg(iniFile));
                valid=false;
                //goto abort;
            }
        }

        setup->readEnum("attached", sbg.attached,
                                    pMerge(attached, 0),
                        {
                            {"bottom", 0},
                            {"top", 1}
                        });

        setup->read("tiled-in-editor", sbg.editing_tiled, pMerge(editing_tiled, false));

        setup->read("magic",    sbg.magic, pMerge(magic, false));
        setup->read("magic-strips", sbg.magic_strips, pMerge(magic_strips, 1));
        setup->read("magic-splits", sbg.magic_splits, pMerge(magic_splits, PGEStringLit("0")));
        setup->read("magic-speeds", sbg.magic_speeds, pMerge(magic_speeds, PGEStringLit("0")));

        setup->read("animated",      sbg.animated,         pMerge(animated, false));//animated
        setup->read("frames",        sbg.frames,           pMerge(frames, 1));
        setup->read("framespeed",    sbg.framespeed,       pMerge(framespeed, 128));
        setup->read("display-frame", sbg.display_frame,    pMerge(display_frame, 0));
        //frames

        if(sbg.type==1)
        {
            setup->read("second-image", sbg.second_image_n, pMerge(second_image_n, sbg.second_image_n));
            if(!merge_with)
            {
                if(!sbg.second_image_n.isEmpty())
                {
                    GraphicsHelps::loadMaskedImage(BGPath,
                       sbg.second_image_n, imgFile,
                       sbg.second_image,
                       errStr);
                } else {
                    sbg.second_image = Themes::Image(Themes::dummy_bg);
                }
            }

            sbg.second_repeat_h = (qFabs(setup->value("second-repeat-h", pMerge(second_repeat_h, 2.0)).toDouble()));

            setup->readEnum("second-repeat-v",
                            sbg.second_repeat_v,
                            pMerge(second_repeat_v, 0u),
                            {
                                {"NR", 0},
                                {"ZR", 1},
                                {"RP", 2},
                                {"RZ", 3}
                            });

            setup->readEnum("second-attached",
                            sbg.second_attached,
                            pMerge(second_attached, 0),
                            {
                                {"overfirst", 0},
                                {"bottom", 1},
                                {"top", 2}
                            });
        }

        if(sbg.animated)
        {
            int fHeight = sbg.image.height() / int(sbg.frames);
            sbg.image = sbg.image.copy(0, 0, sbg.image.width(), fHeight );
        }

        sbg.isValid = true;

    abort:
        closeSection(setup);
        if(internal)
            delete setup;

    #undef pMerge
    #undef pMergeMe
    return valid;
}


void dataconfigs::loadLevelBackgrounds()
{
    unsigned int i;
    obj_BG sbg;
    unsigned long bg_total=0;
    bool useDirectory=false;

    QString bg_ini = getFullIniPath("lvl_bkgrd.ini");
    if( bg_ini.isEmpty() )
        return;

    QString nestDir = "";

    IniProcessing setup(bg_ini);

    main_bg.clear();   //Clear old

    if(!openSection(&setup, "background2-main"))
        return;
    {
        setup.read("total", bg_total, 0);
        total_data += bg_total;
        setup.read("config-dir", nestDir, nestDir);
        if(!nestDir.isEmpty())
        {
            nestDir = config_dir + nestDir;
            useDirectory = true;
        }
    }
    closeSection(&setup);

    emit progressPartNumber(0);
    emit progressMax(int(bg_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Backgrounds..."));

    ConfStatus::total_bg = long(bg_total);
    main_bg.allocateSlots(int(bg_total));

    if(ConfStatus::total_bg == 0)
    {
        addError(QString("ERROR LOADING lvl_bkgrd.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i <= bg_total; i++)
    {
        emit progressValue(int(i));

        bool valid = false;
        if(useDirectory)
        {
            valid = loadLevelBackground(sbg, "background2", nullptr, QString("%1/background2-%2.ini").arg(nestDir).arg(i));
        }
        else
        {
            valid = loadLevelBackground(sbg, QString("background2-%1").arg(i), 0, "", &setup);
        }

        sbg.id = i;
        main_bg.storeElement(int(i), sbg, valid);

        if( setup.lastError() != IniProcessing::ERR_OK )
        {
            addError(QString("ERROR LOADING lvl_bgrnd.ini N:%1 (background2-%2)").arg(setup.lastError()).arg(i), PGE_LogLevel::Critical);
        }
    }
}
