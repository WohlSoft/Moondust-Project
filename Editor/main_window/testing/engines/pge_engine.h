/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PGEENGINE_H
#define PGEENGINE_H

#include <QProcess>
#include <QMutex>
#include <networking/engine_intproc.h>

#include "abstract_engine.h"

class MainWindow;

class PgeEngine : public AbstractRuntimeEngine
{
    Q_OBJECT

    //! Engine application handler
    QProcess m_engineProc;
    //! Mutex which helps to avoid multiple launches of engine
    QMutex   m_engineMutex;

    MainWindow *m_w = nullptr;
    //! List of registered menu items
    QAction *m_menuItems[7];

    IntEngine interface;
private slots:
    void retranslateMenu();

    /********Menu items*******/
    void startTestAction();
    void startSafeTestAction();
    void startGameAction();

public:
    explicit PgeEngine(QObject *parent = nullptr);
    ~PgeEngine();

    virtual void init();
    virtual void unInit();

    virtual void initMenu(QMenu *destmenu);

    virtual bool doTestLevelIPC(const LevelData &d);
    virtual bool doTestLevelFile(const QString &levelFile);

    /*
        FIXME!!! Implement the right world map testing via interprocess
        with ability to start player at absolutely any position on it
    */
//    virtual bool doTestWorldIPC(const WorldData &d);

    virtual bool doTestWorldFile(const QString &worldFile);

    virtual bool runNormalGame();

    virtual void terminate();

    virtual bool isRunning();

    virtual int capabilities();

private slots:
    void testStarted();
    void testFinished();
};

#endif // PGEENGINE_H
