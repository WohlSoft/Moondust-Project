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

bool ConfigManager::loadLevelBGO(obj_bgo &sbgo, QString section, obj_bgo *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal = !setup;
    QString errStr;
    if(internal)
        setup=new QSettings(iniFile, QSettings::IniFormat);

    sbgo.isInit = merge_with ? merge_with->isInit : false;
    sbgo.image  = merge_with ? merge_with->image : nullptr;
    sbgo.textureArrayId = merge_with ? merge_with->textureArrayId : 0;
    sbgo.animator_ID = merge_with ? merge_with->animator_ID : 0;

    setup->beginGroup( section );
        if(sbgo.setup.parse(setup, bgoPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
        {
            valid=true;
        } else {
            addError(errStr);
            valid=false;
        }
    setup->endGroup();
    if(internal) delete setup;
    return valid;
}


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
        if(!loadLevelBGO(sbgo, QString("background-%1").arg(i), nullptr, "", &bgoset))
            return false;

        sbgo.setup.id = i;
        //Store loaded config
        lvl_bgo_indexes.storeElement(sbgo.setup.id, sbgo);
        //Load custom config if possible
        loadCustomConfig<obj_bgo>(lvl_bgo_indexes, i, Dir_BGO, "background", "background", &loadLevelBGO);

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
