/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/graphics_funcs.h>
#include <common_features/number_limiter.h>
#include <common_features/logger.h>
#include <scenes/level/lvl_physenv.h>
#include <Utils/files.h>
#include <fmt/fmt_format.h>

/*****Playable Characters************/
PGE_DataArray<obj_player > ConfigManager::playable_characters;
CustomDirManager ConfigManager::Dir_PlayerWld;
CustomDirManager ConfigManager::Dir_PlayerLvl;
CustomDirManager ConfigManager::Dir_PlayerScript;
/*****Playable Characters************/

static void loadPlayerPhysicsSettings(IniProcessing &set, obj_player_physics &t, const std::string &grp)
{
    set.beginGroup(grp);
    {
        set.read("walk_force", t.walk_force, t.walk_force);
        NumberLimiter::applyD(t.walk_force, 6.5, 0.0);

        set.read("run_force", t.run_force, t.run_force);
        NumberLimiter::applyD(t.run_force, 3.25, 0.0);

        set.read("decelerate_stop", t.decelerate_stop, t.decelerate_stop);
        NumberLimiter::applyD(t.decelerate_stop, 4.55, 0.0);

        set.read("decelerate_run", t.decelerate_run, t.decelerate_run);
        NumberLimiter::applyD(t.decelerate_run, 10.88, 0.0);

        set.read("decelerate_turn", t.decelerate_turn, t.decelerate_turn);
        NumberLimiter::applyD(t.decelerate_turn, 18.2, 0.0);

        set.read("decelerate_air", t.decelerate_air, t.decelerate_air);
        NumberLimiter::applyD(t.decelerate_air, 0.0, 0.0);

        set.read("ground_c", t.ground_c, t.ground_c);
        NumberLimiter::applyD(t.ground_c, 0.0, 0.0);

        set.read("ground_c_max", t.ground_c_max, t.ground_c_max);
        NumberLimiter::applyD(t.ground_c_max, 0.0, 0.0);

        set.read("slippery_c", t.slippery_c, t.slippery_c);
        NumberLimiter::applyD(t.slippery_c, 0.0, 0.0);

        set.read("gravity_accel", t.gravity_accel, t.gravity_accel);
        NumberLimiter::applyD(t.gravity_accel, 26.0, 0.0);

        set.read("gravity_scale", t.gravity_scale, t.gravity_scale);
        set.read("velocity_jump", t.velocity_jump, t.velocity_jump);
        NumberLimiter::applyD(t.velocity_jump, 5.3, 0.0);

        set.read("velocity_jump_bounce", t.velocity_jump_bounce, t.velocity_jump_bounce);
        NumberLimiter::applyD(t.velocity_jump_bounce, 5.3, 0.0);

        set.read("velocity_jump_spring", t.velocity_jump_spring, t.velocity_jump_spring);
        NumberLimiter::applyD(t.velocity_jump_spring, 9.3, 0.0);

        set.read("velocity_jump_c", t.velocity_jump_c, t.velocity_jump_c);
        NumberLimiter::applyD(t.velocity_jump_c, 5.8, 0.0);

        set.read("jump_time", t.jump_time, t.jump_time);
        NumberLimiter::applyD(t.jump_time, 260, 0);

        set.read("jump_time_bounce", t.jump_time_bounce, t.jump_time_bounce);
        NumberLimiter::applyD(t.jump_time_bounce, 370, 0);

        set.read("jump_time_spring", t.jump_time_spring, t.jump_time_spring);
        NumberLimiter::applyD(t.jump_time_spring, 530, 0);

        set.read("velocity_climb_x", t.velocity_climb_x, t.velocity_climb_x);
        NumberLimiter::applyD(t.velocity_climb_x, 1.5, 0.0);

        set.read("velocity_climb_y_up", t.velocity_climb_y_up, t.velocity_climb_y_up);
        NumberLimiter::applyD(t.velocity_climb_y_up, 2.0, 0.0);

        set.read("velocity_climb_y_down", t.velocity_climb_y_down, t.velocity_climb_y_down);
        NumberLimiter::applyD(t.velocity_climb_y_down, 3.0, 0.0);

        set.read("MaxSpeed_walk", t.MaxSpeed_walk, t.MaxSpeed_walk);
        NumberLimiter::applyD(t.velocity_climb_x, 3.0, 0.0);

        set.read("MaxSpeed_run", t.MaxSpeed_run, t.MaxSpeed_run);
        NumberLimiter::applyD(t.MaxSpeed_run, 6.0, 0.0);

        set.read("MaxSpeed_up", t.MaxSpeed_up, t.MaxSpeed_up);
        NumberLimiter::applyD(t.MaxSpeed_up, 74.0, 0.0);

        set.read("MaxSpeed_down", t.MaxSpeed_down, t.MaxSpeed_down);
        NumberLimiter::applyD(t.MaxSpeed_down, 12.0, 0.0);

        set.read("strict_max_speed_on_ground", t.strict_max_speed_on_ground, false);
        set.read("zero_speed_y_on_enter", t.zero_speed_y_on_enter, false);
        set.read("slow_up_speed_y_coeff", t.slow_up_speed_y_coeff, t.slow_up_speed_y_coeff);
        NumberLimiter::applyD(t.slow_up_speed_y_coeff, 0.325, 0.0);

        set.read("slow_speed_x_on_enter", t.slow_speed_x_on_enter, false);
        set.read("slow_speed_x_coeff", t.slow_speed_x_coeff, t.slow_speed_x_coeff);
        NumberLimiter::applyD(t.slow_speed_x_coeff, 0.125, 0.0);
    }
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
    gravity_accel   = 26.0;//!< Gravity acceleration
    gravity_scale   = 1.0; //!< Gravity scale
    velocity_jump   = 5.2; //!< Jump velocity
    velocity_jump_bounce = 5.3;
    velocity_jump_spring = 9.3;
    velocity_jump_c = 5.8; //!< Jump coefficient which provides increzed jump height dependent to speed
    jump_time       = 260;  //!< Time to jump
    jump_time_bounce = 370; //!< Time to jump from bouncing surface
    jump_time_spring = 530; //!< Time to jump from a spring
    velocity_climb_x     = 1.5; //!< Climbing velocity
    velocity_climb_y_up  = 2.0; //!< Climbing velocity
    velocity_climb_y_down = 3.0; //!< Climbing velocity
    MaxSpeed_walk       = 3.0; //!< Max walk speed
    MaxSpeed_run        = 6.0;  //!< Max run speed
    MaxSpeed_up         = 74.0;   //!< Fly UP Max fall speed
    MaxSpeed_down       = 12.0; //!< Max fall down speed
    zero_speed_y_on_enter = false;
    slow_up_speed_y_coeff = 0.325;
    slow_speed_x_on_enter = false;
    slow_speed_x_coeff = 0.125;
}




