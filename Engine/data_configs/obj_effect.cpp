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
#include "config_manager_private.h"
#include "../gui/pge_msgbox.h"

/*****Level Effects************/
PGE_DataArray<obj_effect>   ConfigManager::lvl_effects_indexes;
CustomDirManager            ConfigManager::Dir_EFFECT;
Effects_GlobalSetup         ConfigManager::g_setup_effects;
/*****Level Effects************/

bool ConfigManager::loadLevelEffects()
{
    unsigned long i;
    obj_effect seffect;
    unsigned long effects_total = 0;
    QString effects_ini = config_dir + "lvl_effects.ini";

    if(!QFile::exists(effects_ini))
    {
        addError(QString("ERROR LOADING lvl_effects.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox msgBox(NULL, QString("ERROR LOADING lvl_bgo.ini: file does not exist"),
                          PGE_MsgBox::msg_fatal);
        msgBox.exec();
        return false;
    }

    QSettings effectset(effects_ini, QSettings::IniFormat);
    effectset.setIniCodec("UTF-8");
    lvl_effects_indexes.clear();   //Clear old
    effectset.beginGroup("effects-main");
    effects_total = effectset.value("total", 0).toULongLong();
    //Default effects
    g_setup_effects.m_default.fill("default", &effectset);
    g_setup_effects.m_smoke.fill("smoke", &effectset);
    g_setup_effects.m_waterSplash.fill("water-splash", &effectset);
    g_setup_effects.m_playerTransform.fill("player-transform", &effectset);
    g_setup_effects.m_groundSlide.fill("ground-slide", &effectset);
    g_setup_effects.m_stomp.fill("stomp", &effectset);
    g_setup_effects.m_smash.fill("smash", &effectset);
    g_setup_effects.m_blockDestroy.fill("block-destroy",  &effectset);
    g_setup_effects.m_blockCoinHit.fill("block-coin-hit", &effectset);
    effectset.endGroup();
    lvl_effects_indexes.allocateSlots(effects_total);

    for(i = 1; i <= effects_total; i++)
    {
        seffect.isInit = false;
        seffect.image = NULL;
        seffect.textureArrayId = 0;
        effectset.beginGroup(QString("effect-" + QString::number(i)));
        seffect.name = effectset.value("name", "").toString();

        if(seffect.name == "")
        {
            addError(QString("EFFECT-%1 Item name isn't defined").arg(i));
            goto skipEffect;
        }

        imgFile = effectset.value("image", "").toString();
        seffect.image_n = imgFile;

        if((imgFile != ""))
        {
            tmp = imgFile.split(".", QString::SkipEmptyParts);

            if(tmp.size() == 2)
                imgFileM = tmp[0] + "m." + tmp[1];
            else
                imgFileM = "";

            seffect.mask_n = imgFileM;
        }
        else
        {
            addError(QString("EFFECT-%1 Image filename isn't defined").arg(i));
            goto skipEffect;
        }

        seffect.frames = effectset.value("frames", "1").toUInt();
        seffect.framestyle = effectset.value("frame-style", "0").toInt();
        seffect.framespeed = effectset.value("frame-speed", "125").toInt();
        seffect.frame_h = 0;
        seffect.id = uint64_t(i);
        //Add to Index
        lvl_effects_indexes.storeElement(seffect.id, seffect);
skipEffect:
        effectset.endGroup();

        if(effectset.status() != QSettings::NoError)
            addError(QString("ERROR LOADING lvl_bgo.ini N:%1 (bgo-%2)").arg(effectset.status()).arg(i), QtCriticalMsg);
    }

    if(lvl_effects_indexes.stored() < effects_total)
    {
        addError(QString("Not all Effects loaded! Total: %1, Loaded: %2").arg(effects_total).arg(lvl_effects_indexes.stored()));
        PGE_MsgBox msgBox(NULL, QString("Not all Effectss loaded! Total: %1, Loaded: %2").arg(effects_total).arg(lvl_effects_indexes.stored()),
                          PGE_MsgBox::msg_error);
        msgBox.exec();
    }

    return true;
}
