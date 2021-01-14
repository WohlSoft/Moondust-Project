/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef PGE_JS_COMMON_H
#define PGE_JS_COMMON_H

#include "js_file.h"
#include <QObject>

class PGE_JS_Common : public QObject
{
    Q_OBJECT
public:
    explicit PGE_JS_Common(QObject *parent = nullptr);
    virtual ~PGE_JS_Common();

    Q_INVOKABLE int msgBoxInfo(QString title, QString message);
    Q_INVOKABLE int msgBoxWarning(QString title, QString message);
    Q_INVOKABLE int msgBoxError(QString title, QString message);
};

#endif // PGE_JS_COMMON_H
