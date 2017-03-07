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

#include "global_settings.h"
#include <common_features/app_path.h>
#include <graphics/window.h>
#include <controls/controller_joystick.h>
#include <controls/controller_keyboard.h>
#include <common_features/logger.h>
#include <common_features/number_limiter.h>
#include <IniProcessor/ini_processing.h>
#include <fmt/fmt_format.h>

GlobalSettings g_AppSettings;

GlobalSettings::GlobalSettings()
{
    resetDefaults();
}

GlobalSettings::~GlobalSettings()
{}

void GlobalSettings::initJoysticks()
{
    SDL_JoystickEventState(SDL_ENABLE);

    for(int i = 0; i < SDL_NumJoysticks(); i++)
    {
        joysticks.push_back(SDL_JoystickOpen(i));
        SDL_Joystick* j = joysticks.back();
        pLogDebug("==========================");
        pLogDebug("Josytick %s", SDL_JoystickName(j));
        pLogDebug("--------------------------");
        pLogDebug("Axes:    %d", SDL_JoystickNumAxes(j));
        pLogDebug("Balls:   %d", SDL_JoystickNumBalls(j));
        pLogDebug("Hats:    %d", SDL_JoystickNumHats(j));
        pLogDebug("Buttons: %d", SDL_JoystickNumButtons(j));
        pLogDebug("==========================");
    }
}

void GlobalSettings::closeJoysticks()
{
    for(SDL_Joystick *j : joysticks)
        SDL_JoystickClose(j);
    joysticks.clear();
}

void GlobalSettings::load()
{
    IniProcessing setup(AppPathManager::settingsFileSTD());
    setup.beginGroup("Main");
    {
        //TicksPerSecond=setup.value("phys-step", TicksPerSecond).toUInt();
        //    NumberLimiter::apply(TicksPerSecond, 65, 180);
        setup.read("phys-step-time", timeOfFrame, static_cast<int>(TicksPerSecond));
        NumberLimiter::apply(timeOfFrame, 2, 15);
        TicksPerSecond = 1000.0 / g_AppSettings.timeOfFrame;

        setup.read("show-debug-info", showDebugInfo, showDebugInfo);
        setup.read("full-screen", fullScreen, fullScreen);
        setup.read("frame-skip", frameSkip, frameSkip);
        setup.read("vsync", vsync, vsync);
        setup.read("player1-controller", player1_controller, player1_controller);
        setup.read("player2-controller", player2_controller, player2_controller);
        setup.read("volume-sfx", volume_sound, 128);
        NumberLimiter::applyD(volume_sound, 128, 0, 128);
        setup.read("volume-mus", volume_music, 45);
        NumberLimiter::applyD(volume_sound, 45, 0, 128);
    }
    setup.endGroup();
    loadKeyMap(player1_keyboard, setup, "player-1-keyboard");
    loadKeyMap(player2_keyboard, setup, "player-2-keyboard");
    player1_joysticks.clear();
    player2_joysticks.clear();

    if(PGE_Window::isReady())
        loadJoystickSettings();
}

void GlobalSettings::loadJoystickSettings()
{
    player1_joysticks.clear();
    player2_joysticks.clear();
    player1_joysticks_ctrls_ids.clear();
    player2_joysticks_ctrls_ids.clear();
    player1_joysticks_ctrls_types.clear();
    player2_joysticks_ctrls_types.clear();

    if((player1_controller < -1) || (player1_controller > SDL_NumJoysticks() - 1)) player1_controller = -1;

    if((player2_controller < -1) || (player2_controller > SDL_NumJoysticks() - 1)) player2_controller = -1;

    IniProcessing setup(AppPathManager::settingsFileSTD());

    for(int i = 0; i < SDL_NumJoysticks(); i++)
    {
        KeyMap joy1;
        loadKeyMap(joy1, setup, fmt::format("player-1-joystick{0}", i));
        loadJoyCtrlMapID(joy1, setup, fmt::format("player-1-joystick{0}-ctrls-id", i));
        loadJoyCtrlMapType(joy1, setup, fmt::format("player-1-joystick{0}-ctrls-type", i));
        player1_joysticks.push_back(joy1);
        KeyMap joy2;
        loadKeyMap(joy2, setup, fmt::format("player-2-joystick{0}", i));
        loadJoyCtrlMapID(joy2, setup, fmt::format("player-2-joystick{0}-ctrls-id", i));
        loadJoyCtrlMapType(joy2, setup, fmt::format("player-2-joystick{0}-ctrls-type", i));
        player2_joysticks.push_back(joy2);
    }
}

