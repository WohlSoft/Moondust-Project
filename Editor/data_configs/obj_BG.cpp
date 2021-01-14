/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
    if(!cur_image)
        bg.cur_image   = &image;
    if(!cur_image_second)
        bg.cur_image_second = &image;
    bg.setup = setup;
}

bool DataConfig::loadLevelBackground(obj_BG &sbg,
                                      QString section,
                                      obj_BG *merge_with,
                                      QString iniFile,
                                      IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    QString errStr;
    if(internal)
        setup = new IniProcessing(iniFile);

    m_errOut = merge_with ? ERR_CUSTOM : ERR_GLOBAL;

    // BG entry must have header section
    //if(!openSection(setup, section.toStdString(), internal))
    //    return false;

    // BG allowed to have no header section
    if(!setup->beginGroup(section.toStdString()) && internal)
        setup->beginGroup("General");

    if(sbg.setup.parse(setup, folderLvlBG.graphics, defaultGrid.bgo, merge_with ? &merge_with->setup : nullptr, &errStr))
        sbg.isValid = true;
    else
    {
        addError(errStr);
        sbg.isValid = false;
    }

    if(sbg.setup.fill_color != "auto")
        sbg.fill_color = QColor(sbg.setup.fill_color);

    closeSection(setup);
    if(internal)
        delete setup;
    return valid;
}


void DataConfig::loadLevelBackgrounds()
{
    unsigned int i;
    obj_BG sbg;
    unsigned long bg_total = 0;
    bool useDirectory = false;

    QString bg_ini = getFullIniPath("lvl_bkgrd.ini");
    if(bg_ini.isEmpty())
        return;

    IniProcessing setup(bg_ini);

    QString maskNameDummy;
    folderLvlBG.items.clear();
    main_bg.clear();   //Clear old

    if(!openSection(&setup, "background2-main"))
        return;
    {
        setup.read("total", bg_total, 0);
        total_data += bg_total;
        setup.read("config-dir", folderLvlBG.items, "");
        setup.read("config-dir", folderLvlBG.items, folderLvlBG.items);
        if(!folderLvlBG.items.isEmpty())
        {
            folderLvlBG.items = config_dir + folderLvlBG.items;
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

    for(i = 1; i <= bg_total; i++)
    {
        emit progressValue(int(i));

        bool valid = false;
        if(useDirectory)
            valid = loadLevelBackground(sbg, "background2", nullptr, QString("%1/background2-%2.ini").arg(folderLvlBG.items).arg(i));
        else
            valid = loadLevelBackground(sbg, QString("background2-%1").arg(i), 0, "", &setup);

        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(folderLvlBG.graphics,
                                           sbg.setup.image_n, maskNameDummy,
                                           sbg.image,
                                           errStr);

            if(!errStr.isEmpty())
            {
                valid = false;
                addError(QString("BG-%1 Image: %2").arg(i).arg(errStr));
            }
            else
            {
                /* =======================================================================================
                   TODO: Remove this after implementing of support for in-editor animating backgrounds     */
                if(sbg.setup.animated)
                    sbg.image = sbg.image.copy(0, 0, sbg.image.width(), static_cast<int>(sbg.setup.frame_h));
                /* ======================================================================================= */
            }

            if(sbg.setup.type == BgSetup::BG_TYPE_DoubleRow)
            {
                GraphicsHelps::loadMaskedImage(folderLvlBG.graphics,
                                               sbg.setup.second_image_n, maskNameDummy,
                                               sbg.second_image,
                                               errStr);

                if(!errStr.isEmpty())
                {
                    valid = false;
                    addError(QString("BG-%1 Second image: %2").arg(i).arg(errStr));
                }
            }
        }

        sbg.setup.id = i;
        main_bg.storeElement(static_cast<int>(i), sbg, valid);

        if(setup.lastError() != IniProcessing::ERR_OK)
            addError(QString("ERROR LOADING lvl_bgrnd.ini N:%1 (background2-%2)").arg(setup.lastError()).arg(i), PGE_LogLevel::Critical);
    }
}
