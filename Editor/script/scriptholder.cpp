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

#include "scriptholder.h"
#include "../common_features/logger.h"
#include "command_compiler/lunaluacompiler.h"
#include "command_compiler/autocodecompiler.h"

ScriptHolder::ScriptHolder(QObject *parent) :
    QObject(parent),
    m_usingCompiler(0)
{
    m_usingCompiler = new LunaLuaCompiler(m_events);
}
BasicCompiler *ScriptHolder::usingCompiler() const
{
    return m_usingCompiler;
}

void ScriptHolder::setUsingCompiler(BasicCompiler *usingCompiler)
{
    if(m_usingCompiler)
        delete m_usingCompiler;
    m_usingCompiler = usingCompiler;
}

Script::CompilerType ScriptHolder::usingCompilerType() const
{
    if(qobject_cast<LunaLuaCompiler*>(m_usingCompiler))
        return Script::COMPILER_LUNALUA;
    if(qobject_cast<AutocodeCompiler*>(m_usingCompiler))
        return Script::COMPILER_AUTOCODE;
    return static_cast<Script::CompilerType>(-1);
}

QString ScriptHolder::compileCode()
{
    if(!m_usingCompiler){
        WriteToLog(QtDebugMsg, "Failed to compile: No compiler is set!");
        return QString("");
    }
    m_usingCompiler->setEvents(m_events);
    return m_usingCompiler->compileCode();
}
QList<EventCommand *> ScriptHolder::events() const
{
    return m_events;
}

void ScriptHolder::setEvents(const QList<EventCommand *> &events)
{
    m_events = events;
}

QList<EventCommand *> &ScriptHolder::revents()
{
    return m_events;
}

EventCommand *ScriptHolder::findEvent(EventCommand::EventType evType)
{
    foreach(EventCommand* evCmd, m_events){
        if(evCmd->eventType() == evType){
            return evCmd;
        }
    }
    return NULL;
}

bool ScriptHolder::deleteEvent(EventCommand *evCmd)
{
    if(m_events.removeOne(evCmd)){
        delete evCmd;
        return true;
    }
    return false;
}


ScriptHolder::~ScriptHolder()
{
    if(m_usingCompiler)
        delete m_usingCompiler;
    qDeleteAll(m_events);
}