void GlobalSettings::save()
{
    IniProcessing setup(AppPathManager::settingsFileSTD());
    setup.beginGroup("Main");
    {
        setup.setValue("phys-step", TicksPerSecond);
        setup.setValue("phys-step-time", timeOfFrame);
        setup.setValue("show-debug-info", showDebugInfo);
        setup.setValue("frame-skip", frameSkip);
        setup.setValue("full-screen", fullScreen);
        setup.setValue("vsync", vsync);
        setup.setValue("player1-controller", player1_controller);
        setup.setValue("player2-controller", player2_controller);
        setup.setValue("volume-sfx", volume_sound);
        setup.setValue("volume-mus", volume_music);
    }
    setup.endGroup();
    saveKeyMap(player1_keyboard, setup, "player-1-keyboard");
    saveKeyMap(player2_keyboard, setup, "player-2-keyboard");

    for(size_t i = 0; i < player1_joysticks.size() && i < player2_joysticks.size(); i++)
    {
        saveKeyMap(player1_joysticks[i], setup, fmt::format("player-1-joystick{0}", i));
        saveKeyMap(player2_joysticks[i], setup, fmt::format("player-2-joystick{0}", i));
        saveJoyCtrlMapID(player1_joysticks[i], setup, fmt::format("player-1-joystick{0}-ctrls-id", i));
        saveJoyCtrlMapID(player2_joysticks[i], setup, fmt::format("player-2-joystick{0}-ctrls-id", i));
        saveJoyCtrlMapType(player1_joysticks[i], setup, fmt::format("player-1-joystick{0}-ctrls-type", i));
        saveJoyCtrlMapType(player2_joysticks[i], setup, fmt::format("player-2-joystick{0}-ctrls-type", i));
    }

    setup.writeIniFile();
}

void GlobalSettings::resetDefaults()
{
    debugMode       = false;
    interprocessing = false;
    ScreenWidth = 800;
    ScreenHeight = 600;
    TicksPerSecond = 65;
    timeOfFrame = 15;
    vsync = true;
    showDebugInfo = false;
    frameSkip = true;
    fullScreen = false;
    volume_sound = 128;
    volume_music = 64;
    player1_controller = -1;
    player2_controller = -1;
    player1_keyboard.jump.val       = SDL_SCANCODE_Z;
    player1_keyboard.jump_alt.val   = SDL_SCANCODE_A;
    player1_keyboard.run.val        = SDL_SCANCODE_X;
    player1_keyboard.run_alt.val    = SDL_SCANCODE_S;
    player1_keyboard.drop.val       = SDL_SCANCODE_LSHIFT;
    player1_keyboard.start.val      = SDL_SCANCODE_RETURN;
    player1_keyboard.left.val       = SDL_SCANCODE_LEFT;
    player1_keyboard.right.val      = SDL_SCANCODE_RIGHT;
    player1_keyboard.up.val         = SDL_SCANCODE_UP;
    player1_keyboard.down.val       = SDL_SCANCODE_DOWN;
    renderer_device     = 0;
}

void GlobalSettings::apply()
{
    PGE_Window::TicksPerSecond = 1000.0 / timeOfFrame;
    PGE_Window::Width = ScreenWidth;
    PGE_Window::Height = ScreenHeight;
    PGE_Window::showDebugInfo = showDebugInfo;
    PGE_Window::TimeOfFrame = timeOfFrame;
    PGE_Window::vsync = vsync;
}


void GlobalSettings::loadKeyMap(KeyMap &map, IniProcessing &set, std::string grp)
{
    set.beginGroup(grp);
    set.read("left",       map.left.val,     map.left.val);
    set.read("right",      map.right.val,    map.right.val);
    set.read("up",         map.up.val,       map.up.val);
    set.read("down",       map.down.val,     map.down.val);
    set.read("jump",       map.jump.val,     map.jump.val);
    set.read("jump-alt",   map.jump_alt.val, map.jump_alt.val);
    set.read("run",        map.run.val,      map.run.val);
    set.read("run-alt",    map.run_alt.val,  map.run_alt.val);
    set.read("drop",       map.drop.val,     map.drop.val);
    set.read("start",      map.start.val,    map.start.val);
    set.endGroup();
}

