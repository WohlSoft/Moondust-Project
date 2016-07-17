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
#include <QMutex>
#include <windows.h>
#include <PGE_File_Formats/lvl_filedata.h>
class QMenu;
class QAction;
class MainWindow;
#else
#include <QObject>
#endif

class LunaTester : public QObject
{
    Q_OBJECT
public:
#ifdef Q_OS_WIN
    LunaTester();
    ~LunaTester();
    MainWindow* m_mw;
    /**
     * @brief Initialize menu of the LunaTester
     * @param mw pointer to the Main Window
     * @param mainmenu Menu where insert LunaTester
     * @param insert_before Action where is need to insert LunaTester menu
     */
    void initLunaMenu(MainWindow *mw, QMenu* mainmenu, QAction* insert_before, QAction *defaultTestAction);
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
    //! Don't run same function multiple times
    QMutex              m_engine_mutex;
public slots:
    void startLunaTester();
    void resetCheckPoints();
    void killFrozenThread();
private:
    void _RunSmbxTestHelper(MainWindow *mw, LevelData in_levelData, QString levelPath, bool isUntitled);
#endif
};
#endif // LUNA_TESTER_H
