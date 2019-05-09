/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "debugger.h"
#include <common_features/logger.h>
#include <data_configs/config_manager.h>
#include <gui/pge_textinputbox.h>
#include <gui/pge_msgbox.h>
#include <audio/pge_audio.h>
#include <algorithm>
#include <vector>

#ifdef PGE_ENGINE_DEBUG

#ifdef __gnu_linux__
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#endif//PGE_ENGINE_DEBUG

#include <scenes/scene.h>
#include <scenes/scene_level.h>
#include <common_features/md5.h>

static inline std::string strToLow(std::string data)
{
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    return data;
}

bool PGE_Debugger::cheat_debugkeys = false;
bool PGE_Debugger::cheat_allowed = true;
bool PGE_Debugger::cheat_pagangod = false;
bool PGE_Debugger::cheat_chucknorris = false;
bool PGE_Debugger::cheat_superman = false;

bool PGE_Debugger::cheat_worldfreedom = false;

static inline void showMsg(Scene *parent, const char *msg)
{
    PGE_MsgBox msgBox(parent, msg, PGE_MsgBox::msg_warn);

    if(!ConfigManager::setup_message_box.sprite.empty())
        msgBox.loadTexture(ConfigManager::setup_message_box.sprite);

    msgBox.exec();
}

#ifdef PGE_ENGINE_DEBUG
int PGE_Debugger::isDebuggerPresent()
{
    #ifdef __gnu_linux__
    char buf[1024];
    int debugger_present = 0;

    int status_fd = open("/proc/self/status", O_RDONLY);
    if(status_fd == -1)
        return 0;

    ssize_t num_read = read(status_fd, buf, sizeof(buf));

    if(num_read > 0)
    {
        static const char TracerPid[] = "TracerPid:";
        char *tracer_pid;

        buf[num_read] = 0;
        tracer_pid    = strstr(buf, TracerPid);
        if(tracer_pid)
            debugger_present = static_cast<bool>(SDL_atoi(tracer_pid + sizeof(TracerPid) - 1));
    }
    return debugger_present;
    #endif


    #ifdef _WIN32
    return IsDebuggerPresent();
    #endif
}
#endif

void PGE_Debugger::executeCommand(Scene *parent)
{
    PGE_TextInputBox inputBox(parent, "Please type message:", PGE_TextInputBox::msg_info, PGE_Point(-1, -1),
                              ConfigManager::setup_message_box.box_padding,
                              ConfigManager::setup_message_box.sprite);
    inputBox.exec();

    if(inputBox.inputText().empty())
        return;

    bool cheatfound = false;
    bool en = false;
    /*Special commands part*/

    /*Cheat codes part (must be at bottom!)*/
    if(cheat_allowed && (parent != nullptr))
    {
        ///////////////////////////////////////////////
        ////////////////Common commands////////////////
        ///////////////////////////////////////////////
        std::string input = strToLow(inputBox.inputText());
        std::string hash  = md5(input);
        D_pLogDebug("%s -> %s", input.c_str(), hash.c_str());

        if(input == "redigitiscool")
        {
            showMsg(parent, "Redigit is no more cool,\nSorry!\n\n"
                    "If you wanna a super-secret code, look at SMBX Forums: "
                    "it's automatically appears when you try to quote old code!");
        }
        else if(input == "wohlstandiscool")
            showMsg(parent, "You flatter me!");
        else if(input == "kevsoftiscool")
            showMsg(parent, "Thanks! You got hacked now!");
        else if(input == "raocowiscool")
            showMsg(parent, "Oh, man! Thanks a lot!");
        else if(input == "joeyiscool")
            showMsg(parent, "Don't try to guess code, or I'll ban you on SMBX Forums!");
        else if(strToLow(hash) == "0f89f93d5e6338d384c5d6bddb69e715")
        {
            pLogCritical("What the heck you typed this silly command?!   Joke! You found an... EASTER EGG!");
            showMsg(parent, "Congratulation!\nYou found a secret code!");
            cheatfound = true;
            cheat_chucknorris = true;
            cheat_superman = true;
            cheat_pagangod = true;
            en = true;
        }
        else if(parent->type() == Scene::Level)
        {
            ///////////////////////////////////////////////
            ////////////Level specific commands////////////
            ///////////////////////////////////////////////
            if(input == "takesecretkeychain")
            {
                cheat_debugkeys = !cheat_debugkeys;
                en = cheat_debugkeys;
                cheatfound = true;
            }
            else if(input == "donthurtme")
            {
                cheat_pagangod = !cheat_pagangod;
                en = cheat_pagangod;
                cheatfound = true;
            }
            else if(input == "chucknorris")
            {
                cheat_chucknorris = !cheat_chucknorris;
                en = cheat_chucknorris;
                cheatfound = true;
            }
            else if(input == "iamsuperman")
            {
                cheat_superman = !cheat_superman;
                en = cheat_superman;
                cheatfound = true;
            }
            else if(input.compare(0, 11, "iwishexitas") == 0)
            {
                std::string::size_type dim = input.find(' ');
                if(dim != std::string::npos)
                {
                    try
                    {
                        int exitcode = SDL_atoi(input.substr(dim + 1, input.size() - (dim + 1)).c_str());
                        auto *s = dynamic_cast<LevelScene *>(parent);
                        if(s)
                        {
                            s->setExiting(1500, exitcode);
                            en = true;
                            cheatfound = true;
                        }
                    } catch (...) {}
                }
            }
        }
        else if(parent->type() == Scene::World)
        {
            ///////////////////////////////////////////////
            //////////World map specific commands//////////
            ///////////////////////////////////////////////
            if(input == "illparkwhereiwant")
            {
                cheat_worldfreedom = !cheat_worldfreedom;
                en = cheat_worldfreedom;
                cheatfound = true;
            }
        }
    }

    if(cheatfound)
    {
        if(en)
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerGrow);
        else
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerShrink);
    }
}

void PGE_Debugger::resetEverything()
{
    cheat_pagangod = false;
    cheat_chucknorris = false;
    cheat_superman = false;
}

void PGE_Debugger::setRestriction(bool denyed)
{
    cheat_allowed = denyed;
}
