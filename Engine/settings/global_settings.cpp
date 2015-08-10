#include "global_settings.h"
#include <common_features/app_path.h>
#include <graphics/window.h>
#include <controls/controller_joystick.h>
#include <controls/controller_keyboard.h>
#include <common_features/logger.h>
#include <common_features/number_limiter.h>

#include <QSettings>

GlobalSettings AppSettings;

GlobalSettings::GlobalSettings()
{
    resetDefaults();
}

GlobalSettings::~GlobalSettings()
{}

void GlobalSettings::initJoysticks()
{
    SDL_JoystickEventState(SDL_ENABLE);
    for(int i=0; i<SDL_NumJoysticks();i++)
    {
        joysticks.push_back(SDL_JoystickOpen(i));
        WriteToLog(QtDebugMsg, QString("=========================="));
        WriteToLog(QtDebugMsg, QString("Josytick %1").arg(SDL_JoystickName(joysticks.last())));
        WriteToLog(QtDebugMsg, QString("--------------------------"));
        WriteToLog(QtDebugMsg, QString("Axes:    %1").arg(SDL_JoystickNumAxes(joysticks.last())));
        WriteToLog(QtDebugMsg, QString("Balls:   %1").arg(SDL_JoystickNumBalls(joysticks.last())));
        WriteToLog(QtDebugMsg, QString("Hats:    %1").arg(SDL_JoystickNumHats(joysticks.last())));
        WriteToLog(QtDebugMsg, QString("Buttons: %1").arg(SDL_JoystickNumButtons(joysticks.last())));
        WriteToLog(QtDebugMsg, QString("=========================="));
    }
}

void GlobalSettings::closeJoysticks()
{
    while(!joysticks.isEmpty())
    {
        SDL_JoystickClose(joysticks.first());
        joysticks.pop_front();
    }
}

void GlobalSettings::load()
{
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.beginGroup("Main");
    MaxFPS=setup.value("max-fps", MaxFPS).toUInt();
        NumberLimiter::apply(MaxFPS, 65, 1000);
    //TicksPerSecond=setup.value("phys-step", TicksPerSecond).toUInt();
    //    NumberLimiter::apply(TicksPerSecond, 65, 180);
    timeOfFrame=setup.value("phys-step-time", TicksPerSecond).toUInt();
        NumberLimiter::apply(timeOfFrame, 2, 15);
    TicksPerSecond=1000.0f/AppSettings.timeOfFrame;

    showDebugInfo=setup.value("show-debug-info", showDebugInfo).toBool();
    fullScreen=setup.value("full-screen", fullScreen).toBool();
    frameSkip=setup.value("frame-skip", frameSkip).toBool();
    player1_controller=setup.value("player1-controller", player1_controller).toInt();
    player2_controller=setup.value("player2-controller", player2_controller).toInt();

    volume_sound=setup.value("volume-sfx", 128).toInt();
        NumberLimiter::applyD(volume_sound, 128, 0, 128);
    volume_music=setup.value("volume-mus", 45).toInt();
        NumberLimiter::applyD(volume_sound, 45, 0, 128);

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

    if((player1_controller<-1)||(player1_controller>SDL_NumJoysticks()-1)) player1_controller=-1;
    if((player2_controller<-1)||(player2_controller>SDL_NumJoysticks()-1)) player2_controller=-1;

    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    for(int i=0; i<SDL_NumJoysticks();i++)
    {
        KeyMap joy1;
        loadKeyMap(joy1, setup, QString("player-1-joystick%1").arg(i));
        loadJoyCtrlMapID(joy1, setup, QString("player-1-joystick%1-ctrls-id").arg(i));
        loadJoyCtrlMapType(joy1, setup, QString("player-1-joystick%1-ctrls-type").arg(i));
        player1_joysticks.push_back(joy1);

        KeyMap joy2;
        loadKeyMap(joy2, setup, QString("player-2-joystick%1").arg(i));
        loadJoyCtrlMapID(joy2, setup, QString("player-2-joystick%1-ctrls-id").arg(i));
        loadJoyCtrlMapType(joy2, setup, QString("player-2-joystick%1-ctrls-type").arg(i));
        player2_joysticks.push_back(joy2);
    }

}

void GlobalSettings::save()
{
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.beginGroup("Main");
        setup.setValue("max-fps", MaxFPS);
        setup.setValue("phys-step", TicksPerSecond);
        setup.setValue("phys-step-time", timeOfFrame);
        setup.setValue("show-debug-info", showDebugInfo);
        setup.setValue("frame-skip", frameSkip);
        setup.setValue("full-screen", fullScreen);
        setup.setValue("player1-controller", player1_controller);
        setup.setValue("player2-controller", player2_controller);
        setup.setValue("volume-sfx", volume_sound);
        setup.setValue("volume-mus", volume_music);
    setup.endGroup();

    saveKeyMap(player1_keyboard, setup, "player-1-keyboard");
    saveKeyMap(player2_keyboard, setup, "player-2-keyboard");
    for(int i=0; i<player1_joysticks.size() && i<player2_joysticks.size(); i++)
    {
        saveKeyMap(player1_joysticks[i], setup, QString("player-1-joystick%1").arg(i));
        saveKeyMap(player2_joysticks[i], setup, QString("player-2-joystick%1").arg(i));
        saveJoyCtrlMapID(player1_joysticks[i], setup, QString("player-1-joystick%1-ctrls-id").arg(i));
        saveJoyCtrlMapID(player2_joysticks[i], setup, QString("player-2-joystick%1-ctrls-id").arg(i));
        saveJoyCtrlMapType(player1_joysticks[i], setup, QString("player-1-joystick%1-ctrls-type").arg(i));
        saveJoyCtrlMapType(player2_joysticks[i], setup, QString("player-2-joystick%1-ctrls-type").arg(i));
    }
}

