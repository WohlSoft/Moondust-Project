/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

EditorPipe * IntProc::editor=NULL;
bool IntProc::enabled=false;
QString IntProc::state="";

bool IntProc::cmd_accepted=false;
IntProc::ExternalCommands IntProc::command=IntProc::MsgBox;

QString IntProc::cmd="";

IntProc::IntProc(QObject *parent) :
    QObject(parent)
{
    editor = NULL;
}

void IntProc::init()
{
    qDebug()<<"IntProc constructing...";
    editor = new EditorPipe();
    editor->isWorking=true;
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

bool IntProc::hasCommand()
{
    return cmd_accepted;
}

IntProc::ExternalCommands IntProc::commandType()
{
    return command;
}

QString IntProc::getCMD()
{
    cmd_accepted=false;
    QString tmp=cmd;
    cmd.clear();
    return tmp;
}

bool IntProc::isEnabled()
{
    return enabled;
}
