/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "intproc.h"
#include <common_features/logger.h>

EditorPipe            *IntProc::editor = nullptr;
static bool            ipc_enabled = false;

static QString         state;
static std::mutex      state_lock;

static IntProc::ExternalCommands        cmd_recentType = IntProc::MsgBox;

static std::deque<IntProc::cmdEntry>    cmd_queue;
static std::mutex                       cmd_mutex;

void IntProc::init()
{
    pLogDebug("IntProc constructing...");
    editor = new EditorPipe();
    editor->m_isWorking = true;
    pLogDebug("IntProc started!");
    ipc_enabled = true;
}

void IntProc::quit()
{
    if(editor != nullptr)
    {
        editor->m_isWorking = false;
        editor->shut();
        delete editor;
    }

    editor  = nullptr;
    ipc_enabled = false;
}

bool IntProc::isEnabled()
{
    return ipc_enabled;
}

bool IntProc::isWorking()
{
    return (editor != nullptr);
}

QString IntProc::getState()
{
    state_lock.lock();
    QString tmp = state;
    state_lock.unlock();
    return tmp;
}

void IntProc::setState(QString instate)
{
    state_lock.lock();
    state = instate;
    state_lock.unlock();
}

void IntProc::storeCommand(const char *cmd, size_t length, IntProc::ExternalCommands type)
{
    cmd_mutex.lock();
    std::string in;
    in.reserve(length);
    bool escape = false;

    for(size_t i = 0; i < length; i++)
    {
        const char &c = cmd[i];

        if(escape)
        {
            if(c == 'n')
                in.push_back('\n');

            escape = false;
        }
        else if(c == '\\')
        {
            escape = true;
            continue;
        }
        else
            in.push_back(c);
    }

    cmd_queue.push_back({QString::fromStdString(in), type});
    cmd_recentType = type;
    cmd_mutex.unlock();
}

void IntProc::cmdLock()
{
    cmd_mutex.lock();
}

void IntProc::cmdUnLock()
{
    cmd_mutex.unlock();
}

bool IntProc::hasCommand()
{
    return !cmd_queue.empty();
}

IntProc::ExternalCommands IntProc::commandType()
{
    return cmd_recentType;
}

QString IntProc::getCMD()
{
    cmdEntry tmp = cmd_queue.front();
    cmd_queue.pop_front();

    if(!cmd_queue.empty())
        cmd_recentType = cmd_queue.front().type;

    return tmp.cmd;
}

bool IntProc::sendMessage(QString command)
{
    if(!editor)
        return false;

    editor->sendMessage(command);
    return true;
}

bool IntProc::sendMessageS(std::string command)
{
    if(!editor)
        return false;

    editor->sendMessage(command);
    return true;
}

bool IntProc::sendMessage(const char *command)
{
    if(!editor)
        return false;

    editor->sendMessage(command);
    return true;
}
