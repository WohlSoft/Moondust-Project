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

#ifndef MEMORYCOMMAND_H
#define MEMORYCOMMAND_H

#include "basiccommand.h"

class MemoryCommand : public BasicCommand
{
    Q_OBJECT
public:
    enum FieldType{
        FIELD_BYTE = 1,
        FIELD_WORD,
        FIELD_DWORD,
        FIELD_FLOAT,
        FIELD_DFLOAT
    };

    explicit MemoryCommand(int hexVal, FieldType ftype, double value, QObject *parent = 0);
    virtual QString compileSegment(Script::CompilerType compiler, int tabOffset);
    virtual bool supportCompiler(Script::CompilerType compiler);

    ~MemoryCommand();

    int hexValue() const;
    void setHexValue(int hexValue);

    FieldType fieldType() const;
    void setFieldType(const FieldType &fieldType);

    double getValue() const;
    void setValue(double value);

signals:

public slots:

private:
    int m_hexValue;
    double m_value;
    FieldType m_fieldType;

    QString fieldTypeToString();
    QString valueToString();
};

#endif // MEMORYCOMMAND_H
