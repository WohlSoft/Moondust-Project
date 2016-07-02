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
#include <tgmath.h>

#include <common_features/graphics_funcs.h>
#include <common_features/number_limiter.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>

#include "data_configs.h"

obj_npc::obj_npc()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = nullptr;
    id = 0;
    gfx_offset_x = 0;
    gfx_offset_y = 0;
    gfx_w = 32;
    gfx_h = 32;
    grid = 32;
    grid_offset_x = 0;
    grid_offset_y = 0;
    grid_attach_style = 0;
    framestyle = 0;
    frames = 1;
    framespeed = 80;
    width = 32;
    height = 32;
}

void obj_npc::copyTo(obj_npc &npc)
{
    /* for internal usage */
    npc.isValid         = isValid;
    npc.animator_id     = animator_id;
    npc.cur_image       = cur_image;
    if(cur_image==nullptr)
        npc.cur_image   = &image;
    npc.display_frame   = display_frame;
    /* for internal usage */

    npc.id                          = id;
    npc.name                        = name;
    npc.group                       = group;
    npc.category                    = category;
    npc.image_n                     = image_n;
    npc.mask_n                      = mask_n;
    npc.algorithm_script            = algorithm_script;
    npc.effect_1                    = effect_1;
    npc.effect_2                    = effect_2;
    npc.gfx_offset_x                = gfx_offset_x;
    npc.gfx_offset_y                = gfx_offset_y;
    npc.gfx_h                       = gfx_h;
    npc.gfx_w                       = gfx_w;
    npc.custom_physics_to_gfx       = custom_physics_to_gfx;
    npc.grid                        = grid;
    npc.grid_offset_x               = grid_offset_x;
    npc.grid_offset_y               = grid_offset_y;
    npc.grid_attach_style           = grid_attach_style;
    npc.framestyle                  = framestyle;
    npc.frames                      = frames;
    npc.framespeed                  = framespeed;
    npc.foreground                  = foreground;
    npc.background                  = background;
    npc.ani_bidir                   = ani_bidir;
    npc.ani_direct                  = ani_direct;
    npc.ani_directed_direct         = ani_directed_direct;
    npc.custom_animate              = custom_animate;
    npc.custom_ani_alg              = custom_ani_alg;
    npc.custom_ani_fl               = custom_ani_fl;
    npc.custom_ani_el               = custom_ani_el;
    npc.custom_ani_fr               = custom_ani_fr;
    npc.custom_ani_er               = custom_ani_er;
    npc.frames_left                 = frames_left;
    npc.frames_right                = frames_right;
    npc.container                   = container;
    npc.no_npc_collions             = no_npc_collions;
    npc.special_option              = special_option;
    npc.special_name                = special_name;
    npc.special_type                = special_type;
    npc.special_combobox_opts       = special_combobox_opts;
    npc.special_spin_min            = special_spin_min;
    npc.special_spin_max            = special_spin_max;
    npc.special_spin_value_offset   = special_spin_value_offset;
    npc.special_option_2            = special_option_2;
    npc.special_2_npc_spin_required = special_2_npc_spin_required;
    npc.special_2_npc_box_required  = special_2_npc_box_required;
    npc.special_2_name              = special_2_name;
    npc.special_2_type              = special_2_type;
    npc.special_2_combobox_opts     = special_2_combobox_opts;
    npc.special_2_spin_min          = special_2_spin_min;
    npc.special_2_spin_max          = special_2_spin_max;
    npc.special_2_spin_value_offset = special_2_spin_value_offset;
    npc.score                       = score;
    npc.speed                       = speed;
    npc.movement                    = movement;
    npc.scenery                     = scenery;
    npc.immortal                    = immortal;
    npc.can_be_eaten                = can_be_eaten;
    npc.takable                     = takable;
    npc.grab_side                   = grab_side;
    npc.grab_top                    = grab_top;
    npc.grab_any                    = grab_any;
    npc.health                      = health;
    npc.hurt_player                 = hurt_player;
    npc.hurt_npc                    = hurt_npc;
    npc.direct_alt_title            = direct_alt_title;
    npc.direct_alt_left             = direct_alt_left;
    npc.direct_alt_right            = direct_alt_right;
    npc.direct_disable_random       = direct_disable_random;
    npc.allow_bubble                = allow_bubble;
    npc.allow_egg                   = allow_egg;
    npc.allow_lakitu                = allow_lakitu;
    npc.allow_buried                = allow_buried;
    npc.height                      = height;
    npc.width                       = width;
    npc.block_npc                   = block_npc;
    npc.block_npc_top               = block_npc_top;
    npc.block_player                = block_player;
    npc.block_player_top            = block_player_top;
    npc.collision_with_blocks       = collision_with_blocks;
    npc.gravity                     = gravity;
    npc.adhesion                    = adhesion;
    npc.deactivation                = deactivation;
    npc.kill_slide_slope            = kill_slide_slope;
    npc.kill_on_jump                = kill_on_jump;
    npc.kill_by_npc                 = kill_by_npc;
    npc.kill_by_fireball            = kill_by_fireball;
    npc.freeze_by_iceball           = freeze_by_iceball;
    npc.kill_hammer                 = kill_hammer;
    npc.kill_tail                   = kill_tail;
    npc.kill_by_spinjump            = kill_by_spinjump;
    npc.kill_by_statue              = kill_by_statue;
    npc.kill_by_mounted_item        = kill_by_mounted_item;
    npc.kill_on_eat                 = kill_on_eat;
    npc.turn_on_cliff_detect        = turn_on_cliff_detect;
    npc.lava_protect                = lava_protect;
    npc.is_star                     = is_star;
    npc.default_friendly            = default_friendly;
    npc.default_friendly_value      = default_friendly_value;
    npc.default_nomovable           = default_nomovable;
    npc.default_nomovable_value     = default_nomovable_value;
    npc.default_boss                = default_boss;
    npc.default_boss_value          = default_boss_value;
    npc.default_special             = default_special;
    npc.default_special_value       = default_special_value;
}

