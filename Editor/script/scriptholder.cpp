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

ScriptHolder::ScriptHolder(QObject *parent) :
    QObject(parent),
    m_usingCompiler(0)
{
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

QString ScriptHolder::compileCode()
{
    if(!m_usingCompiler){
        WriteToLog(QtDebugMsg, "Failed to compile: No compiler is set!");
        return QString("");
    }
    return m_usingCompiler->compileCode();
}

ScriptHolder::~ScriptHolder()
{}


