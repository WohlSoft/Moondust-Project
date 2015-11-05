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

#include "debugger.h"
#include <data_configs/config_manager.h>
#include <gui/pge_textinputbox.h>
#include <audio/pge_audio.h>

#include <scenes/scene_level.h>

#include <QStringList>

bool PGE_Debugger::cheat_allowed=true;
bool PGE_Debugger::cheat_pagangod=false;
bool PGE_Debugger::cheat_chucknorris=false;
bool PGE_Debugger::cheat_superman=false;

void PGE_Debugger::executeCommand(Scene *parent)
{
    PGE_TextInputBox inputBox(parent, "Please type message:", PGE_TextInputBox::msg_info, PGE_Point(-1,-1),
                              ConfigManager::setup_message_box.box_padding,
                              ConfigManager::setup_message_box.sprite);
    inputBox.exec();

    if(inputBox.inputText()=="") return;

    bool cheatfound=false;
    bool en=false;
    /*Special commands part*/

    /*Cheat codes part (must be at bottom!)*/
    if(cheat_allowed)
    {
        if(inputBox.inputText()=="donthurtme") {
            cheat_pagangod=!cheat_pagangod;
            en=cheat_pagangod;
            cheatfound=true;
        } else if(inputBox.inputText()=="chucknorris") {
            cheat_chucknorris=!cheat_chucknorris;
            en=cheat_chucknorris;
            cheatfound=true;
        } else if(inputBox.inputText()=="iamsuperman") {
            cheat_superman=!cheat_superman;
            en=cheat_superman;
            cheatfound=true;
        } else if(parent && (inputBox.inputText().startsWith("iwishexitas")) && (parent->type()==Scene::Level)) {
            QStringList args=inputBox.inputText().split(' ');
            if(args.size()==2)
            {
                bool ok=false;
                int exitcode=args[1].toInt(&ok);
                if(ok)
                {
                    LevelScene *s = static_cast<LevelScene *>(parent);
                    if(s) { s->setExiting(1500, exitcode);
                    en=true;
                    cheatfound=true; }
                }
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
    cheat_pagangod=false;
    cheat_chucknorris=false;
    cheat_superman=false;
}

void PGE_Debugger::setRestriction(bool denyed)
{
    cheat_allowed=denyed;
}


