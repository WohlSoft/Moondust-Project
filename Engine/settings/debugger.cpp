/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <gui/pge_msgbox.h>
#include <audio/pge_audio.h>

#include <scenes/scene_level.h>

#include <QStringList>
#include <common_features/md5.h>

static inline QString md5hash(QString word)
{
    return QString::fromStdString(md5(word.toStdString()));
}

bool PGE_Debugger::cheat_allowed=true;
bool PGE_Debugger::cheat_pagangod=false;
bool PGE_Debugger::cheat_chucknorris=false;
bool PGE_Debugger::cheat_superman=false;

bool PGE_Debugger::cheat_worldfreedom=false;

static inline void showMsg(Scene *parent, const char*msg)
{
    PGE_MsgBox msgBox(parent, msg,
                      PGE_MsgBox::msg_warn);
    if(!ConfigManager::setup_message_box.sprite.isEmpty())
        msgBox.loadTexture(ConfigManager::setup_message_box.sprite);
    msgBox.exec();
}

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
    if(cheat_allowed && (parent!=NULL))
    {
        ///////////////////////////////////////////////
        ////////////////Common commands////////////////
        ///////////////////////////////////////////////
        QString input = inputBox.inputText().toLower();
        QString hash  = md5hash(input);
        qDebug() << input << hash;

        if(input=="redigitiscool") {
            showMsg(parent, "Redigit is no more cool,\nSorry!\n\n"
                                      "If you wanna a super-secret code, look at SMBX Forums: "
                                      "it's automatically appears when you try to quote old code!");
        } else
        if(input=="wohlstandiscool") {
            showMsg(parent, "You flatter me!");
        } else
        if(input=="kevsoftiscool") {
            showMsg(parent, "Thanks! You got hacked now!");
        } else
        if(input=="raocowiscool") {
            showMsg(parent, "Oh, man! Thanks a lot!");
        } else
        if(input=="joeyiscool") {
            showMsg(parent, "Don't try to guess code, or I'll ban you on SMBX Forums!");
        } else
        if(hash.toLower()=="0f89f93d5e6338d384c5d6bddb69e715") {
            showMsg(parent, "Congratulation!\nYou found a secret code!");
            cheatfound=true;
            cheat_chucknorris=true;
            cheat_superman=true;
            cheat_pagangod=true;
            en=true;
        } else
        if(parent->type()==Scene::Level) {
            ///////////////////////////////////////////////
            ////////////Level specific commands////////////
            ///////////////////////////////////////////////
            if(input=="donthurtme") {
                cheat_pagangod=!cheat_pagangod;
                en=cheat_pagangod;
                cheatfound=true;
            } else if(input=="chucknorris") {
                cheat_chucknorris=!cheat_chucknorris;
                en=cheat_chucknorris;
                cheatfound=true;
            } else if(input=="iamsuperman") {
                cheat_superman=!cheat_superman;
                en=cheat_superman;
                cheatfound=true;
            } else if(input.startsWith("iwishexitas")) {
                QStringList args=input.split(' ');
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
        } else if(parent->type()==Scene::World) {
            ///////////////////////////////////////////////
            //////////World map specific commands//////////
            ///////////////////////////////////////////////
            if(input=="illparkwhereiwant") {
                cheat_worldfreedom=!cheat_worldfreedom;
                en=cheat_worldfreedom;
                cheatfound=true;
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


