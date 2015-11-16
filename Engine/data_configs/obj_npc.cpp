/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <data_functions/npctxt_manager.h>

#include <QFileInfo>
#include <QDir>

/*****Level NPC************/
QMap<long, obj_npc>   ConfigManager::lvl_npc_indexes;
NPC_GlobalSetup           ConfigManager::marker_npc;
CustomDirManager ConfigManager::Dir_NPC;
CustomDirManager ConfigManager::Dir_NPCScript;
QList<AdvNpcAnimator > ConfigManager::Animator_NPC;
/*****Level NPC************/

bool ConfigManager::loadLevelNPC()
{
    unsigned int i;

    obj_npc snpc;
    unsigned long npc_total=0;
    PGESTRING npc_ini = config_dir + "lvl_npc.ini";

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
        npc_total =                 npcset.value("total", "0").toInt();
        marker_npc.bubble =         npcset.value("bubble", "283").toInt();
        marker_npc.egg =            npcset.value("egg", "96").toInt();
        marker_npc.lakitu =         npcset.value("lakitu", "284").toInt();
        marker_npc.buried =         npcset.value("buried", "91").toInt();
        marker_npc.ice_cube =       npcset.value("icecube", "91").toInt();
        marker_npc.iceball =        npcset.value("iceball", "265").toInt();
        marker_npc.fireball =       npcset.value("fireball", "13").toInt();
        marker_npc.hammer =         npcset.value("hammer", "171").toInt();
        marker_npc.boomerang =      npcset.value("boomerang", "292").toInt();
        marker_npc.coin_in_block =  npcset.value("coin-in-block", "10").toInt();

        marker_npc.phs_gravity_accel= npcset.value("physics-gravity-acceleration", 16.25).toFloat();
        marker_npc.phs_max_fall_speed=npcset.value("physics-max-fall-speed", 8).toFloat();

        marker_npc.eff_lava_burn =  npcset.value("effect-lava-burn", "13").toInt();

        marker_npc.projectile_sound_id = npcset.value("projectile-sound-id", 0).toInt();
        marker_npc.projectile_effect.fill("projectile", &npcset);
        marker_npc.projectile_speed = npcset.value("projectile-speed", 10.0f).toFloat();

        marker_npc.talking_sign_img = npcset.value("talking-sign-image", "").toString();
    npcset.endGroup();

    /*************Buffers*********************/
    int defGFX_h = 0;
    int combobox_size = 0;
    /*************Buffers*********************/

    if(npc_total==0)
    {
        PGE_MsgBox::error(PGESTRING("ERROR LOADING lvl_npc.ini: number of items not define, or empty config"));
        return false;
    }

    for(i=1; i<= npc_total; i++)
    {

        snpc.isInit = false;
        snpc.image = NULL;
        snpc.textureArrayId = 0;
        snpc.animator_ID = 0;
        npcset.beginGroup( PGESTRING("npc-"+PGESTRING::number(i)) );
        snpc.name =         npcset.value("name", "").toString();
        if(snpc.name.isEmpty())
        {
            PGE_MsgBox::warn(PGESTRING("NPC-%1 Item name isn't defined").arg(i));
            goto skipNPC;
        }

        snpc.group =        npcset.value("group", "_NoGroup").toString();
        snpc.category =     npcset.value("category", "_Other").toString();

        snpc.image_n =           npcset.value("image", "").toString();
        /***************Load image*******************/
        imgFile = npcset.value("image", "").toString();
        {
            PGESTRING err;
            GraphicsHelps::loadMaskedImage(npcPath, imgFile, snpc.mask_n, err, &snpc.image_size);
            snpc.image_n = imgFile;
            if( imgFile=="" )
            {
                addError(PGESTRING("NPC-%1 Image filename isn't defined.\n%2").arg(i).arg(err));
                goto skipNPC;
            }
        }
        /***************Load image*end***************/

        snpc.algorithm_script = npcset.value("algorithm", PGESTRING("npc-%1.lua").arg(i) ).toString();
        snpc.effect_1 =     npcset.value("default-effect", "10").toInt();
        snpc.effect_2 =     npcset.value("shell-effect", "10").toInt();

        snpc.block_spawn_type=npcset.value("block-spawn-type", 0).toInt();
        snpc.block_spawn_speed=npcset.value("block-spawn-speed", 3.0f).toFloat();
        snpc.block_spawn_sound=npcset.value("block-spawn-sound", true).toBool();

        //Physics
        snpc.height =               npcset.value("fixture-height", "0").toInt();//Leaved for compatibility
        snpc.height =               npcset.value("physical-height", snpc.height).toInt();
            NumberLimiter::apply(snpc.height, 1u);
        snpc.width =                npcset.value("fixture-width", "0").toInt();//Leaved for compatibility
        snpc.width =                npcset.value("physical-width", snpc.width).toInt();
            NumberLimiter::apply(snpc.width, 1u);
        snpc.block_npc =            npcset.value("block-npc", false).toBool();
        snpc.block_npc_top =        npcset.value("block-npc-top", false).toBool();
        snpc.block_player =         npcset.value("block-player", false).toBool();
        snpc.block_player_top =     npcset.value("block-player-top", false).toBool();
        snpc.collision_with_blocks= npcset.value("collision-blocks", false).toBool();
        snpc.gravity =              npcset.value("gravity", false).toBool();
        snpc.adhesion =             npcset.value("adhesion", false).toBool();

        snpc.container =                  npcset.value("container", false).toBool();
        snpc.contents_id =                (unsigned long)npcset.value("contents-id", "0").toULongLong();
        snpc.container_elastic =          npcset.value("container-elastic", false).toBool();
        snpc.container_elastic_border_w = npcset.value("container-elastic-border-w", "4").toInt();
            NumberLimiter::apply(snpc.container_elastic_border_w, 0);
        snpc.container_show_contents    = npcset.value("container-show-contents", true).toBool();
        snpc.container_content_z_offset = npcset.value("container-content-z-offset", "-0.00001").toFloat();
        snpc.container_crop_contents    = npcset.value("container-crop-contents", false).toBool();
        snpc.container_align_contents   = npcset.value("container-align-contents", "0").toInt();

        snpc.no_npc_collions =      npcset.value("no-npc-collisions", "0").toBool();

        //Graphics
        snpc.gfx_offset_x = npcset.value("gfx-offset-x", "0").toInt();
        snpc.gfx_offset_y = npcset.value("gfx-offset-y", "0").toInt();
        snpc.framestyle = npcset.value("frame-style", "0").toInt();
            NumberLimiter::apply(snpc.framestyle, 0, 4);
        snpc.frames = npcset.value("frames", "1").toInt();
            NumberLimiter::apply(snpc.frames, 1u);

        /****************Calculating of default frame height******************/
        switch(snpc.framestyle)
        {
            case 0: defGFX_h = (int)round(snpc.image_size.h() / snpc.frames); break;
            case 1: defGFX_h = (int)round((snpc.image_size.h() / snpc.frames)/2 ); break;
            case 2: defGFX_h = (int)round((snpc.image_size.h()/snpc.frames)/4); break;
            case 3: defGFX_h = (int)round((snpc.image_size.h()/snpc.frames)/4); break;
            case 4: defGFX_h = (int)round((snpc.image_size.h()/snpc.frames)/8); break;
            default:defGFX_h = 0; break;
        }
        /****************Calculating of default frame height**end*************/

        snpc.custom_physics_to_gfx= npcset.value("physics-to-gfx", true).toBool();
        snpc.gfx_h =                npcset.value("gfx-height", PGESTRING::number(defGFX_h) ).toInt();
            NumberLimiter::apply(snpc.gfx_h, 1);
        snpc.gfx_w =                npcset.value("gfx-width", PGESTRING::number(snpc.image_size.w()) ).toInt();
            NumberLimiter::apply(snpc.gfx_w, 1);
        snpc.framespeed =           npcset.value("frame-speed", "128").toInt();
            NumberLimiter::apply(snpc.framespeed, 1u);
        snpc.display_frame =        npcset.value("display-frame", "0").toInt();
            NumberLimiter::apply(snpc.display_frame, 0u);
        snpc.foreground =           npcset.value("foreground", false).toBool();
        snpc.background =           npcset.value("background", false).toBool();
        snpc.z_offset =             npcset.value("z-offset", 0.0f).toDouble();
        snpc.ani_directed_direct =  npcset.value("animation-directed-direction", false).toBool();
        snpc.ani_direct =           npcset.value("animation-direction", false).toBool();
        snpc.ani_bidir =            npcset.value("animation-bidirectional", false).toBool();

        snpc.custom_animate =   npcset.value("custom-animation", false).toBool();
        snpc.custom_ani_alg =   npcset.value("custom-animation-alg", "0").toInt();
        snpc.custom_ani_fl =    npcset.value("custom-animation-fl", "0").toInt();
        snpc.custom_ani_el =    npcset.value("custom-animation-el", "-1").toInt();
        snpc.custom_ani_fr =    npcset.value("custom-animation-fr", "0").toInt();
        snpc.custom_ani_er =    npcset.value("custom-animation-er", "-1").toInt();

        /*************Build custom animation settings***************/
        snpc.frames_left.clear();
        snpc.frames_right.clear();
        if(snpc.custom_ani_alg==2)
        {
            PGESTRINGList tmp;
            PGESTRING common = npcset.value("ani-frames-cmn", "0").toString(); // Common frames list
            tmp = npcset.value("ani-frames-left", common).toString().remove(' ').split(","); //left direction
            foreach(PGESTRING x, tmp)
                snpc.frames_left.push_back(x.toInt());
            tmp = npcset.value("ani-frames-right", common).toString().remove(' ').split(","); //right direction
            foreach(PGESTRING x, tmp)
                snpc.frames_right.push_back(x.toInt());

        }
        /*************Build custom animation settings**end**********/


        /***************GRID And snap*********************************/
        snpc.grid = npcset.value("grid", default_grid).toInt();
            NumberLimiter::apply(snpc.grid, 1);
        snpc.grid_attach_style = npcset.value("grid-attachement-style", "0").toInt();
            NumberLimiter::apply(snpc.grid_attach_style, 0);

        /***************Calculate the grid offset********************/
        if(((int)snpc.width>=(int)snpc.grid))
            snpc.grid_offset_x = -1 * qRound( qreal((int)snpc.width % snpc.grid)/2 );
        else
            snpc.grid_offset_x = qRound( qreal( snpc.grid - (int)snpc.width )/2 );

        if(snpc.grid_attach_style==1) snpc.grid_offset_x += (snpc.grid/2);

        snpc.grid_offset_y = -snpc.height % snpc.grid;
        /***************Calculate the grid offset********************/

            /*************Manual redefinition of the grid offset if not set******************/
        snpc.grid_offset_x = npcset.value("grid-offset-x", snpc.grid_offset_x).toInt();
        snpc.grid_offset_y = npcset.value("grid-offset-y", snpc.grid_offset_y).toInt();
            /*************Manual redefinition of the grid offset if not set******************/
        /***************GRID And snap***end***************************/

        snpc.special_option =   npcset.value("have-special", "0").toBool();
        snpc.special_name =     npcset.value("special-name", "Special option value").toString();
        snpc.special_type =     npcset.value("special-type", "1").toInt();
        combobox_size =         npcset.value("special-combobox-size", "0").toInt();


        /*************Build special value combobox***********/
        snpc.special_combobox_opts.clear();
        for(int j=0; j<combobox_size; j++)
        {
            snpc.special_combobox_opts.push_back(
                        npcset.value(PGESTRING("special-option-%1").arg(j), "0").toString()
                        );
        }

        snpc.special_spin_min = npcset.value("special-spin-min", "0").toInt();
        snpc.special_spin_max = npcset.value("special-spin-max", "1").toInt();
        snpc.special_spin_value_offset = npcset.value("special-spin-value-offset", "0").toInt();


        snpc.special_option_2 = npcset.value("have-special-2", "0").toBool();
        snpc.special_2_name = npcset.value("special-2-name", "Special option value").toString();;

        if(snpc.special_option_2)
        {
            PGESTRINGList tmp1 = npcset.value("special-2-npc-spin-required", "-1").toString().split(QChar(','));

            if(!tmp1.isEmpty())
                if(tmp1.first()!="-1")
                foreach(PGESTRING x, tmp1)
                    snpc.special_2_npc_spin_required.push_back(x.toInt());

            PGESTRINGList tmp2 = npcset.value("special-2-npc-box-required", "-1").toString().split(QChar(','));

            if(!tmp2.isEmpty())
                if(tmp2.first()!="-1")
                foreach(PGESTRING x, tmp2)
                    snpc.special_2_npc_box_required.push_back(x.toInt());
        }

        snpc.special_2_type = npcset.value("special-2-type", "1").toInt();;
        combobox_size = npcset.value("special-2-combobox-size", "0").toInt();
        for(int j=0; j<combobox_size; j++)
        {
            snpc.special_2_combobox_opts.push_back(
                        npcset.value(PGESTRING("special-2-option-%1").arg(j), "0").toString()
                        );
        }
        snpc.special_2_spin_min = npcset.value("special-2-spin-min", "0").toInt();
        snpc.special_2_spin_max = npcset.value("special-2-spin-max", "1").toInt();
        snpc.special_2_spin_value_offset = npcset.value("special-2-spin-value-offset", "0").toInt();
        /*************Build special value combobox***end*****/


        snpc.score =        npcset.value("score", "0").toInt();
        snpc.coins =        npcset.value("coins", "0").toInt();
        snpc.speed =        npcset.value("speed", "64").toInt();
        snpc.movement =     npcset.value("moving", "1").toBool();
        snpc.activity =     npcset.value("activity", "1").toBool();
        snpc.scenery =      npcset.value("scenery", "0").toBool();
        snpc.keep_position = npcset.value("keep-position", "0").toBool();
        snpc.shared_ani =   npcset.value("shared-animation", "0").toBool();
        snpc.immortal =     npcset.value("immortal", "0").toBool();
        snpc.can_be_eaten = npcset.value("can-be-eaten", "0").toBool();
        snpc.can_be_eaten = npcset.value("yoshicaneat", snpc.can_be_eaten).toBool();
        snpc.takable =      npcset.value("takable", "0").toBool();
        snpc.takable_snd =  npcset.value("takable-sound-id", "0").toInt();

        snpc.grab_side =    npcset.value("grab-side", "0").toBool();
        snpc.grab_top =     npcset.value("grab-top", "0").toBool();
        snpc.grab_any =     npcset.value("grab-any", "0").toBool();
        snpc.health =       npcset.value("default-health", "0").toInt();
        snpc.hurt_player =  npcset.value("hurtplayer", "0").toBool();
        snpc.hurt_npc =     npcset.value("hurtnpc", "0").toBool();

        //Sound effects
        snpc.hit_sound_id = npcset.value("hit-sound-id", "0").toInt();
        snpc.death_sound_id=npcset.value("death-sound-id", "0").toInt();

        //Editor features
        snpc.direct_alt_title =      npcset.value("direction-alt-title", "").toString();
        snpc.direct_alt_left =       npcset.value("direction-alt-left-field", "").toString();
        snpc.direct_alt_right =      npcset.value("direction-alt-right-field", "").toString();
        snpc.direct_disable_random = npcset.value("direction-no-rand-field", "0").toBool();

        //Legacy SMBX64 features
        snpc.allow_bubble = npcset.value("allow-bubble", "0").toBool();
        snpc.allow_egg =    npcset.value("allow-egg", "0").toBool();
        snpc.allow_lakitu = npcset.value("allow-lakitu", "0").toBool();
        snpc.allow_buried = npcset.value("allow-buried", "0").toBool();

        //Events
        snpc.deactivation =         npcset.value("deactivate", false).toBool();
        snpc.deactivetionDelay =    npcset.value("deactivate-delay", 4000).toInt();
            NumberLimiter::applyD(snpc.deactivetionDelay, 4000, 0);
        snpc.kill_slide_slope =     npcset.value("kill-slside", false).toBool();
        snpc.kill_on_jump =         npcset.value("kill-onjump", false).toBool();
        snpc.kill_by_npc =          npcset.value("kill-bynpc", false).toBool();
        snpc.kill_by_fireball =     npcset.value("kill-fireball", false).toBool();
        snpc.freeze_by_iceball =    npcset.value("kill-iceball", false).toBool();
        snpc.kill_hammer =          npcset.value("kill-hammer", false).toBool();
        snpc.kill_tail =            npcset.value("kill-tail", false).toBool();
        snpc.kill_by_spinjump =     npcset.value("kill-spin", false).toBool();
        snpc.kill_by_statue =       npcset.value("kill-statue", false).toBool();
        snpc.kill_by_mounted_item = npcset.value("kill-with-mounted", false).toBool();
        snpc.kill_on_eat =          npcset.value("kill-on-eat", false).toBool();
        snpc.turn_on_cliff_detect = npcset.value("cliffturn", false).toBool();
        snpc.lava_protect =         npcset.value("lava-protection", false).toBool();

        snpc.is_star =              npcset.value("is-star", false).toBool();

        snpc.exit_is =              npcset.value("is-exit", false).toBool();
        snpc.exit_walk_direction =  npcset.value("exit-direction", "0").toInt();
        snpc.exit_code       =      npcset.value("exit-code", "0").toInt();
        snpc.exit_delay      =      npcset.value("exit-delay", "0").toInt();
        snpc.exit_snd        =      npcset.value("exit-sound-id", "0").toInt();

        snpc.climbable       =      npcset.value("is-climbable", "0").toBool();


        //Editor specific flags
        long iTmp;
        iTmp =      npcset.value("default-friendly", "-1").toInt();
            snpc.default_friendly = (iTmp>=0);
            snpc.default_friendly_value = (iTmp>=0)?(bool)iTmp:false;

        iTmp =      npcset.value("default-no-movable", "-1").toInt();
            snpc.default_nomovable = (iTmp>=0);
            snpc.default_nomovable_value = (iTmp>=0)?(bool)iTmp:false;

        iTmp =      npcset.value("default-is-boss", "-1").toInt();
            snpc.default_boss = (iTmp>=0);
            snpc.default_boss_value = (iTmp>=0) ? (bool)iTmp : false;

        iTmp =      npcset.value("default-special-value", "-1").toInt();
            snpc.default_special = (iTmp>=0);
            snpc.default_special_value = (iTmp>=0) ? iTmp : 0;


        snpc.id = i;
        lvl_npc_indexes[i] = snpc;

    skipNPC:
    npcset.endGroup();
        if( npcset.status() != QSettings::NoError )
        {
            PGE_MsgBox::fatal(PGESTRING("ERROR LOADING lvl_npc.ini N:%1 (npc-%2)").arg(npcset.status()).arg(i));
            return false;
        }
    }

    if((unsigned int)lvl_npc_indexes.size()<npc_total)
    {
        PGE_MsgBox::warn(PGESTRING("Not all NPCs loaded! Total: %1, Loaded: %2)").arg(npc_total).arg(lvl_npc_indexes.size()));
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
    if(file.isEmpty()) return;
    npcTxt=FileFormats::ReadNpcTXTFile(file, true);
    if(!npcTxt.ReadFileValid) return;
    if(npcSetup->isInit)
        *npcSetup = mergeNPCConfigs( *npcSetup,npcTxt, QSize(npcSetup->image->w, npcSetup->image->h));
    else
        *npcSetup = mergeNPCConfigs( *npcSetup,npcTxt, QSize(0, 0));
}