void GlobalSettings::saveKeyMap(KeyMap &map, IniProcessing &set, std::string grp)
{
    set.beginGroup(grp);
    set.setValue("left", map.left.val);
    set.setValue("right", map.right.val);
    set.setValue("up", map.up.val);
    set.setValue("down", map.down.val);
    set.setValue("jump", map.jump.val);
    set.setValue("jump-alt", map.jump_alt.val);
    set.setValue("run", map.run.val);
    set.setValue("run-alt", map.run_alt.val);
    set.setValue("drop", map.drop.val);
    set.setValue("start", map.start.val);
    set.endGroup();
}

void GlobalSettings::loadJoyCtrlMapID(KeyMap &map, IniProcessing &set, std::string grp)
{
    set.beginGroup(grp);
    set.read("left",       map.left.id,        map.left.id);
    set.read("right",      map.right.id,       map.right.id);
    set.read("up",         map.up.id,          map.up.id);
    set.read("down",       map.down.id,        map.down.id);
    set.read("jump",       map.jump.id,        map.jump.id);
    set.read("jump-alt",   map.jump_alt.id,    map.jump_alt.id);
    set.read("run",        map.run.id,         map.run.id);
    set.read("run-alt",    map.run_alt.id,     map.run_alt.id);
    set.read("drop",       map.drop.id,        map.drop.id);
    set.read("start",      map.start.id,       map.start.id);
    set.endGroup();
}

void GlobalSettings::saveJoyCtrlMapID(KeyMap &map, IniProcessing &set, std::string grp)
{
    set.beginGroup(grp);
    set.setValue("left", map.left.id);
    set.setValue("right", map.right.id);
    set.setValue("up", map.up.id);
    set.setValue("down", map.down.id);
    set.setValue("jump", map.jump.id);
    set.setValue("jump-alt", map.jump_alt.id);
    set.setValue("run", map.run.id);
    set.setValue("run-alt", map.run_alt.id);
    set.setValue("drop", map.drop.id);
    set.setValue("start", map.start.id);
    set.endGroup();
}

void GlobalSettings::loadJoyCtrlMapType(KeyMap &map, IniProcessing &set, std::string grp)
{
    set.beginGroup(grp);
    set.read("left", map.left.type, map.left.type);
    set.read("right", map.right.type, map.right.type);
    set.read("up", map.up.type, map.up.type);
    set.read("down", map.down.type, map.down.type);
    set.read("jump", map.jump.type, map.jump.type);
    set.read("jump-alt", map.jump_alt.type, map.jump_alt.type);
    set.read("run", map.run.type, map.run.type);
    set.read("run-alt", map.run_alt.type, map.run_alt.type);
    set.read("drop", map.drop.type, map.drop.type);
    set.read("start", map.start.type, map.start.type);
    set.endGroup();
}

void GlobalSettings::saveJoyCtrlMapType(KeyMap &map, IniProcessing &set, std::string grp)
{
    set.beginGroup(grp);
    set.setValue("left", map.left.type);
    set.setValue("right", map.right.type);
    set.setValue("up", map.up.type);
    set.setValue("down", map.down.type);
    set.setValue("jump", map.jump.type);
    set.setValue("jump-alt", map.jump_alt.type);
    set.setValue("run", map.run.type);
    set.setValue("run-alt", map.run_alt.type);
    set.setValue("drop", map.drop.type);
    set.setValue("start", map.start.type);
    set.endGroup();
}


Controller *GlobalSettings::openController(int player)
{
    Controller *TargetController = NULL;

    if(player == 1)
    {
        if(player1_controller >= 0)
        {
            TargetController = new JoystickController();
            size_t DeviceID = static_cast<size_t>(player1_controller);

            if(DeviceID < player1_joysticks.size())
                TargetController->setKeyMap(player1_joysticks[DeviceID]);

            if(DeviceID < joysticks.size())
                dynamic_cast<JoystickController *>(TargetController)->
                setJoystickDevice(joysticks[DeviceID]);
        }
        else
        {
            TargetController = new KeyboardController();
            TargetController->setKeyMap(player1_keyboard);
        }
    }
    else if(player == 2)
    {
        if(player2_controller >= 0)
        {
            TargetController = new JoystickController();
            size_t DeviceID = static_cast<size_t>(player2_controller);

            if(DeviceID < player2_joysticks.size())
                TargetController->setKeyMap(player2_joysticks[DeviceID]);

            if(DeviceID < joysticks.size())
                dynamic_cast<JoystickController *>(TargetController)->
                setJoystickDevice(joysticks[DeviceID]);
        }
        else
        {
            TargetController = new KeyboardController();
            TargetController->setKeyMap(player2_keyboard);
        }
    }

    return TargetController;
}

