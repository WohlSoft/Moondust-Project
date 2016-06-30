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

#ifndef PGE_JS_INI_H
#define PGE_JS_INI_H

#include <QObject>
#include <QJSValue>

class QSettings;
class PGE_JS_INIFile : public QObject
{
    Q_OBJECT

    QSettings* m_ini;
public:
    explicit PGE_JS_INIFile(QObject *parent = NULL);
    PGE_JS_INIFile(QString filePath, QObject *parent = NULL);
    PGE_JS_INIFile(const PGE_JS_INIFile& inif);
    PGE_JS_INIFile & operator=(const PGE_JS_INIFile& inif);

    virtual ~PGE_JS_INIFile();

    Q_INVOKABLE void open(QString filePath);
    Q_INVOKABLE void close();
    Q_INVOKABLE void beginGroup(QString group);
    Q_INVOKABLE void endGroup();
    Q_INVOKABLE QJSValue value(QString name, QJSValue defValue);
    Q_INVOKABLE void setValue(QString name, QJSValue value);
};


class PGE_JS_INI : public QObject
{
    Q_OBJECT

public:
    explicit PGE_JS_INI(QObject *parent = NULL);
    virtual ~PGE_JS_INI();

    Q_INVOKABLE PGE_JS_INIFile *open(QString file);
    Q_INVOKABLE PGE_JS_INIFile *make();
};

#endif // PGE_JS_INI_H
