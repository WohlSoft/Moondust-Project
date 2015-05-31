#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <controls/controller_key_map.h>
#include <QList>

#include <SDL2/SDL.h>
#undef main

class QSettings;
class Controller;
class GlobalSettings
{
public:
    GlobalSettings();
    ~GlobalSettings();
    void initJoysticks();
    void closeJoysticks();
    void load();
    void save();
    void resetDefaults();
    void apply();

    /*Via command line only*/
    bool debugMode;
    bool interprocessing;
    /*Via command line only. End*/

    bool fullScreen;

    int  ScreenWidth;
    int  ScreenHeight;

    int  MaxFPS;
    int  PhysStep;

    bool showDebugInfo;
    bool testJoystickController;

    bool enableDummyNpc;

    int volume_sound;
    int volume_music;

    int player1_controller;//!< -1 - keyboard, >=0 - joysticks
    KeyMap player1_keyboard;
    QList<KeyMap>         player1_joysticks;
    QList<KeyMapJoyCtrls> player1_joysticks_ctrls_ids;
    QList<KeyMapJoyCtrls> player1_joysticks_ctrls_types;

    int player2_controller;//!< -1 - keyboard, >=0 - joysticks
    KeyMap player2_keyboard;
    QList<KeyMap>         player2_joysticks;
    QList<KeyMapJoyCtrls> player2_joysticks_ctrls_ids;
    QList<KeyMapJoyCtrls> player2_joysticks_ctrls_types;

    QList<SDL_Joystick*> joysticks;

    void loadKeyMap(KeyMap &map, QSettings &set, QString grp);
    void saveKeyMap(KeyMap &map, QSettings &set, QString grp);
    void loadJoyCtrlMapID(KeyMap &map, QSettings &set, QString grp);
    void saveJoyCtrlMapID(KeyMap &map, QSettings &set, QString grp);
    void loadJoyCtrlMapType(KeyMap &map, QSettings &set, QString grp);
    void saveJoyCtrlMapType(KeyMap &map, QSettings &set, QString grp);
    Controller *openController(int player);
    void loadJoystickSettings();
};

extern GlobalSettings AppSettings;

#endif // GLOBALSETTINGS_H
