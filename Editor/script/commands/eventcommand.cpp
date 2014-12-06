/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "eventcommand.h"

EventCommand::EventCommand(EventType evType, QObject *parent) :
    BasicCommand(parent)
{
    m_eventType = evType;
}

QString EventCommand::compileSegment(Script::CompilerType compiler, int tabOffset)
{
    QString compiledStr("");

    if(compiler == Script::COMPILER_LUNALUA){
        if(m_eventType == EVENTTYPE_LOAD){
            compiledStr += mkLine(tabOffset) + "function onLoad()\n";
        }else if(m_eventType == EVENTTYPE_LOOP){
            compiledStr += mkLine(tabOffset) + "function onLoop()\n";
        }
        tabOffset++;

        for(int i = 0; i < m_childCmds.size(); ++i){
            compiledStr += m_childCmds[i]->compileSegment(compiler, tabOffset);
        }

        tabOffset--;
        compiledStr += mkLine(tabOffset) + "end\n";
    }
    return compiledStr;
}

bool EventCommand::supportCompiler(Script::CompilerType compiler)
{
    bool supportInternally = false;

    if(compiler == Script::COMPILER_LUNALUA){
        if(m_eventType == EVENTTYPE_LOAD) supportInternally = true;
        if(m_eventType == EVENTTYPE_LOOP) supportInternally = true;
    }else if(compiler == Script::COMPILER_AUTOCODE){
        if(m_eventType == EVENTTYPE_LOAD) supportInternally = true;
        if(m_eventType == EVENTTYPE_LOOP) supportInternally = true;
    }else if(compiler == Script::COMPILER_PGELUA){ /* under constructuion*/
        return false;
    }

    if(supportInternally){
        for(int i = 0; i < m_childCmds.size(); ++i){
            if(!m_childCmds[i]->supportCompiler(compiler)){
                supportInternally = false;
                break;
            }
        }
    }
    return supportInternally;
}


EventCommand::~EventCommand()
{
    qDeleteAll(m_childCmds);
}
EventCommand::EventType EventCommand::eventType() const
{
    return m_eventType;
}

void EventCommand::setEventType(const EventType &eventType)
{
    m_eventType = eventType;
}

void EventCommand::addBasicCommand(BasicCommand *command)
{
    m_childCmds << command;
}

int EventCommand::countCommands()
{
    return m_childCmds.size();
}

void EventCommand::removeBasicCommand(int i)
{
    m_childCmds.removeAt(i);
}

void EventCommand::deleteBasicCommand(BasicCommand *cmd)
{
    if(m_childCmds.removeOne(cmd)){
        delete cmd;
    }
}

QList<BasicCommand *> EventCommand::findCommandsByMarker(const QString &marker)
{
    QList<BasicCommand *> foundItems;
    foreach (BasicCommand* cmd, m_childCmds) {
        if(cmd->marker() == marker)
            foundItems << cmd;
    }
    return foundItems;
}


