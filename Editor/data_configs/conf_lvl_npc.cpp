/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "data_configs.h"

#include "../main_window/global_settings.h"
#include "../common_features/graphics_funcs.h"

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

    npcset.beginGroup("npc-main");
        npc_total = npcset.value("total", "0").toInt();
        total_data +=npc_total;

        marker_npc.bubble = npcset.value("bubble", "283").toInt();
        marker_npc.egg = npcset.value("egg", "96").toInt();
        marker_npc.lakitu = npcset.value("lakitu", "284").toInt();
        marker_npc.buried = npcset.value("buried", "91").toInt();
        marker_npc.ice_cube = npcset.value("icecube", "91").toInt();
        marker_npc.iceball = npcset.value("iceball", "265").toInt();
        marker_npc.fireball = npcset.value("fireball", "13").toInt();
        marker_npc.hammer = npcset.value("hammer", "171").toInt();
        marker_npc.boomerang = npcset.value("boomerang", "292").toInt();
        marker_npc.coin_in_block = npcset.value("coin-in-block", "10").toInt();

    npcset.endGroup();

    if(prgs) prgs->setMaximum(npc_total);
    if(prgs) prgs->setLabelText(QApplication::tr("Loading NPCs..."));

    ConfStatus::total_npc = npc_total;

    //creation of empty indexes of arrayElements
        npcIndexes npcIndex;
        for(i=0;i<=npc_total; i++)
        {
            npcIndex.i=i;
            npcIndex.gi=i;
            npcIndex.type=0;
            index_npc.push_back(npcIndex);
        }

        //Buffers
        int defGFX_h = 0;
        int combobox_size = 0;

        if(ConfStatus::total_npc==0)
        {
            addError(QString("ERROR LOADING lvl_npc.ini: number of items not define, or empty config"), QtCriticalMsg);
            return;
        }

        for(i=1; i<=npc_total; i++)
        {
            qApp->processEvents();
            if(prgs)
            {
                if(!prgs->wasCanceled()) prgs->setValue(i);
            }

            //WriteToLog(QtDebugMsg, QString("NPC Config -> read header data... npc-%1").arg(i));
            npcset.beginGroup( QString("npc-"+QString::number(i)) );
        //    //    [npc-1]
        //        unsigned long id;
            snpc.id = i;
        //    //    name="Goomba"
        //        QString name;
            snpc.name = npcset.value("name", "").toString();
            if(snpc.name.isEmpty())
            {
                addError(QString("NPC-%1 Item name isn't defined").arg(i));
                goto skipNPC;
            }

            snpc.group = npcset.value("group", "_NoGroup").toString();
        //    //    category="Enemy"		;The sort category
        //        QString category;
            snpc.category = npcset.value("category", "_Other").toString();
        //    //    image="npc-1.gif"		;NPC Image file

            //        QString image_n;
            //        QString mask_n;
            //        QPixmap image;
            //        QBitmap mask;

            //WriteToLog(QtDebugMsg, "NPC Config -> Loading image...");

            imgFile = npcset.value("image", "").toString();
            snpc.image_n = imgFile;
            if( (imgFile!="") )
            {
                tmp = imgFile.split(".", QString::SkipEmptyParts);
                if(tmp.size()==2)
                    imgFileM = tmp[0] + "m." + tmp[1];
                else
                    imgFileM = "";
                snpc.mask_n = imgFileM;
                mask = QPixmap();
                if(tmp.size()==2) mask = QPixmap(npcPath + imgFileM);
                snpc.mask = mask;
                snpc.image = GraphicsHelps::setAlphaMask(QPixmap(npcPath + imgFile), snpc.mask);
                if(snpc.image.isNull())
                {
                    addError(QString("NPC-%1 Broken image file").arg(i));
                    goto skipNPC;
                }
            }
            else
            {
                addError(QString("NPC-%1 Image filename isn't defined").arg(i));
                goto skipNPC;
            }

        //    //    algorithm="0"			;NPC's algorithm. Algorithms have states and events (onDie, onTail, onCollisionWithFlyBlock...)
        //        int algorithm;
            snpc.algorithm = npcset.value("algorithm", "0").toInt();
        //        //    ;If algorithm = 0, will using basic parametric algorithm.
        //    //    ;Else, get algorithm from list
        //    //    default-effect=2		;Spawn effect ID on jump-die
        //        unsigned long effect_1;
            snpc.effect_1 = npcset.value("default-effect", "10").toInt();
        //    //    shell-effect=4			;effect on kick by shell or other NPC
        //        unsigned long effect_2;
            snpc.effect_2 = npcset.value("shell-effect", "10").toInt();

        //    //    ; graphics
        //        int gfx_offset_x;
        //    //    gfx-offst-x=0
            snpc.gfx_offset_x = npcset.value("gfx-offset-x", "0").toInt();
        //        int gfx_offset_y;
        //    //    gfx-offst-y=2
            snpc.gfx_offset_y = npcset.value("gfx-offset-y", "0").toInt();
        //        int grid;
        //    //    grid=32
            snpc.grid = npcset.value("grid", default_grid).toInt();
        //    //    grid-offset-x=0
        //        int grid_offset_x;
            snpc.grid_offset_x = npcset.value("grid-offset-x", "0").toInt();
        //    //    grid-offset-y=0
        //        int grid_offset_y;
            snpc.grid_offset_y = npcset.value("grid-offset-y", "0").toInt();

            snpc.grid_attach_style = npcset.value("grid-attachement-style", "0").toInt();
        //    //    frame-style=0	; (0-2) This option in some algorithms can be ignored
        //        int framestyle;


            snpc.framestyle = npcset.value("frame-style", "0").toInt();
        //    //    frames=2
        //        unsigned int frames;
            snpc.frames = npcset.value("frames", "1").toInt();

        //        int gfx_h;
        //    //    gfx-height-y=32
            //WriteToLog(QtDebugMsg, "NPC Config -> Check framestyle");
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

           // WriteToLog(QtDebugMsg, "NPC Config -> calculate GFX size...");
            snpc.gfx_h = npcset.value("gfx-height", QString::number(defGFX_h) ).toInt();
        //        int gfx_w;
        //    //    gfx-width-y=32
            snpc.gfx_w = npcset.value("gfx-width", QString::number(snpc.image.width()) ).toInt();

          //  WriteToLog(QtDebugMsg, "NPC Config -> loading other params...");
        //    //    frame-speed=128
        //        unsigned int framespeed;
            snpc.framespeed = npcset.value("frame-speed", "128").toInt();
        //    //    foreground=0
        //        bool foreground;
            snpc.foreground = npcset.value("foreground", "0").toBool();
        //    //    background=0
        //        bool background;
            snpc.background = npcset.value("background", "0").toBool();
        //    //    animation-direction=0
        //        bool ani_bidir;
            snpc.ani_direct = npcset.value("animation-direction", "0").toBool();
            snpc.ani_directed_direct = npcset.value("animation-directed-direction", "0").toBool();


        //    //    animation-bidirectional=0
        //        bool ani_bidir;
            snpc.ani_bidir = npcset.value("animation-bidirectional", "0").toBool();

        //    //    ; for editor
        //    //    custom-animation=0
        //        bool custom_animate;
           // WriteToLog(QtDebugMsg, "NPC Config -> Custom animate...");
            snpc.custom_animate = npcset.value("custom-animation", "0").toBool();
        //    //    ; this option useful for non-standart algorithmic sprites (for example, bosses)

        //    //    ;custom-animation-alg=0		; Custom animation algorithm - 0 simple frame range, 1 - frame Jump
        //        int custom_ani_alg;
            snpc.custom_ani_alg = npcset.value("custom-animation-alg", "0").toInt();
        //    //    ;custom-animation-fl=0		; First frame for LEFT
        //        int custom_ani_fl;
            snpc.custom_ani_fl = npcset.value("custom-animation-fl", "0").toInt();
        //    //    ;custom-animation-el=0		; end frame for LEFT / Jump step
        //        int custom_ani_el;
            snpc.custom_ani_el = npcset.value("custom-animation-el", "-1").toInt();
        //    //    ;custom-animation-fr=0		; first frame for RIGHT
        //        int custom_ani_fr;
            snpc.custom_ani_fr = npcset.value("custom-animation-fr", "0").toInt();
        //    //    ;custom-animation-er=0		; end frame for RIGHT / Jump step
        //        int custom_ani_er;
            snpc.custom_ani_er = npcset.value("custom-animation-er", "-1").toInt();


        //    //    container=0			; NPC can containing inside other NPC (need enable special option type 2)
        //        bool container;
            snpc.container = npcset.value("container", "0").toBool();

            snpc.no_npc_collions = npcset.value("no-npc-collisions", "0").toBool();
        //    ; this option disabling collisions in editor with other NPCs, but with NPC's of same ID collisions will be checked

        //    //    ; Special option
        //    //    have-special=0			; Special NPC's option, what can used by NPC's algorithm
        //        bool special_option;
          //  WriteToLog(QtDebugMsg, "NPC Config -> Special option...");
            snpc.special_option = npcset.value("have-special", "0").toBool();

        //    //    ;special-name="Cheep-cheep"	; 60
        //        QString special_name;
            snpc.special_name = npcset.value("special-name", "Special option value").toString();
        //    //    ;special-type=0			; 61 0 combobox, 1 - spin, 2 - npc-id
        //        int special_type;
            snpc.special_type = npcset.value("special-type", "1").toInt();

        //    //    ;special-combobox-size=3		; 62 quantity of options

            combobox_size = npcset.value("special-combobox-size", "0").toInt();

            //        QStringList special_combobox_opts;
            //    //    ;special-option-0="Swim"		; 63 set title for 0 option combobox
            //    //    ;special-option-1="Jump"		; 64 set title for 1 option combobox
            //    //    ;special-option-2="Projective"	; 65 set title for 2 option combobox

            snpc.special_combobox_opts.clear();
            for(int j=0; j<combobox_size; j++)
            {
                snpc.special_combobox_opts.push_back(
                            npcset.value(QString("special-option-%1").arg(j), "0").toString()
                            );
            }

        //        int special_spin_min;
        //    //    ;special-spin-min=0		; 66 milimal value of spin
            snpc.special_spin_min = npcset.value("special-spin-min", "0").toInt();
        //        int special_spin_max;
        //    //    ;special-spin-max=25		; 67 maximal value of spin
            snpc.special_spin_max = npcset.value("special-spin-max", "1").toInt();

        //        int special_spin_value_offset;
        //    //    ;special-spin-value-offset=1		; display value in editor with offset
            snpc.special_spin_value_offset = npcset.value("special-spin-value-offset", "0").toInt();

        //    have-special-2=0			; Special NPC's option, what can used by NPC's algorithm
            snpc.special_option_2 = npcset.value("have-special-2", "0").toBool();



        //    ;special-2-name="Cheep-cheep"	; 60
            snpc.special_2_name = npcset.value("special-2-name", "Special option value").toString();;

            //    special-2-npc-required
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

        //    ;special-2-type=0			; 61 0 combobox, 1 - spin
            snpc.special_2_type = npcset.value("special-2-type", "1").toInt();;
        //    ;special-combobox-size=3		; 62 quantity of options
            combobox_size = npcset.value("special-2-combobox-size", "0").toInt();
            for(int j=0; j<combobox_size; j++)
            {
                snpc.special_2_combobox_opts.push_back(
                            npcset.value(QString("special-2-option-%1").arg(j), "0").toString()
                            );
            }
        //    ;special-option-0="Swim"		; 63 set title for 0 option combobox
        //    ;special-option-1="Jump"		; 64 set title for 1 option combobox
        //    ;special-option-2="Projective"	; 65 set title for 2 option combobox
            snpc.special_2_spin_min = npcset.value("special-2-spin-min", "0").toInt();
        //    ;special-2-spin-min=0		; 66 milimal value of spin
            snpc.special_2_spin_max = npcset.value("special-2-spin-max", "1").toInt();
        //    ;special-2-spin-max=25		; 67 maximal value of spin
            snpc.special_2_spin_value_offset = npcset.value("special-2-spin-value-offset", "0").toInt();
            //special-2-spin-value-offset




        //    //    ;game process
        //    //    score=2				; Add scores to player (value 0-13)
        //    //    ; 0, 10, 100, 200, 400, 800, 1000, 2000, 4000, 8000, 1up, 2up, 5up, 3up
        //        int score;
            snpc.score = npcset.value("score", "0").toInt();

        //    //    speed=64			; Default movement speed in px/s
        //        int speed;
            snpc.speed = npcset.value("speed", "64").toInt();
        //    //    moving=1			; NPC simply moving right/left
        //        bool movement;
            snpc.movement = npcset.value("moving", "1").toBool();
        //    //    scenery=0			; NPC as block
        //        bool scenery;
            snpc.scenery = npcset.value("scenery", "0").toBool();
        //    //    immortal=0			; NPC Can't be destroy
        //        bool immortal;
            snpc.immortal = npcset.value("immortal", "0").toBool();
        //    //    yoshicaneat=1			; NPC can be eaten by yoshi
        //        bool can_be_eaten;
            snpc.can_be_eaten = npcset.value("yoshicaneat", "0").toBool();
        //    //    takable=0			; NPC destroyble on contact with player
        //        bool takable;
            snpc.takable = npcset.value("takable", "0").toBool();
        //    //    grab-side=0			; NPC can be grabbed on side
        //        bool grab_side;
            snpc.grab_side = npcset.value("grab-side", "0").toBool();
        //    //    grab-top=0			; NPC can be grabbed on top
        //        bool grab_top;
            snpc.grab_top = npcset.value("grab-top", "0").toBool();
        //    //    grab-any=0			; NPC can be grabbed on any collisions
        //        bool grab_any;
            snpc.grab_any = npcset.value("grab-any", "0").toBool();
        //    //    default-health=1		; NPC's health value
        //        int health;
            snpc.health = npcset.value("default-health", "0").toInt();
        //    //    hurtplayer=1			; Hurt player on contact
        //        bool hurt_player;
            snpc.hurt_player = npcset.value("hurtplayer", "0").toBool();
        //    //    hurtnpc=0			; Hurt other npc on contact
        //        bool hurt_npc;
            snpc.hurt_npc = npcset.value("hurtnpc", "0").toBool();

        //    //    ;Editor featured
        //    //    allow-bubble=1			; Allow packable into the bubble
        //        bool allow_bubble;

            snpc.direct_alt_title = npcset.value("direction-alt-title", "").toString();
            snpc.direct_alt_left = npcset.value("direction-alt-left-field", "").toString();
            snpc.direct_alt_right = npcset.value("direction-alt-right-field", "").toString();
            snpc.direct_disable_random = npcset.value("direction-no-rand-field", "0").toBool();

            snpc.allow_bubble = npcset.value("allow-bubble", "0").toBool();
        //    //    allow-egg=1			; Allow packable into the egg
        //        bool allow_egg;
            snpc.allow_egg = npcset.value("allow-egg", "0").toBool();
        //    //    allow-lakitu=1			; Allow packable into the SMW Lakitu
        //        bool allow_lakitu;
            snpc.allow_lakitu = npcset.value("allow-lakitu", "0").toBool();
        //    //    allow-buried=1			; Allow packable under the herb
        //        bool allow_buried;
            snpc.allow_buried = npcset.value("allow-buried", "0").toBool();


        //    //    ; Physics
        //    //    ; Size of NPC's body (Collision box)
        //    //    fixture-height=32
        //        unsigned int height;
            snpc.height = npcset.value("fixture-height", "0").toInt();
        //    //    fixture-width=32
        //        unsigned int width;
            snpc.width = npcset.value("fixture-width", "0").toInt();
        //    //    block-npc=1		; NPC is a solid object for NPC's
        //        bool block_npc;
            snpc.block_npc = npcset.value("block-npc", "0").toBool();
        //    //    block-npc-top=0		; on NPC's top can be stay other NPC's
        //        bool block_npc_top;
            snpc.block_npc_top = npcset.value("block-npc-top", "0").toBool();
        //    //    block-player=0		; NPC is a solid object for player
        //        bool block_player;
            snpc.block_player = npcset.value("block-player", "0").toBool();
        //    //    block-player-top=0	; on NPC's top can be stay player
        //        bool block_player_top;
            snpc.block_player_top = npcset.value("block-player-top", "0").toBool();
        //    //    collision-blocks=1	; Enable collisions with blocks
        //        bool collision_with_blocks;
            snpc.collision_with_blocks = npcset.value("collision-blocks", "0").toBool();
        //    //    gravity=1		; Enable gravitation for this NPC
        //        bool gravity;
            snpc.gravity = npcset.value("gravity", "0").toBool();
        //    //    adhesion=0		; allows to NPC walking on wall and on celling
        //        bool adhesion;
            snpc.adhesion = npcset.value("adhesion", "0").toBool();

        //    //    ;Events
        //    //    deactivate=1		; Deactivate on state offscreen > 4 sec ago
        //        bool deactivation;
            snpc.deactivation = npcset.value("deactivate", "0").toBool();
        //    //    kill-slside=1		; Kill on Slope slide
        //        bool kill_slide_slope;
            snpc.kill_slide_slope = npcset.value("kill-slside", "0").toBool();
        //    //    kill-onjump=1		; Kill on jump on NPC's head
        //        bool kill_on_jump;
            snpc.kill_slide_slope = npcset.value("kill-slside", "0").toBool();
        //    //    kill-bynpc=1		; Kill by contact with other NPC with hurt-npc
        //    //    ; for example: moving SHELL have "HURT_NPC", and shell kiling ALL NPCs on contact
        //        bool kill_by_npc;
            snpc.kill_by_npc = npcset.value("kill-bynpc", "0").toBool();
        //    //    kill-fireball=1		; kill on collision with NPC, marked as "fireball"
        //        bool kill_by_fireball;
            snpc.kill_by_fireball = npcset.value("kill-fireball", "0").toBool();
        //    //    kill-iceball=1		; freeze on collision with NPC, marked as "iceball"
        //        bool freeze_by_iceball;
            snpc.freeze_by_iceball = npcset.value("kill-iceball", "0").toBool();
        //    //    kill-hammer=1		; kill on collision with NPC, marked as "hammer" or "boomerang"
        //        bool kill_hammer;
            snpc.kill_hammer = npcset.value("kill-hammer", "0").toBool();
        //    //    kill-tail=1		; kill on tail attack
        //        bool kill_tail;
            snpc.kill_tail = npcset.value("kill-tail", "0").toBool();
        //    //    kill-spin=1		; kill on spin jump
        //        bool kill_by_spinjump;
            snpc.kill_by_spinjump = npcset.value("kill-spin", "0").toBool();
        //    //    kill-statue=1		; kill on tanooki statue fall
        //        bool kill_by_statue;
            snpc.kill_by_statue = npcset.value("kill-statue", "0").toBool();
        //    //    kill-with-mounted=1	; kill on jump with mounted items
        //        bool kill_by_mounted_item;
            snpc.kill_by_mounted_item = npcset.value("kill-with-mounted", "0").toBool();
        //    //    kill-on-eat=1		; Kill on eat, or transform into other
        //        bool kill_on_eat;
            snpc.kill_on_eat = npcset.value("kill-on-eat", "0").toBool();
        //    //    cliffturn=0		; NPC turns on cliff
        //        bool turn_on_cliff_detect;
            snpc.turn_on_cliff_detect = npcset.value("cliffturn", "0").toBool();
        //    //    lava-protection=0	; NPC will not be burn in lava
        //        bool lava_protect;
            snpc.lava_protect = npcset.value("lava-protection", "0").toBool();

            snpc.is_star = npcset.value("is-star", "0").toBool();

          //  WriteToLog(QtDebugMsg, "NPC Config -> Loaded");


            main_npc.push_back(snpc);

           // WriteToLog(QtDebugMsg, "NPC Config -> Pushed");

            //Add to Index
            if(i < (unsigned int)index_npc.size())
            {
                index_npc[i].i = i;
                index_npc[i].gi = i;
            }
           // WriteToLog(QtDebugMsg, "NPC Config -> Index added");

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
