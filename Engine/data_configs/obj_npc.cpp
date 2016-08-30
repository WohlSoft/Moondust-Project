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
#include <PGE_File_Formats/file_formats.h>
#include <common_features/util.h>

#include <QFileInfo>
#include <QDir>

/*****Level NPC************/
PGE_DataArray<obj_npc>      ConfigManager::lvl_npc_indexes;
NPC_GlobalSetup             ConfigManager::g_setup_npc;
CustomDirManager            ConfigManager::Dir_NPC;
CustomDirManager            ConfigManager::Dir_NPCScript;
QList<AdvNpcAnimator >      ConfigManager::Animator_NPC;
/*****Level NPC************/

bool ConfigManager::loadLevelNPC(obj_npc &snpc, QString section, obj_npc *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal = !setup;
    QString errStr;
    if(internal)
        setup=new QSettings(iniFile, QSettings::IniFormat);

    snpc.isInit = false;
    snpc.image = NULL;
    snpc.textureArrayId = 0;
    snpc.animator_ID = -1;

    setup->beginGroup( section );
        if(snpc.setup.parse(setup, npcPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
        {
            valid=true;
        } else {
            addError(errStr);
            valid=false;
        }
        snpc.effect_1_def.fill("stomp", setup);
        snpc.effect_2_def.fill("kick",  setup);

        snpc.block_spawn_type  = setup->value("block-spawn-type",  merge_with ? merge_with->block_spawn_type : 0).toInt();
        snpc.block_spawn_speed = setup->value("block-spawn-speed", merge_with ? merge_with->block_spawn_speed : 3.0f).toFloat();
        snpc.block_spawn_sound = setup->value("block-spawn-sound", merge_with ? merge_with->block_spawn_sound : true).toBool();
    setup->endGroup();
    if(internal) delete setup;
    return valid;
}

bool ConfigManager::loadLevelNPC()
{
    unsigned int i;

    obj_npc snpc;
    unsigned long npc_total=0;
    bool useDirectory=false;

    PGESTRING npc_ini = config_dir + "lvl_npc.ini";
    QString nestDir = "";

    if(!QFile::exists(npc_ini))
    {
        addError(PGESTRING("ERROR LOADING lvl_npc.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox msgBox(NULL, PGESTRING("ERROR LOADING lvl_npc.ini: file does not exist"),
                          PGE_MsgBox::msg_fatal);
        msgBox.exec();
        return false;
    }

    QSettings npcset(npc_ini, QSettings::IniFormat);
    npcset.setIniCodec("UTF-8");

    lvl_npc_indexes.clear();   //Clear old

    npcset.beginGroup("npc-main");
        npc_total =                  npcset.value("total", 0).toInt();
        nestDir =                    npcset.value("config-dir", "").toString();
        if(!nestDir.isEmpty())
        {
            nestDir = config_dir + nestDir;
            useDirectory = true;
        }

        g_setup_npc.bubble =         npcset.value("bubble", 283).toInt();
        g_setup_npc.egg =            npcset.value("egg", 96).toInt();
        g_setup_npc.lakitu =         npcset.value("lakitu", 284).toInt();
        g_setup_npc.buried =         npcset.value("buried", 91).toInt();
        g_setup_npc.ice_cube =       npcset.value("icecube", 91).toInt();
        g_setup_npc.iceball =        npcset.value("iceball", 265).toInt();
        g_setup_npc.fireball =       npcset.value("fireball", 13).toInt();
        g_setup_npc.hammer =         npcset.value("hammer", 171).toInt();
        g_setup_npc.boomerang =      npcset.value("boomerang", 292).toInt();
        g_setup_npc.coin_in_block =  npcset.value("coin-in-block", 10).toInt();

        g_setup_npc.phs_gravity_accel= npcset.value("physics-gravity-acceleration", 16.25).toFloat();
        g_setup_npc.phs_max_fall_speed=npcset.value("physics-max-fall-speed", 8).toFloat();

        g_setup_npc.eff_lava_burn =  npcset.value("effect-lava-burn", 13).toInt();

        g_setup_npc.projectile_sound_id = npcset.value("projectile-sound-id", 0).toInt();
        g_setup_npc.projectile_effect.fill("projectile", &npcset);
        g_setup_npc.projectile_speed = npcset.value("projectile-speed", 10.0f).toFloat();

        g_setup_npc.talking_sign_img = npcset.value("talking-sign-image", "").toString();
    npcset.endGroup();

    if(npc_total==0)
    {
        PGE_MsgBox::error(PGESTRING("ERROR LOADING lvl_npc.ini: number of items not define, or empty config"));
        return false;
    }

    lvl_npc_indexes.allocateSlots(npc_total);

    for(i=1; i<= npc_total; i++)
    {
        if(useDirectory)
        {
            if(!loadLevelNPC(snpc, "npc", nullptr, QString("%1/npc-%2.ini").arg(nestDir).arg(i)))
                return false;
        } else {
            if(!loadLevelNPC(snpc, QString("npc-%1").arg(i), nullptr, "", &npcset))
                return false;
        }

        snpc.setup.id = i;
        lvl_npc_indexes.storeElement(snpc.setup.id, snpc);

        //Process NPC.txt if possible
        loadNpcTxtConfig(i);

        if( npcset.status() != QSettings::NoError )
        {
            PGE_MsgBox::fatal(PGESTRING("ERROR LOADING lvl_npc.ini N:%1 (npc-%2)").arg(npcset.status()).arg(i));
            return false;
        }
    }

    if((unsigned int)lvl_npc_indexes.stored()<npc_total)
    {
        PGE_MsgBox::warn(PGESTRING("Not all NPCs loaded! Total: %1, Loaded: %2)").arg(npc_total).arg(lvl_npc_indexes.stored()));
    }
    return true;
}


void ConfigManager::loadNpcTxtConfig(long npcID)
{
    NPCConfigFile npcTxt = FileFormats::CreateEmpytNpcTXT();
    if(!lvl_npc_indexes.contains(npcID))
    {
        return;
    }
    obj_npc* npcSetup=&lvl_npc_indexes[npcID];
    PGESTRING file = Dir_NPC.getCustomFile(PGESTRING("npc-%1.txt").arg(npcID));
    if( file.isEmpty() )
        return;

    if( !FileFormats::ReadNpcTXTFileF(file, npcTxt, true) )
        return;

    PGESTRING image = npcSetup->setup.image_n;

    //Take updated image info
    if( npcTxt.en_image && ( !npcTxt.image.isEmpty() ) && (npcTxt.image != npcSetup->setup.image_n) )
    {
        image = Dir_NPC.getCustomFile(npcTxt.image);
        GraphicsHelps::getImageMetrics(image, &npcSetup->image_size);
    } else {
        image = Dir_NPC.getCustomFile(npcSetup->setup.image_n);
        GraphicsHelps::getImageMetrics(image, &npcSetup->image_size);
    }
    npcSetup->setup.applyNPCtxt(&npcTxt, npcSetup->setup, QSize(npcSetup->image_size.w(), npcSetup->image_size.h()));
}
