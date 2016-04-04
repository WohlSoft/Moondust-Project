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

#ifndef INTPROC_H
#define INTPROC_H

#include <QObject>
#include "editor_pipe.h"

class IntProc : public QObject
{
    Q_OBJECT
public:
    explicit IntProc(QObject *parent = 0);
    static void init();
    static void quit();
    static bool isWorking();
    static QString state;
    enum ExternalCommands
    {
        MsgBox=0,
        Cheat=1,
        PlaceItem=2
    };
    static ExternalCommands command;

    static    bool cmd_accepted;
    static bool hasCommand();
    static ExternalCommands  commandType();
    static QString getCMD();
    static QString cmd;

    static EditorPipe * editor;
    static bool enabled;
    static bool isEnabled();

signals:

public slots:

};

#endif // INTPROC_H
