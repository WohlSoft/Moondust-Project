/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "config_manager.h"
#include "config_manager_private.h"
#include "../gui/pge_msgbox.h"

#include <common_features/fmt_format_ne.h>
#include <Utils/files.h>
#include <IniProcessor/ini_processing.h>

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
    std::string effects_ini = config_dirSTD + "lvl_effects.ini";

    if(!Files::fileExists(effects_ini))
    {
        addError("ERROR LOADING lvl_effects.ini: file does not exist");
        PGE_MsgBox msgBox(NULL, "ERROR LOADING lvl_bgo.ini: file does not exist",
                          PGE_MsgBox::msg_fatal);
        msgBox.exec();
        return false;
    }

    IniProcessing effectset(effects_ini);
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
        effectset.beginGroup(fmt::format_ne("effect-{0}", i));
        seffect.name = effectset.value("name", "").toString();

        if(seffect.name == "")
        {
            addError(fmt::format_ne("EFFECT-{0} Item name isn't defined", i));
            goto skipEffect;
        }

        imgFile = effectset.value("image", "").toString();
        seffect.image_n = imgFile;

        if(!imgFile.empty())
        {
            Files::getGifMask(seffect.mask_n, imgFile);
        }
        else
        {
            addError(fmt::format_ne("EFFECT-{0} Image filename isn't defined", i));
            goto skipEffect;
        }

        effectset.read("frames", seffect.frames, 1);
        effectset.read("frame-style", seffect.framestyle, 0);
        effectset.read("frame-delay", seffect.framespeed, 125);
        effectset.read("frame-speed", seffect.framespeed, seffect.framespeed);//Alias
        seffect.frame_h = 0;
        seffect.id = uint64_t(i);
        //Add to Index
        lvl_effects_indexes.storeElement(seffect.id, seffect);
skipEffect:
        effectset.endGroup();

        if(effectset.lastError() != IniProcessing::ERR_OK)
            addError(fmt::format_ne("ERROR LOADING lvl_bgo.ini N:{0} (bgo-{1})", effectset.lineWithError(), i));
    }

    if(lvl_effects_indexes.stored() < effects_total)
    {
        addError(fmt::format_ne("Not all Effects loaded! Total: {0}, Loaded: {1}", effects_total, lvl_effects_indexes.stored()));
        PGE_MsgBox msgBox(NULL, fmt::format_ne("Not all Effectss loaded! Total: {0}, Loaded: {1}", effects_total, lvl_effects_indexes.stored()),
                          PGE_MsgBox::msg_error);
        msgBox.exec();
    }

    return true;
}
