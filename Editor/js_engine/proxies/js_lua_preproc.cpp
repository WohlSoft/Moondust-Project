/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "js_lua_preproc.h"

PGE_JS_LuaPreprocessor::PGE_JS_LuaPreprocessor(QObject *parent) :
    QObject(parent)
{}

PGE_JS_LuaPreprocessor::PGE_JS_LuaPreprocessor(QString luaFile, QStringList macros, QObject *parent):
    QObject(parent),
    m_luaFile(luaFile),
    m_macros(macros)
{}

PGE_JS_LuaPreprocessor::PGE_JS_LuaPreprocessor(const PGE_JS_LuaPreprocessor &lp):
    QObject(lp.parent()),
    m_luaFile(lp.m_luaFile),
    m_macros(lp.m_macros)
{}

PGE_JS_LuaPreprocessor::~PGE_JS_LuaPreprocessor()
{}
