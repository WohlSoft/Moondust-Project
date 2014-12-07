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

#include "memorycommand.h"

MemoryCommand::MemoryCommand(int hexVal, MemoryCommand::FieldType ftype, double value, QObject *parent) :
    BasicCommand(parent)
{
    m_hexValue = hexVal;
    m_fieldType = ftype;
    m_value = value;
}

QString MemoryCommand::compileSegment(Script::CompilerType compiler, int tabOffset)
{
    QString compiledStr("");
    if(compiler == Script::COMPILER_LUNALUA){
        compiledStr += mkLine(tabOffset) + "mem(" + "0x" + QString::number(hexValue(), 16).toUpper() + "," + fieldTypeToString(compiler) + "," + valueToString() + ")\n";
    }else if(compiler == Script::COMPILER_AUTOCODE){
        compiledStr += mkLine(tabOffset) + "MemAssign," + "0x"+ QString::number(hexValue(), 16).toUpper() + "," + valueToString() + ",0,0,0," + fieldTypeToString(compiler)+"\n";
    }
    return compiledStr;
}

bool MemoryCommand::supportCompiler(Script::CompilerType compiler)
{
    return compiler == Script::COMPILER_AUTOCODE || compiler == Script::COMPILER_LUNALUA;
}

MemoryCommand::~MemoryCommand()
{}
int MemoryCommand::hexValue() const
{
    return m_hexValue;
}

void MemoryCommand::setHexValue(int hexValue)
{
    m_hexValue = hexValue;
}
MemoryCommand::FieldType MemoryCommand::fieldType() const
{
    return m_fieldType;
}

void MemoryCommand::setFieldType(const FieldType &fieldType)
{
    m_fieldType = fieldType;
}
double MemoryCommand::getValue() const
{
    return m_value;
}

void MemoryCommand::setValue(double value)
{
    m_value = value;
}


QString MemoryCommand::fieldTypeToString(Script::CompilerType ct)
{
    if(ct == Script::COMPILER_LUNALUA){
        if(m_fieldType == FIELD_BYTE) return "FIELD_BYTE";
        if(m_fieldType == FIELD_WORD) return "FIELD_WORD";
        if(m_fieldType == FIELD_DWORD) return "FIELD_DWORD";
        if(m_fieldType == FIELD_FLOAT) return "FIELD_FLOAT";
        if(m_fieldType == FIELD_DFLOAT) return "FIELD_DFLOAT";
    }else if(ct == Script::COMPILER_AUTOCODE){
        if(m_fieldType == FIELD_BYTE) return "b";
        if(m_fieldType == FIELD_WORD) return "w";
        if(m_fieldType == FIELD_DWORD) return "dw";
        if(m_fieldType == FIELD_FLOAT) return "f";
        if(m_fieldType == FIELD_DFLOAT) return "df";
    }

    return "";
}

QString MemoryCommand::valueToString()
{
    if(m_fieldType == FIELD_BYTE ||
            m_fieldType == FIELD_WORD ||
            m_fieldType == FIELD_DWORD) return QString::number((int)m_value);

    if(m_fieldType == FIELD_FLOAT ||
            m_fieldType == FIELD_DFLOAT) return QString::number(m_value);
    return "";
}



