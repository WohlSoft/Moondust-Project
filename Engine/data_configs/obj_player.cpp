/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/graphics_funcs.h>
#include <common_features/number_limiter.h>
#include <common_features/logger.h>
#include <scenes/level/lvl_physenv.h>
#include <Utils/files.h>
#include <common_features/fmt_format_ne.h>

/*****Playable Characters************/
PGE_DataArray<obj_player > ConfigManager::playable_characters;
CustomDirManager ConfigManager::Dir_PlayerWld;
CustomDirManager ConfigManager::Dir_PlayerLvl;
CustomDirManager ConfigManager::Dir_PlayerScript;
CustomDirManager ConfigManager::Dir_PlayerCalibrations;
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

bool ConfigManager::loadPlayableCharacters()
{
    try
    {
        pLogDebug("Loading playable characters...");
        unsigned int i;
        unsigned long players_total = 0;
        std::string plr_ini = config_dirSTD + "lvl_characters.ini";
        if(!Files::fileExists(plr_ini))
        {
            addError("ERROR LOADING lvl_characters.ini: file does not exist");
            PGE_MsgBox msgBox(nullptr, "ERROR LOADING lvl_characters.ini: file does not exist", PGE_MsgBox::msg_fatal);
            msgBox.exec();
            return false;
        }

        IniProcessing setup(plr_ini);
        playable_characters.clear();   //Clear old
        setup.beginGroup("main-characters");
        {
            players_total = setup.value("total", 0).toULongLong();
        }
        setup.endGroup();

        if(players_total == 0)
        {
            addError("ERROR LOADING lvl_characters.ini: number of items not define, or empty config");
            PGE_MsgBox msgBox(nullptr, "ERROR LOADING lvl_characters.ini: number of items not define, or empty config", PGE_MsgBox::msg_fatal);
            msgBox.exec();
            return false;
        }

        playable_characters.allocateSlots(players_total);
        for(i = 1; i <= players_total; i++)
        {
            obj_player splayer;
            splayer.isInit_wld = false;
            splayer.image_wld = nullptr;
            splayer.textureArrayId_wld = 0;
            splayer.animator_ID_wld = 0;
            splayer.wld_offset_y = 0;

            //Default size of frame is 100x100, but re-calculates from matrix size and size of target sprite
            splayer.frame_width = 100;
            splayer.frame_height = 100;
            unsigned long total_states = 0;
            setup.beginGroup(fmt::format_ne("character-{0}", i));
            setup.read("name", splayer.name, fmt::format_ne("player {0}", i));

            if(splayer.name.empty())
            {
                addError(fmt::format_ne("Player-{0} Item name isn't defined", i));
                goto skipPLAYER;
            }

            setup.read("sprite-folder", splayer.sprite_folder, fmt::format_ne("player-{0}", i));
            setup.read("sprite-folder", splayer.state_type, 0);
            setup.read("matrix-width",  splayer.matrix_width, 10);
            setup.read("matrix-height", splayer.matrix_height, 10);
            setup.read("script-file",   splayer.script, "");
            setup.read("states-number", total_states, 0);

            if(total_states == 0)
            {
                addError(fmt::format_ne("player-{0} has no states!", i));
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
                setup.beginGroup(fmt::format_ne("character-{0}-physics-common", i));
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
                loadPlayerPhysicsSettings(setup, splayer.phys_default[LVL_PhysEnv::Env_Air], fmt::format_ne("character-{0}-env-common-air", i));
                loadPlayerPhysicsSettings(setup, splayer.phys_default[LVL_PhysEnv::Env_Water], fmt::format_ne("character-{0}-env-common-water", i));
                loadPlayerPhysicsSettings(setup, splayer.phys_default[LVL_PhysEnv::Env_Quicksand], fmt::format_ne("character-{0}-env-common-quicksand", i));

                setup.beginGroup(fmt::format_ne("character-{0}-world", i));
                setup.read("sprite-name", imgFile, "");
                splayer.image_wld_n = imgFile;
                {
                    std::string err;
                    GraphicsHelps::getMaskedImageInfo(playerWldPath, splayer.image_wld_n, splayer.mask_wld_n, err);
                    if(imgFile.empty())
                    {
                        addError(fmt::format_ne("Character-{0} Wld Image filename isn't defined", i));
                        goto skipPLAYER;
                    }
                }
                setup.read("offset-y", splayer.wld_offset_y, 0);
                setup.read("frames-total", splayer.wld_frames, 1);

                if(splayer.wld_frames < 1) splayer.wld_frames = 1;

                setup.read("frame-delay", splayer.wld_framespeed, 128);//Real
                setup.read("frame-speed", splayer.wld_framespeed, splayer.wld_framespeed);//Alias

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
                    pstate.image = nullptr;
                    pstate.textureArrayId = 0;
                    pstate.animator_ID = 0;
                    setup.beginGroup(fmt::format_ne("character-{0}-state-{1}", i, j));
                    {
                        setup.read("name", pstate.name, fmt::format_ne("State {0}", j));
                        setup.read("sprite-name", imgFile, "");
                        pstate.image_n = imgFile;
                        {
                            std::string err;
                            GraphicsHelps::getMaskedImageInfo(playerLvlPath + splayer.sprite_folder + "/", imgFile, pstate.mask_n, err);
                            if(imgFile.empty())
                            {
                                addError(fmt::format_ne("Character-{0} state-{1} Image filename isn't defined", i, j));
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
                        setup.read("events", pstate.event_script, fmt::format_ne("script/player/{1}-{0}.lua", i, splayer.sprite_folder));

                        std::string sprite_settings;
                        setup.read("sprite-settings", sprite_settings, fmt::format_ne("{1}-{0}.ini", i, splayer.sprite_folder));

                        std::string ss_path = Dir_PlayerCalibrations.getCustomFile(sprite_settings);
                        if(pstate.sprite_setup.load(ss_path))
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
                    loadPlayerPhysicsSettings(setup, pstate.phys[LVL_PhysEnv::Env_Air], fmt::format_ne("character-{0}-env-{1}-air", i, j));
                    loadPlayerPhysicsSettings(setup, pstate.phys[LVL_PhysEnv::Env_Water], fmt::format_ne("character-{0}-env-{1}-water", i, j));
                    loadPlayerPhysicsSettings(setup, pstate.phys[LVL_PhysEnv::Env_Quicksand], fmt::format_ne("character-{0}-env-{1}-quicksand", i, j));
                    splayer.states.storeElement(j, pstate);
                }
            }//States

            splayer.id = i;
            playable_characters.storeElement(i, splayer);
    skipPLAYER:

            if(setup.lastError() != IniProcessing::ERR_OK)
            {
                std::string msg = fmt::format_ne("ERROR LOADING lvl_characters.ini:{0} N:{1} (character-{2})",
                                              setup.lineWithError(),
                                              setup.lastError(), i);
                addError(msg);
                PGE_MsgBox msgBox(nullptr, msg, PGE_MsgBox::msg_error);
                msgBox.exec();
                break;
            }
        }

        if(playable_characters.stored() < players_total)
        {
            std::string msg = fmt::format_ne("Not all characters are loaded! Total: {0}, Loaded: {1})",
                                          players_total,
                                          playable_characters.stored());
            addError(msg);
            PGE_MsgBox msgBox(nullptr, msg, PGE_MsgBox::msg_error);
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