void GlobalSettings::resetDefaults()
{
    debugMode       =false;
    interprocessing =false;

    ScreenWidth=800;
    ScreenHeight=600;

    MaxFPS=250;
    TicksPerSecond=65;
    timeOfFrame=15;

    showDebugInfo=false;

    frameSkip=true;

    fullScreen=false;

    volume_sound=128;
    volume_music=64;

    player1_controller=-1;
    player2_controller=-1;

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
}

void GlobalSettings::apply()
{
    PGE_Window::TicksPerSecond=1000.0f/timeOfFrame;
    PGE_Window::MaxFPS=MaxFPS;
    PGE_Window::Width =ScreenWidth;
    PGE_Window::Height=ScreenHeight;
    PGE_Window::showDebugInfo=showDebugInfo;
    PGE_Window::TimeOfFrame=timeOfFrame;
}


void GlobalSettings::loadKeyMap(KeyMap &map, QSettings &set, QString grp)
{
    set.beginGroup(grp);
        map.left.val=set.value("left", map.left.val).toInt();
        map.right.val=set.value("right", map.right.val).toInt();
        map.up.val=set.value("up", map.up.val).toInt();
        map.down.val=set.value("down", map.down.val).toInt();

        map.jump.val=set.value("jump", map.jump.val).toInt();
        map.jump_alt.val=set.value("jump-alt", map.jump_alt.val).toInt();

        map.run.val=set.value("run", map.run.val).toInt();
        map.run_alt.val=set.value("run-alt", map.run_alt.val).toInt();

        map.drop.val=set.value("drop", map.drop.val).toInt();
        map.start.val=set.value("start", map.start.val).toInt();
    set.endGroup();
}

void GlobalSettings::saveKeyMap(KeyMap &map, QSettings &set, QString grp)
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

void GlobalSettings::loadJoyCtrlMapID(KeyMap &map, QSettings &set, QString grp)
{
    set.beginGroup(grp);
        map.left.id=set.value("left", map.left.id).toInt();
        map.right.id=set.value("right", map.right.id).toInt();
        map.up.id=set.value("up", map.up.id).toInt();
        map.down.id=set.value("down", map.down.id).toInt();

        map.jump.id=set.value("jump", map.jump.id).toInt();
        map.jump_alt.id=set.value("jump-alt", map.jump_alt.id).toInt();

        map.run.id=set.value("run", map.run.id).toInt();
        map.run_alt.id=set.value("run-alt", map.run_alt.id).toInt();

        map.drop.id=set.value("drop", map.drop.id).toInt();
        map.start.id=set.value("start", map.start.id).toInt();
    set.endGroup();
}

void GlobalSettings::saveJoyCtrlMapID(KeyMap &map, QSettings &set, QString grp)
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

void GlobalSettings::loadJoyCtrlMapType(KeyMap &map, QSettings &set, QString grp)
{
    set.beginGroup(grp);
        map.left.type=set.value("left", map.left.type).toInt();
        map.right.type=set.value("right", map.right.type).toInt();
        map.up.type=set.value("up", map.up.type).toInt();
        map.down.type=set.value("down", map.down.type).toInt();

        map.jump.type=set.value("jump", map.jump.type).toInt();
        map.jump_alt.type=set.value("jump-alt", map.jump_alt.type).toInt();

        map.run.type=set.value("run", map.run.type).toInt();
        map.run_alt.type=set.value("run-alt", map.run_alt.type).toInt();

        map.drop.type=set.value("drop", map.drop.type).toInt();
        map.start.type=set.value("start", map.start.type).toInt();
    set.endGroup();
}

void GlobalSettings::saveJoyCtrlMapType(KeyMap &map, QSettings &set, QString grp)
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
    Controller *TargetController=NULL;
    if(player==1)
    {
        if(player1_controller>=0) {
            TargetController = new JoystickController();
            int DeviceID = player1_controller;
            if(DeviceID<player1_joysticks.size())
                TargetController->setKeyMap(player1_joysticks[DeviceID]);
            if(DeviceID<joysticks.size())
            dynamic_cast<JoystickController*>(TargetController)->
                    setJoystickDevice(joysticks[DeviceID]);
        } else {
            TargetController = new KeyboardController();
            TargetController->setKeyMap(player1_keyboard);
        }
    } else if (player==2) {
        if(player2_controller>=0) {
            TargetController = new JoystickController();
            int DeviceID = player2_controller;
            if(DeviceID<player2_joysticks.size())
                TargetController->setKeyMap(player2_joysticks[DeviceID]);
            if(DeviceID<joysticks.size())
            dynamic_cast<JoystickController*>(TargetController)->
                    setJoystickDevice(joysticks[DeviceID]);
        } else {
            TargetController = new KeyboardController();
            TargetController->setKeyMap(player2_keyboard);
        }
    }
    return TargetController;
}



