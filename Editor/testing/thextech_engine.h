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


#ifndef THEXTECHENGINE_H
#define THEXTECHENGINE_H

#include <QProcess>
#include <QMutex>

#include "ipc/pge_engine_ipc.h"
#include "abstract_engine.h"

class MainWindow;

class TheXTechEngine : public AbstractRuntimeEngine
{
    Q_OBJECT

    //! Engine application handler
    QProcess m_engineProc;
    //! Mutex which helps to avoid multiple launches of engine
    QMutex   m_engineMutex;

    MainWindow *m_w = nullptr;
    //! List of registered menu items
    QAction *m_menuItems[20];

    /************** Settings **************/
    //! Path to custom TheXTech executable
    QString m_customEnginePath;
    //! Enable magic-hand functionality (IPC mode only)
    bool    m_enableMagicHand = true;
    //! Enable magic-hand functionality (IPC mode only)
    bool    m_enableMaxFps = false;
    //! Enable ability of player to grab everything
    bool    m_enableGrabAll = false;
    //! Renderer type
    int     m_renderType = -1;
    //! Start game in battle mode
    bool    m_battleMode = false;
    /************** Settings **************/


    PgeEngineIpcClient interface;

    QString getEnginePath();

    void loadSetup();
    void saveSetup();

private slots:
    void retranslateMenu();

public slots:
    /********Menu items*******/
    void startTestAction();
    void startSafeTestAction();
    void startBattleTestAction();
    void chooseEnginePath();
    void startGameAction();
public:
    explicit TheXTechEngine(QObject *parent = nullptr);
    ~TheXTechEngine();

    virtual void init();
    virtual void unInit();

    virtual void initMenu(QMenu *destmenu);

    virtual bool doTestLevelIPC(const LevelData &d);
    virtual bool doTestLevelFile(const QString &levelFile);

//    virtual bool doTestWorldIPC(const WorldData &d);
//    virtual bool doTestWorldFile(const QString &worldFile);

    virtual bool runNormalGame();

    virtual void terminate();

    virtual bool isRunning();

    virtual int capabilities();

private slots:
    void testStarted();
    void testFinished();
};

#endif // THEXTECHENGINE_H
