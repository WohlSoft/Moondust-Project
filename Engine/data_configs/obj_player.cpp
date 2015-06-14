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
#include <scenes/level/lvl_physenv.h>

/*****Playable Characters************/
QHash<int, obj_player > ConfigManager::playable_characters;
CustomDirManager ConfigManager::Dir_PlayerWld;
CustomDirManager ConfigManager::Dir_PlayerLvl;
/*****Playable Characters************/

void loadPlayerPhysicsSettings(QSettings &set, obj_player_physics &t, QString grp)
{
    set.beginGroup(grp);
        t.walk_force = set.value("walk_force", t.walk_force).toFloat();
            NumberLimiter::applyD(t.walk_force, 6.5f, 0.0f);
        t.run_force = set.value("run_force", t.run_force).toFloat();
            NumberLimiter::applyD(t.run_force, 3.25f, 0.0f);

        t.decelerate_stop= set.value("decelerate_stop", t.decelerate_stop).toFloat();
            NumberLimiter::applyD(t.decelerate_stop, 4.55f, 0.0f);
        t.decelerate_run = set.value("decelerate_run", t.decelerate_run).toFloat();
            NumberLimiter::applyD(t.decelerate_run, 10.88f, 0.0f);
        t.decelerate_turn= set.value("decelerate_turn", t.decelerate_turn).toFloat();
            NumberLimiter::applyD(t.decelerate_turn, 18.2f, 0.0f);
        t.decelerate_air = set.value("decelerate_air", t.decelerate_air).toFloat();
            NumberLimiter::applyD(t.decelerate_air, 0.0f, 0.0f);

        t.ground_c = set.value("ground_c", t.ground_c).toFloat();
            NumberLimiter::applyD(t.ground_c, 0.0f, 0.0f);
        t.ground_c_max = set.value("ground_c_max", t.ground_c_max).toFloat();
            NumberLimiter::applyD(t.ground_c_max, 0.0f, 0.0f);

        t.slippery_c = set.value("slippery_c", t.slippery_c).toFloat();
            NumberLimiter::applyD(t.slippery_c, 0.0f, 0.0f);

        t.gravity_scale = set.value("gravity_scale", t.gravity_scale ).toFloat();
        t.velocity_jump = set.value("velocity_jump", t.velocity_jump).toFloat();
            NumberLimiter::applyD(t.velocity_jump, 5.2f, 0.0f);
        t.velocity_jump_c = set.value("velocity_jump_c", t.velocity_jump_c).toFloat();
            NumberLimiter::applyD(t.velocity_jump_c, 5.8f, 0.0f);

        t.jump_time     = set.value("jump_time", t.jump_time).toInt();
            NumberLimiter::applyD(t.jump_time, 260, 0);

        t.velocity_climb_x = set.value("velocity_climb_x", t.velocity_climb_x).toFloat();
            NumberLimiter::applyD(t.velocity_climb_x, 1.5f, 0.0f);
        t.velocity_climb_y_up = set.value("velocity_climb_y_up", t.velocity_climb_y_up).toFloat();
            NumberLimiter::applyD(t.velocity_climb_y_up, 2.0f, 0.0f);
        t.velocity_climb_y_down = set.value("velocity_climb_y_down", t.velocity_climb_y_down).toFloat();
            NumberLimiter::applyD(t.velocity_climb_y_down, 3.0f, 0.0f);

        t.MaxSpeed_walk = set.value("MaxSpeed_walk", t.MaxSpeed_walk).toFloat();
            NumberLimiter::applyD(t.velocity_climb_x, 3.0f, 0.0f);
        t.MaxSpeed_run = set.value("MaxSpeed_run", t.MaxSpeed_run).toFloat();
            NumberLimiter::applyD(t.MaxSpeed_run, 6.0f, 0.0f);
        t.MaxSpeed_up = set.value("MaxSpeed_up", t.MaxSpeed_up).toFloat();
            NumberLimiter::applyD(t.MaxSpeed_up, 74.0f, 0.0f);
        t.MaxSpeed_down = set.value("MaxSpeed_down", t.MaxSpeed_down).toFloat();
            NumberLimiter::applyD(t.MaxSpeed_down, 12.0f, 0.0f);

        t.strict_max_speed_on_ground = set.value("strict_max_speed_on_ground", false).toBool();
        t.zero_speed_y_on_enter = set.value("zero_speed_y_on_enter", false).toBool();
        t.slow_speed_x_on_enter = set.value("slow_speed_x_on_enter", false).toBool();
        t.slow_speed_x_coeff = set.value("slow_speed_x_coeff", t.slow_speed_x_coeff).toFloat();
            NumberLimiter::applyD(t.slow_speed_x_coeff, 0.125f, 0.0f);
    set.endGroup();
}

