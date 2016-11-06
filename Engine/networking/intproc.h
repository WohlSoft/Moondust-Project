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

#include <mutex>
#include <deque>
#include <QObject>
#include "editor_pipe.h"

namespace IntProc
{
    void init();
    void quit();
    bool isEnabled();
    bool isWorking();

    QString      getState();
    void         setState(QString instate);

    enum ExternalCommands
    {
        MsgBox = 0,
        Cheat = 1,
        PlaceItem = 2
    };

    struct cmdEntry
    {
        QString cmd;
        ExternalCommands type;
    };

    bool sendMessage(const char *command);
    bool sendMessageS(std::string command);
    bool sendMessage(QString command);

    void             storeCommand(const char *cmd, size_t length, ExternalCommands type);
    void             cmdLock();
    void             cmdUnLock();
    bool             hasCommand();
    ExternalCommands commandType();
    QString          getCMD();

    extern EditorPipe *editor;
};

#endif // INTPROC_H
