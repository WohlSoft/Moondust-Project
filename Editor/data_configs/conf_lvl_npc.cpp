/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>

#include "data_configs.h"

long dataconfigs::getNpcI(unsigned long itemID)
{
    long j;
    bool found=false;

    if(itemID < (unsigned int)index_npc.size())
    {
        j = index_npc[itemID].gi;

        if(j < main_npc.size())
        {
            if( main_npc[j].id == itemID)
                found=true;
        }
    }

    if(!found)
    {
        for(j=0; j < main_npc.size(); j++)
        {
            if(main_npc[j].id==itemID)
            {
                found=true;
                break;
            }
        }
    }

    if(!found) j=-1;

    return j;
}


void dataconfigs::loadLevelNPC(QProgressDialog *prgs)
{
    unsigned int i;

    obj_npc snpc;
    unsigned long npc_total=0;
    QString npc_ini = config_dir + "lvl_npc.ini";

    if(!QFile::exists(npc_ini))
    {
        addError(QString("ERROR LOADING lvl_npc.ini: file does not exist"), QtCriticalMsg);
          return;
    }

    QSettings npcset(npc_ini, QSettings::IniFormat);
    npcset.setIniCodec("UTF-8");

    main_npc.clear();   //Clear old
    index_npc.clear();

    npcset.beginGroup("npc-main");
        npc_total =                 npcset.value("total", "0").toInt();
        total_data +=npc_total;

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

    npcset.endGroup();

    if(prgs) prgs->setMaximum(npc_total);
    if(prgs) prgs->setLabelText(QApplication::tr("Loading NPCs..."));

    ConfStatus::total_npc = npc_total;

    /************Allocation of empty indexes of arrayElements*************/
    npcIndexes npcIndex;
    for(i=0;i<=npc_total; i++)
    {
        npcIndex.i=i;
        npcIndex.gi=i;
        npcIndex.type=0;
        index_npc.push_back(npcIndex);
    }
    /************Allocation of empty indexes of arrayElements*************/

    /*************Buffers*********************/
    int defGFX_h = 0;
    int combobox_size = 0;
    /*************Buffers*********************/

    if(ConfStatus::total_npc==0)
    {
        addError(QString("ERROR LOADING lvl_npc.ini: number of items not define, or empty config"), QtCriticalMsg);
        return;
    }

    for(i=1; i<= npc_total; i++)
    {
        qApp->processEvents();
        if(prgs)
        {
            if(!prgs->wasCanceled()) prgs->setValue(i);
        }

        QString errStr;

        npcset.beginGroup( QString("npc-"+QString::number(i)) );
        snpc.name =         npcset.value("name", "").toString();
        if(snpc.name.isEmpty())
        {
            addError(QString("NPC-%1 Item name isn't defined").arg(i));
            goto skipNPC;
        }

        snpc.group =        npcset.value("group", "_NoGroup").toString();
        snpc.category =     npcset.value("category", "_Other").toString();

        snpc.image_n =           npcset.value("image", "").toString();
        /***************Load image*******************/
        GraphicsHelps::loadMaskedImage(npcPath,
           snpc.image_n, snpc.mask_n,
           snpc.image,   snpc.mask,
           errStr);

        if(!errStr.isEmpty())
        {
            addError(QString("NPC-%1 %2").arg(i).arg(errStr));
            goto skipNPC;
        }
        /***************Load image*end***************/

        snpc.algorithm =    npcset.value("algorithm", "0").toInt();
        snpc.effect_1 =     npcset.value("default-effect", "10").toInt();
        snpc.effect_2 =     npcset.value("shell-effect", "10").toInt();

        //Physics
        snpc.height =               npcset.value("fixture-height", "0").toInt();//Leaved for compatibility
        snpc.height =               npcset.value("physical-height", snpc.height).toInt();
        snpc.width =                npcset.value("fixture-width", "0").toInt();//Leaved for compatibility
        snpc.width =                npcset.value("physical-width", snpc.width).toInt();
        snpc.block_npc =            npcset.value("block-npc", "0").toBool();
        snpc.block_npc_top =        npcset.value("block-npc-top", "0").toBool();
        snpc.block_player =         npcset.value("block-player", "0").toBool();
        snpc.block_player_top =     npcset.value("block-player-top", "0").toBool();
        snpc.collision_with_blocks= npcset.value("collision-blocks", "0").toBool();
        snpc.gravity =              npcset.value("gravity", "0").toBool();
        snpc.adhesion =             npcset.value("adhesion", "0").toBool();

        snpc.container =            npcset.value("container", "0").toBool();
        snpc.no_npc_collions =      npcset.value("no-npc-collisions", "0").toBool();

        //Graphics
        snpc.gfx_offset_x = npcset.value("gfx-offset-x", "0").toInt();
        snpc.gfx_offset_y = npcset.value("gfx-offset-y", "0").toInt();
        snpc.framestyle = npcset.value("frame-style", "0").toInt();
        snpc.frames = npcset.value("frames", "1").toInt();

        /****************Calculating of default frame height******************/
        switch(snpc.framestyle)
        {
        case 0:
            defGFX_h = (int)round(snpc.image.height() / snpc.frames);
            break;
        case 1:
            defGFX_h = (int)round((snpc.image.height() / snpc.frames)/2 );
            break;
        case 2:
            defGFX_h = (int)round((snpc.image.height()/snpc.frames)/4);
            break;
        case 3:
            defGFX_h = (int)round((snpc.image.height()/snpc.frames)/4);
            break;
        case 4:
            defGFX_h = (int)round((snpc.image.height()/snpc.frames)/8);
            break;
        default:
            defGFX_h=0;
            break;
        }
        /****************Calculating of default frame height**end*************/

        snpc.custom_physics_to_gfx= npcset.value("physics-to-gfx", "1").toBool();
        snpc.gfx_h =                npcset.value("gfx-height", QString::number(defGFX_h) ).toInt();
        snpc.gfx_w =                npcset.value("gfx-width", QString::number(snpc.image.width()) ).toInt();
        snpc.framespeed =           npcset.value("frame-speed", "128").toInt();
        snpc.display_frame =        npcset.value("display-frame", "0").toInt();
        snpc.foreground =           npcset.value("foreground", "0").toBool();
        snpc.background =           npcset.value("background", "0").toBool();
        snpc.ani_direct =           npcset.value("animation-direction", "0").toBool();
        snpc.ani_directed_direct =  npcset.value("animation-directed-direction", "0").toBool();
        snpc.ani_bidir =            npcset.value("animation-bidirectional", "0").toBool();

        snpc.custom_animate =   npcset.value("custom-animation", "0").toBool();
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
            QStringList tmp;
            QString common = npcset.value("ani-frames-cmn", "0").toString(); // Common frames list

            #ifdef _DEBUG_
            WriteToLog(QtDebugMsg, QString("Frames Sequance %1").arg(common) );
            #endif

            tmp = npcset.value("ani-frames-left", common).toString().remove(' ').split(","); //left direction
            foreach(QString x, tmp)
                snpc.frames_left.push_back(x.toInt());

            #ifdef _DEBUG_
            WriteToLog(QtDebugMsg, QString("Frames Sequance Left %1").arg(snpc.frames_left.size()) );
            #endif

            tmp = npcset.value("ani-frames-right", common).toString().remove(' ').split(","); //right direction
            foreach(QString x, tmp)
                snpc.frames_right.push_back(x.toInt());
            #ifdef _DEBUG_
            WriteToLog(QtDebugMsg, QString("Frames Sequance Left %1").arg(snpc.frames_right.size()) );
            #endif
        }
        /*************Build custom animation settings**end**********/


        snpc.special_option =   npcset.value("have-special", "0").toBool();
        snpc.special_name =     npcset.value("special-name", "Special option value").toString();
        snpc.special_type =     npcset.value("special-type", "1").toInt();
        combobox_size =         npcset.value("special-combobox-size", "0").toInt();


        /*************Build special value combobox***********/
        snpc.special_combobox_opts.clear();
        for(int j=0; j<combobox_size; j++)
        {
            snpc.special_combobox_opts.push_back(
                        npcset.value(QString("special-option-%1").arg(j), "0").toString()
                        );
        }

        snpc.special_spin_min = npcset.value("special-spin-min", "0").toInt();
        snpc.special_spin_max = npcset.value("special-spin-max", "1").toInt();
        snpc.special_spin_value_offset = npcset.value("special-spin-value-offset", "0").toInt();
        snpc.special_option_2 = npcset.value("have-special-2", "0").toBool();

        snpc.special_2_name = npcset.value("special-2-name", "Special option value").toString();;

        if(snpc.special_option_2)
        {
            QStringList tmp1 = npcset.value("special-2-npc-spin-required", "-1").toString().split(QChar(','));

            if(!tmp1.isEmpty())
                if(tmp1.first()!="-1")
                foreach(QString x, tmp1)
                    snpc.special_2_npc_spin_required.push_back(x.toInt());

            QStringList tmp2 = npcset.value("special-2-npc-box-required", "-1").toString().split(QChar(','));

            if(!tmp2.isEmpty())
                if(tmp2.first()!="-1")
                foreach(QString x, tmp2)
                    snpc.special_2_npc_box_required.push_back(x.toInt());
        }

        snpc.special_2_type = npcset.value("special-2-type", "1").toInt();;
        combobox_size = npcset.value("special-2-combobox-size", "0").toInt();
        for(int j=0; j<combobox_size; j++)
        {
            snpc.special_2_combobox_opts.push_back(
                        npcset.value(QString("special-2-option-%1").arg(j), "0").toString()
                        );
        }
        snpc.special_2_spin_min = npcset.value("special-2-spin-min", "0").toInt();
        snpc.special_2_spin_max = npcset.value("special-2-spin-max", "1").toInt();
        snpc.special_2_spin_value_offset = npcset.value("special-2-spin-value-offset", "0").toInt();
        /*************Build special value combobox***end*****/


        snpc.score =        npcset.value("score", "0").toInt();
        snpc.speed =        npcset.value("speed", "64").toInt();
        snpc.movement =     npcset.value("moving", "1").toBool();
        snpc.scenery =      npcset.value("scenery", "0").toBool();
        snpc.immortal =     npcset.value("immortal", "0").toBool();
        snpc.can_be_eaten = npcset.value("can-be-eaten", "0").toBool();
        snpc.can_be_eaten = npcset.value("yoshicaneat", snpc.can_be_eaten).toBool();
        snpc.takable =      npcset.value("takable", "0").toBool();
        snpc.grab_side =    npcset.value("grab-side", "0").toBool();
        snpc.grab_top =     npcset.value("grab-top", "0").toBool();
        snpc.grab_any =     npcset.value("grab-any", "0").toBool();
        snpc.health =       npcset.value("default-health", "0").toInt();
        snpc.hurt_player =  npcset.value("hurtplayer", "0").toBool();
        snpc.hurt_npc =     npcset.value("hurtnpc", "0").toBool();

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

        /***************GRID And snap*********************************/
        snpc.grid = npcset.value("grid", default_grid).toInt();
        snpc.grid_attach_style = npcset.value("grid-attachement-style", "0").toInt();

        /***************Calculate the grid offset********************/
        if(((int)snpc.width>=(int)snpc.grid))
            snpc.grid_offset_x = -1 * qRound( qreal((int)snpc.width % snpc.grid)/2 );
        else
            snpc.grid_offset_x = qRound( qreal( snpc.grid - (int)snpc.width )/2 );

        if(snpc.grid_attach_style==1) snpc.grid_offset_x += 16;

        snpc.grid_offset_y = -snpc.height % snpc.grid;
        /***************Calculate the grid offset********************/

        /*************Manual redefine of the grid offset if not set******************/
        snpc.grid_offset_x = npcset.value("grid-offset-x", snpc.grid_offset_x).toInt();
        snpc.grid_offset_y = npcset.value("grid-offset-y", snpc.grid_offset_y).toInt();
        /*************Manual redefine of the grid offset if not set******************/
        /***************GRID And snap***end***************************/


        //Events
        snpc.deactivation = npcset.value("deactivate", "0").toBool();
        snpc.kill_slide_slope = npcset.value("kill-slside", "0").toBool();
        snpc.kill_slide_slope = npcset.value("kill-slside", "0").toBool();
        snpc.kill_by_npc = npcset.value("kill-bynpc", "0").toBool();
        snpc.kill_by_fireball = npcset.value("kill-fireball", "0").toBool();
        snpc.freeze_by_iceball = npcset.value("kill-iceball", "0").toBool();
        snpc.kill_hammer = npcset.value("kill-hammer", "0").toBool();
        snpc.kill_tail = npcset.value("kill-tail", "0").toBool();
        snpc.kill_by_spinjump = npcset.value("kill-spin", "0").toBool();
        snpc.kill_by_statue = npcset.value("kill-statue", "0").toBool();
        snpc.kill_by_mounted_item = npcset.value("kill-with-mounted", "0").toBool();
        snpc.kill_on_eat = npcset.value("kill-on-eat", "0").toBool();
        snpc.turn_on_cliff_detect = npcset.value("cliffturn", "0").toBool();
        snpc.lava_protect = npcset.value("lava-protection", "0").toBool();

        snpc.is_star = npcset.value("is-star", "0").toBool();

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

        iTmp = npcset.value("default-special-value", "-1").toInt();
            snpc.default_special = (iTmp>=0);
            snpc.default_special_value = (iTmp>=0) ? iTmp : 0;




        snpc.id = i;
        main_npc.push_back(snpc);
        /************Add to Index**************/
        if(i < (unsigned int)index_npc.size())
        {
            index_npc[i].i = i;
            index_npc[i].gi = i;
        }
        /************Add to Index**************/

    skipNPC:
    npcset.endGroup();

        if( npcset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_npc.ini N:%1 (npc-%2)").arg(npcset.status()).arg(i), QtCriticalMsg);
            break;
        }
    }

    if((unsigned int)main_npc.size()<npc_total)
    {
        addError(QString("Not all NPCs loaded! Total: %1, Loaded: %2)").arg(npc_total).arg(main_npc.size()), QtWarningMsg);
    }

}
