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
#include <number_limiter.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>

#include "data_configs.h"

obj_npc::obj_npc()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = nullptr;
}

void obj_npc::copyTo(obj_npc &npc)
{
    /* for internal usage */
    npc.isValid         = isValid;
    npc.animator_id     = animator_id;
    npc.cur_image       = cur_image;
    if(cur_image==nullptr)
        npc.cur_image   = &image;
    npc.setup.display_frame   = setup.display_frame;

    /* for internal usage */
    npc.setup = setup;
}

bool dataconfigs::loadLevelNPC(obj_npc &snpc, QString section, obj_npc *merge_with, QString iniFile, QSettings *setup)
{
    bool internal = !setup;
    QString errStr;
    if(internal) setup=new QSettings(iniFile, QSettings::IniFormat);

    if(!openSection( setup, section ))
        return false;

    if(snpc.setup.parse(setup, npcPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
    {
        snpc.isValid = true;
    } else {
        addError(errStr);
        snpc.isValid = false;
    }

    #if 0

    /*************Buffers*********************/
    int defGFX_h = 0;
    int combobox_size = 0;
    /*************Buffers*********************/

    snpc.setup.name =         setup->value("name", merge_with ? merge_with->setup.name : section ).toString();
    if( snpc.setup.name.isEmpty() )
    {
        addError(QString("%1 Item name isn't defined").arg(section.toUpper()));
        valid=false;
        goto abort;
    }

    snpc.setup.group =        setup->value("group", merge_with ? merge_with->setup.group : "_NoGroup").toString();
    snpc.setup.category =     setup->value("category", merge_with ? merge_with->setup.category : "_Other").toString();

    snpc.setup.image_n =           setup->value("image", merge_with ? merge_with->setup.image_n : "").toString();
    /***************Load image*******************/
    GraphicsHelps::loadMaskedImage(npcPath,
       snpc.setup.image_n, snpc.setup.mask_n,
       snpc.image,
       errStr);

    if(!errStr.isEmpty())
    {
        valid = false;
        addError(QString("%1 %2").arg(section.toUpper()).arg(errStr));
        //goto skipNPC;
    }
    /***************Load image*end***************/

    snpc.setup.algorithm_script = setup->value("algorithm", merge_with ? merge_with->setup.algorithm_script : QString("%1.lua").arg(section)).toString();
    snpc.setup.effect_1 =         setup->value("default-effect", merge_with ? merge_with->setup.effect_1 : 10u).toUInt();
    snpc.setup.effect_2 =         setup->value("shell-effect", merge_with ? merge_with->setup.effect_2 : 10u).toUInt();

    //Physics
    snpc.setup.height =               setup->value("physical-height", merge_with ? merge_with->setup.height : 0).toUInt();
        NumberLimiter::apply(snpc.setup.height, 1u);
    snpc.setup.width =                setup->value("physical-width", merge_with ? merge_with->setup.width : 0).toUInt();
        NumberLimiter::apply(snpc.setup.width, 1u);
    snpc.setup.block_npc =            setup->value("block-npc", merge_with ? merge_with->setup.block_npc : false).toBool();
    snpc.setup.block_npc_top =        setup->value("block-npc-top", merge_with ? merge_with->setup.block_npc_top : false).toBool();
    snpc.setup.block_player =         setup->value("block-player", merge_with ? merge_with->setup.block_player : false).toBool();
    snpc.setup.block_player_top =     setup->value("block-player-top", merge_with ? merge_with->setup.block_player_top : false).toBool();
    snpc.setup.collision_with_blocks= setup->value("collision-blocks", merge_with ? merge_with->setup.collision_with_blocks : false).toBool();
    snpc.setup.gravity =              setup->value("gravity", merge_with ? merge_with->setup.gravity : false).toBool();
    snpc.setup.adhesion =             setup->value("adhesion", merge_with ? merge_with->setup.adhesion : false).toBool();

    snpc.setup.container =            setup->value("container", merge_with ? merge_with->setup.container : false).toBool();
    snpc.setup.contents_id =                setup->value("contents-id", merge_with ? merge_with->setup.contents_id : 0).toUInt();
    snpc.setup.container_elastic =          setup->value("container-elastic", merge_with ? merge_with->setup.container_elastic : false).toBool();
    snpc.setup.container_elastic_border_w = setup->value("container-elastic-border-w", merge_with ? merge_with->setup.container_elastic_border_w : 4).toInt();
        NumberLimiter::apply(snpc.setup.container_elastic_border_w, 0);
    snpc.setup.container_show_contents    = setup->value("container-show-contents", merge_with ? merge_with->setup.container_show_contents : true).toBool();
    snpc.setup.container_content_z_offset = setup->value("container-content-z-offset", merge_with ? merge_with->setup.container_content_z_offset : -0.00001f).toFloat();
    snpc.setup.container_crop_contents    = setup->value("container-crop-contents", merge_with ? merge_with->setup.container_crop_contents : false).toBool();
    snpc.setup.container_align_contents   = setup->value("container-align-contents", merge_with ? merge_with->setup.container_align_contents : 0 ).toInt();

    snpc.setup.no_npc_collisions =      setup->value("no-npc-collisions", merge_with ? merge_with->setup.no_npc_collisions : false).toBool();

    //Graphics
    snpc.setup.gfx_offset_x = setup->value("gfx-offset-x", merge_with ? merge_with->setup.gfx_offset_x : 0).toInt();
    snpc.setup.gfx_offset_y = setup->value("gfx-offset-y", merge_with ? merge_with->setup.gfx_offset_y : 0).toInt();
    snpc.setup.framestyle = setup->value("frame-style", merge_with ? merge_with->setup.framestyle : 0).toInt();
        NumberLimiter::apply(snpc.setup.framestyle, 0, 4);
    snpc.setup.frames = setup->value("frames", merge_with ? merge_with->setup.frames : 1).toUInt();
        NumberLimiter::apply(snpc.setup.frames, 1u);

    /****************Calculating of default frame height******************/
    defGFX_h = snpc.image.height() / int(snpc.setup.frames*uint(powl(2, snpc.setup.framestyle)));
    /****************Calculating of default frame height**end*************/

    snpc.setup.custom_physics_to_gfx= setup->value("physics-to-gfx", merge_with ? merge_with->setup.custom_physics_to_gfx : true).toBool();
    snpc.setup.gfx_h =                setup->value("gfx-height", merge_with ? merge_with->setup.gfx_h : defGFX_h ).toInt();
        NumberLimiter::apply(snpc.setup.gfx_h, 1);
    snpc.setup.gfx_w =                setup->value("gfx-width", merge_with ? merge_with->setup.gfx_w : snpc.image.width() ).toInt();
        NumberLimiter::apply(snpc.setup.gfx_w, 1);
    snpc.setup.framespeed =           setup->value("frame-speed", merge_with ? merge_with->setup.framespeed : 128).toUInt();
    snpc.setup.display_frame =        setup->value("display-frame", merge_with ? merge_with->setup.display_frame : false).toUInt();
    snpc.setup.foreground =           setup->value("foreground", merge_with ? merge_with->setup.foreground : false).toBool();
    snpc.setup.background =           setup->value("background", merge_with ? merge_with->setup.background : false).toBool();
    snpc.setup.z_offset =             setup->value("z-offset", merge_with ? merge_with->setup.z_offset : 0.0f).toDouble();
    snpc.setup.ani_directed_direct =  setup->value("animation-directed-direction", merge_with ? merge_with->setup.ani_directed_direct : false).toBool();
    snpc.setup.ani_direct =           setup->value("animation-direction", merge_with ? merge_with->setup.ani_direct : false).toBool();
    snpc.setup.ani_bidir =            setup->value("animation-bidirectional", merge_with ? merge_with->setup.ani_bidir : false).toBool();

    snpc.setup.custom_animate =   setup->value("custom-animation", merge_with ? merge_with->setup.custom_animate : 0).toBool();
    snpc.setup.custom_ani_alg =   setup->value("custom-animation-alg", merge_with ? merge_with->setup.custom_ani_alg : 0).toInt();
    snpc.setup.custom_ani_fl =    setup->value("custom-animation-fl", merge_with ? merge_with->setup.custom_ani_fl : 0).toInt();
    snpc.setup.custom_ani_el =    setup->value("custom-animation-el", merge_with ? merge_with->setup.custom_ani_el : -1).toInt();
    snpc.setup.custom_ani_fr =    setup->value("custom-animation-fr", merge_with ? merge_with->setup.custom_ani_fr : 0).toInt();
    snpc.setup.custom_ani_er =    setup->value("custom-animation-er", merge_with ? merge_with->setup.custom_ani_er : -1).toInt();

    /*************Build custom animation settings***************/
    snpc.setup.frames_left.clear();
    snpc.setup.frames_right.clear();
    if( snpc.setup.custom_ani_alg == 2 )
    {
        QString common = setup->value("ani-frames-cmn", 0).toString(); // Common frames list
        QString tmp;
        tmp = setup->value("ani-frames-left", common).toString().remove(' '); //left direction
        util::CSV2IntArr(tmp, snpc.setup.frames_left);
        tmp = setup->value("ani-frames-right", common).toString().remove(' '); //right direction
        util::CSV2IntArr(tmp, snpc.setup.frames_right);
    }
    /*************Build custom animation settings**end**********/


    /***************GRID And snap*********************************/
    snpc.setup.grid = setup->value("grid", merge_with ? merge_with->setup.grid : default_grid).toInt();
        NumberLimiter::apply(snpc.setup.grid, 1);
    snpc.setup.grid_attach_style = setup->value("grid-attachement-style", 0).toInt();
        NumberLimiter::apply(snpc.setup.grid_attach_style, 0);

    /***************Calculate the grid offset********************/
    if( snpc.setup.width >= uint(snpc.setup.grid) )
        snpc.setup.grid_offset_x = -1 * qRound( qreal(int(snpc.setup.width) % snpc.setup.grid)/2 );
    else
        snpc.setup.grid_offset_x = qRound( qreal( snpc.setup.grid - int(snpc.setup.width)) / 2.0 );

    if(snpc.setup.grid_attach_style==1)
        snpc.setup.grid_offset_x += (snpc.setup.grid/2);

    snpc.setup.grid_offset_y = -int(snpc.setup.height) % snpc.setup.grid;
    /***************Calculate the grid offset********************/

    /*************Manual redefinition of the grid offset if not set******************/
    snpc.setup.grid_offset_x = setup->value("grid-offset-x", merge_with ? merge_with->setup.grid_offset_x : snpc.setup.grid_offset_x).toInt();
    snpc.setup.grid_offset_y = setup->value("grid-offset-y", merge_with ? merge_with->setup.grid_offset_y : snpc.setup.grid_offset_y).toInt();
    /*************Manual redefinition of the grid offset if not set******************/
    /***************GRID And snap***end***************************/


    snpc.setup.special_option =   setup->value("have-special", merge_with ? merge_with->setup.special_option : false).toBool();
    snpc.setup.special_name =     setup->value("special-name", merge_with ? merge_with->setup.special_name : "Special option value").toString();
    snpc.setup.special_type =     setup->value("special-type", merge_with ? merge_with->setup.special_type : 1).toInt();
    combobox_size =         setup->value("special-combobox-size", 0).toInt();

    if( (combobox_size <=0) && merge_with )
    {
        snpc.setup.special_combobox_opts = merge_with->setup.special_combobox_opts;
    }
    else
    {
        /*************Build special value combobox***********/
        snpc.setup.special_combobox_opts.clear();
        for(int j=0; j<combobox_size; j++)
        {
            snpc.setup.special_combobox_opts.push_back(
                        setup->value(QString("special-option-%1").arg(j), 0).toString()
                        );
        }
    }

    snpc.setup.special_spin_min = setup->value("special-spin-min", merge_with ? merge_with->setup.special_spin_min : 0).toInt();
    snpc.setup.special_spin_max = setup->value("special-spin-max", merge_with ? merge_with->setup.special_spin_max : 1).toInt();
    snpc.setup.special_spin_value_offset = setup->value("special-spin-value-offset", merge_with ? merge_with->setup.special_spin_value_offset : 0).toInt();


    /**LEGACY, REMOVE THIS LAYER!!!***/
    snpc.setup.special_option_2 = setup->value("have-special-2", merge_with ? merge_with->setup.special_option_2 : 0).toBool();
    snpc.setup.special_2_name = setup->value("special-2-name", merge_with ? merge_with->setup.special_2_name : "Special option value").toString();

    if(snpc.setup.special_option_2)
    {
        QStringList tmp1 = setup->value("special-2-npc-spin-required", "-1").toString().split(QChar(','));

        if(!tmp1.isEmpty())
            if(tmp1.first()!="-1")
            foreach(QString x, tmp1)
                snpc.setup.special_2_npc_spin_required.push_back(x.toInt());

        QStringList tmp2 = setup->value("special-2-npc-box-required", "-1").toString().split(QChar(','));

        if(!tmp2.isEmpty())
            if(tmp2.first()!="-1")
            foreach(QString x, tmp2)
                snpc.setup.special_2_npc_box_required.push_back(x.toInt());
    }

    snpc.setup.special_2_type = setup->value("special-2-type", 1).toInt();
    combobox_size = setup->value("special-2-combobox-size", 0).toInt();
    for(int j=0; j<combobox_size; j++)
    {
        snpc.setup.special_2_combobox_opts.push_back(
                    setup->value(QString("special-2-option-%1").arg(j), 0).toString()
                    );
    }
    snpc.setup.special_2_spin_min = setup->value("special-2-spin-min", 0).toInt();
    snpc.setup.special_2_spin_max = setup->value("special-2-spin-max", 1).toInt();
    snpc.setup.special_2_spin_value_offset = setup->value("special-2-spin-value-offset", 0).toInt();
    /*************Build special value combobox***end*****/

    snpc.setup.score =        setup->value("score", merge_with ? merge_with->setup.score : 0).toInt();
    snpc.setup.speed =        setup->value("speed", merge_with ? merge_with->setup.speed : 64).toInt();
    snpc.setup.movement =     setup->value("moving", merge_with ? merge_with->setup.movement : 1).toBool();
    snpc.setup.activity =     setup->value("activity", merge_with ? merge_with->setup.activity : 1).toBool();
    snpc.setup.scenery =      setup->value("scenery", merge_with ? merge_with->setup.scenery : 0).toBool();
    snpc.setup.keep_position = setup->value("keep-position", merge_with ? merge_with->setup.keep_position : 0).toBool();
    snpc.setup.shared_ani =   setup->value("shared-animation", merge_with ? merge_with->setup.shared_ani : 0).toBool();
    snpc.setup.immortal =     setup->value("immortal", merge_with ? merge_with->setup.immortal : 0).toBool();
    snpc.setup.can_be_eaten = setup->value("can-be-eaten", merge_with ? merge_with->setup.can_be_eaten : 0).toBool();
    //! LEGACY! Update all config packs and remove this!
    snpc.setup.can_be_eaten = setup->value("yoshicaneat", snpc.setup.can_be_eaten).toBool();
    snpc.setup.takable =      setup->value("takable", merge_with ? merge_with->setup.takable : 0).toBool();
    snpc.setup.grab_side =    setup->value("grab-side", merge_with ? merge_with->setup.grab_side : 0).toBool();
    snpc.setup.grab_top =     setup->value("grab-top", merge_with ? merge_with->setup.grab_top : 0).toBool();
    snpc.setup.grab_any =     setup->value("grab-any", merge_with ? merge_with->setup.grab_any : 0).toBool();
    snpc.setup.health =       setup->value("default-health", merge_with ? merge_with->setup.health : 0).toInt();
    snpc.setup.hurt_player =  setup->value("hurtplayer", merge_with ? merge_with->setup.hurt_player : 0).toBool();
    snpc.setup.hurt_npc =     setup->value("hurtnpc", merge_with ? merge_with->setup.hurt_npc : 0).toBool();

    //Sound effects
    snpc.setup.hit_sound_id = setup->value("hit-sound-id", merge_with ? merge_with->setup.hit_sound_id : 0).toInt();
    snpc.setup.death_sound_id=setup->value("death-sound-id", merge_with ? merge_with->setup.death_sound_id : 0).toInt();

    //Editor features
    snpc.setup.direct_alt_title =      setup->value("direction-alt-title", "").toString();
    snpc.setup.direct_alt_left =       setup->value("direction-alt-left-field", "").toString();
    snpc.setup.direct_alt_right =      setup->value("direction-alt-right-field", "").toString();
    snpc.setup.direct_disable_random = setup->value("direction-no-rand-field", merge_with ? merge_with->setup.direct_disable_random : false).toBool();

    //Events
    snpc.setup.deactivation =         setup->value("deactivate", merge_with ? merge_with->setup.deactivation : 0).toBool();
    snpc.setup.deactivationDelay =    setup->value("deactivate-delay", merge_with ? merge_with->setup.deactivationDelay : 4000).toInt();
        NumberLimiter::applyD(snpc.setup.deactivationDelay, 4000, 0);

    snpc.setup.deactivate_off_room =  setup->value("deactivate-off-room", merge_with ? merge_with->setup.deactivate_off_room : false).toBool();
    snpc.setup.kill_slide_slope =     setup->value("kill-slside", merge_with ? merge_with->setup.kill_slide_slope : 0).toBool();
    snpc.setup.kill_on_jump =         setup->value("kill-onjump", merge_with ? merge_with->setup.kill_by_npc : 0).toBool();
    snpc.setup.kill_by_npc =          setup->value("kill-bynpc", merge_with ? merge_with->setup.kill_on_pit_fall : 0).toBool();
    snpc.setup.kill_on_pit_fall =     setup->value("kill-on-pit-fall", merge_with ? merge_with->setup.kill_by_fireball : false).toBool();
    snpc.setup.kill_by_fireball =     setup->value("kill-fireball", merge_with ? merge_with->setup.kill_by_fireball : 0).toBool();
    snpc.setup.freeze_by_iceball =    setup->value("kill-iceball", merge_with ? merge_with->setup.freeze_by_iceball : 0).toBool();
    snpc.setup.kill_hammer =          setup->value("kill-hammer", merge_with ? merge_with->setup.kill_hammer : 0).toBool();
    snpc.setup.kill_tail =            setup->value("kill-tail", merge_with ? merge_with->setup.kill_tail : 0).toBool();
    snpc.setup.kill_by_spinjump =     setup->value("kill-spin", merge_with ? merge_with->setup.kill_by_spinjump : 0).toBool();
    snpc.setup.kill_by_statue =       setup->value("kill-statue", merge_with ? merge_with->setup.kill_by_statue : 0).toBool();
    snpc.setup.kill_by_mounted_item = setup->value("kill-with-mounted", merge_with ? merge_with->setup.kill_by_mounted_item : 0).toBool();
    snpc.setup.kill_on_eat =          setup->value("kill-on-eat", merge_with ? merge_with->setup.kill_on_eat : 0).toBool();
    snpc.setup.turn_on_cliff_detect = setup->value("cliffturn", merge_with ? merge_with->setup.turn_on_cliff_detect : 0).toBool();
    snpc.setup.lava_protect =         setup->value("lava-protection", merge_with ? merge_with->setup.lava_protect : false).toBool();

    snpc.setup.is_star =              setup->value("is-star", merge_with ? merge_with->setup.is_star : 0).toBool();

    snpc.setup.exit_is =              setup->value("is-exit", merge_with ? merge_with->setup.exit_is : false).toBool();
    snpc.setup.exit_walk_direction =  setup->value("exit-direction", merge_with ? merge_with->setup.exit_walk_direction : 0).toInt();
    snpc.setup.exit_code       =      setup->value("exit-code", merge_with ? merge_with->setup.exit_code : 0).toInt();
    snpc.setup.exit_delay      =      setup->value("exit-delay", merge_with ? merge_with->setup.exit_delay : 0).toInt();
    snpc.setup.exit_snd        =      setup->value("exit-sound-id", merge_with ? merge_with->setup.exit_snd : 0).toInt();

    snpc.setup.climbable       =      setup->value("is-climbable", merge_with ? merge_with->setup.climbable : 0).toBool();

    //Editor specific flags
    long iTmp;
    iTmp =      setup->value("default-friendly", -1).toInt();
        snpc.setup.default_friendly = (iTmp>=0);
        snpc.setup.default_friendly_value = (iTmp>=0)?bool(iTmp):false;

    iTmp =      setup->value("default-no-movable", -1).toInt();
        snpc.setup.default_nomovable = (iTmp>=0);
        snpc.setup.default_nomovable_value = (iTmp>=0)?bool(iTmp):false;

    iTmp =      setup->value("default-is-boss", -1).toInt();
        snpc.setup.default_boss = (iTmp>=0);
        snpc.setup.default_boss_value = (iTmp>=0) ? bool(iTmp) : false;

    iTmp =      setup->value("default-special-value", -1).toInt();
        snpc.setup.default_special = (iTmp>=0);
        snpc.setup.default_special_value = (iTmp>=0) ? iTmp : 0;
    abort:
        snpc.isValid = true;
    #endif
        closeSection(setup);
        if(internal) delete setup;
    return snpc.isValid;
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

    if(ConfStatus::total_npc==0)
    {
        addError(QString("ERROR LOADING lvl_npc.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i <= npc_total; i++)
    {
        emit progressValue(int(i));

        bool valid = loadLevelNPC(snpc, QString("npc-%1").arg(i), nullptr, "", &npcset);
        /***************Load image*******************/
        if(valid)
        {
            QString errStr;
            GraphicsHelps::loadMaskedImage(npcPath,
               snpc.setup.image_n, snpc.setup.mask_n,
               snpc.image,
               errStr);

            if(!errStr.isEmpty())
            {
                valid = false;
                addError(QString("NPC-%1 %2").arg(i).arg(errStr));
            }
        }
        /***************Load image*end***************/
        snpc.setup.id = i;
        main_npc.storeElement( int(i), snpc, valid);

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
