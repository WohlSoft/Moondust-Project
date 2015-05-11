#include "global_settings.h"
#include <common_features/app_path.h>
#include <graphics/window.h>


#include <QSettings>

GlobalSettings AppSettings;

GlobalSettings::GlobalSettings() : joysticks(0)
{
    resetDefaults();
}

GlobalSettings::~GlobalSettings()
{

}

void GlobalSettings::load()
{
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.beginGroup("Main");
    MaxFPS=setup.value("max-fps", MaxFPS).toUInt();
        if(MaxFPS<65) MaxFPS=65;
    PhysStep=setup.value("phys-step", PhysStep).toUInt();
        if(PhysStep<65) PhysStep=65;
    showDebugInfo=setup.value("show-debug-info", showDebugInfo).toBool();

    enableDummyNpc=setup.value("enable-dummy-npc", enableDummyNpc).toBool();
    player1_controller=setup.value("player1-controller", player1_controller).toInt();
        if((player1_controller<0)||(player1_controller>SDL_NumJoysticks())) player1_controller=0;

    player2_controller=setup.value("player2-controller", player2_controller).toInt();
        if((player2_controller<0)||(player2_controller>SDL_NumJoysticks())) player2_controller=0;

    volume_sound=setup.value("volume-sfx", 128).toInt();
        if((volume_sound<0)||(volume_sound>128)) volume_sound=128;
    volume_music=setup.value("volume-mus", 64).toInt();
        if((volume_music<0)||(volume_music>128)) volume_music=128;

    setup.endGroup();
    loadKeyMap(player1_keyboard, setup, "player-1-keyboard");
    loadKeyMap(player2_keyboard, setup, "player-2-keyboard");

    player1_joysticks.clear();
    player2_joysticks.clear();
    for(int i=0; i<SDL_NumJoysticks();i++)
    {
        KeyMap joy1;
        loadKeyMap(joy1, setup, QString("player-1-joystick%1").arg(i));
        player1_joysticks.push_back(joy1);
        KeyMap joy2;
        loadKeyMap(joy2, setup, QString("player-2-joystick%1").arg(i));
        player1_joysticks.push_back(joy2);
    }
}

void GlobalSettings::save()
{
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.beginGroup("Main");
        setup.setValue("max-fps", MaxFPS);
        setup.setValue("phys-step", PhysStep);
        setup.setValue("show-debug-info", showDebugInfo);
        setup.setValue("enable-dummy-npc", enableDummyNpc);
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
    }
}

void GlobalSettings::resetDefaults()
{
    debugMode       =false;
    interprocessing =false;

    ScreenWidth=800;
    ScreenHeight=600;

    MaxFPS=250;
    PhysStep=65;

    showDebugInfo=false;

    enableDummyNpc=false;

    volume_sound=128;
    volume_music=64;

    testJoystickController=true;

    player1_controller=0;
    player2_controller=0;

    player1_keyboard.jump       = SDL_SCANCODE_Z;
    player1_keyboard.jump_alt   = SDL_SCANCODE_A;
    player1_keyboard.run        = SDL_SCANCODE_X;
    player1_keyboard.run_alt    = SDL_SCANCODE_S;
    player1_keyboard.drop       = SDL_SCANCODE_LSHIFT;
    player1_keyboard.start      = SDL_SCANCODE_RETURN;
    player1_keyboard.left       = SDL_SCANCODE_LEFT;
    player1_keyboard.right      = SDL_SCANCODE_RIGHT;
    player1_keyboard.up         = SDL_SCANCODE_UP;
    player1_keyboard.down       = SDL_SCANCODE_DOWN;
}

void GlobalSettings::apply()
{
    PGE_Window::PhysStep=PhysStep;
    PGE_Window::MaxFPS=MaxFPS;
    PGE_Window::Width =ScreenWidth;
    PGE_Window::Height=ScreenHeight;
    PGE_Window::showDebugInfo=showDebugInfo;
}


void GlobalSettings::loadKeyMap(KeyMap &map, QSettings &set, QString grp)
{
    set.beginGroup(grp);
        map.left=set.value("left", map.left).toInt();
        map.right=set.value("right", map.right).toInt();
        map.up=set.value("up", map.up).toInt();
        map.down=set.value("down", map.down).toInt();

        map.jump=set.value("jump", map.jump).toInt();
        map.jump_alt=set.value("jump-alt", map.jump_alt).toInt();

        map.run=set.value("run", map.run).toInt();
        map.run_alt=set.value("run-alt", map.run_alt).toInt();

        map.drop=set.value("drop", map.drop).toInt();
        map.start=set.value("start", map.start).toInt();
    set.endGroup();
}

void GlobalSettings::saveKeyMap(KeyMap &map, QSettings &set, QString grp)
{
    set.beginGroup(grp);
        set.setValue("left", map.left);
        set.setValue("right", map.right);
        set.setValue("up", map.up);
        set.setValue("down", map.down);

        set.setValue("jump", map.jump);
        set.setValue("jump-alt", map.jump_alt);

        set.setValue("run", map.run);
        set.setValue("run-alt", map.run_alt);

        set.setValue("drop", map.drop);
        set.setValue("start", map.start);
    set.endGroup();
}

