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
#include <scenes/level/lvl_physenv.h>

/*****Playable Characters************/
QHash<int, obj_player > ConfigManager::playable_characters;
CustomDirManager ConfigManager::Dir_PlayerWld;
CustomDirManager ConfigManager::Dir_PlayerLvl;
/*****Playable Characters************/

void loadPlayerPhysicsSettings(QSettings &set, obj_player_physics &t, QString grp)
{
    set.beginGroup(grp);
        t.walk_force = set.value("walk_force", 900.0).toFloat();
    //    walk_force = 900.0
        t.slippery_c = set.value("slippery_c", 3.0).toFloat();
    //    slippery_c = 3.0
        t.gravity_scale = set.value("gravity_scale", 1.0).toFloat();
    //    gravity_scale = 1.0
        t.velocity_jump = set.value("velocity_jump", 37.0).toFloat();
    //    velocity_jump = 37.0
        t.velocity_climb = set.value("velocity_climb", 15.0).toFloat();
    //    velocity_climb = 15.0
        t.MaxSpeed_walk = set.value("MaxSpeed_walk", 20.0).toFloat();
    //    MaxSpeed_walk = 20.0
        t.MaxSpeed_run = set.value("MaxSpeed_run", 38.0).toFloat();
    //    MaxSpeed_run = 38.0
        t.MaxSpeed_up = set.value("MaxSpeed_up", 74.0).toFloat();
    //    MaxSpeed_up = 74.0
        t.MaxSpeed_down = set.value("MaxSpeed_down", 72.0).toFloat();
    //    MaxSpeed_down = 72.0
        t.damping = set.value("damping", 0.0).toFloat();
    //    damping = 0.0
        t.zero_speed_y_on_enter = set.value("zero_speed_y_on_enter", false).toBool();
    //    zero_speed_y_on_enter=false
        t.slow_speed_x_on_enter = set.value("slow_speed_x_on_enter", false).toBool();
    //    slow_speed_x_on_enter=false
    set.endGroup();
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

            //Default size of frame is 100x100
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
                playerset.beginGroup( QString("character-%1-physics-common").arg(i) );
                    default_duck =  playerset.value("duck-allow", false ).toBool();
                    splayer.allowFloating =  playerset.value("allow-floating", false ).toBool();
                    floating_max_time = playerset.value("floating-max-time", 1500 ).toInt();
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
                    splayer.wld_frames = playerset.value("frames-total", "1").toInt();
                    splayer.wld_framespeed = playerset.value("frame-speed", "128").toInt();
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
                        pstate.floating_max_time = playerset.value("floating-max-time", floating_max_time).toInt();
                        pstate.duck_height = playerset.value("default-duck-height", 30).toInt();
                        pstate.height = playerset.value("default-height", 54).toInt();
                        pstate.width = playerset.value("default-width", 24).toInt();
                        pstate.event_script = playerset.value("events", QString("%2-%1.lua").arg(i).arg(splayer.sprite_folder)).toString();
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

