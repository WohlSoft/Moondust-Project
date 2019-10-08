/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>

#include "data_configs.h"

bool dataconfigs::loadWorldScene(obj_w_scenery &sScene, QString section, obj_w_scenery *merge_with, QString iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    QString errStr;
    if(internal)
        setup = new IniProcessing(iniFile);

    m_errOut = merge_with ? ERR_CUSTOM : ERR_GLOBAL;

    if(!openSection(setup, section.toStdString(), internal))
        return false;

    if(sScene.setup.parse(setup, folderWldScenery.graphics, defaultGrid.scenery, merge_with ? &merge_with->setup : nullptr, &errStr))
        sScene.isValid = true;
    else
    {
        addError(errStr);
        sScene.isValid = false;
    }
    sScene.m_itemType = ItemTypes::WLD_Scenery;
    closeSection(setup);
    if(internal)
        delete setup;
    return valid;
}


void dataconfigs::loadWorldScene()
{
    unsigned int i;

    obj_w_scenery sScene;
    unsigned long scenery_total = 0;
    bool useDirectory = false;

    QString scene_ini = getFullIniPath("wld_scenery.ini");
    if(scene_ini.isEmpty())
        return;

    IniProcessing setup(scene_ini);

    folderWldScenery.items.clear();
    main_wscene.clear();   //Clear old

    if(!openSection(&setup, "scenery-main"))
        return;
    {
        setup.read("total", scenery_total, 0);
        setup.read("grid", defaultGrid.scenery, defaultGrid.scenery);
        total_data += scenery_total;
        setup.read("config-dir", folderWldScenery.items, "");
        setup.read("extra-settings", folderWldScenery.extraSettings, folderWldScenery.items);
        setup.read("extra-settings-combined-view", folderWldScenery.extraSettingsCombinedView, true);
        if(!folderWldScenery.items.isEmpty())
        {
            folderWldScenery.items = config_dir + folderWldScenery.items;
            useDirectory = true;
        }
    }
    closeSection(&setup);

    emit progressPartNumber(5);
    emit progressMax(int(scenery_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Sceneries..."));

    ConfStatus::total_wscene = long(scenery_total);

    if(ConfStatus::total_wscene == 0)
    {
        addError(QString("ERROR LOADING wld_scenery.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    main_wscene.allocateSlots(int(scenery_total));

    for(i = 1; i <= scenery_total; i++)
    {
        emit progressValue(int(i));
        bool valid = false;
        if(useDirectory)
            valid = loadWorldScene(sScene, "scenery", nullptr, QString("%1/scenery-%2.ini").arg(folderWldScenery.items).arg(i));
        else
            valid = loadWorldScene(sScene, QString("scenery-%1").arg(i), 0, "", &setup);
        /***************Load image*******************/
        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(folderWldScenery.graphics,
                                           sScene.setup.image_n, sScene.setup.mask_n,
                                           sScene.image,
                                           errStr);
            if(!errStr.isEmpty())
            {
                valid = false;
                addError(QString("SCENE-%1 %2").arg(i).arg(errStr));
            }
        }
        /***************Load image*end***************/
        sScene.setup.id = i;
        main_wscene.storeElement(int(i), sScene, valid);

        if(setup.lastError() != IniProcessing::ERR_OK)
            addError(QString("ERROR LOADING wld_scenery.ini N:%1 (scene-%2)").arg(setup.lastError()).arg(i), PGE_LogLevel::Critical);
    }

    if(uint(main_wscene.stored()) < scenery_total)
        addError(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(scenery_total).arg(main_wscene.stored()));
}
