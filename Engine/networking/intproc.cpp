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
#include <QtDebug>

EditorPipe *    IntProc::editor  = NULL;
bool            IntProc::enabled = false;

QString         IntProc::state   = "";
std::mutex      IntProc::state_lock;

IntProc::ExternalCommands   IntProc::command = IntProc::MsgBox;

std::deque<QString> IntProc::cmd_queue;
std::mutex          IntProc::cmd_mutex;

IntProc::IntProc(QObject *parent) :
    QObject(parent)
{
    editor = NULL;
}

void IntProc::init()
{
    qDebug()<<"IntProc constructing...";
    editor = new EditorPipe();
    editor->isWorking = true;

    qDebug()<<"IntProc started!";
    enabled=true;
}


void IntProc::quit()
{
    if(editor!=NULL)
    {
        editor->isWorking=false;
        editor->shut();
        delete editor;
        editor = NULL;
        enabled=false;
    }
}

bool IntProc::isWorking()
{
    return (editor!=NULL);
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

void IntProc::storeCommand(QString in, IntProc::ExternalCommands type)
{
    cmd_mutex.lock();
    cmd_queue.push_back(in.replace("\\n", "\n"));
    command = type;
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
    return command;
}

QString IntProc::getCMD()
{
    QString tmp = cmd_queue.front();
    cmd_queue.pop_front();
    return tmp;
}

bool IntProc::isEnabled()
{
    return enabled;
}
