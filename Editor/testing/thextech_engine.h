/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "ipc/thextech_capabilities.h"
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
    //! Size of the menu items array
    static const size_t m_menuItemsSize = 40;
    //! List of registered menu items
    QAction *m_menuItems[m_menuItemsSize];
    QAction *m_menuRunWorldTestFile = nullptr;
    QAction *m_menuRunWorldTestIPC = nullptr;

    QAction *m_startWarpAction = nullptr;
    QAction *m_saveSlotMenu = nullptr;

    QAction *m_renderVSync = nullptr;
    QAction *m_renderModernOpenGL = nullptr;
    QAction *m_renderLegacyOpenGL = nullptr;
    QAction *m_renderModernOpenGLES = nullptr;
    QAction *m_renderLegacyOpenGLES = nullptr;

    QAction *m_renderVSyncFlag = nullptr;

    /************** Settings **************/
    //! Default executable filename
    QString m_defaultEngineName;
    //! Path to custom TheXTech executable
    QString m_customEnginePath;
    //! Capabilities of given TheXTech build
    TheXTechCapabilities m_caps;
    //! Enable magic-hand functionality (IPC mode only)
    bool    m_enableMagicHand = true;
    //! Enable magic-hand functionality (IPC mode only)
    bool    m_enableMaxFps = false;
    //! Enable ability of player to grab everything
    bool    m_enableGrabAll = false;
    //! Renderer type
    int     m_renderType = -1;
    //! Enable VSync if available
    bool    m_vsyncEnable = false;
    //! Start game in battle mode
    bool    m_battleMode = false;
    //! Compatibility level
    int     m_compatLevel = -1;
    //! Speed-run mode
    int     m_speedRunMode = -1;
    //! Enable semi-transparent timer showing
    bool    m_speedRunTimerST = false;
    //! Save slot for world run
    int     m_saveSlot = 0;
    //! Select warp entrance for a level test
    int     m_startWarp = 0;
    /************** Settings **************/


    PgeEngineIpcClient m_interface;

    QString getEnginePath();

    void rescanCapabilities();
    void updateMenuCapabilities();

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
    virtual bool doTestWorldFile(const QString &worldFile);

    virtual bool runNormalGame();

    virtual void terminate();

    virtual bool isRunning();

    virtual int capabilities();

signals:
    void testStarted();
    void testFinished();
};

#endif // THEXTECHENGINE_H
