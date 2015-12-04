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

#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <controls/controller_key_map.h>
#include <QList>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

class QSettings;
class Controller;

/*!
 * \brief Global engine application settings class
 */
class GlobalSettings
{
public:
    GlobalSettings();
    ~GlobalSettings();
    /*!
     * \brief Initialize all plugged joystics which are can be used in game
     */
    void initJoysticks();

    /*!
     * \brief Deinitialize all initialized joysticks
     */
    void closeJoysticks();

    /*!
     * \brief Load settings from a config file
     */
    void load();

    /*!
     * \brief Save settings into config file
     */
    void save();

    /*!
     * \brief Set all settings to default state
     */
    void resetDefaults();

    /*!
     * \brief Applies all changed preferences
     */
    void apply();

/*Via command line only*/
    //! Enable debug mode of engine (Allow any cheats and allow showing of any debug information)
    bool debugMode;
    //! Enable interprocessing mode (Engine will try to find running editor and will ask it for opened file data to play it)
    bool interprocessing;
/*Via command line only. End*/

    //! Enable full-screen mode
    bool fullScreen;

    //! Current width of screen
    int  ScreenWidth;
    //! Current height of screen
    int  ScreenHeight;

    //! How much loop steps will be done per second
    float  TicksPerSecond;
    //! Time of one loop step
    int    timeOfFrame;
    //! Enable vertical synchronization with monitor refresh rate (not all video cards are supports this feature)
    bool   vsync;

    //! Enable printing of engine debug information
    bool showDebugInfo;

    //! Enable skipping of frames if real time delay is longer than predefined
    bool frameSkip;

    //! Current volume of SFX-es (0...128)
    int volume_sound;
    //! Current volume of music (0...128)
    int volume_music;

    //! Type of controller used by first player (-1 keyboard, >=0 joystick ID)
    int player1_controller;
    //! Keyboard controlls map for a first player
    KeyMap player1_keyboard;
    //! Control maps per each joystick ID for a first player
    QList<KeyMap>         player1_joysticks;
    //! Joystick control ID maps per each joystick ID for a first player
    QList<KeyMapJoyCtrls> player1_joysticks_ctrls_ids;
    //! Joystick control types (button, axis, hat, ball) maps per each joystick ID for a first player
    QList<KeyMapJoyCtrls> player1_joysticks_ctrls_types;

    //! Type of controller used by second player (-1 keyboard, >=0 joystick ID)
    int player2_controller;
    //! Keyboard controlls map for a second player
    KeyMap player2_keyboard;
    //! Control maps per each joystick ID for a second player
    QList<KeyMap>         player2_joysticks;
    //! Joystick control ID maps per each joystick ID for a second player
    QList<KeyMapJoyCtrls> player2_joysticks_ctrls_ids;
    //! Joystick control types (button, axis, hat, ball) maps per each joystick ID for a second player
    QList<KeyMapJoyCtrls> player2_joysticks_ctrls_types;

    //! List of all available joysticks
    QList<SDL_Joystick*> joysticks;

    /*!
     * \brief Load settings to specific control key map through opened config file inside specified INI-group
     * \param map Keyboard control map structure which contains key ID's per each control key
     * \param set Opened engine configuration file
     * \param grp INI-group which contains saved control keys map
     */
    void loadKeyMap(KeyMap &map, QSettings &set, QString grp);

    /*!
     * \brief Save settings from specific control keys map into opened config file into specified INI-group
     * \param map Keyboard control map structure which contains key ID's per each control key
     * \param set Opened engine configuration file
     * \param grp INI-group which contains saved control keys map
     */
    void saveKeyMap(KeyMap &map, QSettings &set, QString grp);

    /*!
     * \brief Load settings to specific joysticks control map through opened config file inside specified INI-group
     * \param map Josytick control map structure which contains key ID's per each control key
     * \param set Opened engine configuration file
     * \param grp INI-group which contains saved control keys map
     */
    void loadJoyCtrlMapID(KeyMap &map, QSettings &set, QString grp);

    /*!
     * \brief Save settings from specific joysticks control map into opened config file inside specified INI-group
     * \param map Josytick control map structure which contains key ID's per each control key
     * \param set Opened engine configuration file
     * \param grp INI-group which contains saved control keys map
     */
    void saveJoyCtrlMapID(KeyMap &map, QSettings &set, QString grp);

    /*!
     * \brief Load settings to specific joysticks control types map through opened config file inside specified INI-group
     * \param map Josytick control types map structure which contains key ID's per each control key
     * \param set Opened engine configuration file
     * \param grp INI-group which contains saved control keys map
     */
    void loadJoyCtrlMapType(KeyMap &map, QSettings &set, QString grp);

    /*!
     * \brief Save settings from specific joysticks control types map into opened config file inside specified INI-group
     * \param map Josytick control types map structure which contains key ID's per each control key
     * \param set Opened engine configuration file
     * \param grp INI-group which contains saved control keys map
     */
    void saveJoyCtrlMapType(KeyMap &map, QSettings &set, QString grp);

    /*!
     * \brief Constructs controller class for specific player and return a pointer to it
     * \param player Number of player
     * \return Pointer to constructed controller class (When you fininshed your works with it, delete it youself!)
     */
    Controller *openController(int player);

    /*!
     * \brief Load all joystick control keys maps from engine configuration file
     */
    void loadJoystickSettings();
};

//! Engine application settings container
extern GlobalSettings g_AppSettings;

#endif // GLOBALSETTINGS_H