bool ConfigManager::loadPlayableCharacters()
{
    try
    {
        pLogDebug("Loading playable characters...");
        unsigned int i;
        unsigned long players_total = 0;
        std::string plr_ini = config_dir.toStdString() + "lvl_characters.ini";

        if(!Files::fileExists(plr_ini))
        {
            addError(QString("ERROR LOADING lvl_characters.ini: file does not exist"), QtCriticalMsg);
            PGE_MsgBox msgBox(NULL, QString("ERROR LOADING lvl_characters.ini: file does not exist"),
                              PGE_MsgBox::msg_fatal);
            msgBox.exec();
            return false;
        }

        IniProcessing setup(plr_ini);
        playable_characters.clear();   //Clear old
        setup.beginGroup("main-characters");
        players_total = setup.value("total", 0).toULongLong();
        setup.endGroup();

        if(players_total == 0)
        {
            addError(QString("ERROR LOADING lvl_characters.ini: number of items not define, or empty config"), QtCriticalMsg);
            PGE_MsgBox msgBox(NULL, QString("ERROR LOADING lvl_characters.ini: number of items not define, or empty config"),
                              PGE_MsgBox::msg_fatal);
            msgBox.exec();
            return false;
        }

        playable_characters.allocateSlots(players_total);

        for(i = 1; i <= players_total; i++)
        {
            obj_player splayer;
            splayer.isInit_wld = false;
            splayer.image_wld = NULL;
            splayer.textureArrayId_wld = 0;
            splayer.animator_ID_wld = 0;
            splayer.wld_offset_y = 0;

            //Default size of frame is 100x100, but re-calculates from matrix size and size of target sprite
            splayer.frame_width = 100;
            splayer.frame_height = 100;
            unsigned long total_states = 0;
            setup.beginGroup(fmt::format("character-{0}", i));
            setup.read("name", splayer.name, fmt::format("player {0}", i));

            if(splayer.name.empty())
            {
                addError(QString("Player-%1 Item name isn't defined").arg(i));
                goto skipPLAYER;
            }

            setup.read("sprite-folder", splayer.sprite_folder, fmt::format("player-{0}", i));
            setup.read("sprite-folder", splayer.state_type, 0);
            setup.read("matrix-width",  splayer.matrix_width, 10);
            setup.read("matrix-height", splayer.matrix_height, 10);
            setup.read("script-file",   splayer.script, "");
            setup.read("states-number", total_states, 0);

            if(total_states == 0)
            {
                addError(QString("player-%1 has no states!").arg(i));
                goto skipPLAYER;
            }

            splayer.fail_effect.fill("fail", &setup);
            splayer.slide_effect.fill("floor-slide", &setup);
            setup.endGroup();
            {
                //States
                bool default_duck = false;
                int floating_max_time = 1500;
                double floating_amplutude = 0.8;
                setup.beginGroup(fmt::format("character-{0}-physics-common", i));
                {
                    setup.read("duck-allow", default_duck, false);
                    setup.read("allow-floating", splayer.allowFloating, false);
                    setup.read("floating-max-time", floating_max_time, 1500);
                    setup.read("floating-amplitude", floating_amplutude, 0.8);
                }
                setup.endGroup();

                //default environment specific physics
                splayer.phys_default.allocateSlots(LVL_PhysEnv::numOfEnvironments);

                obj_player_physics physicsDef;
                splayer.phys_default.storeElement(LVL_PhysEnv::Env_Air, physicsDef);
                splayer.phys_default.storeElement(LVL_PhysEnv::Env_Water, physicsDef);
                splayer.phys_default.storeElement(LVL_PhysEnv::Env_Quicksand, physicsDef);
                loadPlayerPhysicsSettings(setup, splayer.phys_default[LVL_PhysEnv::Env_Air], fmt::format("character-{0}-env-common-air", i));
                loadPlayerPhysicsSettings(setup, splayer.phys_default[LVL_PhysEnv::Env_Water], fmt::format("character-{0}-env-common-water", i));
                loadPlayerPhysicsSettings(setup, splayer.phys_default[LVL_PhysEnv::Env_Quicksand], fmt::format("character-{0}-env-common-quicksand", i));

                setup.beginGroup(fmt::format("character-{0}-world", i));
                imgFile = setup.value("sprite-name", "").toQString();
                splayer.image_wld_n = imgFile;
                {
                    QString err;
                    GraphicsHelps::getMaskedImageInfo(playerWldPath, splayer.image_wld_n, splayer.mask_wld_n, err);

                    if(imgFile == "")
                    {
                        addError(QString("Character-%1 Wld Image filename isn't defined").arg(i));
                        goto skipPLAYER;
                    }
                }
                setup.read("offset-y", splayer.wld_offset_y, 0);
                setup.read("frames-total", splayer.wld_frames, 1);

                if(splayer.wld_frames < 1) splayer.wld_frames = 1;

                setup.read("frame-speed", splayer.wld_framespeed, 128);

                if(splayer.wld_framespeed < 1) splayer.wld_framespeed = 1;
                {
                    setup.read("frames-down",   splayer.wld_frames_down);
                    setup.read("frames-right",  splayer.wld_frames_right);
                    setup.read("frames-left",   splayer.wld_frames_left);
                    setup.read("frames-up",     splayer.wld_frames_up);
                }
                setup.endGroup();
                splayer.states.allocateSlots(total_states);

                for(unsigned long j = 1; j <= total_states; j++)
                {
                    obj_player_state pstate;
                    pstate.isInit = false;
                    pstate.image = NULL;
                    pstate.textureArrayId = 0;
                    pstate.animator_ID = 0;
                    setup.beginGroup(fmt::format("character-{0}-state-{1}", i, j));
                    {
                        setup.read("name", pstate.name, fmt::format("State {0}", j));
                        setup.read("sprite-name", imgFile, "");
                        pstate.image_n = imgFile;
                        {
                            QString err;
                            GraphicsHelps::getMaskedImageInfo(playerLvlPath + QString::fromStdString(splayer.sprite_folder) + "/", imgFile, pstate.mask_n, err);

                            if(imgFile == "")
                            {
                                addError(QString("Character-%1 state-%2 Image filename isn't defined").arg(i).arg(j));
                                goto skipPLAYER;
                            }
                        }
                        setup.read("duck-allow", pstate.duck_allow, default_duck);
                        setup.read("allow-floating", pstate.allow_floating, splayer.allowFloating);
                        setup.read("floating-max-time", pstate.floating_max_time, floating_max_time);
                        setup.read("floating-amplitude", pstate.floating_amplitude, floating_amplutude);
                        setup.read("default-duck-height", pstate.duck_height, 30);
                        setup.read("default-height", pstate.height, 54);
                        setup.read("default-width", pstate.width, 24);
                        setup.read("events", pstate.event_script, fmt::format("script/player/{1}-{0}.lua", i, splayer.sprite_folder));

                        std::string sprite_settings;
                        setup.read("sprite-settings", sprite_settings, fmt::format("{1}-{0}.ini", i, splayer.sprite_folder));

                        if(pstate.sprite_setup.load(config_dir.toStdString() + "characters/" + sprite_settings))
                        {
                            pstate.width = pstate.sprite_setup.frameWidth;
                            pstate.height = pstate.sprite_setup.frameHeight;
                            pstate.duck_height = pstate.sprite_setup.frameHeightDuck;
                        }
                    }
                    setup.endGroup();

                    pstate.phys.allocateSlots(LVL_PhysEnv::numOfEnvironments);
                    pstate.phys.storeElement(LVL_PhysEnv::Env_Air, physicsDef);
                    pstate.phys.storeElement(LVL_PhysEnv::Env_Water, physicsDef);
                    pstate.phys.storeElement(LVL_PhysEnv::Env_Quicksand, physicsDef);
                    loadPlayerPhysicsSettings(setup, pstate.phys[LVL_PhysEnv::Env_Air], fmt::format("character-{0}-env-{1}-air", i, j));
                    loadPlayerPhysicsSettings(setup, pstate.phys[LVL_PhysEnv::Env_Water], fmt::format("character-{0}-env-{1}-water", i, j));
                    loadPlayerPhysicsSettings(setup, pstate.phys[LVL_PhysEnv::Env_Quicksand], fmt::format("character-{0}-env-{1}-quicksand", i, j));
                    splayer.states.storeElement(j, pstate);
                }
            }//States

            splayer.id = i;
            playable_characters.storeElement(i, splayer);
    skipPLAYER:

            if(setup.lastError() != IniProcessing::ERR_OK)
            {
                addError(QString("ERROR LOADING lvl_characters.ini:%1 N:%2 (character-%3)")
                         .arg(setup.lineWithError())
                         .arg(setup.lastError())
                         .arg(i), QtCriticalMsg);
                PGE_MsgBox msgBox(NULL,
                                  QString("ERROR LOADING lvl_characters.ini:%1 N:%2 (character-%3)")
                                  .arg(setup.lineWithError())
                                  .arg(setup.lastError())
                                  .arg(i),
                                  PGE_MsgBox::msg_error);
                msgBox.exec();
                break;
            }
        }

        if(playable_characters.stored() < players_total)
        {
            addError(QString("Not all characters are loaded! Total: %1, Loaded: %2)")
                     .arg(players_total)
                     .arg(playable_characters.stored()), QtWarningMsg);
            PGE_MsgBox msgBox(NULL, QString("Not all characters are loaded! Total: %1, Loaded: %2).\n\nGame can't be started!").arg(players_total).arg(playable_characters.stored()),
                              PGE_MsgBox::msg_error);
            msgBox.exec();
        }
    }
    catch(const fmt::FormatError &e)
    {
        pLogCritical("Caugh fmt::FormatError exception: %s", e.what());
        return false;
    }
    catch(const std::exception &e)
    {
        pLogCritical("Caugh std::exception exception: %s", e.what());
        return false;
    }
    catch(...)
    {
        pLogCritical("Caugh unknown exception!");
        return false;
    }

    return true;
}