obj_player_physics::obj_player_physics()
{
    walk_force = 6.5; //!< Move force
    run_force  = 3.25;  //!< Running force

    decelerate_stop = 4.55; //!< Deceleration while stopping
    decelerate_run  = 10.88;  //!< Deceleration running while speed higher than walking
    decelerate_turn = 18.2 ; //!< Deceleration while turning
    decelerate_air  = 0.0;  //!< Decelerate in air

    ground_c_max    = 1.0; //!< On-Ground max speed     coefficient
    ground_c        = 1.0; //!< On-Ground accelerations coefficient
    slippery_c      = 4.0; //!< Slippery coefficien
    gravity_scale   = 1.0; //!< Gravity scale
    velocity_jump   = 5.2; //!< Jump velocity
    velocity_jump_c = 5.8; //!< Jump coefficient which provides increzed jump height dependent to speed
    jump_time       = 260;     //!< Time to jump

    velocity_climb_x     = 1.5; //!< Climbing velocity
    velocity_climb_y_up  = 2.0; //!< Climbing velocity
    velocity_climb_y_down= 3.0; //!< Climbing velocity

    MaxSpeed_walk       = 3.0; //!< Max walk speed
    MaxSpeed_run        = 6.0;  //!< Max run speed

    MaxSpeed_up         = 74.0;   //!< Fly UP Max fall speed
    MaxSpeed_down       = 12.0; //!< Max fall down speed

    zero_speed_y_on_enter=false;
    slow_speed_x_on_enter=false;
    slow_speed_x_coeff=0.125f;
}




