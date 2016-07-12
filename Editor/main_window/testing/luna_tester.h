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

#ifndef LUNA_TESTER_H
#define LUNA_TESTER_H

#ifdef _WIN32
#include <QFuture>
#include <windows.h>

class LunaTester
{
public:
    //! LunaLoader process information
    PROCESS_INFORMATION m_pi;
    //! LunaLUA IPC Out pipe
    HANDLE              m_ipc_pipe_out;
    HANDLE              m_ipc_pipe_out_i;
    //! LunaLUA IPC In pipe
    HANDLE              m_ipc_pipe_in;
    HANDLE              m_ipc_pipe_in_o;
    //! Helper which protects from editor freezing
    QFuture<void>       m_helper;
    //! Ranner thread
    QThread*            m_helperThread;
};

#endif

#endif // LUNA_TESTER_H