void dataconfigs::loadLevelNPC()
{
    unsigned int i;

    obj_npc snpc;
    unsigned long npc_total=0;
    QString npc_ini = getFullIniPath("lvl_npc.ini");
    if(npc_ini.isEmpty())
        return;

    QSettings npcset(npc_ini, QSettings::IniFormat);
    npcset.setIniCodec("UTF-8");

    main_npc.clear();   //Clear old

    if(!openSection(&npcset, "npc-main")) return;
        npc_total =                 npcset.value("total", 0).toUInt();
        total_data +=npc_total;

        marker_npc.bubble =         npcset.value("bubble", 283).toUInt();
        marker_npc.egg =            npcset.value("egg", 96).toUInt();
        marker_npc.lakitu =         npcset.value("lakitu", 284).toUInt();
        marker_npc.buried =         npcset.value("buried", 91).toUInt();
        marker_npc.ice_cube =       npcset.value("icecube", 91).toUInt();
        marker_npc.iceball =        npcset.value("iceball", 265).toUInt();
        marker_npc.fireball =       npcset.value("fireball", 13).toUInt();
        marker_npc.hammer =         npcset.value("hammer", 171).toUInt();
        marker_npc.boomerang =      npcset.value("boomerang", 292).toUInt();
        marker_npc.coin_in_block =  npcset.value("coin-in-block", 10).toUInt();
    closeSection(&npcset);

    emit progressPartNumber(3);
    emit progressMax(int(npc_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading NPCs..."));

    ConfStatus::total_npc = long(npc_total);

    main_npc.allocateSlots(int(npc_total));

    /*************Buffers*********************/
    int defGFX_h = 0;
    int combobox_size = 0;
    /*************Buffers*********************/

    if(ConfStatus::total_npc==0)
    {
        addError(QString("ERROR LOADING lvl_npc.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i <= npc_total; i++)
    {
        bool valid=true;
        emit progressValue(int(i));
        QString errStr;

        if( !openSection(&npcset, QString("npc-%1").arg(i)) )
            break;

        snpc.name =         npcset.value("name", QString("npc-%1").arg(i)).toString();
        if( snpc.name.isEmpty() )
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
           snpc.image,
           errStr);

        if(!errStr.isEmpty())
        {
            valid = false;
            addError(QString("NPC-%1 %2").arg(i).arg(errStr));
            //goto skipNPC;
        }
        /***************Load image*end***************/

        snpc.algorithm_script = npcset.value("algorithm", QString("npc-%1.lua").arg(i)).toString();
        snpc.effect_1 =         npcset.value("default-effect", 10).toUInt();
        snpc.effect_2 =         npcset.value("shell-effect", 10).toUInt();

        //Physics
        snpc.height =               npcset.value("fixture-height", "0").toUInt();//Kept for compatibility
        snpc.height =               npcset.value("physical-height", snpc.height).toUInt();
        NumberLimiter::apply(snpc.height, 1u);
        snpc.width =                npcset.value("fixture-width", "0").toUInt();//Kept for compatibility
        snpc.width =                npcset.value("physical-width", snpc.width).toUInt();
        NumberLimiter::apply(snpc.width, 1u);
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
        NumberLimiter::apply(snpc.framestyle, 0, 4);
        snpc.frames = npcset.value("frames", "1").toUInt();
        NumberLimiter::apply(snpc.frames, 1u);

        /****************Calculating of default frame height******************/
        defGFX_h = snpc.image.height() / int(snpc.frames*uint(powl(2, snpc.framestyle)));
        /****************Calculating of default frame height**end*************/

        snpc.custom_physics_to_gfx= npcset.value("physics-to-gfx", "1").toBool();
        snpc.gfx_h =                npcset.value("gfx-height", QString::number(defGFX_h) ).toInt();
        NumberLimiter::apply(snpc.gfx_h, 1);
        snpc.gfx_w =                npcset.value("gfx-width", QString::number(snpc.image.width()) ).toInt();
        NumberLimiter::apply(snpc.gfx_w, 1);
        snpc.framespeed =           npcset.value("frame-speed", "128").toUInt();
        snpc.display_frame =        npcset.value("display-frame", "0").toUInt();
        snpc.foreground =           npcset.value("foreground", "0").toBool();
        snpc.background =           npcset.value("background", "0").toBool();
        snpc.ani_directed_direct =  npcset.value("animation-directed-direction", "0").toBool();
        snpc.ani_direct =           npcset.value("animation-direction", "0").toBool();
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
        if( snpc.custom_ani_alg == 2 )
        {
            QString common = npcset.value("ani-frames-cmn", "0").toString(); // Common frames list
            QString tmp;
            tmp = npcset.value("ani-frames-left", common).toString().remove(' '); //left direction
            util::CSV2IntArr(tmp, snpc.frames_left);
            tmp = npcset.value("ani-frames-right", common).toString().remove(' '); //right direction
            util::CSV2IntArr(tmp, snpc.frames_right);
        }
        /*************Build custom animation settings**end**********/


        /***************GRID And snap*********************************/
        snpc.grid = npcset.value("grid", default_grid).toInt();
        NumberLimiter::apply(snpc.grid, 1);
        snpc.grid_attach_style = npcset.value("grid-attachement-style", "0").toInt();
        NumberLimiter::apply(snpc.grid_attach_style, 0);

        /***************Calculate the grid offset********************/
        if( snpc.width >= uint(snpc.grid) )
            snpc.grid_offset_x = -1 * qRound( qreal(int(snpc.width) % snpc.grid)/2 );
        else
            snpc.grid_offset_x = qRound( qreal( snpc.grid - int(snpc.width)) / 2.0 );

        if(snpc.grid_attach_style==1)
            snpc.grid_offset_x += (snpc.grid/2);

        snpc.grid_offset_y = -int(snpc.height) % snpc.grid;
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

        //Events
        snpc.deactivation =         npcset.value("deactivate", "0").toBool();
        snpc.kill_slide_slope =     npcset.value("kill-slside", "0").toBool();
        snpc.kill_on_jump =         npcset.value("kill-onjump", "0").toBool();
        snpc.kill_by_npc =          npcset.value("kill-bynpc", "0").toBool();
        snpc.kill_by_fireball =     npcset.value("kill-fireball", "0").toBool();
        snpc.freeze_by_iceball =    npcset.value("kill-iceball", "0").toBool();
        snpc.kill_hammer =          npcset.value("kill-hammer", "0").toBool();
        snpc.kill_tail =            npcset.value("kill-tail", "0").toBool();
        snpc.kill_by_spinjump =     npcset.value("kill-spin", "0").toBool();
        snpc.kill_by_statue =       npcset.value("kill-statue", "0").toBool();
        snpc.kill_by_mounted_item = npcset.value("kill-with-mounted", "0").toBool();
        snpc.kill_on_eat =          npcset.value("kill-on-eat", "0").toBool();
        snpc.turn_on_cliff_detect = npcset.value("cliffturn", "0").toBool();
        snpc.lava_protect =         npcset.value("lava-protection", "0").toBool();

        snpc.is_star =              npcset.value("is-star", "0").toBool();

        //Editor specific flags
        long iTmp;
        iTmp =      npcset.value("default-friendly", "-1").toInt();
            snpc.default_friendly = (iTmp>=0);
            snpc.default_friendly_value = (iTmp>=0)?bool(iTmp):false;

        iTmp =      npcset.value("default-no-movable", "-1").toInt();
            snpc.default_nomovable = (iTmp>=0);
            snpc.default_nomovable_value = (iTmp>=0)?bool(iTmp):false;

        iTmp =      npcset.value("default-is-boss", "-1").toInt();
            snpc.default_boss = (iTmp>=0);
            snpc.default_boss_value = (iTmp>=0) ? bool(iTmp) : false;

        iTmp =      npcset.value("default-special-value", "-1").toInt();
            snpc.default_special = (iTmp>=0);
            snpc.default_special_value = (iTmp>=0) ? iTmp : 0;

        snpc.isValid = true;
        snpc.id = i;

        main_npc.storeElement( int(i), snpc, valid);

    skipNPC:
        closeSection(&npcset);

        if( npcset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_npc.ini N:%1 (npc-%2)").arg(npcset.status()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }

    if( uint(main_npc.stored()) < npc_total )
    {
        addError(QString("Not all NPCs loaded! Total: %1, Loaded: %2)").arg(npc_total).arg(main_npc.stored()), PGE_LogLevel::Warning);
    }
}