bool ConfigManager::loadPlayableCharacters()
{
    qDebug() << "Loading playable characters...";

    unsigned int i;
    unsigned long players_total=0;

    QString plr_ini = config_dir + "lvl_characters.ini";

    if(!QFile::exists(plr_ini))
    {
        addError(QString("ERROR LOADING lvl_characters.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox msgBox(NULL, QString("ERROR LOADING lvl_characters.ini: file does not exist"),
                          PGE_MsgBox::msg_fatal);
        msgBox.exec();
        return false;
    }

    QSettings playerset(plr_ini, QSettings::IniFormat);
    playerset.setIniCodec("UTF-8");

    playable_characters.clear();   //Clear old

    playerset.beginGroup("main-characters");
        players_total = playerset.value("total", "0").toInt();
    playerset.endGroup();

    if(players_total==0)
    {
        addError(QString("ERROR LOADING lvl_characters.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox msgBox(NULL, QString("ERROR LOADING lvl_characters.ini: number of items not define, or empty config"),
                          PGE_MsgBox::msg_fatal);
        msgBox.exec();

        return false;
    }


        for(i=1; i<=players_total; i++)
        {
            obj_player splayer;
            splayer.isInit_wld=false;
            splayer.image_wld = NULL;
            splayer.textureArrayId_wld= 0;
            splayer.animator_ID_wld = 0;
            splayer.wld_offset_y = 0;

            //Default size of frame is 100x100, but re-calculates from matrix size and size of target sprite
            splayer.frame_width=100;
            splayer.frame_height=100;

            int total_states=0;

            playerset.beginGroup( QString("character-%1").arg(i) );
                splayer.name = playerset.value("name", QString("player %1").arg(i) ).toString();
                if(splayer.name=="")
                {
                    addError(QString("Player-%1 Item name isn't defined").arg(i));
                    goto skipPLAYER;
                }
                splayer.sprite_folder = playerset.value("sprite-folder", QString("player-%1").arg(i) ).toString();
                splayer.state_type =  playerset.value("sprite-folder", 0 ).toInt();
                splayer.matrix_width = playerset.value("matrix-width", 10 ).toInt();
                splayer.matrix_height = playerset.value("matrix-height", 10 ).toInt();
                splayer.event_script = playerset.value("script-file", QString("script/player/%1.lua").arg(splayer.sprite_folder)).toString();
                total_states = playerset.value("states-number", 0 ).toInt();
                if(total_states==0)
                {
                    addError(QString("player-%1 has no states!").arg(i));
                    goto skipPLAYER;
                }
            playerset.endGroup();

            {//States
                bool default_duck=false;
                int floating_max_time=1500;
                float floating_amplutude=0.8;
                playerset.beginGroup( QString("character-%1-physics-common").arg(i) );
                    default_duck =  playerset.value("duck-allow", false ).toBool();
                    splayer.allowFloating =  playerset.value("allow-floating", false ).toBool();
                    floating_max_time = playerset.value("floating-max-time", 1500 ).toInt();
                    floating_amplutude = playerset.value("floating-amplitude", 0.8).toFloat();
                playerset.endGroup();


                //default environment specific physics
                    splayer.phys_default[LVL_PhysEnv::Env_Air].make();
                loadPlayerPhysicsSettings(playerset, splayer.phys_default[LVL_PhysEnv::Env_Air], QString("character-%1-env-common-air").arg(i));
                    splayer.phys_default[LVL_PhysEnv::Env_Water].make();
                loadPlayerPhysicsSettings(playerset, splayer.phys_default[LVL_PhysEnv::Env_Water], QString("character-%1-env-common-water").arg(i));
                    splayer.phys_default[LVL_PhysEnv::Env_Quicksand].make();
                loadPlayerPhysicsSettings(playerset, splayer.phys_default[LVL_PhysEnv::Env_Quicksand], QString("character-%1-env-common-quicksand").arg(i));

                playerset.beginGroup( QString("character-%1-world").arg(i) );
                    imgFile = playerset.value("sprite-name", "").toString();
                    splayer.image_wld_n = imgFile;
                    {
                        QString err;
                        GraphicsHelps::loadMaskedImage(playerWldPath, splayer.image_wld_n, splayer.mask_wld_n, err);
                        if( imgFile=="" )
                        {
                            addError(QString("Character-%1 Wld Image filename isn't defined").arg(i));
                            goto skipPLAYER;
                        }
                    }

                    splayer.wld_offset_y = playerset.value("offset-y", "0").toInt();

                    splayer.wld_frames = playerset.value("frames-total", "1").toInt();
                        if(splayer.wld_frames<1) splayer.wld_frames=1;
                    splayer.wld_framespeed = playerset.value("frame-speed", "128").toInt();
                        if(splayer.wld_framespeed<1) splayer.wld_framespeed=1;
                    {
                    QStringList frms;
                            frms = playerset.value("frames-down", "").toString().split(",");
                        foreach(QString x, frms) splayer.wld_frames_down.push_back(x.toInt());
                            frms = playerset.value("frames-right", "").toString().split(",");
                        foreach(QString x, frms) splayer.wld_frames_right.push_back(x.toInt());
                            frms = playerset.value("frames-left", "").toString().split(",");
                        foreach(QString x, frms) splayer.wld_frames_left.push_back(x.toInt());
                            frms = playerset.value("frames-up", "").toString().split(",");
                        foreach(QString x, frms) splayer.wld_frames_up.push_back(x.toInt());
                    }
                playerset.endGroup();


                for(int j=1;j<=total_states;j++)
                {
                    obj_player_state pstate;
                    pstate.isInit=false;
                    pstate.image = NULL;
                    pstate.textureArrayId = 0;
                    pstate.animator_ID = 0;
                    playerset.beginGroup( QString("character-%1-state-%2").arg(i).arg(j) );
                        imgFile = playerset.value("sprite-name", "").toString();
                        pstate.image_n = imgFile;
                        {
                            QString err;
                            GraphicsHelps::loadMaskedImage(playerLvlPath+splayer.sprite_folder+"/", imgFile, pstate.mask_n, err);
                            if( imgFile=="" )
                            {
                                addError(QString("Character-%1 state-%2 Image filename isn't defined").arg(i).arg(j));
                                goto skipPLAYER;
                            }
                        }
                        pstate.duck_allow = playerset.value("duck-allow", default_duck).toBool();
                        pstate.allow_floating = playerset.value("allow-floating", splayer.allowFloating).toBool();
                        pstate.floating_max_time  = playerset.value("floating-max-time", floating_max_time).toInt();
                        pstate.floating_amplitude = playerset.value("floating-amplitude", floating_amplutude).toFloat();
                        pstate.duck_height = playerset.value("default-duck-height", 30).toInt();
                        pstate.height = playerset.value("default-height", 54).toInt();
                        pstate.width = playerset.value("default-width", 24).toInt();
                        pstate.event_script = playerset.value("events", QString("script/player/%2-%1.lua").arg(i).arg(splayer.sprite_folder)).toString();
                        QString sprite_settings = playerset.value("sprite-settings", QString("%2-%1.ini").arg(i).arg(splayer.sprite_folder)).toString();
                        pstate.sprite_setup.init(splayer.matrix_width, splayer.matrix_height);
                        if(pstate.sprite_setup.load(config_dir+"characters/"+sprite_settings))
                        {
                            pstate.width = pstate.sprite_setup.frameWidth;
                            pstate.height = pstate.sprite_setup.frameHeight;
                            pstate.duck_height = pstate.sprite_setup.frameHeightDuck;
                        }
                    playerset.endGroup();

                        pstate.phys[LVL_PhysEnv::Env_Air].make();
                    loadPlayerPhysicsSettings(playerset, pstate.phys[LVL_PhysEnv::Env_Air], QString("character-%1-env-%2-air").arg(i).arg(j));
                        pstate.phys[LVL_PhysEnv::Env_Water].make();
                    loadPlayerPhysicsSettings(playerset, pstate.phys[LVL_PhysEnv::Env_Water], QString("character-%1-env-%2-water").arg(i).arg(j));
                        splayer.phys_default[LVL_PhysEnv::Env_Quicksand].make();
                    loadPlayerPhysicsSettings(playerset, pstate.phys[LVL_PhysEnv::Env_Quicksand], QString("character-%1-env-%2-quicksand").arg(i).arg(j));


                    splayer.states[j] = pstate;
                }
            }//States

            splayer.id = i;
            playable_characters[i] = splayer;

          skipPLAYER:
          if( playerset.status()!=QSettings::NoError)
          {
            addError(QString("ERROR LOADING lvl_characters.ini N:%1 (character-%2)").arg(playerset.status()).arg(i), QtCriticalMsg);

            PGE_MsgBox msgBox(NULL, QString("ERROR LOADING lvl_characters.ini N:%1 (character-%2)").arg(playerset.status()).arg(i),
                              PGE_MsgBox::msg_error);
            msgBox.exec();

             break;
          }
       }

       if((unsigned int)playable_characters.size() < players_total)
       {
           addError(QString("Not all characters are loaded! Total: %1, Loaded: %2)").arg(players_total).arg(playable_characters.size()), QtWarningMsg);
           PGE_MsgBox msgBox(NULL, QString("Not all characters are loaded! Total: %1, Loaded: %2).\n\nGame can't be started!").arg(players_total).arg(playable_characters.size()),
                             PGE_MsgBox::msg_error);
           msgBox.exec();
       }

       return true;
}

