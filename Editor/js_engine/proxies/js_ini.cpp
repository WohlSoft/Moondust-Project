/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "js_ini.h"
#include "js_utils.h"
#include <QSettings>
#include <QJSEngine>

PGE_JS_INI::PGE_JS_INI(QObject *parent)
    : QObject(parent)
{}

PGE_JS_INI::~PGE_JS_INI()
{}

QJSValue PGE_JS_INI::open(QString file)
{
    return genQObjectWrapper<PGE_JS_INIFile>(this, file, this);
}

QJSValue PGE_JS_INI::make()
{
    return genQObjectWrapper<PGE_JS_INIFile>(this, this);
}







PGE_JS_INIFile::PGE_JS_INIFile(QObject *parent) :
    QObject(parent), m_ini(NULL)
{}

PGE_JS_INIFile::PGE_JS_INIFile(QString filePath, QObject *parent) :
    QObject(parent), m_ini(NULL)
{
    open(filePath);
}

PGE_JS_INIFile::PGE_JS_INIFile(const PGE_JS_INIFile &inif) :
    QObject(inif.parent()), m_ini(NULL)
{
    if( inif.m_ini )
        open( inif.m_ini->fileName() );
}

PGE_JS_INIFile &PGE_JS_INIFile::operator=(const PGE_JS_INIFile &inif)
{
    setParent(inif.parent());
    m_ini = NULL;
    if( inif.m_ini )
        open( inif.m_ini->fileName() );
    return *this;
}

PGE_JS_INIFile::~PGE_JS_INIFile()
{
    close();
}

void PGE_JS_INIFile::open(QString filePath)
{
    close();
    m_ini     = new QSettings(filePath, QSettings::IniFormat, this);
}

void PGE_JS_INIFile::close()
{
    if(m_ini)
    {
        delete m_ini;
    }
    m_ini = NULL;
}

void PGE_JS_INIFile::beginGroup(QString group)
{
    if(m_ini)
    {
        m_ini->beginGroup(group);
    }
}

void PGE_JS_INIFile::endGroup()
{
    if(m_ini)
    {
        m_ini->endGroup();
    }
}

QJSValue PGE_JS_INIFile::value(QString name, QJSValue defValue)
{
    if(m_ini)
    {
        QVariant value = m_ini->value(name, defValue.toVariant());
        switch(value.type())
        {
        case QVariant::Int:     return QJSValue(value.toInt());
        case QVariant::UInt:    return QJSValue(value.toUInt());
        case QVariant::Double:  return QJSValue(value.toDouble());
        case QVariant::String:  return QJSValue(value.toString());
        default: return QJSValue();
        }
    }
    return QJSValue();
}

void PGE_JS_INIFile::setValue(QString name, QJSValue value)
{
    if(m_ini)
    {
        m_ini->setValue(name, value.toVariant());
    }
}
